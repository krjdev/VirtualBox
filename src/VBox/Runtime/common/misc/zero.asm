; $Id: zero.asm 93253 2022-01-16 02:27:00Z vboxsync $
;; @file
; IPRT - Zero Memory.
;

;
; Copyright (C) 2013-2022 Oracle Corporation
;
; This file is part of VirtualBox Open Source Edition (OSE), as
; available from http://www.virtualbox.org. This file is free software;
; you can redistribute it and/or modify it under the terms of the GNU
; General Public License (GPL) as published by the Free Software
; Foundation, in version 2 as it comes in the "COPYING" file of the
; VirtualBox OSE distribution. VirtualBox OSE is distributed in the
; hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
;
; The contents of this file may alternatively be used under the terms
; of the Common Development and Distribution License Version 1.0
; (CDDL) only, as it comes in the "COPYING.CDDL" file of the
; VirtualBox OSE distribution, in which case the provisions of the
; CDDL are applicable instead of those of the GPL.
;
; You may elect to license modified versions of this file under the
; terms and conditions of either the GPL or the CDDL or both.
;

;*******************************************************************************
;* Header Files                                                                *
;*******************************************************************************
%include "iprt/asmdefs.mac"

;
; Section to put this in.
;
;       - PE/COFF: Unless we put this in an BSS like section, the linker will
;         write out 64KB of zeros.  (Tried using .rdata$zz and put it at the end
;         of that section, but the linker did not reduce the RawDataSize.)  The
;         assmebler does not let us control the write flag directly, so we emit
;         a linker directive that switches of the write flag for the section.
;
;       - Fallback: Code section.
;
%ifdef ASM_FORMAT_PE
section .drectve info
        db '-section:.zero,!W '
section .zero bss align=4096
%else
BEGINCODE
%endif

;;
; 64KB of zero memory with various sized labels.
;
EXPORTEDNAME_EX g_abRTZeroPage, object
EXPORTEDNAME_EX g_abRTZero4K, object
EXPORTEDNAME_EX g_abRTZero8K, object
EXPORTEDNAME_EX g_abRTZero16K, object
EXPORTEDNAME_EX g_abRTZero32K, object
EXPORTEDNAME_EX g_abRTZero64K, object
%ifdef ASM_FORMAT_PE
        resb  0x10000
%else
        times 0x10000/(16*4) dd 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
%endif
%ifdef ASM_FORMAT_ELF
size g_abRTZeroPage     _4K
size g_abRTZero4K       _4K
size g_abRTZero8K       _8K
size g_abRTZero16K     _16K
size g_abRTZero32K     _32K
size g_abRTZero64K     _64K
%endif

