/** @file
 * PDM - Pluggable Device Manager, Read/Write Critical Section.
 */

/*
 * Copyright (C) 2006-2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#ifndef VBOX_INCLUDED_vmm_pdmcritsectrw_h
#define VBOX_INCLUDED_vmm_pdmcritsectrw_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <VBox/types.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_pdm_critsectrw      The PDM Read/Write Critical Section API
 * @ingroup grp_pdm
 * @{
 */

/**
 * A PDM read/write critical section.
 * Initialize using PDMDRVHLP::pfnCritSectRwInit().
 */
typedef union PDMCRITSECTRW
{
    /** Padding. */
    uint8_t padding[HC_ARCH_BITS == 32 ? 0xc0 : 0x100];
#ifdef PDMCRITSECTRWINT_DECLARED
    /** The internal structure (not normally visible). */
    struct PDMCRITSECTRWINT s;
#endif
} PDMCRITSECTRW;

VMMR3DECL(int)      PDMR3CritSectRwInit(PVM pVM, PPDMCRITSECTRW pCritSect, RT_SRC_POS_DECL,
                                        const char *pszNameFmt, ...) RT_IPRT_FORMAT_ATTR(6, 7);
VMMR3DECL(int)      PDMR3CritSectRwDelete(PVM pVM, PPDMCRITSECTRW pCritSect);
VMMR3DECL(const char *) PDMR3CritSectRwName(PCPDMCRITSECTRW pCritSect);
VMMR3DECL(int)      PDMR3CritSectRwEnterSharedEx(PVM pVM, PPDMCRITSECTRW pThis, bool fCallRing3);
VMMR3DECL(int)      PDMR3CritSectRwEnterExclEx(PVM pVM, PPDMCRITSECTRW pThis, bool fCallRing3);

VMMDECL(int)        PDMCritSectRwEnterShared(PVMCC pVM, PPDMCRITSECTRW pCritSect, int rcBusy);
VMMDECL(int)        PDMCritSectRwEnterSharedDebug(PVMCC pVM, PPDMCRITSECTRW pCritSect, int rcBusy, RTHCUINTPTR uId, RT_SRC_POS_DECL);
VMMDECL(int)        PDMCritSectRwTryEnterShared(PVMCC pVM, PPDMCRITSECTRW pCritSect);
VMMDECL(int)        PDMCritSectRwTryEnterSharedDebug(PVMCC pVM, PPDMCRITSECTRW pCritSect, RTHCUINTPTR uId, RT_SRC_POS_DECL);
VMMDECL(int)        PDMCritSectRwLeaveShared(PVMCC pVM, PPDMCRITSECTRW pCritSect);
VMMDECL(int)        PDMCritSectRwEnterExcl(PVMCC pVM, PPDMCRITSECTRW pCritSect, int rcBusy);
VMMDECL(int)        PDMCritSectRwEnterExclDebug(PVMCC pVM, PPDMCRITSECTRW pCritSect, int rcBusy, RTHCUINTPTR uId, RT_SRC_POS_DECL);
VMMDECL(int)        PDMCritSectRwTryEnterExcl(PVMCC pVM, PPDMCRITSECTRW pCritSect);
VMMDECL(int)        PDMCritSectRwTryEnterExclDebug(PVMCC pVM, PPDMCRITSECTRW pCritSect, RTHCUINTPTR uId, RT_SRC_POS_DECL);
VMMDECL(int)        PDMCritSectRwLeaveExcl(PVMCC pVM, PPDMCRITSECTRW pCritSect);

VMMDECL(bool)       PDMCritSectRwIsWriteOwner(PVMCC pVM, PPDMCRITSECTRW pCritSect);
VMMDECL(bool)       PDMCritSectRwIsReadOwner(PVMCC pVM, PPDMCRITSECTRW pCritSect, bool fWannaHear);
VMMDECL(uint32_t)   PDMCritSectRwGetWriteRecursion(PPDMCRITSECTRW pCritSect);
VMMDECL(uint32_t)   PDMCritSectRwGetWriterReadRecursion(PPDMCRITSECTRW pCritSect);
VMMDECL(uint32_t)   PDMCritSectRwGetReadCount(PPDMCRITSECTRW pCritSect);
VMMDECL(bool)       PDMCritSectRwIsInitialized(PCPDMCRITSECTRW pCritSect);

/* Lock strict build: Remap the three enter calls to the debug versions. */
#ifdef VBOX_STRICT
# ifdef IPRT_INCLUDED_asm_h
#  define PDMCritSectRwEnterExcl(a_pVM, pCritSect, rcBusy)      PDMCritSectRwEnterExclDebug((a_pVM), pCritSect, rcBusy, (uintptr_t)ASMReturnAddress(), RT_SRC_POS)
#  define PDMCritSectRwTryEnterExcl(a_pVM, pCritSect)           PDMCritSectRwTryEnterExclDebug((a_pVM), pCritSect, (uintptr_t)ASMReturnAddress(), RT_SRC_POS)
#  define PDMCritSectRwEnterShared(a_pVM, pCritSect, rcBusy)    PDMCritSectRwEnterSharedDebug((a_pVM), pCritSect, rcBusy, (uintptr_t)ASMReturnAddress(), RT_SRC_POS)
#  define PDMCritSectRwTryEnterShared(a_pVM, pCritSect)         PDMCritSectRwTryEnterSharedDebug((a_pVM), pCritSect, (uintptr_t)ASMReturnAddress(), RT_SRC_POS)
# else
#  define PDMCritSectRwEnterExcl(a_pVM, pCritSect, rcBusy)      PDMCritSectRwEnterExclDebug((a_pVM), pCritSect, rcBusy, 0, RT_SRC_POS)
#  define PDMCritSectRwTryEnterExcl(a_pVM, pCritSect)           PDMCritSectRwTryEnterExclDebug((a_pVM), pCritSect, 0, RT_SRC_POS)
#  define PDMCritSectRwEnterShared(a_pVM, pCritSect, rcBusy)    PDMCritSectRwEnterSharedDebug((a_pVM), pCritSect, rcBusy, 0, RT_SRC_POS)
#  define PDMCritSectRwTryEnterShared(a_pVM, pCritSect)         PDMCritSectRwTryEnterSharedDebug((a_pVM), pCritSect, 0, RT_SRC_POS)
# endif
#endif

/** @} */

RT_C_DECLS_END

#endif /* !VBOX_INCLUDED_vmm_pdmcritsectrw_h */

