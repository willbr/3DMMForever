/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    File management.

***************************************************************************/
#ifndef FILE_H
#define FILE_H

/****************************************
    Byte order issues
****************************************/

const short kboCur = 0x0001;
const short kboOther = 0x0100;

/****************************************
    Basic types
****************************************/

typedef long FP;

enum
{
    ffilNil = 0x00,
    ffilWriteEnable = 0x01, // we can write to the file
    ffilDenyWrite = 0x02,   // others can't write to the file
    ffilDenyRead = 0x04,    // others can't read the file

    ffilTemp = 0x10,
    ffilMark = 0x20
};
const ulong kgrffilPerm = ffilWriteEnable | ffilDenyWrite | ffilDenyRead;

// file error levels - in order of severity
enum
{
    elNil,
    kelWrite,
    kelRead,
    kelSeek,
    kelCritical
};

/****************************************
    FIL class
****************************************/
typedef class FIL *PFIL;
#define FIL_PAR BaseLinkedList
#define kclsFIL 'FIL'
class FIL : public FIL_PAR
{
    RTCLASS_DEC
    BLL_DEC(FIL, PfilNext)
    ASSERT

  protected:
    // static member variables
    static Mutex _mutxList;
    static PFIL _pfilFirst;

    Mutex _mutx;

    Filename _fni;
    bool _fOpen : 1;
    bool _fEverOpen : 1;
    bool _fWrote : 1;
    ulong _grffil; // permissions, mark and temp flags
    long _el;

#ifdef MAC
    short _fref;
#elif defined(WIN)
    HANDLE _hfile;
#endif // WIN

    // private methods
    FIL(Filename *pfni, ulong grffil);
    ~FIL(void);

    bool _FOpen(bool fCreate, ulong grffil);
    void _Close(bool fFinal = fFalse);
    void _SetFpPos(FP fp);

  public:
    // public static members
    static FileType vftgCreator;

  public:
    // static methods
    static void ClearMarks(void);
    static void CloseUnmarked(void);
    static void ShutDown(void);

    // static methods returning a PFIL
    static PFIL PfilFirst(void)
    {
        return _pfilFirst;
    }
    static PFIL PfilOpen(Filename *pfni, ulong grffil = ffilDenyWrite);
    static PFIL PfilCreate(Filename *pfni, ulong grffil = ffilWriteEnable | ffilDenyWrite);
    static PFIL PfilCreateTemp(Filename *pfni = pvNil);
    static PFIL PfilFromFni(Filename *pfni);

    virtual void Release(void);
    void Mark(void)
    {
        _grffil |= ffilMark;
    }

    long ElError(void)
    {
        return _el;
    }
    void SetEl(long el = elNil)
    {
        _el = el;
    }
    bool FSetGrffil(ulong grffil, ulong grffilMask = ~0);
    ulong GrffilCur(void)
    {
        return _grffil;
    }
    void SetTemp(bool fTemp = fTrue);
    bool FTemp(void)
    {
        return FPure(_grffil & ffilTemp);
    }
    void GetFni(Filename *pfni)
    {
        *pfni = _fni;
    }
    void GetStnPath(PString pstn);

    bool FSetFpMac(FP fp);
    FP FpMac(void);
    bool FReadRgb(void *pv, long cb, FP fp);
    bool FReadRgbSeq(void *pv, long cb, FP *pfp)
    {
        AssertVarMem(pfp);
        if (!FReadRgb(pv, cb, *pfp))
            return fFalse;
        *pfp += cb;
        return fTrue;
    }
    bool FWriteRgb(void *pv, long cb, FP fp);
    bool FWriteRgbSeq(void *pv, long cb, FP *pfp)
    {
        AssertVarMem(pfp);
        if (!FWriteRgb(pv, cb, *pfp))
            return fFalse;
        *pfp += cb;
        return fTrue;
    }
    bool FSwapNames(PFIL pfil);
    bool FRename(Filename *pfni);
    bool FSetFni(Filename *pfni);
    void Flush(void);
};

/****************************************
    File Location Class
****************************************/
// for AssertValid
enum
{
    ffloNil,
    ffloReadable,
};

typedef struct FLO *PFLO;
struct FLO
{
    PFIL pfil;
    FP fp;
    long cb;

    bool FRead(void *pv)
    {
        return FReadRgb(pv, cb, 0);
    }
    bool FWrite(void *pv)
    {
        return FWriteRgb(pv, cb, 0);
    }
    bool FReadRgb(void *pv, long cbRead, FP dfp);
    bool FWriteRgb(void *pv, long cbWrite, FP dfp);
    bool FCopy(PFLO pfloDst);
    bool FReadHq(HQ *phq, long cbRead, FP dfp = 0);
    bool FWriteHq(HQ hq, FP dfp = 0);
    bool FReadHq(HQ *phq)
    {
        return FReadHq(phq, cb, 0);
    }
    bool FTranslate(short osk);

    ASSERT
};

/***************************************************************************
    Data block class - wrapper around either a flo or an hq.
***************************************************************************/
enum
{
    fblckNil = 0,
    fblckPacked = 1,
    fblckUnpacked = 2,
    fblckFile = 4,
    fblckMemory = 8,
    fblckReadable = 16,
};

typedef class DataBlock *PDataBlock;
#define DataBlock_PAR BASE
#define kclsDataBlock 'BLCK'
class DataBlock : public DataBlock_PAR
{
    RTCLASS_DEC
    NOCOPY(DataBlock)
    ASSERT
    MARKMEM

  protected:
    bool _fPacked;

    // for file based blocks
    FLO _flo;

    // for memory based blocks
    HQ _hq;
    long _ibMin;
    long _ibLim;

  public:
    DataBlock(PFLO pflo, bool fPacked = fFalse);
    DataBlock(PFIL pfil, FP fp, long cb, bool fPacked = fFalse);
    DataBlock(HQ *phq, bool fPacked = fFalse);
    DataBlock(void);
    ~DataBlock(void);

    void Set(PFLO pflo, bool fPacked = fFalse);
    void Set(PFIL pfil, FP fp, long cb, bool fPacked = fFalse);
    void SetHq(HQ *phq, bool fPacked = fFalse);
    void Free(void);
    HQ HqFree(bool fPackedOk = fFalse);
    long Cb(bool fPackedOk = fFalse);

    // changing the range of the block
    bool FMoveMin(long dib);
    bool FMoveLim(long dib);

    // create a temp block
    bool FSetTemp(long cb, bool fForceFile = fFalse);

    // reading from and writing to
    bool FRead(void *pv, bool fPackedOk = fFalse)
    {
        return FReadRgb(pv, Cb(fPackedOk), 0, fPackedOk);
    }
    bool FWrite(void *pv, bool fPackedOk = fFalse)
    {
        return FWriteRgb(pv, Cb(fPackedOk), 0, fPackedOk);
    }
    bool FReadRgb(void *pv, long cb, long ib, bool fPackedOk = fFalse);
    bool FWriteRgb(void *pv, long cb, long ib, bool fPackedOk = fFalse);
    bool FReadHq(HQ *phq, long cb, long ib, bool fPackedOk = fFalse);
    bool FWriteHq(HQ hq, long ib, bool fPackedOk = fFalse);
    bool FReadHq(HQ *phq, bool fPackedOk = fFalse)
    {
        return FReadHq(phq, Cb(fPackedOk), 0, fPackedOk);
    }

    // writing a block to a flo or another blck.
    bool FWriteToFlo(PFLO pfloDst, bool fPackedOk = fFalse);
    bool FWriteToBlck(PDataBlock pblckDst, bool fPackedOk = fFalse);
    bool FGetFlo(PFLO pflo, bool fPackedOk = fFalse);

    // packing and unpacking
    bool FPacked(long *pcfmt = pvNil);
    bool FPackData(long cfmt = cfmtNil);
    bool FUnpackData(void);

    // Amount of memory being used
    long CbMem(void);
};

/***************************************************************************
    Message sink class. Basic interface for output streaming.
***************************************************************************/
typedef class MSNK *PMSNK;
#define MSNK_PAR BASE
#define kclsMSNK 'MSNK'
class MSNK : public MSNK_PAR
{
    RTCLASS_INLINE(MSNK)

  public:
    virtual void ReportLine(PSZ psz) = 0;
    virtual void Report(PSZ psz) = 0;
    virtual bool FError(void) = 0;
};

/***************************************************************************
    File based message sink.
***************************************************************************/
typedef class MSFIL *PMSFIL;
#define MSFIL_PAR MSNK
#define kclsMSFIL 'msfl'
class MSFIL : public MSFIL_PAR
{
    ASSERT
    RTCLASS_DEC

  protected:
    bool _fError;
    PFIL _pfil;
    FP _fpCur;
    void _EnsureFile(void);

  public:
    MSFIL(PFIL pfil = pvNil);
    ~MSFIL(void);

    virtual void ReportLine(PSZ psz);
    virtual void Report(PSZ psz);
    virtual bool FError(void);

    void SetFile(PFIL pfil);
    PFIL PfilRelease(void);
};

#endif //! FILE_H
