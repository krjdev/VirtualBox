/* $Id: VMMDevTesting.h 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * VMMDev - Guest <-> VMM/Host communication device, internal header.
 */

/*
 * Copyright (C) 2010-2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef VBOX_INCLUDED_SRC_VMMDev_VMMDevTesting_h
#define VBOX_INCLUDED_SRC_VMMDev_VMMDevTesting_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <VBox/types.h>
#include <VBox/VMMDevTesting.h>

RT_C_DECLS_BEGIN

int  vmmdevR3TestingInitialize(PPDMDEVINS pDevIns);
void vmmdevR3TestingTerminate(PPDMDEVINS pDevIns);
int  vmmdevRZTestingInitialize(PPDMDEVINS pDevIns);

RT_C_DECLS_END

#endif /* !VBOX_INCLUDED_SRC_VMMDev_VMMDevTesting_h */

