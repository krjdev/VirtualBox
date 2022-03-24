# // SPDX-License-Identifier: GPL-2.0
# 
# Copyright (c) 2022 Johannes Krottmayer <krjdev@gmail.com>
# 
# Wrapper Makefile to build VirtualBox

ifndef KRJDEV_VBOX_KMK
ifeq ($(origin KRJDEV_VBOX_KMK),undefined)
KRJDEV_VBOX_KMK		:= kBuild/bin/linux.amd64/kmk
endif
else
KRJDEV_VBOX_KMK		:= kBuild/bin/linux.amd64/kmk
endif

# Match all (unknown) targets rule
%::
	@$(KRJDEV_VBOX_KMK) $@

all:
	@$(KRJDEV_VBOX_KMK)

.PHONY: all
