ARG BASE_IMAGE=fedora:27
FROM ${BASE_IMAGE} as base

FROM base as reqs_base

# needed for installation of utilities in madgraph
RUN dnf update -y && \
    dnf install -y \
      make \
      bzr \
      cmake \
      gcc-c++ \
      gcc-gfortran \
      redhat-rpm-config \
      wget \
      rsync \
      tar \
      less \
      bzip2 \
      findutils \
      which \
      vim \
      emacs \
      zlib-devel \
      libXpm \
      tbb \
      python \
      python-devel \
      python-requests \
      file \
      gnuplot \
      gsl-devel \
      ghostscript \
      patch && \
    dnf clean all && \
    yum -y install \
      openssl-devel \
      bzip2-devel \
      boost-devel \
      libffi-devel && \
    yum clean all

ENV LD_LIBRARY_PATH=/usr/local/lib
ENV PYTHONPATH=/usr/local/lib64/python2.7/site-packages

FROM reqs_base as builder

#############################################
# perform everything as root
#############################################
USER root
WORKDIR /dockerMC

#############################################
# Install ROOT from binaries
# FROM : https://root.cern.ch/content/release-61406.
#############################################
RUN wget https://root.cern.ch/download/root_v6.14.06.Linux-fedora27-x86_64-gcc7.3.tar.gz && \
    tar -xzvf root_v6.14.06.Linux-fedora27-x86_64-gcc7.3.tar.gz && \
    rm -f root_v6.14.06.Linux-fedora27-x86_64-gcc7.3.tar.gz && \
    source /dockerMC/root/bin/thisroot.sh

#######################################
# install MadGraph
# FROM : https://cp3.irmp.ucl.ac.be/projects/madgraph/
#######################################

# set variables used throughout to allow for easy changing of versions
ENV MG5_PATH      2.7.x   
ENV MG5_TARBALL   MG5_aMC_v2.7.3.tar.gz
ENV MG5_DIRECTORY MG5_aMC_v2_7_3
    
# get the MG5 tarball
RUN wget https://launchpad.net/mg5amcnlo/2.0/$MG5_PATH/+download/$MG5_TARBALL && \
    tar -xzf $MG5_TARBALL && \
    rm $MG5_TARBALL
    
# install hepmc
RUN cd $MG5_DIRECTORY && \
    echo "install hepmc" >> install.dat && \
    ./bin/mg5_aMC install.dat && \
    rm install.dat
    
# install ninja
RUN cd $MG5_DIRECTORY && \
    echo "install ninja" >> install.dat && \
    ./bin/mg5_aMC install.dat && \
    rm install.dat

# get pythia tarball and then install pythia
RUN wget http://home.thep.lu.se/~torbjorn/pythia8/pythia8244.tgz && \
    cd $MG5_DIRECTORY && \
    echo "install pythia8 --pythia8_tarball=/dockerMC/pythia8244.tgz" >> install.dat && \
    ./bin/mg5_aMC install.dat && \
    rm install.dat && \
    cd .. && \
    rm pythia8244.tgz

# install lhapdf6 and put a couple of the additional PDF sets in it
RUN cd $MG5_DIRECTORY && \
    echo "install lhapdf6" >> install.dat && \
    ./bin/mg5_aMC install.dat && \
    rm install.dat

RUN wget http://pcteserver.mi.infn.it/~nnpdf/nnpdf30/NNPDF30_lo_as_0118.tar.gz -O- | tar xz -C /dockerMC/$MG5_DIRECTORY/HEPTools/lhapdf6/share/LHAPDF && \
    wget http://pcteserver.mi.infn.it/~nnpdf/nnpdf30/NNPDF30_nlo_as_0118.tar.gz -O- | tar xz -C /dockerMC/$MG5_DIRECTORY/HEPTools/lhapdf6/share/LHAPDF

# install madanalysis5 directly in the MG5 location
RUN cd $MG5_DIRECTORY && \
    echo "install MadAnalysis5" >> install.dat && \
    ./bin/mg5_aMC install.dat && \
    rm install.dat
        
# clean up and organize build
FROM reqs_base

COPY --from=builder /dockerMC /dockerMC

# Use C.UTF-8 locale to avoid issues with ASCII encoding
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

ENV HOME /home
WORKDIR ${HOME}/data
ENV PATH ${HOME}/.local/bin:$PATH
ENV PATH /dockerMC/$MG5_DIRECTORY/bin:$PATH

RUN echo 'export PATH=/dockerMC/$MG5_DIRECTORY/bin:"${PATH}"' >> /root/.bashrc && \
    cp /root/.bash_profile ${HOME}/.bash_profile && \
    cp /root/.bashrc ${HOME}/.bashrc
    
WORKDIR /dockerMC

COPY AnalysisMacros /dockerMC/AnalysisMacros
COPY PythiaMacros /dockerMC/PythiaMacros

COPY entrypoint.sh /dockerMC/entrypoint.sh
RUN chmod 777 /dockerMC/entrypoint.sh
ENTRYPOINT ["/bin/bash", "/dockerMC/entrypoint.sh"]
CMD ["/bin/bash", "/dockerMC/entrypoint.sh"]
