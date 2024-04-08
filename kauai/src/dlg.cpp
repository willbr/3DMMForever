/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Standard dialogs.

***************************************************************************/
#include "frame.h"
ASSERTNAME

RTCLASS(Dialog)

/***************************************************************************
    Constructor for a dialog object.
***************************************************************************/
Dialog::Dialog(long rid) : GeneralGroup(size(DialogItem))
{
    _rid = rid;
}

/***************************************************************************
    Static method to create a new Dialog.  Does NewObj then calls _FInit.
***************************************************************************/
PDialog Dialog::PdlgNew(long rid, PFNDLG pfn, void *pv)
{
    PDialog pdlg;

    if ((pdlg = NewObj Dialog(rid)) == pvNil)
        return pvNil;

    pdlg->_pfn = pfn;
    pdlg->_pv = pv;

    if (!pdlg->_FInit())
        ReleasePpo(&pdlg);

    return pdlg;
}

/***************************************************************************
    Get the values for [iditMin, iditLim) from the actual dialog and put
    them in the GGDIT.
***************************************************************************/
bool Dialog::FGetValues(long iditMin, long iditLim)
{
    AssertThis(0);
    long idit;
    DialogItem dit;
    long lw;
    String stn;

    AssertIn(iditMin, 0, iditLim);
    if (_pgob == pvNil)
    {
        Bug("why are you calling this when the dialog doesn't exist?");
        return fFalse;
    }

    iditLim = LwMin(iditLim, IvMac());
    for (idit = iditMin; idit < iditLim; idit++)
    {
        GetDit(idit, &dit);
        switch (dit.ditk)
        {
        case ditkCheckBox:
            lw = _FGetCheckBox(idit);
            goto LPutLw;

        case ditkRadioGroup:
            lw = _LwGetRadioGroup(idit);
        LPutLw:
            PutRgb(idit, 0, size(lw), &lw);
            break;

        case ditkEditText:
        case ditkCombo:
            _GetEditText(idit, &stn);
            if (!FPutStn(idit, &stn))
                return fFalse;
            break;
        }
    }

    return fTrue;
}

/***************************************************************************
    Set the values for [iditMin, iditLim) from the GGDIT into the actual
    dialog.
***************************************************************************/
void Dialog::SetValues(long iditMin, long iditLim)
{
    AssertThis(0);
    long idit;
    DialogItem dit;
    String stn;
    long lw;
    long cb, cbT, ib;
    byte *prgb;

    if (_pgob == pvNil)
    {
        Bug("why are you calling this when the dialog doesn't exist?");
        return;
    }

    iditLim = LwMin(iditLim, IvMac());
    for (idit = iditMin; idit < iditLim; idit++)
    {
        GetDit(idit, &dit);
        switch (dit.ditk)
        {
        case ditkCheckBox:
            GetRgb(idit, 0, size(lw), &lw);
            _SetCheckBox(idit, lw);
            break;

        case ditkRadioGroup:
            GetRgb(idit, 0, size(lw), &lw);
            _SetRadioGroup(idit, lw);
            break;

        case ditkEditText:
            GetStn(idit, &stn);
            _SetEditText(idit, &stn);
            break;

        case ditkCombo:
            _ClearList(idit);
            cb = Cb(idit);
            if (cb <= 0)
            {
                stn.SetNil();
                _SetEditText(idit, &stn);
                break;
            }
            prgb = (byte *)PvLock(idit);
            if (!stn.FSetData(prgb, cb, &cbT))
            {
                Bug("bad combo item");
                cbT = cb;
            }
            _SetEditText(idit, &stn);
            for (ib = cbT; ib < cb;)
            {
                if (!stn.FSetData(prgb + ib, cb - ib, &cbT))
                {
                    BugVar("bad combo item", &ib);
                    break;
                }
                ib += cbT;
                _FAddToList(idit, &stn);
            }
            Unlock();
            break;
        }
    }
}

/***************************************************************************
    Get the item number from a system item number.
***************************************************************************/
long Dialog::IditFromSit(long sit)
{
    long idit;
    DialogItem dit;

    for (idit = IvMac(); idit-- != 0;)
    {
        GetDit(idit, &dit);
        if (sit >= dit.sitMin && sit < dit.sitLim)
            return idit;
    }
    return ivNil;
}

/***************************************************************************
    Calls the PFNDLG (if not nil) to notify of a change.  PFNDLG should
    return true if the dialog should be dismissed.  The PFNDLG is free
    to change *pidit.  If a nil PFNDLG was specified (in PdlgNew),
    this returns true (dismisses the dialog) on any button hit.
***************************************************************************/
bool Dialog::_FDitChange(long *pidit)
{
    if (pvNil == _pfn)
    {
        DialogItem dit;

        if (ivNil == *pidit)
            return fFalse;

        GetDit(*pidit, &dit);
        return dit.ditk == ditkButton;
    }

    return (*_pfn)(this, pidit, _pv);
}

/***************************************************************************
    Get the stn (for an edit item).
***************************************************************************/
void Dialog::GetStn(long idit, PString pstn)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    AssertPo(pstn, 0);
    long cb;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkEditText == dit.ditk || dit.ditk == ditkCombo, "not a text item or combo");
#endif // DEBUG

    cb = Cb(idit);
    if (cb <= 0)
        pstn->SetNil();
    else
    {
        AssertDo(pstn->FSetData(PvLock(idit), cb, &cb), 0);
        Unlock();
    }
}

/***************************************************************************
    Put the stn into the Dialog.
***************************************************************************/
bool Dialog::FPutStn(long idit, PString pstn)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    AssertPo(pstn, 0);
    DialogItem dit;
    long cbOld, cbNew;

    GetDit(idit, &dit);
    cbOld = Cb(idit);
    cbNew = pstn->CbData();
    switch (dit.ditk)
    {
    default:
        Bug("not a text item or combo");
        return fFalse;

    case ditkEditText:
        if (cbOld != cbNew && !FPut(idit, cbNew, pvNil))
            return fFalse;
        break;

    case ditkCombo:
        if (cbOld > 0)
        {
            String stn;

            if (!stn.FSetData(PvLock(idit), cbOld, &cbOld))
            {
                Bug("why did setting the data fail?");
                cbOld = Cb(idit);
            }
            Unlock();
        }
        if (cbOld > cbNew)
            DeleteRgb(idit, 0, cbOld - cbNew);
        else if (cbOld < cbNew && !FInsertRgb(idit, 0, cbNew - cbOld, pvNil))
            return fFalse;
        break;
    }

    pstn->GetData(PvLock(idit));
    Unlock();
    return fTrue;
}

/***************************************************************************
    Get the value of a radio group.
***************************************************************************/
long Dialog::LwGetRadio(long idit)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    long lw;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkRadioGroup == dit.ditk, "not a radio group");
#endif // DEBUG

    GetRgb(idit, 0, size(long), &lw);
    return lw;
}

/***************************************************************************
    Set the value of the radio group.
***************************************************************************/
void Dialog::PutRadio(long idit, long lw)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkRadioGroup == dit.ditk, "not a radio group");
    AssertIn(lw, 0, dit.sitLim - dit.sitMin);
#endif // DEBUG

    PutRgb(idit, 0, size(long), &lw);
}

/***************************************************************************
    Get the value of a check box.
***************************************************************************/
bool Dialog::FGetCheck(long idit)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    long lw;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkCheckBox == dit.ditk, "not a check box");
#endif // DEBUG

    GetRgb(idit, 0, size(long), &lw);
    return lw;
}

/***************************************************************************
    Set the value of a check box item.
***************************************************************************/
void Dialog::PutCheck(long idit, bool fOn)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    long lw;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkCheckBox == dit.ditk, "not a check box");
#endif // DEBUG

    lw = FPure(fOn);
    PutRgb(idit, 0, size(long), &lw);
}

/***************************************************************************
    Get the indicated edit item from the dialog and convert it to a long.
    If the string is empty, sets *plw to zero and sets *pfEmpty (if pfEmpty
    is not nil) and returns false.  If the string doesn't parse as a number,
    returns false.
***************************************************************************/
bool Dialog::FGetLwFromEdit(long idit, long *plw, bool *pfEmpty)
{
    AssertThis(0);
    AssertVarMem(plw);
    AssertNilOrVarMem(pfEmpty);
    String stn;

    GetStn(idit, &stn);
    if (0 == stn.Cch())
    {
        if (pvNil != pfEmpty)
            *pfEmpty = fTrue;
        *plw = 0;
        return fFalse;
    }
    if (pvNil != pfEmpty)
        *pfEmpty = fFalse;
    if (!stn.FGetLw(plw, 0))
    {
        TrashVar(plw);
        return fFalse;
    }
    return fTrue;
}

/***************************************************************************
    Put the long into the indicated edit item (in decimal).
***************************************************************************/
bool Dialog::FPutLwInEdit(long idit, long lw)
{
    AssertThis(0);
    String stn;

    stn.FFormatSz(PszLit("%d"), lw);
    return FPutStn(idit, &stn);
}

/***************************************************************************
    Add the string to the given list item.
***************************************************************************/
bool Dialog::FAddToList(long idit, PString pstn)
{
    AssertThis(0);
    long cb, cbTot;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkCombo == dit.ditk, "not a combo");
#endif // DEBUG

    cbTot = Cb(idit);
    if (cbTot == 0)
    {
        String stn;

        stn.SetNil();
        if (!FPut(idit, cbTot = stn.CbData(), pvNil))
            return fFalse;
        stn.GetData(PvLock(idit));
        Unlock();
    }
    cb = pstn->CbData();
    if (!FInsertRgb(idit, cbTot, cb, pvNil))
        return fFalse;

    pstn->GetData(PvAddBv(PvLock(idit), cbTot));
    Unlock();
    return fTrue;
}

/***************************************************************************
    Empty the list of options for the list item.
***************************************************************************/
void Dialog::ClearList(long idit)
{
    AssertThis(0);
    AssertIn(idit, 0, IvMac());
    long cbOld, cbNew;
    String stn;

#ifdef DEBUG
    DialogItem dit;
    GetDit(idit, &dit);
    Assert(ditkCombo == dit.ditk, "not a combo");
#endif // DEBUG

    cbOld = Cb(idit);
    if (cbOld <= 0)
        return;

    if (!stn.FSetData(PvLock(idit), cbOld, &cbNew))
    {
        Bug("why did setting the data fail?");
        cbNew = 0;
    }
    Unlock();
    if (cbOld > cbNew)
        DeleteRgb(idit, cbNew, cbOld - cbNew);
}
