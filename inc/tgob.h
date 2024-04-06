/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    Lite, low-cholestoral, politically correct, ethinically and genderally
    mixed text gobs.

        TGOB 	--->   	GraphicsObject

***************************************************************************/

#ifndef TGOB_H
#define TGOB_H

#include "frame.h"

//
// Tgob class
//
#define TGOB_PAR GraphicsObject
#define kclsTGOB 'tgob'
typedef class TGOB *PTGOB;
class TGOB : public TGOB_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    long _onn;
    long _dypFont;
    String _stn;
    long _tah;
    long _tav;
    AbstractColor _acrFore;
    AbstractColor _acrBack;
    ~TGOB(void)
    {
    }

  public:
    //
    // Create and destroy functions
    //
    TGOB(PGCB pgcb);
    TGOB(long hid);

    void SetFont(long onn)
    {
        AssertThis(0);
        _onn = onn;
    }
    void SetFontSize(long dypFont)
    {
        AssertThis(0);
        _dypFont = dypFont;
    }
    void SetText(PString pstn)
    {
        AssertThis(0);
        _stn = *pstn;
        InvalRc(pvNil, kginMark);
    }
    void SetAcrFore(AbstractColor acrFore)
    {
        AssertThis(0);
        _acrFore = acrFore;
    }
    void SetAcrBack(AbstractColor acrBack)
    {
        AssertThis(0);
        _acrBack = acrBack;
    }
    void SetAlign(long tah = tahLim, long tav = tavLim);
    long GetFont(void)
    {
        AssertThis(0);
        return (_onn);
    }
    long GetFontSize(void)
    {
        AssertThis(0);
        return _dypFont;
    }
    AbstractColor GetAcrFore(void)
    {
        AssertThis(0);
        return (_acrFore);
    }
    AbstractColor GetAcrBack(void)
    {
        AssertThis(0);
        return (_acrBack);
    }
    void GetAlign(long *ptah = pvNil, long *ptav = pvNil);
    static PTGOB PtgobCreate(long kidFrm, long idsFont, long tav = tavTop, long hid = hidNil);

    virtual void Draw(PGraphicsEnvironment pgnv, RC *prcClip);
};

#endif
