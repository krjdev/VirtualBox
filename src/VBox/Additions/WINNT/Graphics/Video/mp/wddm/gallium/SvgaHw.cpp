/* $Id: SvgaHw.cpp 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * VirtualBox Windows Guest Mesa3D - VMSVGA hardware access.
 */

/*
 * Copyright (C) 2016-2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#include "SvgaHw.h"

int SvgaEnable(PVBOXWDDM_EXT_VMSVGA pSvga)
{
    NOREF(pSvga);
    return 0;
}
