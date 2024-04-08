/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/******************************************************************************
    Author: ******
    Project: Socrates
    Review Status: Reviewed

    Include file for the scene sorter class

************************************************************ PETED ***********/

#ifndef SCNSORT_H
#define SCNSORT_H

#define SCRT_PAR KidspaceGraphicObject
#define kclsSCRT 'SCRT'
typedef class SCRT *PSCRT;
class SCRT : public SCRT_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM
    CMD_MAP_DEC(SCRT)

  protected:
    static const TRANS _mplwtrans[];

    /* Obtained from the script */
    long _kidFrameMin;  // kid of first frame KidspaceGraphicObject in the easel
    long _kidScbtnsMin; // kid of the first scroll button (scroll up)
    long _cfrmPage;     // number of frame GOKs on the easel
    long _cgokFrame;    // number of pieces to a frame KidspaceGraphicObject

    /* Hidden from the script */
    long _iscenCur;   // currently selected scene
    long _iscenTop;   // first scene visible in the browser
    long _iscenMac;   // number of scenes
    PMovie _pmvie;     // pointer to movie we're editing
    CMVI _cmvi;       // Composite movie
    bool _fError : 1, // Did an error occur during the easel?
        _fInited : 1; // Have I seen the cidSceneSortInit yet?
    PStudio _pstdio;   // The Studio that instantiated me

  protected:
    long _IscenFromKid(long kid)
    {
        AssertIn(kid, _kidFrameMin, _kidFrameMin + _cfrmPage * _cgokFrame + 1);
        return LwMin(_iscenMac, (_iscenTop + (kid - _kidFrameMin) / _cgokFrame));
    }
    long _KidFromIscen(long iscen)
    {
        AssertIn(iscen, _iscenTop, _iscenTop + _cfrmPage);
        return (_kidFrameMin + (iscen - _iscenTop) * _cgokFrame);
    }
    void _EnableScroll(void);
    void _SetSelectionVis(bool fShow, bool fHideSel = fFalse);
    void _ErrorExit(void);
    bool _FResetThumbnails(bool fHideSel);
    bool _FResetTransition(PKidspaceGraphicObject pgokPar, TRANS trans);
    TRANS _TransFromLw(long lwTrans);
    long _LwFromTrans(TRANS trans);

  public:
    SCRT(PGCB pgcb);
    ~SCRT(void);

    static PSCRT PscrtNew(long hid, PMovie pmvie, PStudio pstdio, PResourceCache prca);
    static bool FSceneSortMovie(long hid, PMovie pmvie);

    /* Command API */
    bool FCmdInit(PCommand pcmd);
    bool FCmdSelect(PCommand pcmd);
    bool FCmdInsert(PCommand pcmd);
    bool FCmdScroll(PCommand pcmd);
    bool FCmdNuke(PCommand pcmd);
    bool FCmdDismiss(PCommand pcmd);
    bool FCmdPortfolio(PCommand pcmd);
    bool FCmdTransition(PCommand pcmd);
};

/******************************************************************************

    GOMP class -- wraps an MaskedBitmapMBMP in a GraphicsObject for display in the Scene Sorter

************************************************************ PETED ***********/

#define GOMP_PAR GraphicsObject
#define kclsGOMP 'GOMP'
typedef class GOMP *PGOMP;
class GOMP : public GOMP_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM
    /* REVIEW peted: do I need to declare a command map? */

  protected:
    PMaskedBitmapMBMP _pmbmp;

  public:
    GOMP(PGCB pgcb);
    ~GOMP(void)
    {
        AssertThis(0);
        ReleasePpo(&_pmbmp);
    }

    static PGOMP PgompNew(PGraphicsObject pgobPar, long hid);
    static PGOMP PgompFromHidScr(long hid);
    bool FSetMbmp(PMaskedBitmapMBMP pmbmp);

    /* Makes the GOMP invisible to mouse actions */
    virtual bool FPtIn(long xp, long yp)
    {
        return fFalse;
    }
    virtual bool FPtInBounds(long xp, long yp)
    {
        return fFalse;
    }

    virtual void Draw(PGraphicsEnvironment pgnv, RC *prcClip);
};

#endif /* SCNSORT_H */
