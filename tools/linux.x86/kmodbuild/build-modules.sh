#!/bin/bash -x
# $Id: build-modules.sh 78140 2019-04-16 02:20:36Z vboxsync $
## @file
# Script for test building the VirtualBox kernel modules against a kernel.
#
# This script assumes the kernel directory it is pointed to was prepared using
# build-kernel.sh, as that script plants a couple of files and symlinks needed
# by this script.
#

#
# Copyright (C) 2019 Oracle Corporation
#
# This file is part of VirtualBox Open Source Edition (OSE), as
# available from http://www.virtualbox.org. This file is free software;
# you can redistribute it and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software
# Foundation, in version 2 as it comes in the "COPYING" file of the
# VirtualBox OSE distribution. VirtualBox OSE is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
#

if [ $# -lt 2 ]; then
    echo "usage: modules.sh <PATH_STAGE> <kernel-dir>"
    exit 2
fi
PATH_STAGE=$1
PATH_STAGE_GUEST_SRC="${PATH_STAGE}/bin/additions/src"
PATH_STAGE_HOST_SRC="${PATH_STAGE}/bin/src"
KERN_DIR=$2
KERN_NAME=`basename "${KERN_DIR}"`
KERN_VER=`echo ${KERN_NAME} | sed -e 's/^.*linux-//'`
KERN_BASE_DIR=`dirname "${KERN_DIR}"`
BLDDIR_BASE="/tmp/modbld"
BLDDIR="${BLDDIR_BASE}/`echo ${PATH_STAGE} ${KERN_BASE_DIR} | sha1sum | cut -b1-16`-${KERN_NAME}/"
JOBS=36
shift
shift


# Process other options
OPT_CLOBBER=
while [ $# -gt 0 ];
do
    case "$1" in
        clobber) OPT_CLOBBER=1;;
        --version) KERN_VER="$2"; shift;;
        *)  echo "syntax error: $1" 1>&2
            exit 2;;
    esac
    shift;
done

#
# Prepare the sources we're to build.
#
set -e
test -n "${BLDDIR}"
if [ -d "${BLDDIR}" -a -n "${OPT_CLOBBER}" ]; then
    rm -R "${BLDDIR}/"
fi

mkdir -p "${BLDDIR}/" "${BLDDIR}/guest/" "${BLDDIR}/host/"
rsync -crlDp --exclude="*.tmp_versions/" --include="*/" --include="*.h" --include="*.c" --include="*.cpp" --include="Makefile*" --include="build_in_tmp" --exclude="*" "${PATH_STAGE_HOST_SRC}/"  "${BLDDIR}/host/"
rsync -crlDp --exclude="*.tmp_versions/" --include="*/" --include="*.h" --include="*.c" --include="*.cpp" --include="Makefile*" --include="build_in_tmp" --exclude="*" "${PATH_STAGE_GUEST_SRC}/" "${BLDDIR}/guest/"

#
# Do the building.
#
if [ -f "${KERN_DIR}/.bird-make" -a ! -f "${KERN_DIR}/.bird-failed" ]; then
    if "${KERN_DIR}/.bird-make" --help 2>&1 | grep -q output-sync -; then
        SYNC_OUTPUT="--output-sync=target"
    else
        SYNC_OUTPUT=""
    fi
    "${KERN_DIR}/.bird-make" -C "${BLDDIR}/guest/" \
        VBOX_NOJOBS=1 -j${JOBS} `cat "${KERN_DIR}/.bird-overrides"` ${SYNC_OUTPUT} "KERN_DIR=${KERN_DIR}" "KERN_VER=${KERN_VER}"
    case "${KERN_VER}" in
        [3-9].*|2.6.3[789]*) ## todo fix this so it works back to 2.6.18 (-fno-pie, -Wno-declaration-after-statement)
            "${KERN_DIR}/.bird-make" -C "${BLDDIR}/host/"  \
                VBOX_NOJOBS=1 -j${JOBS} `cat "${KERN_DIR}/.bird-overrides"` ${SYNC_OUTPUT} "KERN_DIR=${KERN_DIR}" "KERN_VER=${KERN_VER}"
            ;;
    esac
else
    echo "${KERN_DIR}: Skipping..."
fi

