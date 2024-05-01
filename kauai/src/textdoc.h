/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    For editing a text file or text stream as a document.  Unlike the edit
    controls in text.h/text.cpp, all the text need not be in memory (this
    uses a FileByteStream) and there can be multiple views on the same text.

***************************************************************************/
#ifndef TEXTDOC_H
#define TEXTDOC_H

/***************************************************************************
    Text document.  A doc wrapper for a FileByteStream.
***************************************************************************/
typedef class TXDC *PTXDC;
#define TXDC_PAR DocumentBase
#define kclsTXDC 'TXDC'
class TXDC : public TXDC_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    PFileByteStream _pbsf;
    PFileObject _pfil;

    TXDC(PDocumentBase pdocb = pvNil, ulong grfdoc = fdocNil);
    ~TXDC(void);
    bool _FInit(PFilename pfni = pvNil, PFileByteStream pbsf = pvNil);

  public:
    static PTXDC PtxdcNew(PFilename pfni = pvNil, PFileByteStream pbsf = pvNil, PDocumentBase pdocb = pvNil, ulong grfdoc = fdocNil);

    PFileByteStream Pbsf(void)
    {
        return _pbsf;
    }

    virtual PDocumentDisplayGraphicsObject PddgNew(PGraphicsObjectBlock pgcb);
    virtual bool FGetFni(Filename *pfni);
    virtual bool FSaveToFni(Filename *pfni, bool fSetFni);
};

/***************************************************************************
    Text document display GraphicsObject - DocumentDisplayGraphicsObject for a TXDC.
***************************************************************************/
const long kcchMaxLine = 512;
const long kdxpIndentTxdd = 5;

typedef class TXDD *PTXDD;
#define TXDD_PAR DocumentDisplayGraphicsObject
#define kclsTXDD 'TXDD'
class TXDD : public TXDD_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    PFileByteStream _pbsf;
    long _clnDisp;
    long _clnDispWhole;
    PDynamicArray _pglichStarts;

    // the selection
    long _ichAnchor;
    long _ichOther;
    bool _fSelOn : 1;
    ulong _tsSel;
    long _xpSel;
    bool _fXpValid;

    // the font
    long _onn;
    ulong _grfont;
    long _dypFont;
    long _dypLine;
    long _dxpTab;

    // the cache
    achar _rgchCache[kcchMaxLine];
    long _ichMinCache;
    long _ichLimCache;

    TXDD(PDocumentBase pdocb, PGraphicsObjectBlock pgcb, PFileByteStream pbsf, long onn, ulong grfont, long dypFont);
    ~TXDD(void);
    virtual bool _FInit(void);
    virtual void _NewRc(void);
    virtual void _Activate(bool fActive);

    void _Reformat(long lnMin, long *pclnIns = pvNil, long *pclnDel = pvNil);
    void _ReformatEdit(long ichMinEdit, long cchIns, long cchDel, long *pln, long *pclnIns = pvNil,
                       long *pclnDel = pvNil);
    bool _FFetchCh(long ich, achar *pch);
    void _FetchLineLn(long ln, achar *prgch, long cchMax, long *pcch, long *pichMin = pvNil);
    void _FetchLineIch(long ich, achar *prgch, long cchMax, long *pcch, long *pichMin = pvNil);
    bool _FFindNextLineStart(long ich, long *pich, achar *prgch = pvNil, long cchMax = 0);
    bool _FFindLineStart(long ich, long *pich);
    bool _FFindNextLineStartCached(long ich, long *pich, achar *prgch = pvNil, long cchMax = 0);
    bool _FFindLineStartCached(long ich, long *pich);
    void _DrawLine(PGraphicsEnvironment pgnv, RC *prcClip, long yp, achar *prgch, long cch);
    void _SwitchSel(bool fOn, bool fDraw);
    void _InvertSel(PGraphicsEnvironment pgnv, bool fDraw);
    void _InvertIchRange(PGraphicsEnvironment pgnv, long ich1, long ich2, bool fDraw);
    long _LnFromIch(long ich);
    long _IchMinLn(long ln);
    long _XpFromLnIch(PGraphicsEnvironment pgnv, long ln, long ich);
    long _XpFromIch(long ich);
    long _XpFromRgch(PGraphicsEnvironment pgnv, achar *prgch, long cch);
    long _IchFromLnXp(long ln, long xp);
    long _IchFromIchXp(long ich, long xp);
    long _IchFromRgchXp(achar *prgch, long cch, long ichMinLine, long xp);

    long *_QichLn(long ln)
    {
        return (long *)_pglichStarts->QvGet(ln);
    }

    void _InvalAllTxdd(long ich, long cchIns, long cchDel);
    void _InvalIch(long ich, long cchIns, long cchDel);

    // scrolling support
    virtual long _ScvMax(bool fVert);
    virtual void _Scroll(long scaHorz, long scaVert, long scvHorz = 0, long scvVert = 0);

    // clipboard support
    virtual bool _FCopySel(PDocumentBase *ppdocb = pvNil);
    virtual void _ClearSel(void);
    virtual bool _FPaste(PClipboardObject pclip, bool fDoIt, long cid);

  public:
    static PTXDD PtxddNew(PDocumentBase pdocb, PGraphicsObjectBlock pgcb, PFileByteStream pbsf, long onn, ulong grfont, long dypFont);

    virtual void Draw(PGraphicsEnvironment pgnv, RC *prcClip);
    virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
    virtual bool FCmdKey(PCMD_KEY pcmd);
    virtual bool FCmdSelIdle(PCommand pcmd);

    void SetSel(long ichAnchor, long ichOther, bool fDraw);
    void ShowSel(bool fDraw);
    bool FReplace(achar *prgch, long cch, long ich1, long ich2, bool fDraw);
};

#endif //! TEXTDOC_H
