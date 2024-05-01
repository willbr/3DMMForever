/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    These are globals common to the util layer.

***************************************************************************/
#ifndef UTILGLOB_H
#define UTILGLOB_H

/***************************************************************************
    Universal scalable application clock and other time stuff
***************************************************************************/
const ulong kdtsSecond = MacWin(60, 1000);
const ulong kluTimeScaleNormal = 0x00010000;

typedef class UniversalScalableApplicationClock *PUniversalScalableApplicationClock;
#define UniversalScalableApplicationClock_PAR BASE
#define kclsUniversalScalableApplicationClock 'USAC'
class UniversalScalableApplicationClock : public UniversalScalableApplicationClock_PAR
{
    RTCLASS_DEC

  private:
    ulong _tsBaseSys; // base system time
    ulong _tsBaseApp; // base application time
    ulong _luScale;

  public:
    UniversalScalableApplicationClock(void);

    ulong TsCur(void);
    void Scale(ulong luScale);
    ulong LuScale(void)
    {
        return _luScale;
    }
    void Jump(ulong dtsJump)
    {
        _tsBaseApp += dtsJump;
    }
};

extern PUniversalScalableApplicationClock vpusac;

inline ulong TsCurrent(void)
{
    return vpusac->TsCur();
}
inline ulong TsCurrentSystem(void)
{
    // n.b. WIN: timeGetTime is more accurate than GetTickCount
    return MacWin(TickCount(), timeGetTime());
}
inline ulong DtsCaret(void)
{
    return MacWin(GetCaretTime(), GetCaretBlinkTime());
}

/***************************************************************************
    Mutexes to protect various global linked lists, etc.
***************************************************************************/
#ifdef DEBUG
extern Mutex vmutxBase;
#endif // DEBUG
extern Mutex vmutxMem;

/***************************************************************************
    Global random number generator and shuffler. These are used by the
    script interpreter.
***************************************************************************/
extern SFL vsflUtil;
extern RND vrndUtil;

/***************************************************************************
    Global standard Kauai codec, compression manager, and pointer to
    a compression manager. The blck-level compression uses vpcodmUtil.
    Clients are free to redirect this to their own compression manager.
***************************************************************************/
extern KCDC vkcdcUtil;
extern CODM vcodmUtil;
extern PCODM vpcodmUtil;

/***************************************************************************
    Debug memory globals
***************************************************************************/
#ifdef DEBUG
extern DebugMemoryGlobals vdmglob;
#endif // DEBUG

#endif //! UTILGLOB_H
