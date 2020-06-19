# Description
This is houses the docker file that creates the image with all the necessary contents
for the [LHC-DMWG April 2020 workshop](https://indico.cern.ch/event/900402/)

# Authors
 - Sam Meehan <samuel.meehan@cern.ch>
 - Matthew Feickert <matthew.feickert@cern.ch> (aka "The docker Guru")

# Quickstart
The only requirement is [docker](https://www.docker.com/).  If you don't have this, refer to the bottom
of this page. If you already have docker installed, then pull the image :
```
docker pull gitlab-registry.cern.ch/meehan/docker-lhcdmwg-april2020
```
and then prune it
```
docker system prune
```
you can then boot up the container
```
docker run --rm -it -v $PWD:$PWD gitlab-registry.cern.ch/meehan/docker-lhcdmwg-april2020:latest
```
you'll have the directory in which you executed that command (your $PWD) bind mounted 
__inside the container's filesystem__ at the same path, however the 
__default__ working directory you'll be dropped into in an interactive 
session is `/home/data`. If you would like to change this so that you enter the 
container filesystem at the same path that you are on at your local machine then 
simply add the `-w` flag:
```
docker run --rm -it -v $PWD:$PWD -w $PWD gitlab-registry.cern.ch/meehan/docker-lhcdmwg-april2020:latest
```

# Stuff Included Here
The following versions and packages are included in the docker image stored in 
the [gitlab registry](https://gitlab.cern.ch/meehan/docker-lhcdmwg-april2020/container_registry).

## MadGraph5 
Version : v2.6.7

Along with MG5, the following non standard models have been included in the `models` directory :
 - [DMSimp](http://feynrules.irmp.ucl.ac.be/wiki/DMsimp) : The V/AV benchmark model
 - [t-Channel](http://feynrules.irmp.ucl.ac.be/wiki/DMsimpt) : The t-channel model under consideration

## Python
Version : 2.7

## pythia8
Version : 8.2.44

This is bundled into MG5 and was installed via that interface.  
Pythia 8.2, which is the one that comes when you try to `install` it in MadGraph, has been
deleted from that area.  Therefore, this has been installed with the call
```
install pythia8 --pythia8_tarball=/LHC-DMWG/pythia8244.tgz
```

## LHAPDF 
Version : 6.2.3

The following additional PDF sets have been included in the install directory of the 
installation within MG5
 - [npdf30_lo_as_0118.tar.gz](https://github.com/LHC-DMWG/DMWG-tChannel-whitepaper/blob/master/tutorial/colliders/tools/nnpdf30_lo_as_0118.tar.gz)
 - [npdf30_nlo_as_0118.tar.gz](https://github.com/LHC-DMWG/DMWG-tChannel-whitepaper/blob/master/tutorial/colliders/tools/nnpdf30_nlo_as_0118.tar.gz)

You will need to `wget` different versions of the PDF sets using commands like the following
```
wget http://lhapdfsets.web.cern.ch/lhapdfsets/current/CT10nlo.tar.gz -O- | tar xz -C /LHC-DMWG/MG5_aMC_v2_7_2/HEPTools/lhapdf6/share/LHAPDF
```

## HepMC  
Version : 2.06.09

This is bundled into MG5 and was installed via that interface.

## ROOT
Version : 6.14.06

This is being included directly from the precompiled binaries because they were the ones
available for Fedora-v27 which is the base OS of the base image. (Yes, this could probably be better.)  
 
# Technical Stuff

## Requirements
To be able to use this, you need to have [docker](https://www.docker.com/) installed. If
you are new to docker, I recommend following [this tutorial from Matthew Feickert](https://matthewfeickert.github.io/intro-to-docker/).

### Mac OSX

Download Docker for MacOS - [instructions](https://docs.docker.com/docker-for-mac/install/).

Docker is a full development platform for creating containerized apps, and Docker Desktop 
for Mac is the best way to get started with Docker on a Mac. To download Docker Desktop 
for MacOS, head to [Docker Hub](https://hub.docker.com/editions/community/docker-ce-desktop-mac).

Please read the relevant information on these pages, it should take no more than 5 minutes.
Another common way to install packages on Mac OSX is via the 
[homebrew package manager](https://brew.sh/). In the case of docker, you can easily 
install docker by setting up homebrew and executing brew cask install docker.

### Linux

Here are directions for two of the more common Linux distributions : 
[Ubuntu](https://docs.docker.com/engine/install/ubuntu/), [CentOS](https://docs.docker.com/engine/install/centos/)

Note that downloading and using docker in linux may be a bit more challenging due to permissions.

### Windows

It is highly recommended that you DO NOT use Windows. Few individuals use this OS within 
the HEP community as most tools are designed for Unix-based systems. If you do have a 
Windows machine, consider making your computer a dual-boot machine - [Link to Directions](https://opensource.com/article/18/5/dual-boot-linux).

Download Docker for Windows - [instructions](https://docs.docker.com/docker-for-windows/install/).

Docker Desktop for Windows is the Community Edition (CE) of Docker for Microsoft Windows. 
To download Docker Desktop for Windows, head to [Docker Hub](https://hub.docker.com/editions/community/docker-ce-desktop-windows).

## Build the Image Yourself
If you want to try and build the image yourself, then you can clone this repo and run the
following command
```
docker build -t myTagName .
```

