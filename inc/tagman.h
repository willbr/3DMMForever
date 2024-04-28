/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*************************************************************************

    tagman.h: Tag Manager class (TagManager)

    Primary Author: ******
    Review Status: REVIEWED - any changes to this file must be reviewed!

    BASE ---> TagManager

    A TAG is a reference to a piece of content: a background, actor
    template, sound, etc.  In addition to a ChunkTag and ChunkNumber, a TAG specifies
    a SID, or source ID, that helps TagManager find the content.

    A source (identified by a SID) is a group of chunky files (managed
    by a ChunkyResourceManager) in one directory of one disk whose chunks all have unique
    ChunkTag/CNOs.  Each Socrates series member will be a source, and the
    user rolls might also be implemented as a source.  A SID of less
    than 0 is invalid; a TAG with a negative SID is an invalid TAG.

    Each source has a name, managed by _pgstSource.  This name is used
    with the _pfninscd callback when the source cannot be found.  The
    callback should put up an alert saying (for example) "The source
    *Socrates* cannot be found...please insert the CD."

    TagManager supports caching chunks to the local hard disk.  In Socrates,
    the studio should call FCacheTagToHD as soon as the chunk is
    requested by the kid, and when the tag is resolved to a BaseCacheableObject, the
    HD copy is used.  This reduces headaches about dealing with a missing
    CD all over the place.

    If a tag has ksidUseCrf for its sid, the chunk is read from the tag's
    pcrf rather than a source or a source's cache.  Use this
    functionality for things like content chunks embedded in user
    documents.

*************************************************************************/
#ifndef TAGM_H
#define TAGM_H

const long ksidInvalid = -1; // negative SIDs imply an invalid TAG
const long sidNil = 0;
const long ksidUseCrf = 0; // chunk is in ptag->pcrf

typedef struct TAG *PTAG;
struct TAG
{
#ifdef DEBUG
    // I can't use the MARKMEM macro because that makes MarkMem() virtual,
    // which changes size(TAG), which I don't want to do.
    void MarkMem(void);
#endif // DEBUG

    long sid;  // Source ID (or ksidUseCrf)
    PChunkyResourceFile pcrf; // File to look in for this chunk if sid is ksidUseCrf
    ChunkTag ctg;   // ChunkTag of chunk
    ChunkNumber cno;   // ChunkNumber of chunk
};
const ByteOrderMask kbomTag = 0xFF000000;

// FNINSCD is a client-supplied callback function to alert the user to
// insert the given CD.  The name of the source is passed to the callback.
// The function should return fTrue if the user wants to retry searching
// for the chunk, or fFalse to cancel.
typedef bool FNINSCD(PString pstnSourceTitle);
typedef FNINSCD *PFNINSCD;

enum
{
    ftagmNil = 0x0000,
    ftagmFile = 0x0001,   // for ClearCache: clear HD cache
    ftagmMemory = 0x0002, // for ClearCache: clear ChunkyResourceFile RAM cache
};

/****************************************
    Tag Manager class
****************************************/
typedef class TagManager *PTagManager;
#define TagManager_PAR BASE
#define kclsTagManager 'TAGM'
class TagManager : public TagManager_PAR
{
    RTCLASS_DEC
    MARKMEM
    ASSERT

  protected:
    Filename _fniHDRoot;     // Root HD directory to search for content
    long _cbCache;      // Size of RAM Cache on files in ChunkyResourceManager for each source
    PDynamicArray _pglsfs;        // DynamicArray of source file structs
    PStringTable_GST _pgstSource;   // String table of source descriptions
    PFNINSCD _pfninscd; // Function to call when source is not found

  protected:
    TagManager(void)
    {
    }
    bool _FFindSid(long sid, long *pistn = pvNil);
    bool _FGetStnMergedOfSid(long sid, PString pstn);
    bool _FGetStnSplitOfSid(long sid, PString pstnLong, PString pstnShort);
    bool _FRetry(long sid);
    bool _FEnsureFniCD(long sid, PFilename pfniCD, PString pstn = pvNil);
    bool _FFindFniCD(long sid, PFilename pfniCD, bool *pfFniChanged);
    bool _FDetermineIfSourceHD(long sid, bool *pfSourceIsOnHD);
    bool _FDetermineIfContentOnFni(PFilename pfni, bool *pfContentOnFni);

    bool _FGetFniHD(long sid, PFilename pfniHD);
    bool _FGetFniCD(long sid, PFilename pfniHD, bool fAskForCD);

    bool _FBuildFniHD(long sid, PFilename pfniHD, bool *pfExists);
    PChunkyResourceManager _PcrmSourceNew(long sid, PFilename pfniInfo);
    PChunkyResourceManager _PcrmSourceGet(long sid, bool fDontHitCD = fFalse);
    PChunkyFile _PcflFindTag(PTAG ptag);

  public:
    static PTagManager PtagmNew(PFilename pfniHDRoot, PFNINSCD pfninscd, long cbCache);
    ~TagManager(void);

    // GstSource stuff:
    PStringTable_GST PgstSource(void);
    bool FMergeGstSource(PStringTable_GST pgst, short bo, short osk);
    bool FAddStnSource(PString pstnMerged, long sid);
    bool FGetSid(PString pstn, long *psid); // pstn can be short or long

    bool FFindFile(long sid, PString pstn, PFilename pfni, bool fAskForCD);
    void SplitString(PString pstnMerged, PString pstnLong, PString pstnShort);

    bool FBuildChildTag(PTAG ptagPar, ChildChunkID chid, ChunkTag ctgChild, PTAG ptagChild);
    bool FCacheTagToHD(PTAG ptag, bool fCacheChildChunks = fTrue);
    PBaseCacheableObject PbacoFetch(PTAG ptag, PFNRPO pfnrpo, bool fUseCD = fFalse);
    void ClearCache(long sid = sidNil,
                    ulong grftagm = ftagmFile | ftagmMemory); // sidNil clears all caches

    // For ksidUseCrf tags:
    static bool FOpenTag(PTAG ptag, PChunkyResourceFile pcrfDest, PChunkyFile pcflSrc = pvNil);
    static bool FSaveTag(PTAG ptag, PChunkyResourceFile pcrf, bool fRedirect);
    static void DupTag(PTAG ptag); // call this when you're copying a tag
    static void CloseTag(PTAG ptag);

    static ulong FcmpCompareTags(PTAG ptag1, PTAG ptag2);
};

#endif // TAGM_H
