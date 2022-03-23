/* $Id: tstRTStrSimplePattern.cpp 93115 2022-01-01 11:31:46Z vboxsync $ */
/** @file
 * IPRT Testcase - RTStrSimplePattern.
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
#include <iprt/string.h>
#include <iprt/test.h>


int main()
{
    RTTEST hTest;
    RTEXITCODE rcExit = RTTestInitAndCreate("tstRTStrSimplePattern", &hTest);
    if (rcExit != RTEXITCODE_SUCCESS)
        return rcExit;

    RTTESTI_CHECK(RTStrSimplePatternMatch("*", ""));
    RTTESTI_CHECK(RTStrSimplePatternMatch("*", "asdfasdflkjasdlfkj"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("*?*?*?*?*", "asdfasdflkjasdlfkj"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("asdf??df", "asdfasdf"));
    RTTESTI_CHECK(!RTStrSimplePatternMatch("asdf??dq", "asdfasdf"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("asdf*df", "asdfasdf"));
    RTTESTI_CHECK(!RTStrSimplePatternMatch("asdf*dq", "asdfasdf"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("a*", "asdfasdf"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("a*f", "asdfasdf"));
    RTTESTI_CHECK(!RTStrSimplePatternMatch("a*q", "asdfasdf"));
    RTTESTI_CHECK(!RTStrSimplePatternMatch("a*q?", "asdfasdf"));
    RTTESTI_CHECK(RTStrSimplePatternMatch("?*df", "asdfasdf"));

    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", 1, "", 0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", ~(size_t)0, "", 0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", ~(size_t)0, "", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", 1, "asdfasdflkjasdlfkj", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", ~(size_t)0, "asdfasdflkjasdlfkj", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", 1, "asdfasdflkjasdlfkj", 3));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", 2, "asdfasdflkjasdlfkj", 10));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*", 15, "asdfasdflkjasdlfkj", 10));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*?*?*?*?*", 1, "asdfasdflkjasdlfkj", 128));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*?*?*?*?*", 5, "asdfasdflkjasdlfkj", 0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*?*?*?*?*", 5, "asdfasdflkjasdlfkj", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("*?*?*?*?*", ~(size_t)0, "asdfasdflkjasdlfkj", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf??df", 8, "asdfasdf", 8));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf??df", ~(size_t)0, "asdfasdf", 8));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf??df", ~(size_t)0, "asdfasdf", ~(size_t)0));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf??df", 7, "asdfasdf", 7));
    RTTESTI_CHECK(!RTStrSimplePatternNMatch("asdf??df", 7, "asdfasdf", 8));
    RTTESTI_CHECK(!RTStrSimplePatternNMatch("asdf??dq", 8, "asdfasdf", 8));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf??dq", 7, "asdfasdf", 7));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("asdf*df", 8, "asdfasdf", 8));
    RTTESTI_CHECK(!RTStrSimplePatternNMatch("asdf*dq", 8, "asdfasdf", 8));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("a*", 10, "asdfasdf", 8));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("a*f", 3, "asdfasdf", ~(size_t)0));
    RTTESTI_CHECK(!RTStrSimplePatternNMatch("a*q", 3, "asdfasdf", ~(size_t)0));
    RTTESTI_CHECK(!RTStrSimplePatternNMatch("a*q?", 4, "asdfasdf", 9));
    RTTESTI_CHECK(RTStrSimplePatternNMatch("?*df", 4, "asdfasdf", 8));

    size_t offPattern;
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a*f|a??t", ~(size_t)0, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a*f|a??t", ~(size_t)0, "asdf", 4, &offPattern));
    RTTESTI_CHECK(offPattern == 5);
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a??t|a??f", ~(size_t)0, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a??t|a??f", ~(size_t)0, "asdf", 4, &offPattern));
    RTTESTI_CHECK(offPattern == 10);
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("a*f|a??t|a??f", ~(size_t)0, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("a*f|a??t|a??f", ~(size_t)0, "asdf", 4, &offPattern));
    RTTESTI_CHECK(offPattern == 0);
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a??y|a??x", ~(size_t)0, "asdf", 4, NULL));
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a??y|a??x", ~(size_t)0, "asdf", 4, &offPattern));
    RTTESTI_CHECK(offPattern == ~(size_t)0);
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 9, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 8, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 7, "asdf", 4, NULL));
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 6, "asdf", 4, NULL));
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 5, "asdf", 4, NULL));
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 4, "asdf", 4, NULL));
    RTTESTI_CHECK(!RTStrSimplePatternMultiMatch("asdq|a*f|a??t", 3, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdf", 4, "asdf", 4, NULL));
    RTTESTI_CHECK(RTStrSimplePatternMultiMatch("asdf|", 5, "asdf", 4, NULL));


    /*
     * Summary.
     */
    return RTTestSummaryAndDestroy(hTest);
}

