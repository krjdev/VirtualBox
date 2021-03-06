/* $Id: strchr.cpp 93137 2022-01-07 03:07:03Z vboxsync $ */
/** @file
 * IPRT - CRT Strings, strchr().
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

#include <iprt/string.h>

#ifdef _MSC_VER
_CRTIMP char * __cdecl strchr
#elif defined(__WATCOMC__) && !defined(IPRT_NO_CRT)
_WCRTLINK char *std::strchr
#else
char *strchr
#endif
    (const char *pszStr, int ch)
#if defined(__THROW) && !defined(RT_OS_WINDOWS) && !defined(RT_OS_OS2)
    __THROW
#endif
{
    char chCur;
    while ((chCur = *pszStr) != '\0')
        if (chCur != ch)
            pszStr++;
        else
            return (char *)pszStr;
    return NULL;
}

