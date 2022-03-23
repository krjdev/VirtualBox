/* $Id: RTKrnlModInfo.cpp 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * IPRT - Utility for getting information about loaded kernel modules.
 */

/*
 * Copyright (C) 2017-2022 Oracle Corporation
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
#include <iprt/krnlmod.h>

#include <iprt/assert.h>
#include <iprt/errcore.h>
#include <iprt/getopt.h>
#include <iprt/initterm.h>
#include <iprt/mem.h>
#include <iprt/message.h>
#include <iprt/path.h>
#include <iprt/stream.h>
#include <iprt/string.h>


/**
 * Handles loading a kernel module by name.
 *
 * @returns Process status code.
 * @param   pszName             THe module name to load.
 */
static RTEXITCODE rtKrnlModInfoHandleLoad(const char *pszName)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    int rc = RTKrnlModLoadByName(pszName);
    if (RT_SUCCESS(rc))
        RTPrintf("Kernel module '%s' loaded successfully\n", pszName);
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Error %Rrc loading kernel module '%s'", rc, pszName);

    return rcExit;
}


/**
 * Handles unloading a kernel module by name.
 *
 * @returns Process status code.
 * @param   pszName             THe module name to load.
 */
static RTEXITCODE rtKrnlModInfoHandleUnload(const char *pszName)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    int rc = RTKrnlModUnloadByName(pszName);
    if (RT_SUCCESS(rc))
        RTPrintf("Kernel module '%s' unloaded successfully\n", pszName);
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Error %Rrc unloading kernel module '%s'", rc, pszName);

    return rcExit;
}


/**
 * Handles listing all loaded kernel modules.
 *
 * @returns Process status code.
 */
static RTEXITCODE rtKrnlModInfoHandleList(void)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    uint32_t cKrnlMods = RTKrnlModLoadedGetCount();
    if (cKrnlMods)
    {
        PRTKRNLMODINFO pahKrnlModInfo = (PRTKRNLMODINFO)RTMemAllocZ(cKrnlMods * sizeof(RTKRNLMODINFO));
        if (pahKrnlModInfo)
        {
            int rc = RTKrnlModLoadedQueryInfoAll(pahKrnlModInfo, cKrnlMods, &cKrnlMods);
            if (RT_SUCCESS(rc))
            {
                RTPrintf("Index Load address        Size       Ref count  Name \n");
                for (unsigned i = 0; i < cKrnlMods; i++)
                {
                    RTKRNLMODINFO hKrnlModInfo = pahKrnlModInfo[i];
                    RTPrintf("%5u %#-18RHv  %-10u %-10u %s\n", i,
                             RTKrnlModInfoGetLoadAddr(hKrnlModInfo),
                             RTKrnlModInfoGetSize(hKrnlModInfo),
                             RTKrnlModInfoGetRefCnt(hKrnlModInfo),
                             RTKrnlModInfoGetName(hKrnlModInfo));
                    RTKrnlModInfoRelease(hKrnlModInfo);
                }
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Error %Rrc querying kernel modules", rc);

            RTMemFree(pahKrnlModInfo);
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Error %Rrc allocating memory", VERR_NO_MEMORY);
    }

    return rcExit;
}


int main(int argc, char **argv)
{
    int rc = RTR3InitExe(argc, &argv, 0);
    if (RT_FAILURE(rc))
        return RTMsgInitFailure(rc);

    /*
     * Parse arguments.
     */
    static const RTGETOPTDEF s_aOptions[] =
    {
        { "--load",         'l', RTGETOPT_REQ_STRING  },
        { "--unload",       'u', RTGETOPT_REQ_STRING  },
        { "--show-loaded",  's', RTGETOPT_REQ_NOTHING },
        { "--help",         'h', RTGETOPT_REQ_NOTHING }
    };

    RTGETOPTUNION   ValueUnion;
    RTGETOPTSTATE   GetState;
    RTGetOptInit(&GetState, argc, argv, s_aOptions, RT_ELEMENTS(s_aOptions), 1, RTGETOPTINIT_FLAGS_OPTS_FIRST);
    while ((rc = RTGetOpt(&GetState, &ValueUnion)))
    {
        switch (rc)
        {
            case 'l':
                return rtKrnlModInfoHandleLoad(ValueUnion.psz);
            case 'u':
                return rtKrnlModInfoHandleUnload(ValueUnion.psz);
            case 's':
                return rtKrnlModInfoHandleList();
            case 'h':
                RTPrintf("Usage: %s [options]\n"
                         "\n"
                         "Options:\n"
                         "  -l, --load <module name>\n"
                         "      Tries to load the given kernel module.\n"
                         "  -s, --show-loaded\n"
                         "      Lists all loaded kernel modules.\n"
                         , RTPathFilename(argv[0]));
                return RTEXITCODE_SUCCESS;

            case 'V':
                RTPrintf("$Revision: 93115 $\n");
                return RTEXITCODE_SUCCESS;

            default:
                return RTGetOptPrintError(rc, &ValueUnion);
        }
    }

    /* No arguments means listing all loaded kernel modules. */
    return rtKrnlModInfoHandleList();
}

