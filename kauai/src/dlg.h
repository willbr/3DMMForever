/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Dialog header file.

***************************************************************************/
#ifndef DLG_H
#define DLG_H

using Group::GeneralGroup;

#ifdef MAC
typedef DialogPtr HDLG;
#endif // MAC
#ifdef WIN
typedef HWND HDLG;
#endif // WIN

// type of dialog item
enum
{
    ditkButton,     // no value
    ditkCheckBox,   // long (bool)
    ditkRadioGroup, // long (index)
    ditkEditText,   // streamed stn
    ditkCombo,      // streamed stn, followed by a list of streamed stn's.
    ditkLim
};

// dialog item
struct DialogItem
{
    long sitMin; // first system item number (for this DialogItem)
    long sitLim; // lim of system item numbers (for this DialogItem)
    long ditk;   // kind of item
};

typedef class Dialog *PDLG;

// callback to notify of an item change (while the dialog is active)
typedef bool (*PFNDLG)(PDLG pdlg, long *pidit, void *pv);

// dialog class - a Dialog is a GeneralGroup of DITs
#define Dialog_PAR GeneralGroup
#define kclsDialog 'DLG'
class Dialog : public Dialog_PAR
{
    RTCLASS_DEC

  private:
    PGraphicsObject _pgob;
    long _rid;
    PFNDLG _pfn;
    void *_pv;

#ifdef WIN
    friend BOOL CALLBACK _FDlgCore(HWND hdlg, UINT msg, WPARAM w, LPARAM lw);
#endif // WIN

    Dialog(long rid);
    bool _FInit(void);

    long _LwGetRadioGroup(long idit);
    void _SetRadioGroup(long idit, long lw);
    bool _FGetCheckBox(long idit);
    void _InvertCheckBox(long idit);
    void _SetCheckBox(long idit, bool fOn);
    void _GetEditText(long idit, PString pstn);
    void _SetEditText(long idit, PString pstn);
    bool _FDitChange(long *pidit);
    bool _FAddToList(long idit, PString pstn);
    void _ClearList(long idit);

  public:
    static PDLG PdlgNew(long rid, PFNDLG pfn = pvNil, void *pv = pvNil);

    long IditDo(long iditFocus = ivNil);

    // these are only valid while the dialog is up
    bool FGetValues(long iditMin, long iditLim);
    void SetValues(long iditMin, long iditLim);
    void SelectDit(long idit);

    // argument access
    long IditFromSit(long sit);
    void GetDit(long idit, DialogItem *pdit)
    {
        GetFixed(idit, pdit);
    }
    void PutDit(long idit, DialogItem *pdit)
    {
        PutFixed(idit, pdit);
    }

    void GetStn(long idit, PString pstn);
    bool FPutStn(long idit, PString pstn);
    long LwGetRadio(long idit);
    void PutRadio(long idit, long lw);
    bool FGetCheck(long idit);
    void PutCheck(long idit, bool fOn);

    bool FGetLwFromEdit(long idit, long *plw, bool *pfEmpty = pvNil);
    bool FPutLwInEdit(long idit, long lw);

    bool FAddToList(long idit, PString pstn);
    void ClearList(long idit);
};

#endif //! DLG_H
