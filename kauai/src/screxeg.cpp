/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Script interpreter for GraphicsObject based scripts.

***************************************************************************/
#include "kidframe.h"
ASSERTNAME

namespace ScriptInterpreter {

using GraphicalObjectRepresentation::fgokNil;
using GraphicalObjectRepresentation::fgokKillAnim;
using GraphicalObjectRepresentation::fgokNoAnim;

RTCLASS(GraphicsObjectInterpreter)

#ifdef DEBUG
// these strings are for debug only error messages
static String _stn;
#endif // DEBUG

/***************************************************************************
    Constructor for a GraphicsObject based script interpreter. We don't just keep
    the pgob in case the GraphicsObject goes away while the script is running.
***************************************************************************/
GraphicsObjectInterpreter::GraphicsObjectInterpreter(PWorldOfKidspace pwoks, PResourceCache prca, PGraphicsObject pgob) : GraphicsObjectInterpreter_PAR(prca, pwoks->Pstrg())
{
    AssertPo(pwoks, 0);
    AssertPo(prca, 0);
    AssertPo(pgob, 0);

    _pwoks = pwoks;
    _hid = pgob->Hid();
    _grid = pgob->Grid();
    _pgob = pvNil;

    AssertThis(0);
}

#ifdef DEBUG
/***************************************************************************
    Assert the validity of a GraphicsObjectInterpreter.
***************************************************************************/
void GraphicsObjectInterpreter::AssertValid(ulong grf)
{
    GraphicsObjectInterpreter_PAR::AssertValid(0);
    Assert(hidNil != _hid, 0);
    AssertPo(_pwoks, 0);
    Assert(_prca != pvNil, 0);
}
#endif // DEBUG

/***************************************************************************
    The script is being resumed, so set _pgob to nil (don't know whether
    it exists).
***************************************************************************/
bool GraphicsObjectInterpreter::FResume(long *plwReturn, bool *pfPaused)
{
    GobMayDie();
    return GraphicsObjectInterpreter_PAR::FResume(plwReturn, pfPaused);
}

/***************************************************************************
    Return the gob that corresponds to this script interpreter.
***************************************************************************/
PGraphicsObject GraphicsObjectInterpreter::_PgobThis(void)
{
    AssertThis(0);

    if (pvNil != _pgob)
    {
        AssertPo(_pgob, 0);
        Assert(_pgob->Grid() == _grid, "bad value of _pgob");
        return _pgob;
    }

    _pgob = _pwoks->PgobFromGrid(_grid);
    return _pgob;
}

/***************************************************************************
    Return the gob that is accessible to this script interpreter and has
    the given hid.
***************************************************************************/
PGraphicsObject GraphicsObjectInterpreter::_PgobFromHid(long hid)
{
    AssertThis(0);

    if (hid == _hid)
        return _PgobThis();

    return _pwoks->PgobFromHid(hid);
}

/***************************************************************************
    Return the address of the variable table for the GraphicsObject associated with
    this script interpreter.
***************************************************************************/
PDynamicArray *GraphicsObjectInterpreter::_PpglrtvmThis(void)
{
    PGraphicsObject pgob = _PgobThis();

    if (pvNil == pgob)
        return pvNil;
    return pgob->Ppglrtvm();
}

/***************************************************************************
    Return the address of the variable table for the WorldOfKidspace associated with
    this script interpreter.
***************************************************************************/
PDynamicArray *GraphicsObjectInterpreter::_PpglrtvmGlobal(void)
{
    AssertThis(0);

    return _pwoks->Ppglrtvm();
}

/***************************************************************************
    Return the address of the variable table for the GraphicsObject with given hid.
***************************************************************************/
PDynamicArray *GraphicsObjectInterpreter::_PpglrtvmRemote(long lw)
{
    PGraphicsObject pgob = _PgobFromHid(lw);

    if (pvNil == pgob)
        return pvNil;
    return pgob->Ppglrtvm();
}

/***************************************************************************
    Return the current version number of the script compiler.
***************************************************************************/
short GraphicsObjectInterpreter::_SwCur(void)
{
    return kswCurSccg;
}

/***************************************************************************
    Return the min version number of the script compiler. Read can read
    scripts back to this version.
***************************************************************************/
short GraphicsObjectInterpreter::_SwMin(void)
{
    return kswMinSccg;
}

/***************************************************************************
    Execute a script command.
***************************************************************************/
bool GraphicsObjectInterpreter::_FExecOp(long op)
{
    Command cmd;
    PGraphicsObject pgob;
    PClock pclok;
    long hid;
    long dtim;
    long dxp, dyp;
    long lw1, lw2, lw3, lw4, clw;
    long *qrglw;
    void *pv;

    hid = _hid; // for "this" operations
    switch (op)
    {
    case kopDestroyGob:
        hid = _LwPop();
        // fall through
    case kopDestroyThis:
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)))
        {
            GobMayDie();
            if (pgob == _pwoks)
            {
                Debug(_WarnSz(PszLit("Can't Destroy WorldOfKidspace - destroying all its children")));
                PGraphicsObject pgobT;

                while (pvNil != (pgobT = pgob->PgobFirstChild()))
                    ReleasePpo(&pgobT);
            }
            else
                ReleasePpo(&pgob);
        }
        break;

    case kopCreateChildGob:
        hid = _LwPop();
        // fall through
    case kopCreateChildThis:
        lw1 = _LwPop();
        lw2 = _LwPop();
        pgob = pvNil;
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)))
        {
            GobMayDie();
            pgob = _pwoks->PgokNew(pgob, lw1, lw2, _prca);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing parent GraphicsObject for CreateChild(Gob|This) (gid = %d)"), hid));
        }
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopCreateHelpGob:
        hid = _LwPop();
        // fall through
    case kopCreateHelpThis:
        lw1 = _LwPop();
        pgob = pvNil;
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)))
        {
            GobMayDie();
            pgob = _pwoks->PhbalNew(pgob, _prca, lw1);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing parent GraphicsObject for CreateHelp(Gob|This) (gid = %d)"), hid));
        }
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopResizeGob:
        hid = _LwPop();
        // fall through
    case kopResizeThis:
        dxp = _LwPop();
        dyp = _LwPop();
        // REVIEW shonk: should we handle hwnd based gob's?
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && hNil == pgob->Hwnd())
        {
            RC rc;

            pgob->GetRc(&rc, cooParent);
            rc.xpRight = rc.xpLeft + LwMax(0, dxp);
            rc.ypBottom = rc.ypTop + LwMax(0, dyp);
            pgob->SetPos(&rc, pvNil);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Resize(Gob|This) (gid = %d)"), hid));
        }
        break;

    case kopMoveRelGob:
        hid = _LwPop();
        // fall through
    case kopMoveRelThis:
        dxp = _LwPop();
        dyp = _LwPop();
        // REVIEW shonk: should we handle hwnd based gob's?
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && hNil == pgob->Hwnd())
        {
            RC rc;

            pgob->GetRc(&rc, cooParent);
            rc.Offset(dxp, dyp);
            pgob->SetPos(&rc, pvNil);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for MoveRel(Gob|This) (gid = %d)"), hid));
        }
        break;

    case kopMoveAbsGob:
        hid = _LwPop();
        // fall through
    case kopMoveAbsThis:
        dxp = _LwPop();
        dyp = _LwPop();
        // REVIEW shonk: should we handle hwnd based gob's?
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && hNil == pgob->Hwnd())
        {
            RC rc;

            pgob->GetRc(&rc, cooParent);
            if (pgob->FIs(kclsKidspaceGraphicObject))
            {
                PT pt;
                ((PKidspaceGraphicObject)pgob)->GetPtReg(&pt, cooLocal);
                dxp -= pt.xp;
                dyp -= pt.yp;
            }
            rc.Offset(dxp - rc.xpLeft, dyp - rc.ypTop);
            pgob->SetPos(&rc, pvNil);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for MoveAbs(Gob|This) (gid = %d)"), hid));
        }
        break;

    case kopRunScriptGob:
    case kopRunScriptThis:
    case kopRunScriptCnoGob:
    case kopRunScriptCnoThis:
        clw = _LwPop(); // the number of parameters
        if (kopRunScriptGob == op || kopRunScriptCnoGob == op)
            hid = _LwPop();
        lw1 = _LwPop(); // the chid of the script
        if (_fError)
            break;

        if (clw > 0)
        {
            if (pvNil == _QlwGet(clw))
                break;
            if (!FAllocPv(&pv, LwMul(size(long), clw), fmemNil, mprNormal))
            {
                Debug(_WarnSz(PszLit("OOM attempting to run script")));
                _PopList(clw);
                clw = 0;
                pv = pvNil;
            }
            else
            {
                CopyPb(_QlwGet(clw), pv, LwMul(size(long), clw));
                ReverseRglw(pv, clw);
                _PopList(clw);
            }
        }
        else
            pv = pvNil;

        if (pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for RunScript[Cno](Gob|This) (gid = %d)"), hid));
            lw2 = 0;
        }
        else
        {
            tribool tRet;

            GobMayDie();
            if (kopRunScriptCnoGob == op || kopRunScriptCnoThis == op)
                ((PKidspaceGraphicObject)pgob)->FRunScriptCno(lw1, (long *)pv, clw, &lw2, &tRet);
            else
                ((PKidspaceGraphicObject)pgob)->FRunScript(lw1, (long *)pv, clw, &lw2, &tRet);
            if (tYes != tRet)
            {
                Debug(_WarnSz(PszLit("Running script failed (chid = 0x%x, gid = %d)"), lw1, hid));
                lw2 = 0;
            }
        }
        FreePpv(&pv);
        _Push(lw2);
        break;

    case kopChangeStateGob:
        hid = _LwPop();
        // fall through
    case kopChangeStateThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject) || !FIn(lw1, 0, kswMax))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject or state out of range for ")
                              PszLit("ChangeState(Gob|This) (gid = %d, sno = %d)"),
                          hid, lw1));
        }
        else
        {
            GobMayDie();
            ((PKidspaceGraphicObject)pgob)->FChangeState(lw1);
        }
        break;

    case kopAnimateGob:
        hid = _LwPop();
        // fall through
    case kopAnimateThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Animate(Gob|This) (gid = %d)"), hid));
        }
        else
        {
            GobMayDie();
            ((PKidspaceGraphicObject)pgob)->FSetRep(lw1, fgokNil, kctgAnimation);
        }
        break;

    case kopSetPictureGob:
        hid = _LwPop();
        // fall through
    case kopSetPictureThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for SetPicture(Gob|This) (gid = %d)"), hid));
        }
        else
        {
            GobMayDie();
            ((PKidspaceGraphicObject)pgob)->FSetRep(lw1, fgokKillAnim, kctgMbmp);
        }
        break;

    case kopSetRepGob:
        hid = _LwPop();
        // fall through
    case kopSetRepThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for SetRep(Gob|This) (gid = %d)"), hid));
        }
        else
        {
            GobMayDie();
            ((PKidspaceGraphicObject)pgob)->FSetRep(lw1);
        }
        break;

    case kopStateGob:
        hid = _LwPop();
        // fall through
    case kopStateThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            lw1 = 0;
            Debug(_WarnSz(PszLit("Missing GraphicsObject for State(Gob|This) (gid = %d)"), hid));
        }
        else
            lw1 = ((PKidspaceGraphicObject)pgob)->Sno();
        _Push(lw1);
        break;

    case kopGidThis:
        _Push(_hid);
        break;

    case kopGidParGob:
        hid = _LwPop();
        // fall through
    case kopGidParThis:
        if (pvNil != (pgob = _PgobFromHid(hid)))
            pgob = _pwoks->PgobParGob(pgob);
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopGidNextSib:
        if (pvNil != (pgob = _PgobFromHid(_LwPop())))
            pgob = pgob->PgobNextSib();
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopGidPrevSib:
        if (pvNil != (pgob = _PgobFromHid(_LwPop())))
            pgob = pgob->PgobPrevSib();
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopGidChild:
        if (pvNil != (pgob = _PgobFromHid(_LwPop())))
            pgob = pgob->PgobFirstChild();
        _Push(pvNil == pgob ? hidNil : pgob->Hid());
        break;

    case kopFGobExists:
        pgob = _PgobFromHid(_LwPop());
        _Push(pvNil != pgob);
        break;

    case kopEnqueueCid:
        cmd.cid = _LwPop();
        cmd.pgg = pvNil;
        hid = _LwPop();
        if (hidNil == hid)
            cmd.pcmh = pvNil;
        else
            cmd.pcmh = _pwoks->PcmhFromHid(hid);
        cmd.rglw[0] = _LwPop();
        cmd.rglw[1] = _LwPop();
        cmd.rglw[2] = _LwPop();
        cmd.rglw[3] = _LwPop();
        if (!_fError)
            vpcex->EnqueueCmd(&cmd);
        break;

    case kopAlert:
    case kopPrint:
    case kopPrintStat:
    case kopAlertStr:
    case kopPrintStr:
    case kopPrintStrStat:
        GobMayDie();
        _DoAlert(op);
        break;

    case kopCreateClock:
        hid = _LwPop();
        if (hidNil != hid && pvNil != (pclok = _pwoks->PclokFromHid(hid)))
        {
            Debug(_WarnSz(PszLit("Clock already exists - incrementing ref count (hid = %d)"), hid));
            pclok->AddRef();
        }
        else
        {
            if (hidNil == hid)
                hid = CommandHandler::HidUnique();
            if (pvNil == NewObj Clock(hid))
                hid = hidNil;
        }
        _Push(hid);
        break;

    case kopDestroyClock:
        if (pvNil != (pclok = _pwoks->PclokFromHid(_LwPop())))
            ReleasePpo(&pclok);
        break;

    case kopStartClock:
        hid = _LwPop();
        lw1 = _LwPop();
        if (!_fError && pvNil != (pclok = _pwoks->PclokFromHid(hid)))
            pclok->Start(lw1);
        else
        {
            Debug(_WarnSz(PszLit("Missing clock for StartClock (hid = %d)"), hid));
        }
        break;

    case kopStopClock:
        if (pvNil != (pclok = _pwoks->PclokFromHid(_LwPop())))
            pclok->Stop();
        else
        {
            Debug(_WarnSz(PszLit("Missing clock for StopClock (hid = %d)"), hid));
        }
        break;

    case kopTimeCur:
        if (pvNil != (pclok = _pwoks->PclokFromHid(_LwPop())))
            _Push(pclok->TimCur());
        else
        {
            Debug(_WarnSz(PszLit("Missing clock for TimeCur (hid = %d)"), hid));
            _Push(0);
        }
        break;

    case kopSetAlarmGob:
        hid = _LwPop();
    case kopSetAlarmThis:
    case kopSetAlarm:
        lw1 = _LwPop();
        dtim = _LwPop();
        if (kopSetAlarm == op)
            lw2 = chidNil;
        else
            lw2 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || pvNil == (pclok = _pwoks->PclokFromHid(lw1)) ||
            !pclok->FSetAlarm(dtim, pgob, lw2))
        {
            Debug(_WarnSz(PszLit("Setting Alarm failed (hid = %d)"), lw1));
        }
        break;

    case kopXMouseGob:
    case kopYMouseGob:
        hid = _LwPop();
        // fall through
    case kopXMouseThis:
    case kopYMouseThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for (X|Y)Mouse(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        else
        {
            PT pt;
            bool fDown;
            pgob->GetPtMouse(&pt, &fDown);
            _Push(op == kopXMouseThis || op == kopXMouseGob ? pt.xp : pt.yp);
        }
        break;

    case kopGidUnique:
        _Push(CommandHandler::HidUnique());
        break;

    case kopXGob:
    case kopYGob:
        hid = _LwPop();
        // fall through
    case kopXThis:
    case kopYThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for (X|Y)(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        else if (pgob->FIs(kclsKidspaceGraphicObject))
        {
            PT pt;
            ((PKidspaceGraphicObject)pgob)->GetPtReg(&pt);
            _Push(op == kopXThis || op == kopXGob ? pt.xp : pt.yp);
        }
        else
        {
            RC rc;
            pgob->GetRc(&rc, cooParent);
            _Push(op == kopXThis || op == kopXGob ? rc.xpLeft : rc.ypTop);
        }
        break;

    case kopZGob:
        hid = _LwPop();
        // fall through
    case kopZThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Z(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        else
            _Push(((PKidspaceGraphicObject)pgob)->ZPlane());
        break;

    case kopSetZGob:
        hid = _LwPop();
        // fall through
    case kopSetZThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for SetZ(Gob|This) (gid = %d)"), hid));
        }
        else
            ((PKidspaceGraphicObject)pgob)->SetZPlane(lw1);
        break;

    case kopSetColorTable:
        _SetColorTable(_LwPop());
        break;

    case kopCell:
        _fPaused = fTrue;
        // fall through
    case kopCellNoPause:
        lw1 = _LwPop();
        dxp = _LwPop();
        dyp = _LwPop();
        dtim = _LwPop();
        if (_fError || pvNil == (pgob = _PgobThis()))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Cell[NoPause] (gid = %d)"), hid));
        }
        else
        {
            GobMayDie();
            ((PKidspaceGraphicObject)pgob)->FSetRep(lw1, fgokNoAnim, ctgNil, dxp, dyp, dtim);
        }
        break;

    case kopGetModifierState:
        _Push(_pwoks->GrfcustCur());
        break;

    case kopChangeModifierState:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError)
            _pwoks->ModifyGrfcust(lw1, lw2);
        break;

    case kopTransition:
        lw1 = _LwPop();
        lw2 = _LwPop();
        dtim = _LwPop();
        lw3 = _LwPop();
        lw4 = _LwPop();
        if (!_fError)
        {
            AbstractColor acr;
            PDynamicArray pglclr = _PglclrGet((ChunkNumber)lw4);
            acr.SetFromLw(lw3);
            vpappb->SetGft(lw1, lw2, LuMulDiv(dtim, kdtsSecond, kdtimSecond), pglclr, acr);
            ReleasePpo(&pglclr);
        }
        break;

    case kopGetEdit:
        lw1 = _LwPop();
        _DoEditControl(lw1, _LwPop(), fTrue);
        break;

    case kopSetEdit:
        lw1 = _LwPop();
        _DoEditControl(lw1, _LwPop(), fFalse);
        break;

    case kopGetProp:
        lw1 = _LwPop();
        if (!_fError)
        {
            GobMayDie();
            if (!vpappb->FGetProp(lw1, &lw2))
            {
                Debug(_WarnSz(PszLit("GetProp failed (prid = %d)"), lw1));
                lw2 = 0;
            }
            _Push(lw2);
        }
        break;

    case kopSetProp:
        lw1 = _LwPop();
        lw2 = _LwPop();
        GobMayDie();
        if (!_fError && !vpappb->FSetProp(lw1, lw2))
        {
            Debug(_WarnSz(PszLit("SetProp failed (prid = %d, val = %d)"), lw1, lw2));
        }
        break;

    case kopLaunch:
        lw1 = _LwPop();
        GobMayDie();
        if (!_fError)
            _Push(_FLaunch(lw1));
        break;

    case kopPlayGob:
        hid = _LwPop();
        // fall through
    case kopPlayThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Play(Gob|This) (gid = %d)"), hid));
        }
        else
            ((PKidspaceGraphicObject)pgob)->FPlay();
        break;

    case kopPlayingGob:
        hid = _LwPop();
        // fall through
    case kopPlayingThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Playing(Gob|This) (gid = %d)"), hid));
            _Push(fFalse);
        }
        else
            _Push(((PKidspaceGraphicObject)pgob)->FPlaying());
        break;

    case kopStopGob:
        hid = _LwPop();
        // fall through
    case kopStopThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Stop(Gob|This) (gid = %d)"), hid));
        }
        else
            ((PKidspaceGraphicObject)pgob)->Stop();
        break;

    case kopCurFrameGob:
        hid = _LwPop();
        // fall through
    case kopCurFrameThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for (CurFrame(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        else
            _Push(((PKidspaceGraphicObject)pgob)->NfrCur());
        break;

    case kopCountFramesGob:
        hid = _LwPop();
        // fall through
    case kopCountFramesThis:
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for CountFrames(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        else
            _Push(((PKidspaceGraphicObject)pgob)->NfrMac());
        break;

    case kopGotoFrameGob:
        hid = _LwPop();
        // fall through
    case kopGotoFrameThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for GotoFrame(Gob|This) (gid = %d)"), hid));
        }
        else
            ((PKidspaceGraphicObject)pgob)->GotoNfr(lw1);
        break;

    case kopFilterCmdsGob:
        hid = _LwPop();
        // fall through
    case kopFilterCmdsThis:
        lw1 = _LwPop();
        lw2 = _LwPop();
        lw3 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for FilterCmds(Gob|This) (gid = %d)"), hid));
        }
        else if (!((PKidspaceGraphicObject)pgob)->FFilterCidHid(lw1, lw2, lw3))
        {
            Debug(_WarnSz(PszLit("Filtering failed (gid = %d)"), hid));
        }
        break;

    case kopDestroyChildrenGob:
        hid = _LwPop();
        // fall through
    case kopDestroyChildrenThis:
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)))
        {
            PGraphicsObject pgobT;

            GobMayDie();
            while (pvNil != (pgobT = pgob->PgobFirstChild()))
                ReleasePpo(&pgobT);
        }
        break;

    case kopPlaySoundGob:
        hid = _LwPop();
        // fall through
    case kopPlaySoundThis:
        lw1 = siiNil;
        if (!_fError)
        {
            pgob = (hidNil == hid) ? pvNil : _PgobFromHid(hid);

            if (pvNil != pgob && pgob->FIs(kclsKidspaceGraphicObject))
            {
                if (pvNil != (qrglw = _QlwGet(7)))
                {
                    lw1 = ((PKidspaceGraphicObject)pgob)
                              ->SiiPlaySound(qrglw[6], qrglw[5], qrglw[4], qrglw[3], qrglw[2], 0, qrglw[1], qrglw[0]);
                }
            }
            else
            {
#ifdef DEBUG
                if (hidNil != hid)
                {
                    _WarnSz(PszLit("No KidspaceGraphicObject for PlaySound(Gob|This) (gid = %d)"), hid);
                }
#endif // DEBUG

                if (pvNil != vpsndm && pvNil != (qrglw = _QlwGet(7)))
                {
                    lw1 =
                        vpsndm->SiiPlay(_prca, qrglw[6], qrglw[5], qrglw[4], qrglw[3], qrglw[2], 0, qrglw[1], qrglw[0]);
                }
            }
            _PopList(7);
        }
        _Push(lw1);
        break;

    case kopStopSound:
        lw1 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->Stop(lw1);
        break;

    case kopStopSoundClass:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->StopAll(lw1, lw2);
        break;

    case kopPlayingSound:
        lw1 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            _Push(vpsndm->FPlaying(lw1));
        break;

    case kopPlayingSoundClass:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            _Push(vpsndm->FPlayingAll(lw1, lw2));
        break;

    case kopPauseSound:
        lw1 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->Pause(lw1);
        break;

    case kopPauseSoundClass:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->PauseAll(lw1, lw2);
        break;

    case kopResumeSound:
        lw1 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->Resume(lw1);
        break;

    case kopResumeSoundClass:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->ResumeAll(lw1, lw2);
        break;

    case kopPlayMouseSoundGob:
        hid = _LwPop();
        // fall through
    case kopPlayMouseSoundThis:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && pgob->FIs(kclsKidspaceGraphicObject))
        {
            _Push(((PKidspaceGraphicObject)pgob)->SiiPlayMouseSound(lw1, lw2));
        }
        else
            _Push(siiNil);
        break;

    case kopWidthGob:
    case kopHeightGob:
        hid = _LwPop();
        // fall thru
    case kopWidthThis:
    case kopHeightThis:
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)))
        {
            RC rc;

            pgob->GetRc(&rc, cooLocal);
            if (kopWidthGob == op || kopWidthThis == op)
                _Push(rc.Dxp());
            else
                _Push(rc.Dyp());
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for (Width|Height)(Gob|This) (gid = %d)"), hid));
            _Push(0);
        }
        break;

    case kopSetNoSlipGob:
        hid = _LwPop();
        // fall through
    case kopSetNoSlipThis:
        lw1 = _LwPop();
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && pgob->FIs(kclsKidspaceGraphicObject))
        {
            ((PKidspaceGraphicObject)pgob)->SetNoSlip(lw1);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for SetNoSlip(Gob|This) (gid = %d)"), hid));
        }
        break;

    case kopFIsDescendent:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError)
        {
            PGraphicsObject pgobPar = _PgobFromHid(lw2);

            if (pvNil == pgobPar || pvNil == (pgob = _PgobFromHid(lw1)))
                _Push(0);
            else
            {
                while (pgob != pvNil && pgob != pgobPar)
                    pgob = _pwoks->PgobParGob(pgob);
                _Push(pgob == pgobPar);
            }
        }
        break;

    case kopSetMasterVolume:
        lw1 = _LwPop();
        if (!_fError && pvNil != vpsndm)
            vpsndm->SetVlm(lw1);
        break;

    case kopGetMasterVolume:
        if (pvNil != vpsndm)
            _Push(vpsndm->VlmCur());
        else
            _Push(0);
        break;

    case kopStartLongOp:
        vpappb->BeginLongOp();
        break;

    case kopEndLongOp:
        vpappb->EndLongOp(_LwPop());
        break;

    case kopSetToolTipSourceGob:
        hid = _LwPop();
        // fall through
    case kopSetToolTipSourceThis:
        lw1 = _LwPop();
        if (!_fError && pvNil != (pgob = _PgobFromHid(hid)) && pgob->FIs(kclsKidspaceGraphicObject))
        {
            ((PKidspaceGraphicObject)pgob)->SetHidToolTip(lw1);
        }
        else
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for SetToolTipSource(Gob|This) (gid = %d)"), hid));
        }
        break;

    case kopModalHelp:
        lw1 = _LwPop();
        lw2 = _LwPop();
        if (!_fError)
        {
            GobMayDie();
            if (_pwoks->FModalTopic(_prca, lw1, &lw3))
                lw2 = lw3;
            _Push(lw2);
        }
        break;

    case kopFlushUserEvents:
        vpappb->FlushUserEvents(_LwPop());
        break;

    case kopStreamGob:
        hid = _LwPop();
        // fall through
    case kopStreamThis:
        lw1 = _LwPop();
        if (_fError || pvNil == (pgob = _PgobFromHid(hid)) || !pgob->FIs(kclsKidspaceGraphicObject))
        {
            Debug(_WarnSz(PszLit("Missing GraphicsObject for Stream(Gob|This) (gid = %d)"), hid));
        }
        else
            ((PKidspaceGraphicObject)pgob)->Stream(FPure(lw1));
        break;

    default:
        return GraphicsObjectInterpreter_PAR::_FExecOp(op);
    }

    return !_fError;
}

/***************************************************************************
    Put up an alert containing a list of numbers.
***************************************************************************/
void GraphicsObjectInterpreter::_DoAlert(long op)
{
    String stn1;
    String stn2;
    long lw;
    bool fStrings;
    long clw = _LwPop();

    switch (op)
    {
    default:
        return;

#ifdef CHUNK_STATS
    case kopPrintStat:
#endif // CHUNK_STATS
    case kopAlert:
    case kopPrint:
        fStrings = fFalse;
        break;

#ifdef CHUNK_STATS
    case kopPrintStrStat:
#endif // CHUNK_STATS
    case kopAlertStr:
    case kopPrintStr:
        fStrings = fTrue;
        break;
    }

    if (fStrings && pvNil == _pstrg)
    {
        _Error(fTrue);
        return;
    }

    stn1.FFormatSz(PszLit("Script Message ('%f', 0x%x, %d): "), _pscpt->Ctg(), _pscpt->Cno(), _ilwCur);

    while (clw-- > 0)
    {
        lw = _LwPop();
        if (fStrings)
            _pstrg->FGet(lw, &stn2);
        else
            stn2.FFormatSz(PszLit("%d (0x%x) "), lw, lw);
        stn1.FAppendStn(&stn2);
    }

    switch (op)
    {
#ifdef CHUNK_STATS
    case kopPrintStat:
    case kopPrintStrStat:
        ChunkyFile::DumpStn(&stn1);
        break;
#endif // CHUNK_STATS

    case kopAlert:
    case kopAlertStr:
        _pwoks->TGiveAlert(&stn1, bkOk, cokInformation);
        break;

    case kopPrint:
    case kopPrintStr:
        _pwoks->Print(&stn1);
        break;
    }
}

/***************************************************************************
    Get or set the string in an edit control.
***************************************************************************/
void GraphicsObjectInterpreter::_DoEditControl(long hid, long stid, bool fGet)
{
    PEDCB pedcb;
    long cch;
    achar rgch[kcchMaxStn];
    String stn;

    if (_fError || pvNil == (pedcb = (PEDCB)_PgobFromHid(hid)) || !pedcb->FIs(kclsEDCB))
    {
        Debug(_WarnSz(PszLit("Missing edit control for (Get|Set)Edit (gid = %d)"), hid));
        return;
    }

    if (pvNil == _pstrg)
    {
        _Error(fTrue);
        return;
    }

    cch = pedcb->IchMac();
    if (fGet)
    {
        cch = pedcb->CchFetch(rgch, 0, LwMin(kcchMaxStn, cch));
        stn.SetRgch(rgch, cch);
        _pstrg->FPut(stid, &stn);
    }
    else
    {
        _pstrg->FGet(stid, &stn);
        pedcb->FReplace(stn.Psz(), stn.Cch(), 0, cch);
    }
}

/***************************************************************************
    Set the current color table.
***************************************************************************/
void GraphicsObjectInterpreter::_SetColorTable(ChunkNumber cno)
{
    PDynamicArray pglclr;

    if (pvNil == (pglclr = _PglclrGet(cno)))
        return;

    GraphicsPort::SetActiveColors(pglclr, fpalIdentity);
    ReleasePpo(&pglclr);
}

/***************************************************************************
    Read the indicated color table and return a reference to it.
***************************************************************************/
PDynamicArray GraphicsObjectInterpreter::_PglclrGet(ChunkNumber cno)
{
    PGenericCacheableObject pcabo;
    PDynamicArray pglclr;

    if (cnoNil == cno)
        return pvNil;

    pcabo = (PGenericCacheableObject)_prca->PbacoFetch(kctgColorTable, cno, FReadColorTable);
    if (pvNil == pcabo)
        return pvNil;

    pglclr = (PDynamicArray)pcabo->po;
    AssertPo(pglclr, 0);
    pglclr->AddRef();
    pcabo->SetCrep(crepTossFirst);
    ReleasePpo(&pcabo);

    return pglclr;
}

/***************************************************************************
    A chunky resource reader to read a color table. Wraps the color table in
    a GenericCacheableObject.
***************************************************************************/
bool FReadColorTable(PChunkyResourceFile pcrf, ChunkTag ctg, ChunkNumber cno, PDataBlock pblck, PBaseCacheableObject *ppbaco, long *pcb)
{
    PGenericCacheableObject pcabo;
    PDynamicArray pglclr = pvNil;

    *pcb = pblck->Cb(fTrue);
    if (pvNil == ppbaco)
        return fTrue;

    if (!pblck->FUnpackData())
        goto LFail;
    *pcb = pblck->Cb();

    if (pvNil == (pglclr = DynamicArray::PglRead(pblck)) || pglclr->CbEntry() != size(Color))
        goto LFail;

    if (pvNil == (pcabo = NewObj GenericCacheableObject(pglclr)))
    {
    LFail:
        ReleasePpo(&pglclr);
        TrashVar(pcb);
        TrashVar(ppbaco);
        return fFalse;
    }

    *ppbaco = pcabo;
    return fTrue;
}

/***************************************************************************
    Launch an app with the given command line. Return true iff the launch
    was successful.
***************************************************************************/
bool GraphicsObjectInterpreter::_FLaunch(long stid)
{
    AssertThis(0);
    String stn;

    if (pvNil == _pstrg)
    {
        _Error(fTrue);
        return fFalse;
    }

    if (!_pstrg->FGet(stid, &stn))
    {
        Debug(_WarnSz(PszLit("String missing for Launch (stid = %d)"), stid));
        Warn("string missing");
        return fFalse;
    }

#ifdef WIN
    STARTUPINFO sui;
    PROCESS_INFORMATION pi;

    ClearPb(&sui, size(sui));
    sui.cb = size(sui);

    return CreateProcess(pvNil, stn.Psz(), pvNil, pvNil, fFalse, DETACHED_PROCESS, pvNil, pvNil, &sui, &pi);
#else  //! WIN
    RawRtn(); // REVIEW shonk: Mac: implement GraphicsObjectInterpreter::_FLaunch
    return fFalse;
#endif //! WIN
}

} // end of namespace ScriptInterpreter
