/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    tagl.cpp: Tag list class

    Primary Author: ******
    Review Status: REVIEWED - any changes to this file must be reviewed!

    The GeneralGroup of TAGFs is maintained in sorted order.  It is sorted by sid,
    then by	ChunkTagOrType, then by ChunkNumber.

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(TagList)

/****************************************
    TAGF, or "tag-flag" struct, stores
    the tag that you want to cache and
    whether to cache its children
    automatically or not.
****************************************/
struct TAGF
{
    TAG tag;
    bool fCacheChildren;
};

/****************************************
    CC, or "chid-ctg" struct, for
    children of a tag.  An array of
    these is the variable part of the
    GeneralGroup.
****************************************/
struct CC
{
    ChildChunkID chid;
    ChunkTagOrType ctg;
};

/***************************************************************************
    Create a new TagList
***************************************************************************/
PTagList TagList::PtaglNew(void)
{
    PTagList ptagl;

    ptagl = NewObj TagList;
    if (pvNil == ptagl)
        return pvNil;
    if (!ptagl->_FInit())
    {
        ReleasePpo(&ptagl);
        return pvNil;
    }
    AssertPo(ptagl, 0);
    return ptagl;
}

/***************************************************************************
    Initialize the TagList
***************************************************************************/
bool TagList::_FInit(void)
{
    AssertBaseThis(0);

    _pggtagf = GeneralGroup::PggNew(size(TAGF));
    if (pvNil == _pggtagf)
        return fFalse;
    return fTrue;
}

/***************************************************************************
    Clean up and delete this tag list
***************************************************************************/
TagList::~TagList(void)
{
    AssertBaseThis(0);
    ReleasePpo(&_pggtagf);
}

/***************************************************************************
    Return the count of tags in the TagList
***************************************************************************/
long TagList::Ctag(void)
{
    AssertThis(0);

    return _pggtagf->IvMac();
}

/***************************************************************************
    Get the itag'th tag from the TagList
***************************************************************************/
void TagList::GetTag(long itag, PTAG ptag)
{
    AssertThis(0);
    AssertIn(itag, 0, Ctag());
    AssertVarMem(ptag);

    TAGF tagf;

    _pggtagf->GetFixed(itag, &tagf);
    *ptag = tagf.tag;
}

/***************************************************************************
    Find ptag in the TagList.  If the tag is found, the function returns
    fTrue and *pitag is the location of the tag in the GeneralGroup.  If the tag
    is not found, the function returns fFalse and *pitag is the location
    at which the tag should be inserted into the GeneralGroup to maintain correct
    sorting order in the GeneralGroup.
***************************************************************************/
bool TagList::_FFindTag(PTAG ptag, long *pitag)
{
    AssertThis(0);
    AssertVarMem(ptag);
    AssertVarMem(pitag);

    TAGF *qtagf;
    long itagfMin, itagfLim, itagf;
    long sid = ptag->sid;
    ChunkTagOrType ctg = ptag->ctg;
    ChunkNumber cno = ptag->cno;

    if (_pggtagf->IvMac() == 0)
    {
        *pitag = 0;
        return fFalse;
    }

    // Do a binary search.  The TAGFs are sorted by (sid, ctg, cno).
    for (itagfMin = 0, itagfLim = _pggtagf->IvMac(); itagfMin < itagfLim;)
    {
        itagf = (itagfMin + itagfLim) / 2;
        qtagf = (TAGF *)_pggtagf->QvFixedGet(itagf);
        if (sid < qtagf->tag.sid)
            itagfLim = itagf;
        else if (sid > qtagf->tag.sid)
            itagfMin = itagf + 1;
        else if (ctg < qtagf->tag.ctg)
            itagfLim = itagf;
        else if (ctg > qtagf->tag.ctg)
            itagfMin = itagf + 1;
        else if (cno < qtagf->tag.cno)
            itagfLim = itagf;
        else if (cno > qtagf->tag.cno)
            itagfMin = itagf + 1;
        else
        {
            *pitag = itagf;
            return fTrue;
        }
    }

    // Tag not found
    *pitag = itagfMin;
    return fFalse;
}

/***************************************************************************
    Insert the given tag into the TagList, if it isn't already in there.
***************************************************************************/
bool TagList::FInsertTag(PTAG ptag, bool fCacheChildren)
{
    AssertThis(0);
    AssertVarMem(ptag);

    long itag;
    TAGF tagf;

    if (!_FFindTag(ptag, &itag))
    {
        // Build and insert TAGF into fixed part of GeneralGroup
        tagf.tag = *ptag;
        tagf.fCacheChildren = fCacheChildren;
        if (!_pggtagf->FInsert(itag, 0, pvNil, &tagf))
            return fFalse;
        return fTrue;
    }
    // Tag is already in GeneralGroup, see if fCacheChildren needs to be updated
    _pggtagf->GetFixed(itag, &tagf);
    if (!tagf.fCacheChildren && fCacheChildren)
    {
        // FIXME(bruxisma): The compiler has correctly identified that this
        // should be an assignment.
        tagf.fCacheChildren = fTrue;
        _pggtagf->PutFixed(itag, &tagf);
    }
    return fTrue;
}

/***************************************************************************
    Insert a TAG child into the TagList
***************************************************************************/
bool TagList::FInsertChild(PTAG ptag, ChildChunkID chid, ChunkTagOrType ctg)
{
    AssertThis(0);
    AssertVarMem(ptag);

    long itagf;
    CC ccNew;
    CC *prgcc;
    long ccc; // count of CCs
    long icc;

    if (!_FFindTag(ptag, &itagf))
    {
        Bug("You should have inserted ptag first");
        return fFalse;
    }
#ifdef DEBUG
    TAGF tagf;
    _pggtagf->GetFixed(itagf, &tagf);
    if (tagf.tag.ctg != ptag->ctg || tagf.tag.cno != ptag->cno)
        Bug("_FFindTag has a bug");
#endif // DEBUG

    ccNew.chid = chid;
    ccNew.ctg = ctg;
    ccc = _pggtagf->Cb(itagf) / size(CC);
    if (ccc == 0)
    {
        if (!_pggtagf->FPut(itagf, size(CC), &ccNew))
            return fFalse;
        return fTrue;
    }
    prgcc = (CC *)_pggtagf->QvGet(itagf);
    // linear search through prgcc to find where to insert ccNew
    for (icc = 0; icc < ccc; icc++)
    {
        if (prgcc[icc].ctg > ccNew.ctg)
            break;
        if (prgcc[icc].ctg == ccNew.ctg && prgcc[icc].chid > ccNew.chid)
            break;
    }
    if (!_pggtagf->FInsertRgb(itagf, icc * size(CC), size(CC), &ccNew))
        return fFalse;
    return fTrue;
}

/***************************************************************************
    Cache all the tags and child tags in TagList
***************************************************************************/
bool TagList::FCacheTags(void)
{
    AssertThis(0);

    long itagf;
    TAGF tagf;
    long ccc; // count of CCs
    long icc;
    CC cc;
    TAG tag;

    for (itagf = 0; itagf < _pggtagf->IvMac(); itagf++)
    {
        // Cache the main tag
        _pggtagf->GetFixed(itagf, &tagf);
        if (!vptagm->FCacheTagToHD(&tagf.tag, tagf.fCacheChildren))
            return fFalse;

        // Cache the child tags
        ccc = _pggtagf->Cb(itagf) / size(CC);
        for (icc = 0; icc < ccc; icc++)
        {
            _pggtagf->GetRgb(itagf, icc * size(CC), size(CC), &cc);
            if (!vptagm->FBuildChildTag(&tagf.tag, cc.chid, cc.ctg, &tag))
                return fFalse;
            // Note that if we ever have the case where we don't always
            // want the CC tag to be cached with all its children, we could
            // change the CC structure to hold a boolean and pass it to
            // FCacheTagToHD here.
            if (!vptagm->FCacheTagToHD(&tag, fTrue))
                return fFalse;
        }
    }
    return fTrue;
}

#ifdef DEBUG
/***************************************************************************
    Assert the validity of the TagList.
***************************************************************************/
void TagList::AssertValid(ulong grf)
{
    TagList_PAR::AssertValid(fobjAllocated);
    AssertPo(_pggtagf, 0);
}

/***************************************************************************
    Mark memory used by the TagList
***************************************************************************/
void TagList::MarkMem(void)
{
    AssertThis(0);
    TagList_PAR::MarkMem();
    MarkMemObj(_pggtagf);
}
#endif // DEBUG
