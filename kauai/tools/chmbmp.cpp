/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/****************************************************************************

    DOCMBMP methods.

****************************************************************************/
#include "ched.h"
ASSERTNAME

RTCLASS(DOCMBMP)
RTCLASS(DCMBMP)

/****************************************************************************
    Constructor for a MaskedBitmapMBMP document.
****************************************************************************/
DOCMBMP::DOCMBMP(PDocumentBase pdocb, PChunkyFile pcfl, ChunkTag ctg, ChunkNumber cno) : DOCE(pdocb, pcfl, ctg, cno)
{
    _pmbmp = pvNil;
}

/****************************************************************************
    Destructor for a MaskedBitmapMBMP document.
****************************************************************************/
DOCMBMP::~DOCMBMP(void)
{
    ReleasePpo(&_pmbmp);
}

/****************************************************************************
    Static method to create a new MaskedBitmapMBMP document.
****************************************************************************/
PDOCMBMP DOCMBMP::PdocmbmpNew(PDocumentBase pdocb, PChunkyFile pcfl, ChunkTag ctg, ChunkNumber cno)
{
    PDOCMBMP pdocmbmp;

    if (pvNil == (pdocmbmp = NewObj DOCMBMP(pdocb, pcfl, ctg, cno)))
        return pvNil;
    if (!pdocmbmp->_FInit())
    {
        ReleasePpo(&pdocmbmp);
        return pvNil;
    }
    AssertPo(pdocmbmp, 0);
    return pdocmbmp;
}

/****************************************************************************
    Create a new display gob for the MaskedBitmapMBMP document.
****************************************************************************/
PDocumentDisplayGraphicsObject DOCMBMP::PddgNew(PGCB pgcb)
{
    return DCMBMP::PdcmbmpNew(this, _pmbmp, pgcb);
}

/***************************************************************************
    Return the size of the thing on file.
***************************************************************************/
long DOCMBMP::_CbOnFile(void)
{
    return _pmbmp->CbOnFile();
}

/****************************************************************************
    Write the data out.
****************************************************************************/
bool DOCMBMP::_FWrite(PDataBlock pblck, bool fRedirect)
{
    AssertThis(0);
    AssertPo(pblck, 0);

    return _pmbmp->FWrite(pblck);
}

/*****************************************************************************
    Read the MaskedBitmapMBMP.
*****************************************************************************/
bool DOCMBMP::_FRead(PDataBlock pblck)
{
    Assert(pvNil == _pmbmp, "losing existing MaskedBitmapMBMP");
    AssertPo(pblck, 0);

    _pmbmp = MaskedBitmapMBMP::PmbmpRead(pblck);
    return pvNil != _pmbmp;
}

#ifdef DEBUG
/***************************************************************************
    Assert the validity of a DOCMBMP.
***************************************************************************/
void DOCMBMP::AssertValid(ulong grf)
{
    DOCMBMP_PAR::AssertValid(0);
    AssertPo(_pmbmp, 0);
}

/***************************************************************************
    Mark memory for the DOCMBMP.
***************************************************************************/
void DOCMBMP::MarkMem(void)
{
    AssertValid(0);
    DOCMBMP_PAR::MarkMem();
    MarkMemObj(_pmbmp);
}
#endif // DEBUG

/*****************************************************************************
    Constructor for a pic display gob.
*****************************************************************************/
DCMBMP::DCMBMP(PDocumentBase pdocb, PMaskedBitmapMBMP pmbmp, PGCB pgcb) : DocumentDisplayGraphicsObject(pdocb, pgcb)
{
    _pmbmp = pmbmp;
}

/*****************************************************************************
    Get the min-max for a DCMBMP.
*****************************************************************************/
void DCMBMP::GetMinMax(RC *prcMinMax)
{
    prcMinMax->Set(0, 0, kswMax, kswMax);
}

/*****************************************************************************
    Static method to create a new DCMBMP.
*****************************************************************************/
PDCMBMP DCMBMP::PdcmbmpNew(PDocumentBase pdocb, PMaskedBitmapMBMP pmbmp, PGCB pgcb)
{
    PDCMBMP pdcmbmp;

    if (pvNil == (pdcmbmp = NewObj DCMBMP(pdocb, pmbmp, pgcb)))
        return pvNil;

    if (!pdcmbmp->_FInit())
    {
        ReleasePpo(&pdcmbmp);
        return pvNil;
    }
    pdcmbmp->Activate(fTrue);

    AssertPo(pdcmbmp, 0);
    return pdcmbmp;
}

/***************************************************************************
    Draw the MaskedBitmapMBMP.
***************************************************************************/
void DCMBMP::Draw(PGraphicsEnvironment pgnv, RC *prcClip)
{
    RC rcMbmp, rcDdg;

    // retrieve appropriate rectangles
    GetRc(&rcDdg, cooLocal);
    _pmbmp->GetRc(&rcMbmp);
    rcMbmp.CenterOnRc(&rcDdg);

    // erase *prcClip
    pgnv->FillRc(prcClip, kacrWhite);
    pgnv->FillRcApt(&rcMbmp, &vaptLtGray, kacrLtGray, kacrWhite);

    // draw mbmp in GraphicsPort
    pgnv->DrawMbmp(_pmbmp, &rcMbmp);
}

#ifdef DEBUG
/***************************************************************************
    Assert the validity of a DCMBMP.
***************************************************************************/
void DCMBMP::AssertValid(ulong grf)
{
    DCMBMP_PAR::AssertValid(0);
    AssertPo(_pmbmp, 0);
}

/***************************************************************************
    Mark memory for the DCMBMP.
***************************************************************************/
void DCMBMP::MarkMem(void)
{
    AssertValid(0);
    DCMBMP_PAR::MarkMem();
    MarkMemObj(_pmbmp);
}
#endif // DEBUG
