/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef SITOBREN_H
#define SITOBREN_H

#include <stdio.h>
#include "util.h"
#include "soc.h"     // Needs knowledge of Socrates data structs
#include "tyModel.h" // API for SoftImage db lib
#include "dkfilter.h"
#include "tySceneT.h"
#include "mssio.h"

/* HASH_FIXED means use a fixed-length hash table; the implication is that
    the hash value will be a small number of bits, so that the table isn't
    too large.  A variable-length hash table will allow for large numbers
    of bits in the hash value. */
#define HASH_FIXED 0

/* Generic hashed database structure */
typedef struct _hshdb
{
    struct _hshdb *phshdbNext;
    uint luHash;
} HSHDB, *PHSHDB;

/* Brender Model DataBase structure */
typedef struct _bmdb
{
    HSHDB hshdb;   // hash DB header
    MODLF *pmodlf; // the file data
    long cbModlf;
    ChildChunkID chidBmdl;  // BMDL child ID
    ChunkNumber cnoBmdl;    // BMDL ChunkNumber
    char *pszName;  // name of the BMDL
    PDynamicArray pglkidCmtl; // DynamicArray of CustomMaterial_CMTL parents' CNOs
    unsigned fFixWrap : 1, fSpherical : 1;
} BMDB, *PBMDB;

/* Brender MATrix34 DataBase structure */
typedef struct _bmatdb
{
    HSHDB hshdb;
    int ixf;
} BMATDB, *PBMATDB;

/* Brender Model HieraRchy */
typedef struct _bmhr
{
    MODLF *pmodlf; // the file data
    long cbModlf;
    BMAT34 bmat34; // XF
    MTRLF mtrlf;
    PString pstnMtrlFile;
    BRUFR brufrUOffset; // Material offsets
    BRUFR brufrVOffset;
    short uMinCrop; // Material cropping
    short uMaxCrop;
    short vMinCrop;
    short vMaxCrop;
    unsigned fMtrlf : 1, fCrop : 1, fAccessory : 1, fFixWrap : 1, fSpherical : 1;
    char *pszName;
    short ibps;
    struct _bmhr *pbmhrChild;
    struct _bmhr *pbmhrSibling;
} BMHR, *PBMHR;

/* A color range */
typedef struct _crng
{
    long lwBase, lwRange;
} CRNG;

/* A CustomMaterial_CMTL descriptor */
typedef struct _cmtld
{
    ChunkNumber cno;      // the CustomMaterial_CMTL's ChunkNumber
    ChildChunkID chidCur; // the next ChildChunkID for the CustomMaterial_CMTL's children
    short ibps;   // the body part set as specified in the .hrc file
    ChildChunkID chid;    // the ChildChunkID of this CustomMaterial_CMTL
} CMTLD, *PCMTLD;

/* A TextureMap descriptor */
typedef struct _tmapd
{
    PString pstn;    // the name of the TextureMap
    long ccnoPar; // the number of Material_MTRL parents
    long xp;      // the size of the bitmap
    long yp;
} TMAPD, *PTMAPD;

enum
{
    ttActor = ttLimChlx, // TMPL initialization
    ttActionS2B,         // Define a single action for the TMPL
    ttBackgroundS2B,     // Define a background
    ttCostume,           // Define a costime for a TMPL

    ttPosition, // SoftImage POS_STATIC token
    ttInterest, // SoftImage INT_STATIC token
    ttNearCam,
    ttFarCam,
    ttFovCam,
    ttStatic, // SoftImage STATIC token

    ttCno, // Tokens for converter script cmd parms
    ttCalled,
    ttXRest,
    ttYRest,
    ttZRest,
    ttFilebase,
    ttFirst,
    ttLast,
    ttFlags,
    ttScale,
    ttSkip,
    ttSubmodel,
    ttBPS,
    ttLights,
    ttCameras,
    ttLength,
    ttStep,
    ttUseSets,
    ttNewActorPos,
    ttMaterials,
    ttFilename,

    ttFloat, // data type is floating point

    ttLimS2B
};

typedef struct _s2btk
{
    Token tok;
    double fl;
} S2BTK, *PS2BTK;

typedef class S2BLX *PS2BLX;
#define kclsS2BLX 's2bl'
#define S2BLX_PAR LexerBase
class S2BLX : public S2BLX_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    long _ttCur;
    double _fl;

  protected:
    virtual void _ReadNumTok(PToken ptok, achar ch, long lwBase, long cchMax);

  public:
    S2BLX(PFileObject pfil, bool fUnionStrings = fTrue) : S2BLX_PAR(pfil, fUnionStrings)
    {
    }
    ~S2BLX(void)
    {
    }

    virtual bool FGetTok(PToken ptok);
    bool FGetS2btk(PS2BTK ps2btk);
    void GetFni(PFilename pfni)
    {
        _pfil->GetFni(pfni);
    }
    bool FIsDigit(char ch)
    {
        return FPure(_GrfctCh(ch) & fctDec);
    }
    virtual bool FTextFromTt(long tt, PString pstn);
    bool FTextFromS2btk(PS2BTK ps2btk, PString pstn);
};

#define CnoAdd(ccno) (_cnoCur = (((_cnoPar & 0x0FFFF0000) + 0x010000) | (_cnoCur & 0x0FFFF) + (ccno)))
#define CnoNext() CnoAdd(1)

/* Some useful helper functions */
PDynamicArray PglcrngFromPal(PDynamicArray pglclr);
long LwcrngNearestBrclr(BRCLR brclr, PDynamicArray pglclr, PDynamicArray pglcrng);

#if HASH_FIXED

#define kcpbmdb 64 // must be a power of 2
#define kmskBmdb (kcpbmdb - 1)
#define kcbrgpbmdb (size(PBMDB) * kcpbmdb)
// #define an appropriate fixed-size polynomial here

#else // HASH_FIXED

#define kluCrcPoly 0xEDB88320L
#define kluHashInit 0xFFFFFFFFL

#endif // !HASH_FIXED

#define kszLight "%s-light%d.1-0.sal"
#define kszCam "%s-cam.1-%d.sac"
#define kszBmp "%s-view.1-%d.bmp"
#define kszZpic "%s-view.1-%d.Zpic"
#define kszZbmp "%s-view.1-%d.Zbmp"
#define kftgALite 'SAL'
#define kftgACam 'SAC'
#define kftgS2b 'S2B'
#define kftgCht 'CHT'
#define kftgInc 'H'
#define kftgTmapChkFile 'CPF'
#define kftgZpic 'ZPIC'
#define kftgZbmp 'ZBMP'

// Script parameter types
enum
{
    ptString = 0,
    ptLong,
    ptBRS,
    ptBRA,

    ptNil = -1
};

#define kcscrpMax 10

typedef struct _scrp
{
    long pt; // parameter type
    long tt; // token for this parm
    char szErr[80];
} SCRP, *PSCRP;

extern const SCRP rgscrpActor[];
extern const SCRP rgscrpAction[];
extern const SCRP rgscrpBackground[];

enum
{
    kmdQuiet = 0,
    kmdHelpful,
    kmdVerbose
};

enum
{
    kmdbpsNone = 0,     // ignore body part set info
    kmdbpsNumberedOnly, // keep only those nodes with body part set info
    kmdbpsAllMesh,      // keep null nodes with body part set info, and all mesh nodes
    kmdbpsAllMeshNull,  // keep all null and mesh nodes

    kmdbpsLim
};

enum
{
    fs2bNil = 0,
    kfs2bContinue = 1,
    kfs2bPreprocess = 2,
    kfs2bFixWrap = 4
};

typedef class S2B *PS2B;
#define kclsS2B 'S2B'
#define S2B_PAR BASE
class S2B : public S2B_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM
    NOCOPY(S2B)

#ifdef DEBUG
    void AssertValidBmhr(PBMHR pbmhr);
    void MarkBmhr(PBMHR pbmhr);
#endif // DEBUG

  protected:
    /* Used by script interpreter and chunk output code */
    PS2BLX _ps2blx; // used to process script file
    SourceEmitter _chse;     // used to dump chunk text to output file
    ChunkTag _ctgPar;    // ChunkTag and ChunkNumber of current parent
    ChunkNumber _cnoPar;
    ChunkNumber _cnoCur;  // Current chunk number
    String _stnT;    // tmp buf for S2B to use
    S2BTK _s2btk; // current script token
    int _iZsign;  // Z multiplier

    /* Used by TMPL-specific stuff */
    String _stnTmpl;
    String _stnActn;
    ChildChunkID _chidActn; // Next available ACTN ChildChunkID
    ChildChunkID _chidBmdl;
    ChildChunkID _chidCmtl;
    short _ibpCur; // current body part #
    PDynamicArray _pglibactPar;
    PDynamicArray _pglbs;
    PDynamicArray _pglcmtld;
    PGeneralGroup _pggcl;
    PDynamicArray _pglxf;
    PDynamicArray _pglibps; // list of body part sets to generate costumes for
    PGeneralGroup _pggcm;
    PGeneralGroup _pggtmapd; // list of TextureMap chunks used by the current actor
#if HASH_FIXED
    PBMDB *_prgpbmdb; // BMDL database
//	PBMATDB *_prgpbmatdb;	// BMAT34 database
#else             /* HASH_FIXED */
    PDynamicArray _pglpbmdb;   // BMDL database
    PDynamicArray _pglpbmatdb; // XF database
#endif            /* !HASH_FIXED */
    PBMHR _pbmhr; // BMDL hierarchy for current cel
    int _cMesh;   // count of mesh nodes for current cel
    int _cFace;   // count of all polygons (faces) for current cel
    CPS *_prgcps;

    /* Bitfields */
    /* General items */
    uint _mdVerbose : 2, _uRound : 4, _uRoundXF : 4;
    bool _fContinue : 1, _fPreprocess : 1, _fFixWrap : 1;
    /* TMPL-specific items */
    bool _fMakeGlpi : 1, _fColorOnly : 1, _fMakeCostume : 1, _fCostumeOnly : 1;
    uint _mdBPS : 2;

    /* Useful data that doesn't wind up in chunks */
    PDynamicArray _pglcrng;
    PDynamicArray _pglclr;

  protected:
    /* General script interpreter and chunk output stuff */
    bool _FDoTtActor(bool *pfHaveActor);
    bool _FDoTtActionS2B(void);
    bool _FDoTtBackgroundS2B(void);
    bool _FDoTtCostume(void);
    bool _FReadCmdline(char *szResult, bool *pfGotTok, const SCRP rgscrp[], ...);
    void _DumpHeader(ChunkTag ctg, ChunkNumber cno, PString pstnName, bool fPack);

    /* TMPL-specific stuff */
    bool _FInitGlpiCost(bool fForceCost);
    bool _FProcessModel(Model *pmodel, BMAT34 bmat34Acc, PBMHR *ppbmhr, PString pstnSubmodel = pvNil,
                        PBMHR pbmhrParent = pvNil, int cLevel = 0);
    PBMHR _PbmhrFromModel(Model *pmodel, BMAT34 *pbmat34, PBMHR *ppbmhr, PBMHR pbmhrParent, int ibps, bool fAccessory);
    BRS _BrsdwrFromModel(Model *pmodel, BRS rgbrsDwr[]);
    void _CopyVertices(DK_Vertex *vertices, void *pvDst, long cVertices);
    void _CopyFaces(DK_Polygon *polygons, void *pvDst, long cFaces, BRV rgbrv[], long cVertices);
    bool _FProcessBmhr(PBMHR *ppbmhr, short ibpPar = -1);
    bool _FEnsureOneRoot(PBMHR *ppbmhr);
    void _InitBmhr(PBMHR pbmhr);
    void _FlushTmplKids(void);
    bool _FModlfToBmdl(PMODLF pmodlf, PBMDL *ppbmdl);
    bool _FBmdlToModlf(PBMDL pbmdl, PMODLF *ppmodlf, long *pcb);
    bool _FSetCps(PBMHR pbmhr, CPS *pcps);
    bool _FChidFromModlf(PBMHR pbmhr, ChildChunkID *pchid, PBMDB *ppbmdb = pvNil);
    bool _FAddBmdlParent(PBMDB pbmdb, ChildChunkIdentification *pkid);
    bool _FInsertPhshdb(PHSHDB phshdb, PDynamicArray pglphshdb);
    bool _FIphshdbFromLuHash(uint luHash, long *piphshdb, PDynamicArray pglphshdb);
    PBMDB _PbmdbFindModlf(MODLF *pmodlf, long cbModlf, uint *pluHashList);
    void _InitCrcTable(void);
    uint _LuHashBytesNoTable(uint luHash, void *pv, long cb);
    uint _LuHashBytes(uint luHash, void *pv, long cb);
    bool _FImat34GetBmat34(BMAT34 *pbmat34, long *pimat34);
    void _DisposeBmhr(PBMHR *ppbmhr);
    bool _FDoBodyPart(PBMHR pbmhr, long ibp);
    void _ApplyBmdlXF(PBMHR pbmhr);
    void _TextureFileFromModel(Model *pmodel, PBMHR pbmhr, bool fWrapOnly = fFalse);
    bool _FTmapFromBmp(PBMHR pbmhr, ChunkNumber cnoPar, PString pstnMtrl);
    bool _FFlushTmaps(void);

    /* Background-specific stuff */
    bool _FDumpLites(int cLite, PString stnBkgd);
    bool _FDumpCameras(int cCam, PString pstnBkgd, int iPalBase, int cPal);
    bool _FBvec3Read(PS2BLX ps2blx, BVEC3 *pbvec3, PS2BTK ps2btk);
    void _Bmat34FromVec3(BVEC3 *pbvec3, BMAT34 *pbmat34);
    void _ReadLite(PString pstnLite, LightPosition *plite);
    void _ReadCam(PString pstnCam, CameraPosition *pcam, PDynamicArray *ppglapos);
    bool _FZbmpFromZpic(PString pstnBkgd, ChunkNumber cnoPar, int iCam, long dxp, long dyp, CameraPosition *pcam);

    /* Brender-knowledgable utilities */
    bool _FBrsFromS2btk(PS2BTK ps2btk, BRS *pbrs)
    {
        if (ps2btk->tok.tt == ttLong)
            *pbrs = BrIntToScalar(ps2btk->tok.lw);
        else if (ps2btk->tok.tt == ttFloat)
            *pbrs = BrFloatToScalar(ps2btk->fl);
        else
            return fFalse;
        return fTrue;
    }

    S2B(bool fSwapHand, uint mdVerbose, int iRound, int iRoundXF, achar *pszApp);
    ~S2B(void);

  public:
    static PS2B Ps2bNew(PFileObject pfilSrc, bool fSwapHand, uint mdVerbose, int iRound, int iRoundXF, char *pszApp);
    bool FConvertSI(PMSNK pmsnkErr, PMSNK pmsnkDst, PFilename pfniInc = pvNil, ulong grfs2b = fs2bNil);
};

#endif // !SITOBREN_H
