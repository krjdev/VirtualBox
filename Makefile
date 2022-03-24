# // SPDX-License-Identifier: GPL-2.0
# 
# Makefile for building VirtualBox
# 
# Copyright (c) 2022 Johannes Krottmayer <krjdev@gmail.com>

SHELL			:= sh

ifndef VIRTUALBOX_KMK
ifeq ($(origin VIRTUALBOX_KMK),undefined)
VIRTUALBOX_KMK		:= kBuild/bin/linux.amd64/kmk
endif
else
VIRTUALBOX_KMK		:= kBuild/bin/linux.amd64/kmk
endif

# Match all (unknown) targets rule
%::
	$(SHELL) scripts/build.gSOAP.sh
	./configure --disable-hardening
	@$(VIRTUALBOX_KMK) $@

all:
	$(SHELL) scripts/build.gSOAP.sh
	./configure --disable-hardening
	@$(VIRTUALBOX_KMK)

.PHONY: all
