/*
 * egman.c
 *
 * SOFTWARE RIGHTS
 *
 * We reserve no LEGAL rights to the Purdue Compiler Construction Tool
 * Set (PCCTS) -- PCCTS is in the public domain.  An individual or
 * company may do whatever they wish with source code distributed with
 * PCCTS or the code generated by PCCTS, including the incorporation of
 * PCCTS, or its output, into commerical software.
 *
 * We encourage users to develop software with PCCTS.  However, we do ask
 * that credit is given to us for developing PCCTS.  By "credit",
 * we mean that if you incorporate our source code into one of your
 * programs (commercial product, research project, or otherwise) that you
 * acknowledge this fact somewhere in the documentation, research report,
 * etc...  If you like PCCTS and have developed a nice tool with the
 * output, please mention that you developed it using PCCTS.  In
 * addition, we ask that this header remain intact in our source code.
 * As long as these guidelines are kept, we expect to continue enhancing
 * this system and expect to make other tools available as they are
 * completed.
 *
 * ANTLR 1.33MR10
 * 2001
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "set.h"
#include "syn.h"
#include "hash.h"
#include "generic.h"
#include "proto.h"

static ExceptionGroup **egArray=NULL;   /* ExceptionGroup by BlkLevel */
static LabelEntry     **leArray=NULL;   /* LabelEntry by BlkLevel     */
static Junction       **altArray=NULL;  /* start of alternates        */
static int              arraySize=0;
static int              highWater=0;
static ExceptionGroup *lastEG=NULL;     /* used in altFixup()         */
static int             lastBlkLevel=0;  /* used in altFixup()         */

#ifdef __USE_PROTOS
static void arrayCheck(void);
#else
static void arrayCheck();
#endif

/* Called to add an exception group for an alternative EG */

#ifdef __USE_PROTOS
void egAdd(ExceptionGroup * eg)
#else
void egAdd(eg)
ExceptionGroup *eg;
#endif
{
  int               i;

  ExceptionGroup    *nextEG;
  ExceptionGroup    *innerEG;

  LabelEntry        *nextLE;
  LabelEntry        *innerLE;

  Junction          *nextAlt;
  Junction          *innerAlt;

  lastEG=eg;
  lastBlkLevel=BlkLevel;

  arrayCheck();
  eg->pendingLink=egArray[BlkLevel];
  egArray[BlkLevel]=eg;

  /* EG for alternates already have their altID filled in      */

  for (i=BlkLevel+1; i<=highWater ; i++) {
    for (innerEG=egArray[i]; innerEG != NULL ; innerEG=nextEG) {
      nextEG=innerEG->pendingLink;
      innerEG->pendingLink=NULL;
      innerEG->outerEG=eg;
    };
    egArray[i]=NULL;
  };

  /*
   *  for patching up the LabelEntry you might use an EG for the
   *  current alternative - unlike patching up an alternative EG
   *    i.e. start the loop at BlkLevel rather than (BlkLevel+1)
   *  fill it in only if the EG and the LE are for the very
   *    same alternative if they're at the same BlkLevel
   *  it's easier to leave the LE on this list (filled in) rather than
   *    trying to selectively remove it.  It will eventually be
   *    removed anyway when the BlkLevel gets small enough.
   */

  for (i=BlkLevel; i<=highWater ; i++) {
    for (innerLE=leArray[i]; innerLE != NULL ; innerLE=nextLE) {
      nextLE=innerLE->pendingLink;
      if (BlkLevel != i ||
        innerLE->curAltNum == CurAltNum_array[BlkLevel]) {
        if (innerLE->outerEG == NULL) {
          innerLE->outerEG=eg;
        };
      };
    };
    if (BlkLevel != i) leArray[i]=NULL;
  };

/*
 * For the start of alternatives it is necessary to make a
 * distinction between the exception group for the current
 * alternative and the "fallback" EG for the block which
 * contains the alternative
 *
 * The fallback outerEG is used to handle the case where
 * no alternative of a block matches.  In that case the
 * signal is "NoViableAlt" (or "NoSemViableAlt" and the
 * generator needs the EG of the block CONTAINING the
 * current one.
 *
 *      rule: ( ( ( a
 *                | b
 *                )
 *              | c
 *              )
 *            | d
 *            );
 */

  for (i=BlkLevel; i <= highWater ; i++) {
    for (innerAlt=altArray[i]; innerAlt != NULL ; innerAlt=nextAlt) {
      nextAlt=innerAlt->pendingLink;

      /*  first fill in the EG for the current alternative         */
      /*  but leave it on the list in order to get the fallback EG */
      /*  if the EG is at the same LEVEL as the alternative then   */
      /*    fill it in only if in the very same alternative        */
      /*                                                           */
      /*        rule: ( a                                          */
      /*              | b                                          */
      /*              | c  exception ...                           */
      /*              )                                            */
      /*                                                           */
      /*  if the EG is outside the alternative (e.g. BlkLevel < i) */
      /*    then it doesn't matter about the alternative           */
      /*                                                           */
      /*        rule: ( a                                          */
      /*              | b                                          */
      /*              | c                                          */
      /*              )   exception ...                            */
      /*                                                           */

#if 0
      printf("BlkLevel=%d i=%d altnum=%d CurAltNum=%d altID=%s\n",
        BlkLevel,i,innerAlt->curAltNum,CurAltNum_array[BlkLevel],eg->altID);
#endif
      if (BlkLevel != i ||
          innerAlt->curAltNum == CurAltNum_array[BlkLevel]) {
        if (innerAlt->exception_label == NULL) {
          innerAlt->exception_label=eg->altID;
        };
      };

      /*  occurs at a later pass then for the exception_label      */
      /*  if an outerEG has been found then fill in the outer EG   */
      /*  remove if from the list when the BlkLevel gets smaller   */

      if (BlkLevel != i) {
        if (innerAlt->outerEG == NULL) {
          innerAlt->outerEG=eg;
        };
      };
    };
    if (BlkLevel != i) altArray[i]=NULL;
  };
}

#ifdef __USE_PROTOS
void leAdd(LabelEntry * le)
#else
void leAdd(le)
LabelEntry *le;
#endif

{
  arrayCheck();
  le->pendingLink=leArray[BlkLevel];
  le->curAltNum=CurAltNum_array[BlkLevel];
  leArray[BlkLevel]=le;
}

#ifdef __USE_PROTOS
void altAdd(Junction *alt)
#else
void altAdd(alt)
Junction *alt;
#endif

{
  arrayCheck();
#if 0
  printf("BlkLevel=%d CurAltNum=%d\n",
            BlkLevel,CurAltNum_array[BlkLevel]);
#endif
  alt->curAltNum=CurAltNum_array[BlkLevel];
  alt->pendingLink=altArray[BlkLevel];
  altArray[BlkLevel]=alt;
}

static void
#ifdef __USE_PROTOS
arrayCheck(void)
#else
arrayCheck()
#endif
{
  ExceptionGroup    **egArrayNew;
  LabelEntry        **leArrayNew;
  Junction          **altArrayNew;
  int               arraySizeNew;
  int               i;

  if (BlkLevel > highWater) highWater=BlkLevel;

  if (BlkLevel >= arraySize) {
    arraySizeNew=BlkLevel+5;	/* MR20 */
    egArrayNew=(ExceptionGroup **)
        calloc(arraySizeNew,sizeof(ExceptionGroup *));
    leArrayNew=(LabelEntry **)
        calloc(arraySizeNew,sizeof(LabelEntry *));
    altArrayNew=(Junction **)
        calloc(arraySizeNew,sizeof(Junction *));
    for (i=0; i<arraySize ; i++) {
      egArrayNew[i]=egArray[i];
      leArrayNew[i]=leArray[i];
      altArrayNew[i]=altArray[i];
    };
    arraySize=arraySizeNew;
    if (egArray != NULL) free( (char *) egArray);
    if (leArray != NULL) free( (char *) leArray);
    if (altArray != NULL) free( (char *) altArray);
    egArray=egArrayNew;
    leArray=leArrayNew;
    altArray=altArrayNew;
  };
}

/* always call leFixup() BEFORE egFixup() */

void
#ifdef __USE_PROTOS
egFixup(void)
#else
egFixup()
#endif
{
  int               i;
  ExceptionGroup    *nextEG;
  ExceptionGroup    *innerEG;

  for (i=1; i<=highWater ; i++) {
    for (innerEG=egArray[i]; innerEG != NULL ; innerEG=nextEG) {
      nextEG=innerEG->pendingLink;
      innerEG->pendingLink=NULL;
    };
    egArray[i]=NULL;
  };
  lastEG=NULL;
  lastBlkLevel=0;
}

/* always call leFixup() BEFORE egFixup() */

#ifdef __USE_PROTOS
void leFixup(void)
#else
void leFixup()
#endif
{

  int               i;
  LabelEntry        *nextLE;
  LabelEntry        *innerLE;

  for (i=BlkLevel; i<=highWater ; i++) {
    for (innerLE=leArray[i]; innerLE != NULL ; innerLE=nextLE) {
      nextLE=innerLE->pendingLink;
      innerLE->pendingLink=NULL;
    };
    leArray[i]=NULL;
  };
}

/* always call altFixup() BEFORE egFixup() */

#ifdef __USE_PROTOS
void altFixup(void)
#else
void altFixup()
#endif
{

  int               i;
  Junction          *nextAlt;
  Junction          *innerAlt;

  for (i=BlkLevel; i<=highWater ; i++) {
    for (innerAlt=altArray[i]; innerAlt != NULL ; innerAlt=nextAlt) {

      /*  if an outerEG has been found then fill in the outer EG   */

      if (lastBlkLevel <= i) {
        if (innerAlt->outerEG == NULL) {
          innerAlt->outerEG=lastEG;
        };
      };
      nextAlt=innerAlt->pendingLink;
      innerAlt->pendingLink=NULL;
    };
    altArray[i]=NULL;
  };
}

