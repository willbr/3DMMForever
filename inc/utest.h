/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    utest.h: Socrates main app class

    Primary Author: ******
    Review Status: REVIEWED - any changes to this file must be reviewed!

***************************************************************************/
#ifndef UTEST_H
#define UTEST_H

/****************************************
    KidWorld for the App class
****************************************/
typedef class KidWorld *PKidWorld;
#define KidWorld_PAR WorldOfKidspace
#define kclsKidWorld 'KWA'
class KidWorld : public KidWorld_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    PMaskedBitmapMBMP _pmbmp; // MaskedBitmapMBMP to draw in KidWorld (may be pvNil)
    bool _fAskForCD;

  public:
    KidWorld(GraphicsObjectBlock *pgcb) : WorldOfKidspace(pgcb)
    {
        _fAskForCD = fTrue;
    }
    ~KidWorld(void);
    virtual void Draw(PGraphicsEnvironment pgnv, RC *prcClip);
    virtual bool FFindFile(PString pstnSrc, PFilename pfni); // for finding AVIs
    virtual bool FModalTopic(PResourceCache prca, ChunkNumber cnoTopic, long *plwRet);
    void SetMbmp(PMaskedBitmapMBMP pmbmp);
    void SetCDPrompt(bool fAskForCD)
    {
        _fAskForCD = fAskForCD;
    }
    bool FAskForCD(void)
    {
        return _fAskForCD;
    }
};

//
// If you change anything for the registry, notify SeanSe for setup changes.
//
#define kszSocratesKey PszLit("Software\\Microsoft\\Microsoft Kids\\3D Movie Maker")
#define kszWaveOutMsgValue PszLit("WaveOutMsg")
#define kszMidiOutMsgValue PszLit("MidiOutMsg")
#define kszGreaterThan8bppMsgValue PszLit("GreaterThan8bppMsg")
#define kszSwitchResolutionValue PszLit("SwitchResolution")
#define kszHomeDirValue PszLit("HomeDirectory")
#define kszInstallDirValue PszLit("InstallDirectory")
#define kszProductsKey PszLit("Software\\Microsoft\\Microsoft Kids\\3D Movie Maker\\Products")
#define kszUserDataValue PszLit("UserData")
#define kszBetterSpeedValue PszLit("BetterSpeed")
#define kszSkipSplashScreenValue PszLit("SkipSplashScreen")

// FGetSetRegKey flags
enum
{
    fregNil = 0,
    fregSetKey = 0x01,
    fregSetDefault = 0x02,
    fregString = 0x04,
    fregBinary = 0x08, // not boolean
    fregMachine = 0x10
};

/****************************************
    The app class
****************************************/
typedef class APP *PAPP;
#define APP_PAR ApplicationBase
#define kclsAPP 'APP'
class APP : public APP_PAR
{
    RTCLASS_DEC
    CMD_MAP_DEC(APP)
    ASSERT
    MARKMEM

  protected:
    bool _fDontReportInitFailure; // init failure was already reported
    bool _fOnscreenDrawing;
    PChunkyFile _pcfl;                   // resource file for app
    PStudio _pstdio;               // Current studio
    PTheater _ptatr;                 // Current theater
    PChunkyResourceManager _pcrmAll;                // The app ChunkyResourceManager -- all crfs are loaded into this.
    PDynamicArray _pglicrfBuilding;         // List of crfs in _pcrmAll belonging to Building.
    PDynamicArray _pglicrfStudio;           // List of crfs in _pcrmAll belonging to Studio.
    bool _fDontMinimize : 1,      // "/M" command-line switch
        _fSlowCPU : 1,            // running on slow CPU
        _fSwitchedResolution : 1, // we successfully switched to 640x480 mode
        _fMainWindowCreated : 1, _fMinimized : 1,
        _fRunInWindow : 1, // run in a window (as opposed to fullscreen)
        _fFontError : 1,   // Have we already seen a font error?
        _fInPortfolio : 1; // Is the portfolio active?
    PCommandExecutionManager _pcex;            // Pointer to suspended cex.
    Filename _fniPortfolioDoc;  // document last opened in portfolio
    PMovie _pmvieHandoff;   // Stores movie for studio to use
    PKidWorld _pkwa;            // Kidworld for App
    PStringTable_GST _pgstBuildingFiles;
    PStringTable_GST _pgstStudioFiles;
    PStringTable_GST _pgstSharedFiles;
    PStringTable_GST _pgstApp;        // Misc. app global strings
    String _stnAppName;      // App name
    String _stnProductLong;  // Long version of product name
    String _stnProductShort; // Short version of product name
    String _stnUser;         // User's name
    long _sidProduct;
    Filename _fniCurrentDir; // fni of current working directory
    Filename _fniExe;        // fni of this executable file
    Filename _fniMsKidsDir;  // e.g., \mskids
    Filename _fniUsersDir;   // e.g., \mskids\users
    Filename _fniMelanieDir; // e.g., \mskids\users\melanie
    Filename _fniProductDir; // e.g., \mskids\3dmovie or \mskids\otherproduct
    Filename _fniUserDir;    // User's preferred directory
    Filename _fni3DMovieDir; // e.g., \mskids\3dMovie
    long _dypTextDef;   // Default text height

    long _cactDisable; // disable count for keyboard accelerators
#ifdef BUG1085
    long _cactCursHide; // hide count for cursor
    long _cactCursSav;  // saved count for cursor
#endif

    //
    //
    //
    bool _fDown;
    long _cactToggle;

#ifdef WIN
    HACCEL _haccel;
    HACCEL _haccelGlobal;
#endif

  protected:
    bool _FAppAlreadyRunning(void);
    void _TryToActivateWindow(void);
    bool _FEnsureOS(void);
    bool _FEnsureAudio(void);
    bool _FEnsureVideo(void);
    bool _FEnsureColorDepth(void);
    bool _FEnsureDisplayResolution(void);
    bool _FDisplaySwitchSupported(void);
    void _ParseCommandLine(void);
    void _SkipToSpace(char **ppch);
    void _SkipSpace(char **ppch);
    bool _FEnsureProductNames(void);
    bool _FFindProductDir(PStringTable_GST pgst);
    bool _FQueryProductExists(String *pstnLong, String *pstnShort, Filename *pfni);
    bool _FFindMsKidsDir(void);
    bool _FFindMsKidsDirAt(Filename *path);
    bool _FCantFindFileDialog(PString pstn);
    bool _FGenericError(PSTZ message);
    bool _FGenericError(PString message);
    bool _FGenericError(Filename *path);
    bool _FGetUserName(void);
    bool _FGetUserDirectories(void);
    bool _FReadUserData(void);
    bool _FWriteUserData(void);
    bool _FDisplayHomeLogo(bool fSkipSplashScreen);
    bool _FDetermineIfSlowCPU(void);
    bool _FOpenResourceFile(void);
    bool _FInitKidworld(void);
    bool _FInitProductNames(void);
    bool _FReadTitlesFromReg(PStringTable_GST *ppgst);
    bool _FInitTdt(void);
    PStringTable_GST _PgstRead(ChunkNumber cno);
    bool _FReadStringTables(void);
    bool _FSetWindowTitle(void);
    bool _FInitCrm(void);
    bool _FAddToCrm(PStringTable_GST pgstFiles, PChunkyResourceManager pcrm, PDynamicArray pglFiles);
    bool _FInitBuilding(void);
    bool _FInitStudio(PFilename pfniUserDoc, bool fFailIfDocOpenFailed = fTrue);
    void _GetWindowProps(long *pxp, long *pyp, long *pdxp, long *pdyp, DWORD *pdwStyle);
    void _RebuildMainWindow(void);
    bool _FSwitch640480(bool fTo640480);
    bool _FDisplayIs640480(void);
    bool _FShowSplashScreen(void);
    bool _FPlaySplashSound(void);
    PMovie _Pmvie(void);
    void _CleanupTemp(void);
#ifdef WIN
    bool _FSendOpenDocCmd(HWND hwnd, PFilename pfniUserDoc);
    bool _FProcessOpenDocCmd(void);
#endif // WIN

    // ApplicationBase methods that we override
    virtual bool _FInit(ulong grfapp, ulong grfgob, long ginDef);
    virtual bool _FInitOS(void);
    virtual bool _FInitMenu(void)
    {
        return fTrue;
    } // no menubar
    virtual void _CopyPixels(PGraphicsEnvironment pgvnSrc, RC *prcSrc, PGraphicsEnvironment pgnvDst, RC *prcDst);
    virtual void _FastUpdate(PGraphicsObject pgob, PRegion pregnClip, ulong grfapp = fappNil, PGraphicsPort pgpt = pvNil);
    virtual void _CleanUp(void);
    virtual void _Activate(bool fActive);
    virtual bool _FGetNextEvt(PEVT pevt);
#ifdef WIN
    virtual bool _FFrameWndProc(HWND hwnd, uint wm, WPARAM wParam, LPARAM lw, long *plwRet);
#endif // WIN

  public:
    APP(void)
    {
        _dypTextDef = 0;
    }

    // Overridden ApplicationBase functions
    virtual void GetStnAppName(PString pstn);
    virtual long OnnDefVariable(void);
    virtual long DypTextDef(void);
    virtual tribool TQuerySaveDoc(PDocumentBase pdocb, bool fForce);
    virtual void Quit(bool fForce);
    virtual void UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp = fappNil);
    virtual void Run(ulong grfapp, ulong grfgob, long ginDef);
#ifdef BUG1085
    virtual void HideCurs(void);
    virtual void ShowCurs(void);

    // New cursor methods
    void PushCurs(void);
    void PopCurs(void);
#endif // BUG 1085

    // Command processors
    bool FCmdLoadStudio(PCommand pcmd);
    bool FCmdLoadBuilding(PCommand pcmd);
    bool FCmdTheaterOpen(PCommand pcmd);
    bool FCmdTheaterClose(PCommand pcmd);
    bool FCmdIdle(PCommand pcmd);
    bool FCmdInfo(PCommand pcmd);
    bool FCmdPortfolioClear(PCommand pcmd);
    bool FCmdPortfolioOpen(PCommand pcmd);
    bool FCmdDisableAccel(PCommand pcmd);
    bool FCmdEnableAccel(PCommand pcmd);
    bool FCmdInvokeSplot(PCommand pcmd);
    bool FCmdExitStudio(PCommand pcmd);
    bool FCmdDeactivate(PCommand pcmd);

    static bool FInsertCD(PString pstnTitle);
    void DisplayErrors(void);
    void SetPortfolioDoc(PFilename pfni)
    {
        _fniPortfolioDoc = *pfni;
    }
    void GetPortfolioDoc(PFilename pfni)
    {
        *pfni = _fniPortfolioDoc;
    }
    void SetFInPortfolio(bool fInPortfolio)
    {
        _fInPortfolio = fInPortfolio;
    }
    bool FInPortfolio(void)
    {
        return _fInPortfolio;
    }

    PStudio Pstdio(void)
    {
        return _pstdio;
    }
    PKidWorld Pkwa(void)
    {
        return _pkwa;
    }
    PChunkyResourceManager PcrmAll(void)
    {
        return _pcrmAll;
    }
    bool FMinimized()
    {
        return _fMinimized;
    }

    bool FGetStnApp(long ids, PString pstn)
    {
        return _pgstApp->FFindExtra(&ids, pstn);
    }
    void GetStnProduct(PString pstn)
    {
        *pstn = _stnProductLong;
    }
    void GetStnUser(PString pstn)
    {
        *pstn = _stnUser;
    }
    void GetFniExe(PFilename pfni)
    {
        *pfni = _fniExe;
    }
    void GetFniProduct(PFilename pfni)
    {
        *pfni = _fniProductDir;
    }
    void GetFniUsers(PFilename pfni)
    {
        *pfni = _fniUsersDir;
    }
    void GetFniUser(PFilename pfni)
    {
        *pfni = _fniUserDir;
    }
    void GetFniMelanie(PFilename pfni)
    {
        *pfni = _fniMelanieDir;
    }
    long SidProduct(void)
    {
        return _sidProduct;
    }
    bool FGetOnn(PString pstn, long *ponn);
    void MemStat(long *pdwTotalPhys, long *pdwAvailPhys = pvNil);
    bool FSlowCPU(void)
    {
        return _fSlowCPU;
    }

    // Kid-friendly modal dialog stuff:
    void EnsureInteractive(void)
    {
#ifdef WIN
        if (_fMinimized || GetForegroundWindow() != vwig.hwndApp)
#else
        if (_fMinimized)
#endif
        {
#ifdef WIN
            SetForegroundWindow(vwig.hwndApp);
            ShowWindow(vwig.hwndApp, SW_RESTORE);
#else  //! WIN
            RawRtn();
#endif //! WIN
        }
    }
    tribool TModal(PResourceCache prca, long tpc, PString pstnBackup = pvNil, long bkBackup = ivNil, long stidSubst = ivNil,
                   PString pstnSubst = pvNil);

    // Enable/disable accelerator keys
    void DisableAccel(void);
    void EnableAccel(void);

    // Registry access function
    bool FGetSetRegKey(PSZ pszValueName, void *pvData, long cbData, ulong grfreg = fregSetDefault,
                       bool *pfNoValue = pvNil);

    // Movie handoff routines
    void HandoffMovie(PMovie pmvie);
    PMovie PmvieRetrieve(void);

    // Determines whether screen savers should be blocked.
    virtual bool FAllowScreenSaver(void);
};

#define vpapp ((APP *)vpappb)

#endif //! UTEST_H
