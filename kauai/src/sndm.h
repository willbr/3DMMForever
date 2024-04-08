/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Sound Management class for WAVE and MIDI support

***************************************************************************/
#ifndef SNDM_H
#define SNDM_H

const long siiNil = 0;
const FileType kftgMidi = MacWin('MIDI', 'MID'); // REVIEW shonk: Mac: file type
const FileType kftgWave = MacWin('WAVE', 'WAV'); // REVIEW shonk: Mac: file type

/***************************************************************************
    Sound device - like audioman or our midi player.
***************************************************************************/
typedef class SoundDevice *PSoundDevice;
#define SoundDevice_PAR BASE
#define kclsSoundDevice 'SNDV'
class SoundDevice : public SoundDevice_PAR
{
    RTCLASS_DEC

  protected:
    static long _siiLast;

    static long _SiiAlloc(void)
    {
        return ++_siiLast;
    }

  public:
    virtual bool FActive(void) = 0;
    virtual void Activate(bool fActive) = 0; // boolean state
    virtual void Suspend(bool fSuspend) = 0; // reference count
    virtual void SetVlm(long vlm) = 0;
    virtual long VlmCur(void) = 0;

    virtual long SiiPlay(PResourceCache prca, ChunkTag ctg, ChunkNumber cno, long sqn = ksqnNone, long vlm = kvlmFull, long cactPlay = 1,
                         ulong dtsStart = 0, long spr = 0, long scl = sclNil) = 0;

    virtual void Stop(long sii) = 0;
    virtual void StopAll(long sqn = sqnNil, long scl = sclNil) = 0;

    virtual void Pause(long sii) = 0;
    virtual void PauseAll(long sqn = sqnNil, long scl = sclNil) = 0;

    virtual void Resume(long sii) = 0;
    virtual void ResumeAll(long sqn = sqnNil, long scl = sclNil) = 0;

    virtual bool FPlaying(long sii) = 0;
    virtual bool FPlayingAll(long sqn = sqnNil, long scl = sclNil) = 0;

    virtual void Flush(void) = 0;
    virtual void BeginSynch(void);
    virtual void EndSynch(void);
};

/****************************************
    Sound manager class
****************************************/
typedef class SoundManager *PSoundManager;
#define SoundManager_PAR SoundDevice
#define kclsSoundManager 'SNDM'
class SoundManager : public SoundManager_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    struct SNDMPE
    {
        ChunkTag ctg;
        PSoundDevice psndv;
    };

    PDynamicArray _pglsndmpe; // sound type to device mapper

    long _cactSuspend;  // nesting level for suspending
    bool _fActive : 1;  // whether the app is active
    bool _fFreeing : 1; // we're in the destructor

    SoundManager(void);
    bool _FInit(void);
    bool _FFindCtg(ChunkTag ctg, SNDMPE *psndmpe, long *pisndmpe = pvNil);

  public:
    static PSoundManager PsndmNew(void);
    ~SoundManager(void);

    // new methods
    virtual bool FAddDevice(ChunkTag ctg, PSoundDevice psndv);
    virtual PSoundDevice PsndvFromCtg(ChunkTag ctg);
    virtual void RemoveSndv(ChunkTag ctg);

    // inherited methods
    virtual bool FActive(void);
    virtual void Activate(bool fActive);
    virtual void Suspend(bool fSuspend);
    virtual void SetVlm(long vlm);
    virtual long VlmCur(void);

    virtual long SiiPlay(PResourceCache prca, ChunkTag ctg, ChunkNumber cno, long sqn = ksqnNone, long vlm = kvlmFull, long cactPlay = 1,
                         ulong dtsStart = 0, long spr = 0, long scl = sclNil);

    virtual void Stop(long sii);
    virtual void StopAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Pause(long sii);
    virtual void PauseAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Resume(long sii);
    virtual void ResumeAll(long sqn = sqnNil, long scl = sclNil);

    virtual bool FPlaying(long sii);
    virtual bool FPlayingAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Flush(void);
    virtual void BeginSynch(void);
    virtual void EndSynch(void);
};

/***************************************************************************
    A useful base class for devices that support multiple queues.
***************************************************************************/
typedef class SNQUE *PSNQUE;

typedef class SNDMQ *PSNDMQ;
#define SNDMQ_PAR SoundDevice
#define kclsSNDMQ 'snmq'
class SNDMQ : public SNDMQ_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    // queue descriptor
    struct SNQD
    {
        PSNQUE psnque;
        long sqn;
    };

    PDynamicArray _pglsnqd; // the queues

    long _cactSuspend;
    bool _fActive : 1;

    virtual bool _FInit(void);
    virtual bool _FEnsureQueue(long sqn, SNQD *psnqd, long *pisnqd);

    virtual PSNQUE _PsnqueNew(void) = 0;
    virtual void _Suspend(bool fSuspend) = 0;

  public:
    ~SNDMQ(void);

    // inherited methods
    virtual bool FActive(void);
    virtual void Activate(bool fActive);
    virtual void Suspend(bool fSuspend);

    virtual long SiiPlay(PResourceCache prca, ChunkTag ctg, ChunkNumber cno, long sqn = ksqnNone, long vlm = kvlmFull, long cactPlay = 1,
                         ulong dtsStart = 0, long spr = 0, long scl = sclNil);

    virtual void Stop(long sii);
    virtual void StopAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Pause(long sii);
    virtual void PauseAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Resume(long sii);
    virtual void ResumeAll(long sqn = sqnNil, long scl = sclNil);

    virtual bool FPlaying(long sii);
    virtual bool FPlayingAll(long sqn = sqnNil, long scl = sclNil);

    virtual void Flush(void);
};

/***************************************************************************
    The sound instance structure.
***************************************************************************/
struct SNDIN
{
    PBaseCacheableObject pbaco;    // the sound to play
    long sii;       // the sound instance id
    long vlm;       // volume to play at
    long cactPlay;  // how many times to play
    ulong dtsStart; // offset to start at
    long spr;       // sound priority
    long scl;       // sound class

    // 0 means play, < 0 means skip, > 0 means pause
    long cactPause;
};

/***************************************************************************
    Sound queue for a SNDMQ
***************************************************************************/
#define SNQUE_PAR BASE
#define kclsSNQUE 'snqu'
class SNQUE : public SNQUE_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    PDynamicArray _pglsndin;   // the queue
    long _isndinCur; // SNDIN that we should be playing

    SNQUE(void);

    virtual bool _FInit(void);
    virtual void _Queue(long isndinMin) = 0;
    virtual void _PauseQueue(long isndinMin) = 0;
    virtual void _ResumeQueue(long isndinMin) = 0;
    virtual PBaseCacheableObject _PbacoFetch(PResourceCache prca, ChunkTag ctg, ChunkNumber cno) = 0;

    virtual void _Enter(void);
    virtual void _Leave(void);
    virtual void _Flush(void);

  public:
    ~SNQUE(void);

    void Enqueue(long sii, PResourceCache prca, ChunkTag ctg, ChunkNumber cno, long vlm, long cactPlay, ulong dtsStart, long spr, long scl);

    long SprCur(void);
    void Stop(long sii);
    void StopAll(long scl = sclNil);
    void Pause(long sii);
    void PauseAll(long scl = sclNil);
    void Resume(long sii);
    void ResumeAll(long scl = sclNil);
    bool FPlaying(long sii);
    bool FPlayingAll(long scl = sclNil);
    void Flush(void);
};

extern ulong LuVolScale(ulong luVolSys, long vlm);
extern ulong vluSysVolFake;

#endif //! SNDM_H
