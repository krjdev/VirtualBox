/* $Id: RTUtf16NLenEx.cpp 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * IPRT - RTUtf16NLenEx.
 */

/*
 * Copyright (C) 2008-2022 Oracle Corporation
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


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include <iprt/utf16.h>
#include "internal/iprt.h"

#include <iprt/errcore.h>


RTDECL(int) RTUtf16NLenEx(PCRTUTF16 pwszString, size_t cwcMax, size_t *pcwc)
{
    PCRTUTF16 pwcEnd = RTUtf16End(pwszString, cwcMax);
    if (!pwcEnd)
    {
        *pcwc = cwcMax;
        return VERR_BUFFER_OVERFLOW;
    }
    *pcwc = pwcEnd - pwszString;
    return VINF_SUCCESS;
}
RT_EXPORT_SYMBOL(RTUtf16NLenEx);

