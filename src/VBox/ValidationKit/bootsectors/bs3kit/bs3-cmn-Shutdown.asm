; $Id: bs3-cmn-Shutdown.asm 93115 2022-01-01 11:31:46Z vboxsync $
;; @file
; BS3Kit - Bs3Shutdown
;

;
; Copyright (C) 2007-2022 Oracle Corporation
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

%include "bs3kit-template-header.mac"
%include "VBox/bios.mac"

BS3_EXTERN_CMN Bs3Panic

BS3_PROC_BEGIN_CMN Bs3Shutdown, BS3_PBC_HYBRID_0_ARGS
        cli
%ifdef TMPL_16BIT
        mov     ax, cs
        mov     ds, ax
%endif
        mov     bl, 64
        mov     dx, VBOX_BIOS_SHUTDOWN_PORT
        mov     ax, VBOX_BIOS_OLD_SHUTDOWN_PORT
.retry:
        mov     ecx, 8
        mov     esi, .s_szShutdown
        rep outsb
        xchg    ax, dx                  ; alternate between the new (VBox) and old (Bochs) ports.
        dec     bl
        jnz     .retry
        ; Shutdown failed!
        jmp     Bs3Panic
.s_szShutdown:
        db      'Shutdown', 0
BS3_PROC_END_CMN Bs3Shutdown

