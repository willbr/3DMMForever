/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    bkgd.h: Background class

    Primary Author: ******
    Review Status: REVIEWED - any changes to this file must be reviewed!

    BASE ---> BaseCacheableObject ---> Background

***************************************************************************/
#ifndef BKGD_H
#define BKGD_H

/****************************************
    Background on file
****************************************/
struct BackgroundFile
{
    short bo;
    short osk;
    byte bIndexBase;
    byte bPad;
    short swPad;
};
const ByteOrderMask kbomBkgdf = 0x50000000;

/****************************************
    Specifies a light's kind, position,
    orientation, and brightness
****************************************/
struct LightPosition
{
    BMAT34 bmat34;
    BRS rIntensity;
    long lt; // light type
};
const ByteOrderMask kbomLite = 0xfffffff0;

/****************************************
    Specifies a camera for a view
****************************************/
typedef union _apos {
    struct
    {
        BRS xrPlace; // Initial Actor Placement point
        BRS yrPlace;
        BRS zrPlace;
    };
    BVEC3 bvec3Actor;
} APOS;

struct CameraPosition
{
    short bo;
    short osk;
    BRS zrHither; // Hither (near) plane
    BRS zrYon;    // Yon (far) plane
    BRA aFov;     // Field of view
    short swPad;
    APOS apos;
    BMAT34 bmat34Cam; // Camera view matrix
    // APOS rgapos[];
};
const ByteOrderMask kbomCamOld = 0x5f4fc000;
const ByteOrderMask kbomCam = BomField(
    kbomSwapShort,
    BomField(kbomSwapShort,
             BomField(kbomSwapLong,
                      BomField(kbomSwapLong,
                               BomField(kbomSwapShort,
                                        BomField(kbomLeaveShort,
                                                 BomField(kbomSwapLong,
                                                          BomField(kbomSwapLong, BomField(kbomSwapLong, 0)))))))));

// Note that CameraPosition is too big for a complete kbomCam.  To SwapBytes one,
// SwapBytesBom the cam, then SwapBytesRgLw from bmat34Cam on.

/****************************************
    Background Default Sound
****************************************/
struct BackgroundDefaultSound
{
    short bo;
    short osk;
    long vlm;
    bool fLoop;
    TAG tagSnd;
};
const ByteOrderMask kbomBds = 0x5f000000 | kbomTag >> 8;

/****************************************
    The background class
****************************************/
typedef class Background *PBackground;
#define Background_PAR BaseCacheableObject
#define kclsBackground 'BKGD'
class Background : public Background_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    BACT *_prgbactLight; // array of light br_actors
    BLIT *_prgblitLight; // array of light data
    long _cbactLight;    // count of lights
    bool _fLites;        // lights are on
    bool _fLeaveLitesOn; // Don't turn out the lights
    long _ccam;          // count of cameras in this background
    long _icam;          // current camera
    BMAT34 _bmat34Mouse; // camera matrix for mouse model
    BRA _braRotY;        // Y rotation of current camera
    ChunkNumber _cnoSnd;         // background sound
    STN _stn;            // name of this background
    PGL _pglclr;         // palette for this background
    byte _bIndexBase;    // first index for palette
    long _iaposLast;     // Last placement point we used
    long _iaposNext;     // Next placement point to use
    PGL _pglapos;        // actor placement point(s) for current view
    BRS _xrPlace;
    BRS _yrPlace;
    BRS _zrPlace;
    BackgroundDefaultSound _bds;   // background default sound
    BRS _xrCam; // camera position in worldspace
    BRS _yrCam;
    BRS _zrCam;

  protected:
    bool _FInit(PCFL pcfl, ChunkTag ctg, ChunkNumber cno);
    long _Ccam(PCFL pcfl, ChunkTag ctg, ChunkNumber cno);
    void _SetupLights(PGL pgllite);

  public:
    static bool FAddTagsToTagl(PTAG ptagBkgd, PTAGL ptagl);
    static bool FCacheToHD(PTAG ptagBkgd);
    static bool FReadBkgd(PChunkyResourceFile pcrf, ChunkTag ctg, ChunkNumber cno, PDataBlock pblck, PBaseCacheableObject *ppbaco, long *pcb);
    ~Background(void);
    void GetName(PSTN pstn);

    void TurnOnLights(PBWLD pbwld);
    void TurnOffLights(void);
    bool FLeaveLitesOn(void)
    {
        return _fLeaveLitesOn;
    }
    void SetFLeaveLitesOn(bool fLeaveLitesOn)
    {
        _fLeaveLitesOn = fLeaveLitesOn;
    }

    long Ccam(void)
    {
        return _ccam;
    } // count of cameras in background
    long Icam(void)
    {
        return _icam;
    }                                        // currently selected camera
    bool FSetCamera(PBWLD pbwld, long icam); // change camera to icam

    void GetMouseMatrix(BMAT34 *pbmat34);
    BRA BraRotYCamera(void)
    {
        return _braRotY;
    }
    void GetActorPlacePoint(BRS *pxr, BRS *pyr, BRS *pzr);
    void ReuseActorPlacePoint(void);

    void GetDefaultSound(PTAG ptagSnd, long *pvlm, bool *pfLoop)
    {
        *ptagSnd = _bds.tagSnd;
        *pvlm = _bds.vlm;
        *pfLoop = _bds.fLoop;
    }

    bool FGetPalette(PGL *ppglclr, long *piclrMin);
    void GetCameraPos(BRS *pxr, BRS *pyr, BRS *pzr);

#ifdef DEBUG
    // Authoring only.  Writes a special file with the given place info.
    bool FWritePlaceFile(BRS xrPlace, BRS yrPlace, BRS zrPlace);
#endif // DEBUG
};

#endif BKGD_H
