/* $Id: mppresent-generic-online.cpp 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * IPRT - Multiprocessor, Stubs for the RTMp*Present* API mapping to RTMp*Online.
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
#include <iprt/mp.h>
#include "internal/iprt.h"


RTDECL(PRTCPUSET) RTMpGetPresentSet(PRTCPUSET pSet)
{
    return RTMpGetOnlineSet(pSet);
}
RT_EXPORT_SYMBOL(RTMpGetPresentSet);


RTDECL(RTCPUID) RTMpGetPresentCount(void)
{
    return RTMpGetOnlineCount();
}
RT_EXPORT_SYMBOL(RTMpGetPresentCount);


RTDECL(RTCPUID) RTMpGetPresentCoreCount(void)
{
    return RTMpGetOnlineCoreCount();
}
RT_EXPORT_SYMBOL(RTMpGetPresentCoreCount);


RTDECL(bool) RTMpIsCpuPresent(RTCPUID idCpu)
{
    return RTMpIsCpuOnline(idCpu);
}
RT_EXPORT_SYMBOL(RTMpIsCpuPresent);

