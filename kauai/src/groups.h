/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Basic collection classes:
        General List (DynamicArray), Allocated List (AllocatedArray),
        General Group (GG), Allocated Group (AG),
        General String Table (StringTable), Allocated String Table (AllocatedStringTable).

        BASE ---> GRPB -+-> VirtualArray -+-> DynamicArray
                        |        +-> AllocatedArray
                        |
                        +-> VirtualGroup -+-> GG
                        |        +-> AG
                        |
                        +-> VirtualStringTable-+-> StringTable
                                 +-> AllocatedStringTable

***************************************************************************/
#ifndef GROUPS_H
#define GROUPS_H

namespace Group {

enum
{
    fgrpNil = 0,
    fgrpShrink = 1,
};

/****************************************
    GRPB is a virtual class supporting
    all group classes
****************************************/
#define GRPB_PAR BASE
#define kclsGRPB 'GRPB'
class GRPB : public GRPB_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  private:
    long _cb1;
    long _cb2;
    HQ _hqData1;
    HQ _hqData2;

    bool _FEnsureHqCb(HQ *phq, long cb, long cbMinGrow, long *pcb);

  protected:
    long _cbMinGrow1;
    long _cbMinGrow2;
    long _ivMac;

    byte *_Qb1(long ib)
    {
        return (byte *)QvFromHq(_hqData1) + ib;
    }
    byte *_Qb2(long ib)
    {
        return (byte *)QvFromHq(_hqData2) + ib;
    }
    long _Cb1(void)
    {
        return _cb1;
    }
    long _Cb2(void)
    {
        return _cb2;
    }
    bool _FEnsureSizes(long cbMin1, long cbMin2, ulong grfgrp);
    bool _FWrite(PDataBlock pblck, void *pv, long cb, long cb1, long cb2);
    bool _FReadData(PDataBlock pblck, long ib, long cb1, long cb2);
    bool _FDup(PGRPB pgrpbDst, long cb1, long cb2);

    GRPB(void)
    {
    }

  public:
    ~GRPB(void);

    void Lock(void)
    {
        if (_hqData1 != hqNil)
            PvLockHq(_hqData1);
    }
    void Unlock(void)
    {
        if (_hqData1 != hqNil)
            UnlockHq(_hqData1);
    }
    long IvMac(void)
    {
        return _ivMac;
    }
    virtual bool FFree(long iv) = 0;
    virtual void Delete(long iv) = 0;

    // writing
    virtual bool FWriteFlo(PFLO pflo, short bo = kboCur, short osk = koskCur);
    virtual bool FWrite(PDataBlock pblck, short bo = kboCur, short osk = koskCur) = 0;
    virtual long CbOnFile(void) = 0;
};

/****************************************
    VirtualArray is a virtual class supporting
    DynamicArray and AllocatedArray
****************************************/
#define VirtualArray_PAR GRPB
#define kclsVirtualArray 'GLB'
class VirtualArray : public VirtualArray_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    long _cbEntry;

    VirtualArray(long cb);

  public:
    long CbEntry(void)
    {
        return _cbEntry;
    }
    void *QvGet(long iv);
    void Get(long iv, void *pv);
    void Put(long iv, void *pv);
    void *PvLock(long iv);
    void SetMinGrow(long cvAdd);

    virtual bool FAdd(void *pv, long *piv = pvNil) = 0;
};

/****************************************
    DynamicArray is the basic dynamic array
****************************************/
#define DynamicArray_PAR VirtualArray
#define kclsDynamicArray 'GL'
class DynamicArray : public DynamicArray_PAR
{
    RTCLASS_DEC

  protected:
    DynamicArray(long cb);
    bool _FRead(PDataBlock pblck, short *pbo, short *posk);

  public:
    // static methods
    static PDynamicArray PglNew(long cb, long cvInit = 0);
    static PDynamicArray PglRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PDynamicArray PglRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PDynamicArray PglDup(void);

    // methods required by parent class
    virtual bool FAdd(void *pv, long *piv = pvNil);
    virtual void Delete(long iv);
    virtual bool FWrite(PDataBlock pblck, short bo = kboCur, short osk = koskCur);
    virtual long CbOnFile(void);
    virtual bool FFree(long iv);

    // new methods
    void Delete(long iv, long cv);
    bool FInsert(long iv, void *pv = pvNil, long cv = 1);
    bool FSetIvMac(long ivMacNew);
    bool FEnsureSpace(long cvAdd, ulong grfgrp = fgrpNil);
    void Move(long ivSrc, long ivTarget);
    bool FPush(void *pv)
    {
        return FInsert(_ivMac, pv);
    }
    bool FPop(void *pv = pvNil);
    bool FEnqueue(void *pv)
    {
        return FInsert(0, pv);
    }
    bool FDequeue(void *pv = pvNil)
    {
        return FPop(pv);
    }
};

/****************************************
    Allocated (fixed index) list class
****************************************/
#define AllocatedArray_PAR VirtualArray
#define kclsAllocatedArray 'AL'
class AllocatedArray : public AllocatedArray_PAR
{
    RTCLASS_DEC
    ASSERT

  private:
    long _cvFree;

  private:
    // section 2 of the data contains a bit array
    byte *_Qgrfbit(long iv)
    {
        return _Qb2(IbFromIbit(iv));
    }

  protected:
    AllocatedArray(long cb);
    bool _FRead(PDataBlock pblck, short *pbo, short *posk);

  public:
    // static methods
    static PAllocatedArray PalNew(long cb, long cvInit = 0);
    static PAllocatedArray PalRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PAllocatedArray PalRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PAllocatedArray PalDup(void);

    // methods required by parent class
    virtual bool FAdd(void *pv, long *piv = pvNil);
    virtual void Delete(long iv);
    virtual bool FWrite(PDataBlock pblck, short bo = kboCur, short osk = koskCur);
    virtual long CbOnFile(void);
    virtual bool FFree(long iv);

    // new methods
    bool FEnsureSpace(long cvAdd, ulong grfgrp = fgrpNil);
    void DeleteAll(void);
};

/****************************************
    VirtualGroup is a virtual class supporting
    GG and AG
****************************************/
const ByteOrderMask kbomLoc = 0xF0000000;
#define VirtualGroup_PAR GRPB
#define kclsVirtualGroup 'GGB'
class VirtualGroup : public VirtualGroup_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    struct LOC
    {
        long bv;
        long cb;
    };

    long _bvMac;
    long _clocFree;
    long _cbFixed;

  protected:
    VirtualGroup(long cbFixed, bool fAllowFree);

    void _RemoveRgb(long bv, long cb);
    void _AdjustLocs(long bvMin, long bvLim, long dcb);
    LOC *_Qloc(long iloc)
    {
        return (LOC *)_Qb2(LwMul(iloc, size(LOC)));
    }
    bool _FRead(PDataBlock pblck, short *pbo, short *posk);

    bool _FDup(PVirtualGroup pggbDst);

  public:
    // methods required by parent class
    virtual bool FWrite(PDataBlock pblck, short bo = kboCur, short osk = koskCur);
    virtual long CbOnFile(void);
    virtual bool FFree(long iv);

    bool FEnsureSpace(long cvAdd, long cbAdd, ulong grfgrp = fgrpNil);
    void SetMinGrow(long cvAdd, long cbAdd);

    virtual bool FAdd(long cb, long *piv = pvNil, void *pv = pvNil, void *pvFixed = pvNil) = 0;

    // access to the fixed portion
    long CbFixed(void)
    {
        return _cbFixed;
    }
    void *QvFixedGet(long iv, long *pcbVar = pvNil);
    void *PvFixedLock(long iv, long *pcbVar = pvNil);
    void GetFixed(long iv, void *pv);
    void PutFixed(long iv, void *pv);

    // access to the variable portion
    long Cb(long iv);
    void *QvGet(long iv, long *pcb = pvNil);
    void *PvLock(long iv, long *pcb = pvNil);
    void Get(long iv, void *pv);
    void Put(long iv, void *pv);
    bool FPut(long iv, long cb, void *pv);
    void GetRgb(long iv, long bv, long cb, void *pv);
    void PutRgb(long iv, long bv, long cb, void *pv);
    void DeleteRgb(long iv, long bv, long cb);
    bool FInsertRgb(long iv, long bv, long cb, void *pv);
    bool FMoveRgb(long ivSrc, long bvSrc, long ivDst, long bvDst, long cb);
    void Merge(long ivSrc, long ivDst);
};

/****************************************
    General Group - based on VirtualGroup
****************************************/
#define GG_PAR VirtualGroup
#define kclsGG 'GG'
class GG : public GG_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    GG(long cbFixed) : VirtualGroup(cbFixed, fFalse)
    {
    }

  public:
    // static methods
    static PGG PggNew(long cbFixed = 0, long cvInit = 0, long cbInit = 0);
    static PGG PggRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PGG PggRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PGG PggDup(void);

    // methods required by parent class
    virtual bool FAdd(long cb, long *piv = pvNil, void *pv = pvNil, void *pvFixed = pvNil);
    virtual void Delete(long iv);

    // new methods
    bool FInsert(long iv, long cb, void *pv = pvNil, void *pvFixed = pvNil);
    bool FCopyEntries(PGG pggSrc, long ivSrc, long ivDst, long cv);
    void Move(long ivSrc, long ivTarget);
    void Swap(long iv1, long iv2);
};

/****************************************
    Allocated Group - based on VirtualGroup
****************************************/
#define AG_PAR VirtualGroup
#define kclsAG 'AG'
class AG : public AG_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    AG(long cbFixed) : VirtualGroup(cbFixed, fTrue)
    {
    }

  public:
    // static methods
    static PAG PagNew(long cbFixed = 0, long cvInit = 0, long cbInit = 0);
    static PAG PagRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PAG PagRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PAG PagDup(void);

    // methods required by parent class
    virtual bool FAdd(long cb, long *piv = pvNil, void *pv = pvNil, void *pvFixed = pvNil);
    virtual void Delete(long iv);
};

/****************************************
    String table classes
****************************************/
enum
{
    fgstNil = 0,
    fgstSorted = 1,
    fgstUserSorted = 2,
    fgstAllowFree = 4,
};

const long kcchMaxGst = kcchMaxStn;

/****************************************
    VirtualStringTable is a virtual class supporting
    StringTable and AllocatedStringTable.
****************************************/
#define VirtualStringTable_PAR GRPB
#define kclsVirtualStringTable 'GSTB'
class VirtualStringTable : public VirtualStringTable_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    long _cbEntry;
    long _bstMac;
    long _cbstFree; // this is cvNil for non-allocated GSTBs

  protected:
    VirtualStringTable(long cbExtra, ulong grfgst);

    long _Bst(long ibst)
    {
        return *(long *)_Qb2(LwMul(ibst, _cbEntry));
    }
    long *_Qbst(long ibst)
    {
        return (long *)_Qb2(LwMul(ibst, _cbEntry));
    }
    PST _Qst(long ibst);
    void _RemoveSt(long bst);
    void _AppendRgch(achar *prgch, long cch);
    void _SwapBytesRgbst(void);
    void _TranslateGrst(short osk, bool fToCur);
    bool _FTranslateGrst(short osk);
    bool _FRead(PDataBlock pblck, short *pbo, short *posk);

    bool _FDup(PVirtualStringTable pgstbDst);

  public:
    // methods required by parent class
    virtual bool FWrite(PDataBlock pblck, short bo = kboCur, short osk = koskCur);
    virtual long CbOnFile(void);
    virtual bool FFree(long istn);

    bool FEnsureSpace(long cstnAdd, long cchAdd, ulong grfgrp = fgrpNil);
    void SetMinGrow(long cstnAdd, long cchAdd);

    virtual bool FAddRgch(achar *prgch, long cch, void *pvExtra = pvNil, long *pistn = pvNil) = 0;
    virtual bool FFindRgch(achar *prgch, long cch, long *pistn, ulong grfgst = fgstNil);

    long IstnMac(void)
    {
        return _ivMac;
    }
    long CbExtra(void)
    {
        return _cbEntry - size(long);
    }

    bool FAddStn(PSTN pstn, void *pvExtra = pvNil, long *pistn = pvNil);
    bool FPutRgch(long istn, achar *prgch, long cch);
    bool FPutStn(long istn, PSTN pstn);
    void GetRgch(long istn, achar *prgch, long cchMax, long *pcch);
    void GetStn(long istn, PSTN pstn);
    bool FFindStn(PSTN pstn, long *pistn, ulong grfgst = fgstNil);

    void GetExtra(long istn, void *pv);
    void PutExtra(long istn, void *pv);
    bool FFindExtra(void *prgbFind, PSTN pstn = pvNil, long *pistn = pvNil);
};

/****************************************
    String table
****************************************/
#define StringTable_PAR VirtualStringTable
#define kclsStringTable 'GST'
class StringTable : public StringTable_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    StringTable(long cbExtra) : VirtualStringTable(cbExtra, fgstNil)
    {
    }

  public:
    // static methods
    static PStringTable PgstNew(long cbExtra = 0, long cstnInit = 0, long cchInit = 0);
    static PStringTable PgstRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PStringTable PgstRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PStringTable PgstDup(void);

    // methods required by parent class
    virtual bool FAddRgch(achar *prgch, long cch, void *pvExtra = pvNil, long *pistn = pvNil);
    virtual bool FFindRgch(achar *prgch, long cch, long *pistn, ulong grfgst = fgstNil);
    virtual void Delete(long istn);

    // new methods
    bool FInsertRgch(long istn, achar *prgch, long cch, void *pvExtra = pvNil);
    bool FInsertStn(long istn, PSTN pstn, void *pvExtra = pvNil);
    void Move(long istnSrc, long istnDst);
};

/****************************************
    Allocated string table
****************************************/
#define AllocatedStringTable_PAR VirtualStringTable
#define kclsAllocatedStringTable 'AST'
class AllocatedStringTable : public AllocatedStringTable_PAR
{
    RTCLASS_DEC
    ASSERT

  protected:
    AllocatedStringTable(long cbExtra) : VirtualStringTable(cbExtra, fgstAllowFree)
    {
    }

  public:
    // static methods
    static PAllocatedStringTable PastNew(long cbExtra = 0, long cstnInit = 0, long cchInit = 0);
    static PAllocatedStringTable PastRead(PDataBlock pblck, short *pbo = pvNil, short *posk = pvNil);
    static PAllocatedStringTable PastRead(PFIL pfil, FP fp, long cb, short *pbo = pvNil, short *posk = pvNil);

    // duplication
    PAllocatedStringTable PastDup(void);

    // methods required by parent class
    virtual bool FAddRgch(achar *prgch, long cch, void *pvExtra = pvNil, long *pistn = pvNil);
    virtual void Delete(long istn);
};

#endif //! GROUPS_H

} // end of namespace Group
