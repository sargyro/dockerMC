# Put root in path
source /dockerMC/root/bin/thisroot.sh

# Set up nice prompt
export PS1='\[\033[01;35m\][\u]\[\033[01;31m\][LHC-DMWG]\[\033[01;34m\]:\W >\[\033[00m\] '

# Welcome message
echo "=========================================="
echo  "You are running in the dockerMC container"
echo "=========================================="

# ROOT alias to not try to use graphics
alias root="root -l -b"

/bin/bash

