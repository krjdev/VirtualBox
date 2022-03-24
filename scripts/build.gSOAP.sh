#!/bin/bash

# // SPDX-License-Identifier: GPL-2.0
# 
# Helper script for downloading and building the gSOAP toolkit
# 
# Copyright (c) 2022 Johannes Krottmayer <krjdev@gmail.com>

SCRIPT_VERSION="0.1.0.0"
SOURCE_URL="https://sourceforge.net/projects/gsoap2/files/latest/download"
OUT_FILENAME="gsoap.zip"
OUT_DIR="gsoap-"
BUILD_DIR="build"

# Flags
_SCRIPT_KEEP=0

# Cleanup 
env_cleanup()
{
    unset SCRIPT_VERSION
    unset SOURCE_URL
    unset _SCRIPT_DEFAULT
    unset _SCRIPT_KEEP
}

show_usage()
{
    _SCRIPT=$(basename $0)
    echo "Usage: $_SCRIPT [OPTS]"
    echo "Usage: $_SCRIPT [OPTS] [ARGS...]"
    echo
    echo "Description:"
    echo "Helper script for downloading and building the gSOAP toolkit"
    echo
    echo "[OPTS]"
    echo "  -V, --version"
    echo "  Print script version and exit."
    echo "  -H, --help"
    echo "  Print this text and exit."
    echo "  -k, --keep"
    echo "  Don't remove the gSOAP source folder after successfull build (Not implemented)."
    unset _SCRIPT
}

show_version()
{
    _SCRIPT=$(basename $0)
    echo "$_SCRIPT: Version $SCRIPT_VERSION "
    unset _SCRIPT
}

build_toolkit()
{
    # Download toolkit
    mkdir $BUILD_DIR
    wget $SOURCE_URL -O $BUILD_DIR/$OUT_FILENAME
    
    # Extract archive
    cd $BUILD_DIR
    unzip $OUT_FILENAME
    
    # FIXME
    # We use a wildcard for the change dir command, because we
    # don't know the extracted sub-dir...
    cd $OUT_DIR*
    
    # Build toolkit
    ./configure
    make
    
    # Install toolkit
    #
    # FIXME
    # We use here currently the sudo command...
    sudo make install
    
    # Leave dirs
    cd ../..
}

if [ $# -eq 0 ]
then
    _SCRIPT_DEFAULT=1
else
    while [ $# -ne 0 ]
    do
        case $1 in
        -V|--version)
            show_version
            env_cleanup
            exit 0
            ;;
        -H|--help)
            show_usage
            env_cleanup
            exit 0
            ;;
        -k|--keep)
            _SCRIPT_KEEP=1
            shift
            ;;
        *)
            if [ $# -ne 0 ]
            then
                echo "Error: Too many arguments"
                env_cleanup
                exit 1
            fi
            ;;
        esac
    done
fi

build_toolkit
env_cleanup
exit 0
