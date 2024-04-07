/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Graphic object class.

***************************************************************************/
#include "frame.h"
ASSERTNAME

PGraphicsObject GraphicsObject::_pgobScreen;

/***************************************************************************
    Create the screen gob.  If fgobEnsureHwnd is set, ensures that the
    screen gob has an OS window associated with it.
***************************************************************************/
bool GraphicsObject::FInitScreen(ulong grfgob, long ginDef)
{
    PGraphicsObject pgob;
    GraphicsObjectBlock gcb(khidScreen, pvNil);

    switch (ginDef)
    {
    case kginDraw:
    case kginMark:
    case kginSysInval:
        _ginDefGob = ginDef;
        break;
    }

    if (pvNil == (pgob = NewObj GraphicsObject(&gcb)))
        return fFalse;
    Assert(pgob == _pgobScreen, 0);

    if (!pgob->FAttachHwnd(vwig.hwndApp))
        return fFalse;

    return fTrue;
}

/***************************************************************************
    Make the GraphicsObject a wrapper for the given system window.
***************************************************************************/
bool GraphicsObject::FAttachHwnd(HWND hwnd)
{
    if (_hwnd != hNil)
    {
        ReleasePpo(&_pgpt);
        // don't destroy the hwnd - the caller must do that
        _hwnd = hNil;
    }
    if (hwnd != hNil)
    {
        if (pvNil == (_pgpt = GraphicsPort::PgptNewHwnd(hwnd)))
            return fFalse;
        _hwnd = hwnd;
        SetRcFromHwnd();
    }
    return fTrue;
}

/***************************************************************************
    Find the GraphicsObject associated with the given hwnd (if there is one).
***************************************************************************/
PGraphicsObject GraphicsObject::PgobFromHwnd(HWND hwnd)
{
    // NOTE: we used to use SetProp and GetProp for this, but profiling
    // indicated that GetProp is very slow.
    Assert(hwnd != hNil, "nil hwnd");
    GraphicsObjectTreeEnumerator gte;
    ulong grfgte;
    PGraphicsObject pgob;

    gte.Init(_pgobScreen, fgteNil);
    while (gte.FNextGob(&pgob, &grfgte, fgteNil))
    {
        if (pgob->_hwnd == hwnd)
            return pgob;
    }
    return pvNil;
}

/***************************************************************************
    Return the active MDI window.
***************************************************************************/
HWND GraphicsObject::HwndMdiActive(void)
{
    if (vwig.hwndClient == hNil)
        return hNil;

    return (HWND)SendMessage(vwig.hwndClient, WM_MDIGETACTIVE, 0, 0);
}

/***************************************************************************
    Creates a new MDI window and returns it.  This is normally then
    attached to a gob.
***************************************************************************/
HWND GraphicsObject::_HwndNewMdi(PString pstnTitle)
{
    AssertPo(pstnTitle, 0);
    HWND hwnd, hwndT;
    long lwStyle;

    if (vwig.hwndClient == hNil)
    {
        // create the client first
        CLIENTCREATESTRUCT ccs;
        SystemRectangle rcs;

        ccs.hWindowMenu = hNil;
        ccs.idFirstChild = 1;
        GetClientRect(vwig.hwndApp, &rcs);
        Assert(rcs.left == 0 && rcs.top == 0, 0);
        vwig.hwndClient = CreateWindow(PszLit("MDICLIENT"), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0,
                                       rcs.right, rcs.bottom, vwig.hwndApp, NULL, vwig.hinst, (LPVOID)&ccs);
        if (vwig.hwndClient == hNil)
            return hNil;
    }

    lwStyle = MDIS_ALLCHILDSTYLES | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_CAPTION |
              WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    hwndT = HwndMdiActive();
    if (hNil == hwndT || IsZoomed(hwndT))
        lwStyle |= WS_MAXIMIZE;

    hwnd = CreateMDIWindow(PszLit("MDI"), pstnTitle->Psz(), lwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, vwig.hwndClient, vwig.hinst, 0L);
    if (hNil != hwnd && pvNil != vpmubCur)
        vpmubCur->FAddListCid(cidChooseWnd, (long)hwnd, pstnTitle);
    return hwnd;
}

/***************************************************************************
    Destroy an hwnd.
***************************************************************************/
void GraphicsObject::_DestroyHwnd(HWND hwnd)
{
    if (hwnd == vwig.hwndApp)
    {
        Bug("can't destroy app window");
        return;
    }
    if (GetParent(hwnd) == vwig.hwndClient && vwig.hwndClient != hNil)
    {
        if (pvNil != vpmubCur)
            vpmubCur->FRemoveListCid(cidChooseWnd, (long)hwnd);
        SendMessage(vwig.hwndClient, WM_MDIDESTROY, (WPARAM)hwnd, 0);
    }
    else
        DestroyWindow(hwnd);
}

/***************************************************************************
    Gets the current mouse location in this gob's coordinates (if ppt is
    not nil) and determines if the mouse button is down (if pfDown is
    not nil).
***************************************************************************/
void GraphicsObject::GetPtMouse(PT *ppt, bool *pfDown)
{
    AssertThis(0);
    if (ppt != pvNil)
    {
        PTS pts;
        long xp, yp;
        PGraphicsObject pgob;

        xp = yp = 0;
        for (pgob = this; pgob != pvNil && pgob->_hwnd == hNil; pgob = pgob->_pgobPar)
        {
            xp += pgob->_rcCur.xpLeft;
            yp += pgob->_rcCur.ypTop;
        }
        GetCursorPos(&pts);
        if (pgob != pvNil)
            ScreenToClient(pgob->_hwnd, &pts);
        *ppt = PT(pts);
        ppt->xp -= xp;
        ppt->yp -= yp;
    }
    if (pfDown != pvNil)
        *pfDown = GetAsyncKeyState(VK_LBUTTON) < 0;
}

/***************************************************************************
    Makes sure the GraphicsObject is clean (no update is pending).
***************************************************************************/
void GraphicsObject::Clean(void)
{
    AssertThis(0);
    HWND hwnd;
    RC rc, rcT;
    SystemRectangle rcs;

    if (hNil == (hwnd = _HwndGetRc(&rc)))
        return;

    vpappb->InvalMarked(hwnd);
    GetUpdateRect(hwnd, &rcs, fFalse);
    rcT = RC(rcs);
    if (rc.FIntersect(&rcT))
        UpdateWindow(hwnd);
}

/***************************************************************************
    Set the window name.
***************************************************************************/
void GraphicsObject::SetHwndName(PString pstn)
{
    if (hNil == _hwnd)
    {
        Bug("GraphicsObject doesn't have an hwnd");
        return;
    }
    if (pvNil != vpmubCur)
    {
        vpmubCur->FChangeListCid(cidChooseWnd, (long)_hwnd, pvNil, (long)_hwnd, pstn);
    }
    SetWindowText(_hwnd, pstn->Psz());
}

/***************************************************************************
    If this is one of our MDI windows, make it the active MDI window.
***************************************************************************/
void GraphicsObject::MakeHwndActive(HWND hwnd)
{
    if (IsWindow(hwnd) && GetParent(hwnd) == vwig.hwndClient && vwig.hwndClient != hNil)
        SendMessage(vwig.hwndClient, WM_MDIACTIVATE, (WPARAM)hwnd, 0);
}
