/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    Status: All changes must be code reviewed.

    Movie Stuff

        A single view on a movie (MovieView)

                DocumentDisplayGraphicsObject  	--->	MovieView

        Callbacks to client (MovieClientCallbacks)

                BASE 	--->	MovieClientCallbacks

        A single movie (Movie)

                DocumentBase	--->	Movie

***************************************************************************/

#ifndef MOVIE_H
#define MOVIE_H

//
// Tools that can be "loaded" on the mouse cursor
//
enum
{
    toolPlace, // (to position an actor when you first add it)
    toolCompose,
    toolAction, // action is in _anidTool if we're motion filling
    toolTweak,
    toolRotateX,
    toolRotateY,
    toolRotateZ,
    toolCostumeCmid, // cmid is in _cmidTool
    toolSquashStretch,
    toolSoonerLater,
    toolResize,
    toolNormalizeRot,
    toolCopyObject,
    toolCutObject,
    toolPasteObject,
    toolCopyRte,
    toolDefault,
    toolTboxMove,
    toolTboxUpDown,
    toolTboxLeftRight,
    toolTboxFalling,
    toolTboxRising,
    toolSceneNuke,
    toolIBeam,
    toolSceneChop,
    toolSceneChopBack,
    toolCutText,
    toolCopyText,
    toolPasteText,
    toolPasteRte,
    toolActorEasel,
    toolActorSelect,
    toolActorNuke,
    toolTboxPaintText,
    toolTboxFillBkgd,
    toolTboxStory,
    toolTboxCredit,
    toolNormalizeSize,
    toolRecordSameAction,
    toolSounder,
    toolLooper,
    toolMatcher,
    toolTboxFont,
    toolTboxStyle,
    toolTboxSize,
    toolListener,
    toolAddAFrame,
    toolFWAFrame,
    toolRWAFrame,
    toolComposeAll,
    toolUndo,     // For playing UI Sound only
    toolRedo,     // For playing UI Sound only
    toolFWAScene, // For playing UI Sound only
    toolRWAScene, // For playing UI Sound only

    toolLimMvie
};

//
// Used to tell the client the change the state of the undo UI
//
enum
{
    undoDisabled = 1,
    undoUndo,
    undoRedo
};

//
// grfbrws flags
//
enum
{
    fbrwsNil = 0,
    fbrwsProp = 1, // fTrue implies prop or 3d
    fbrwsTdt = 2   // fTrue means this is a 3-D Text object
};

//
//
// A class for handling a single Movie view
//
//

//
// The class definition
//
#define MovieView_PAR DocumentDisplayGraphicsObject

typedef class MovieView *PMovieView;
#define kclsMovieView 'MVU'
class MovieView : public MovieView_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM
    CMD_MAP_DEC(MovieView)

  protected:
    /* Make these static; we want to be able to set and restore without having
        an actual MovieView, and they shouldn't be getting set per-MovieView anyway */
    static bool _fKbdDelayed;  // fTrue == we have delayed the keyboard
    static long _dtsKbdDelay;  // System keyboard delay before first repeat
    static long _dtsKbdRepeat; // System keyboard delay between repeats

    long _dxp;
    long _dyp; // width and height rendered area.

    bool _fTrackingMouse; // Is the mouse currently being tracked?
    long _xpPrev;         // X location of the mouse.
    long _ypPrev;         // Y location of the mouse.
    BRS _dzrPrev;         // Z motion of the "mouse" (arrow keys)
    long _grfcust;        // Options in effect when mouse was down.
    PCURS _pcursDefault;  // Default cursor for when a tool is not applicable.
    PActor _pactrListener; // Pactr of the actor being auditioned
    PActor _pactrRestore;  // Restore for actor recording

    long _anidTool;     // Current selected action
    TAG _tagTool;       // Tag associated with current tool.
    PTMPL _ptmplTool;   // Template associated with the current tool.
    long _cmidTool;     // Costume id associated with current tool.
    bool _fCyclingCels; // Are we cycling cels in toolRecord?
    long _tool;         // Current tool loaded on cursor

    // REVIEW Seanse(SeanSe): Movie should not be creating/mucking with actor undo
    //   objects.  V2.666 should revisit this issue and see if we can get Actor to
    //   do all its own undo objects (e.g. Growing over a long drag could become
    //   a StartGrow/Grow/EndGrow sequence).  This also effects _pactrRestore.
    PActorUndo _paund;        // Actor undo object to save from mouse down to drag.
    ulong _tsLast;       // Last time a cell was recorded.
    ulong _tsLastSample; // Last time mouse/kbd sampled
    RC _rcFrame;         // Frame for creating a text box.

    BRS _rgrAxis[3][3];   // Conversion from mouse points to 3D points.
    bool _fRecordDefault; // fTrue = Record; fFalse = Rerecord

    bool _fPause : 1;             // fTrue if pausing play until a click.
    bool _fTextMode : 1;          // fTrue if Text boxes are active.
    bool _fRespectGround : 1;     // fTrue if Y=0 is enforced.
    bool _fSetFRecordDefault : 1; // fTrue if using hotkeys to rerecord.
    bool _fMouseOn : 1;
    bool _fEntireScene : 1; // Does positioning effect the entire scene?

    bool _fMouseDownSeen; // Was the mouse depressed during a place.
    PActor _pactrUndo;     // Actor to use for undo object when roll-calling.

    AbstractColor _acr;         // Color for painting text.
    long _onn;        // Font for text
    long _dypFont;    // Font size for text
    ulong _grfont;    // Font style for text
    long _lwLastTime; // State variable for the last time through.

    MovieView(PDocumentBase pdocb, PGCB pgcb) : DocumentDisplayGraphicsObject(pdocb, pgcb)
    {
    }

    //
    // Clipboard support
    //
    bool _FCopySel(PDocumentBase *ppdocb, bool fRteOnly);
    void _ClearSel(void);
    bool _FPaste(PClipboardObject pclip);

    void _PositionActr(BRS dxrWld, BRS dyrWld, BRS dzrWld);
    void _MouseDown(CMD_MOUSE *pcmd);
    void _MouseDrag(CMD_MOUSE *pcmd);
    void _MouseUp(CMD_MOUSE *pcmd);

    void _ActorClicked(PActor pactr, bool fDown);

  public:
    static void SlowKeyboardRepeat(void);
    static void RestoreKeyboardRepeat(void);

    //
    // Constructors and desctructors
    //
    static MovieView *PmvuNew(PMovie pmvie, PGCB pgcb, long dxy, long dyp);
    ~MovieView(void);

    //
    // Accessor for getting the owning movie
    //
    PMovie Pmvie()
    {
        return (PMovie)_pdocb;
    }

    //
    // Command handlers
    //
    virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
    virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
    virtual bool FCmdClip(Command *pcmd);
    virtual bool FCmdUndo(PCommand pcmd);
    virtual bool FCloseDoc(bool fAssumeYes, bool fSaveDDG = fFalse);
    virtual bool FCmdSave(PCommand pcmd);
    bool FDoClip(long tool);
    bool FCmdIdle(Command *pcmd); // Called whenever an idle loop is seen.
    bool FCmdRollOff(Command *pcmd);

    //
    // View specific functions.
    //
    void SetTool(long tool);
    long Tool(void)
    {
        return _tool;
    }
    long AnidTool(void)
    {
        return _anidTool;
    }
    long CmidTool(void)
    {
        return _cmidTool;
    }
    PTAG PtagTool(void)
    {
        return &_tagTool;
    }
    void SetAnidTool(long anid)
    {
        _anidTool = anid;
    }
    void SetTagTool(PTAG ptag);
    void SetCmidTool(long cmid)
    {
        _cmidTool = cmid;
    }
    void StartPlaceActor(bool fEntireScene = fFalse);
    void EndPlaceActor(void);
    void WarpCursToCenter(void);
    void WarpCursToActor(PActor pactr);
    void AdjustCursor(long xp, long yp);
    void MouseToWorld(BRS dxrMouse, BRS dyrMouse, BRS dzrMouse, BRS *pdxrWld, BRS *pdyrWld, BRS *pdzrWld, bool fRecord);
    void SetAxis(BRS rgrAxis[3][3])
    {
        BltPb(rgrAxis, _rgrAxis, size(BRS) * 9);
    }
    void SetFRecordDefault(bool f)
    {
        _fRecordDefault = f;
    }
    void SetFRespectGround(bool f)
    {
        _fRespectGround = f;
    }
    bool FRecordDefault()
    {
        return _fRecordDefault;
    }
    void PauseUntilClick(bool fPause)
    {
        _fPause = fPause;
    }
    bool FPausing(void)
    {
        return _fPause;
    }
    bool FTextMode(void)
    {
        return _fTextMode;
    }
    bool FActrMode(void)
    {
        return !_fTextMode;
    }
    bool FRespectGround(void)
    {
        return _fRespectGround;
    }
    void SetActrUndo(PActor pactr)
    {
        _pactrUndo = pactr;
    }
    void SetPaintAcr(AbstractColor acr)
    {
        _acr = acr;
    }
    AbstractColor AcrPaint(void)
    {
        return _acr;
    }
    void SetOnnTextCur(long onn)
    {
        _onn = onn;
    }
    long OnnTextCur(void)
    {
        return _onn;
    }
    void SetDypFontTextCur(long dypFont)
    {
        _dypFont = dypFont;
    }
    long DypFontTextCur(void)
    {
        return _dypFont;
    }
    void SetStyleTextCur(ulong grfont)
    {
        _grfont = grfont;
    }
    ulong GrfontStyleTextCur(void)
    {
        return _grfont;
    }

    //
    // Routines for communicating with the framework
    //
    void Draw(PGraphicsEnvironment pgnv, RC *prcClip);
};

//
//
// Movie Client Callbacks.  Used for filling in
// parameters for this movie, and for notifying
// client of state changes.
//
//

#define MovieClientCallbacks_PAR BASE

class MovieClientCallbacks;

typedef MovieClientCallbacks *PMovieClientCallbacks;
#define kclsMovieClientCallbacks 'MCC'
class MovieClientCallbacks : public MovieClientCallbacks_PAR
{
  protected:
    long _dxp;
    long _dyp;
    long _cbCache;

  public:
    MovieClientCallbacks(long dxp, long dyp, long cbCache)
    {
        _dxp = dxp;
        _dyp = dyp;
        _cbCache = cbCache;
    }
    virtual long Dxp(void)
    {
        return _dxp;
    } // Width of the rendering area
    virtual long Dyp(void)
    {
        return _dyp;
    } // Height of the rendering area
    virtual long CbCache(void)
    {
        return _cbCache;
    } // Number of bytes to use for caching.
    virtual void SetCurs(long tool)
    {
    } // Sets the cursor based on the tool, may be pvNil.
    virtual void UpdateRollCall(void)
    {
    } // Tells the client to update its roll call.
    virtual void UpdateAction(void)
    {
    } // Tells the client to update its action menu.
    virtual void UpdateScrollbars(void)
    {
    } // Tells the client to update its scrollbars.
    virtual void PlayStopped(void)
    {
    } // Tells the client that playback was stopped internally.
    virtual void ChangeTool(long tool)
    {
    } // Tells the client that the tool was changed internally.
    virtual void SceneNuked(void)
    {
    } // Tells the client that a scene was nuked.
    virtual void SceneUnnuked(void)
    {
    } // Tells the client that a scene was nuked and now undone.
    virtual void ActorNuked(void)
    {
    } // Tells the client that an actor was nuked.
    virtual void EnableActorTools(void)
    {
    } // Tells the client that the first actor was added.
    virtual void EnableTboxTools(void)
    {
    } // Tells the client that the first textbox was added.
    virtual void TboxSelected(void)
    {
    } // Tells the client that a new text box was selected.
    virtual void ActorSelected(long arid)
    {
    } // Tells the client that an actor was selected
    virtual void ActorEasel(bool *pfActrChanged)
    {
    } // Lets client edit 3-D Text or costume
    virtual void SetUndo(long undo)
    {
    } // Tells the client the state of the undo buffer.
    virtual void SceneChange(void)
    {
    } // Tells the client that a different scene is the current one
    virtual void PauseType(WIT wit)
    {
    } // Tells the client of a new pause type for this frame.
    virtual void Recording(bool fRecording, bool fRecord)
    {
    } // Tells the client that the movie engine is recording or not.
    virtual void StartSoonerLater(void)
    {
    } // Tells the client that an actor is selected for sooner/latering.
    virtual void EndSoonerLater(void)
    {
    } // Tells the client that an actor is done for sooner/latering.
    virtual void NewActor(void)
    {
    } // Tells the client that an actor has just been placed.
    virtual void StartActionBrowser(void)
    {
    } // Tells the client to start up the action browser.
    virtual void StartListenerEasel(void)
    {
    } // Tells the client to start up the listener easel.
    virtual bool GetFniSave(Filename *pfni, long lFilterLabel, long lFilterExt, long lTitle, LPTSTR lpstrDefExt,
                            PString pstnDefFileName)
    {
        return fFalse;
    } // Tells the client to start up the save portfolio.
    virtual void PlayUISound(long tool, long grfcust = 0)
    {
    } // Tells the client to play sound associated with use of tool.
    virtual void StopUISound(void)
    {
    } // Tells the client to stop sound associated with use of tools.
    virtual void UpdateTitle(PString pstnTitle)
    {
    } // Tells the client that the movie name has changed.
    virtual void EnableAccel(void)
    {
    } // Tells the client to enable keyboard accelerators.
    virtual void DisableAccel(void)
    {
    } // Tells the client to disable keyboard accelerators.
    virtual void GetStn(long ids, PString pstn)
    {
    } // Requests the client to fetch the given ids string.
    virtual long DypTextDef(void)
    {
        return vpappb->DypTextDef();
    }
    virtual long DypTboxDef(void)
    {
        return 14;
    }
    virtual void SetSndFrame(bool fSoundInFrame)
    {
    }
    virtual bool FMinimized(void)
    {
        return fFalse;
    }
    virtual bool FQueryPurgeSounds(void)
    {
        return fFalse;
    }
};

/* A SCENe Descriptor */
typedef struct _scend
{
    /* The first fields are private...the client shouldn't change them, and
        in fact, generally shouldn't even look at them */
    long imvied;     // index of the MVIED for this scene
    ChunkNumber cno;         // the ChunkNumber of this scene chunk
    ChildChunkID chid;       // the original ChildChunkID
    PMaskedBitmapMBMP pmbmp;     // pointer to thumbnail MaskedBitmapMBMP
                     /* The client can read or write the following fields */
    TRANS trans;     // the transition that will occur after this scene
    bool fNuked : 1; // fTrue if this scene has been deleted
} SCEND, *PSCEND;

/* A MoVIE Descriptor */
typedef struct _mvied
{
    PChunkyResourceFile pcrf;    // the file this scene's movie is in
    ChunkNumber cno;      // ChunkNumber of the Movie chunk
    long aridLim; // _aridLim from the Movie
} MVIED, *PMVIED;

/* A Composite MoVIe */
typedef struct _cmvi
{
    PDynamicArray pglmvied; // DynamicArray of movie descriptors
    PDynamicArray pglscend; // DynamicArray of scene descriptors

    void Empty(void);
#ifdef DEBUG
    void MarkMem(void);
#endif
} CMVI, *PCMVI;

//
//
// Movie Class.
//
//

const long kccamMax = 9;

typedef class Movie *PMovie;

#define Movie_PAR DocumentBase
#define kclsMovie 'MVIE'
class Movie : public Movie_PAR
{
    RTCLASS_DEC
    MARKMEM
    ASSERT
    CMD_MAP_DEC(Movie)

  protected:
    long _aridLim; // Highest actor id in use.

    PChunkyResourceFile _pcrfAutoSave; // ChunkyResourceFile/ChunkyFile of auto save file.
    PFIL _pfilSave;     // User's document

    ChunkNumber _cno; // ChunkNumber of movie in current file.

    String _stnTitle; // Title of the movie

    PStringTable _pgstmactr;             // StringTable of actors in the movie (for roll call)
    PScene _pscenOpen;            // Index of current open scene.
    long _cscen;                 // Number of scenes in the movie.
    long _iscen;                 // Number of scene open in the movie.
    bool _fAutosaveDirty : 1;    // Is the movie in memory different than disk
    bool _fFniSaveValid : 1;     // Does _fniSave contain a file name.
    bool _fPlaying : 1;          // Is the movie playing?
    bool _fScrolling : 1;        // During playback only, are we scrolling textboxes
    bool _fPausing : 1;          // Are we executing a pause.
    bool _fIdleSeen : 1;         // fTrue if we have seen an idle since this was cleared.
    bool _fStopPlaying : 1;      // Should we stop the movie playing
    bool _fSoundsEnabled : 1;    // Should we play sounds or not.
    bool _fOldSoundsEnabled : 1; // Old value of above.
    bool _fDocClosing : 1;       // Flags doc is to be closed
    bool _fGCSndsOnClose : 1;    // Garbage collection of sounds on close
    bool _fReadOnly : 1;         // Is the original file read-only?

    PWorld _pbwld;   // The brender world for this movie
    PMSQ _pmsq;     // Message Sound Queue
    Clock _clok;     // Clock for playing the film
    ulong _tsStart; // Time last play started.
    long _cnfrm;    // Number of frames since last play started.

    PMovieClientCallbacks _pmcc; // Parameters and callbacks.

    WIT _wit;     // Pausing type
    long _dts;    // Number of clock ticks to pause.
    TRANS _trans; // Transition type to execute.

    long _vlmOrg; // original SNDM volume, before fadeout, if we are done with fadeout, then 0

#ifdef DEBUG
    bool _fWriteBmps;
    long _lwBmp;
#endif // DEBUG

    PDynamicArray _pglclrThumbPalette; // Palette to use for thumbnail rendering.

  private:
    Movie(void);
    PTagList _PtaglFetch(void);                      // Returns a list of all tags used in movie
    bool _FCloseCurrentScene(void);               // Closes and releases current scene, if any
    bool _FMakeCrfValid(void);                    // Makes sure there is a file to work with.
    bool _FUseTempFile(void);                     // Switches to using a temp file.
    void _MoveChids(ChildChunkID chid, bool fDown);       // Move the chids of scenes in the movie.
    bool _FDoGarbageCollection(PChunkyFile pcfl);        // Remove unused chunks from movie.
    void _DoSndGarbageCollection(bool fPurgeAll); // Remove unused user sounds from movie
    bool _FDoMtrlTmplGC(PChunkyFile pcfl);               // Material and template garbage collection
    ChildChunkID _ChidScenNewSnd(void);                   // Choose an unused chid for a new scene child user sound
    ChildChunkID _ChidMvieNewSnd(void);                   // Choose an unused chid for a new movie child user sound
    void _SetTitle(PFilename pfni = pvNil);            // Set the title of the movie based on given file name.
    bool _FIsChild(PChunkyFile pcfl, ChunkTag ctg, ChunkNumber cno);
    bool _FSetPfilSave(PFilename pfni);

  public:
    //
    // Begin client useable functions
    //

#ifdef DEBUG
    void SetFWriteBmps(bool fWriteBmps)
    {
        if (fWriteBmps && !_fWriteBmps)
            _lwBmp = 0;
        _fWriteBmps = fWriteBmps;
    }
    bool FWriteBmps(void)
    {
        return _fWriteBmps;
    }
#endif // DEBUG

    //
    // Getting views
    //
    PMovieView PmvuCur(void);
    PMovieView PmvuFirst(void);

    //
    // Create and Destroy
    //
    static PMovie PmvieNew(bool fHalfMode, PMovieClientCallbacks pmcc, Filename *pfni = pvNil, ChunkNumber cno = cnoNil);
    // Create a movie and read it if
    //   pfni != pvNil
    static bool FReadRollCall(PChunkyResourceFile pcrf, ChunkNumber cno, PStringTable *ppgst, long *paridLim = pvNil);
    // reads roll call for a given movie
    void ForceSaveAs(void)
    {
        ReleasePpo(&_pfilSave);
        _fFniSaveValid = fFalse;
    }
    void Flush(void);
    bool FReadOnly(void)
    {
        return FPure(_fReadOnly);
    }

    ~Movie(void);

    //
    // MovieClientCallbacks maintenance
    //
    PMovieClientCallbacks Pmcc(void)
    {
        return _pmcc;
    } // Accessor for getting to client callbacks.
    void SetMcc(PMovieClientCallbacks pmcc)
    {
        ReleasePpo(&_pmcc);
        _pmcc = pmcc;
        _pmcc->AddRef();
    }

    //
    // Title stuff
    //
    void GetName(PString pstnTitle); // Gets the title of the movie.
    PString PstnTitle(void)
    {
        return &_stnTitle;
    }
    void ResetTitle(void);

    //
    // Scene stuff
    //
    long Cscen(void) // Returns number of scenes in movie
    {
        return _cscen;
    }
    long Iscen(void) // Returns the current scene number
    {
        return _iscen;
    }
    bool FSwitchScen(long iscen);                                // Loads and returns pointer to scene iscen,
                                                                 //   saving any current scene.
    bool FRemScen(long iscen);                                   // Removes a scene from the movie, and undo
    bool FChangeCam(long camid);                                 // Change the camera view in the scene.
    bool FInsTbox(RC *prc, bool fStory);                         // Insert a text box into the scene.
    bool FHideTbox(void);                                        // Hide selected text box from the scene at this fram.
    bool FNukeTbox(void);                                        // Remove selected text box from the scene.
    void SelectTbox(long itbox);                                 // Select the itbox'th text box in the frame.
    void SetPaintAcr(AbstractColor acr);                                   // Sets color that painting will occur with.
    void SetOnnTextCur(long onn);                                // Sets font that text will be in
    void SetDypFontTextCur(long dypFont);                        // Sets font size that text will be in
    void SetStyleTextCur(ulong grfont);                          // Sets font style that text will be in
    bool FInsActr(PTAG ptag);                                    // Insert an actor into the scene.
    bool FRemActr(void);                                         // Remove selected actor from scene.
    bool FAddOnstage(long arid);                                 // Bring this actor onto the stage.
    bool FRotateActr(BRA xa, BRA ya, BRA za, bool fFromHereFwd); // Rotate selected actor by degrees
    bool FSquashStretchActr(BRS brs);                            // Squash/Stretch selected actor
    bool FSoonerLaterActr(long nfrm);                            // Sooner/Later selected actor
    bool FScaleActr(BRS brs);                                    // Scale selected actor
    bool FCostumeActr(long ibprt, PTAG ptag, long cmid, tribool fCustom);
    bool FAddScen(PTAG ptag);         // Add a scene after the current one, or change
                                      //   change bkgd if scene is empty
    bool FSetTransition(TRANS trans); // Set the transition type for the current scene.
    void Play(void);                  // Start/Stop a movie playing.
    bool FPause(WIT wit, long dts);   // Insert a pause here.

    bool FAddToCmvi(PCMVI pcmvi, long *piscendIns);
    // Add this movie to the CMVI
    bool FSetCmvi(PCMVI pcmvi); // Re-build the movie from the CMVI
    bool _FAddMvieToRollCall(ChunkNumber cno, long aridMin);
    // Updates roll call for an imported movie
    bool _FInsertScend(PDynamicArray pglscend, long iscend, PSCEND pscend);
    // Insert an imported scene
    void _DeleteScend(PDynamicArray pglscend, long iscend);   // Delete an imported scene
    bool _FAdoptMsndInMvie(PChunkyFile pcfl, ChunkNumber cnoScen); // Adopt msnd chunks as children of the movie

    bool FAddBkgdSnd(PTAG ptag, tribool fLoop, tribool fQueue, long vlm = vlmNil,
                     long sty = styNil);                                                              // Adds a sound
    bool FAddActrSnd(PTAG ptag, tribool fLoop, tribool fQueue, tribool fActnCel, long vlm, long sty); // Adds a sound

    //
    // Auto save stuff
    //
    bool FAutoSave(PFilename pfni = pvNil, bool fCleanRollCall = fFalse); // Save movie in temp file
    bool FSaveTagSnd(TAG *ptag)
    {
        return TagManager::FSaveTag(ptag, _pcrfAutoSave, fTrue);
    }
    bool FCopySndFileToMvie(PFIL pfil, long sty, ChunkNumber *pcno, PString pstn = pvNil);
    bool FVerifyVersion(PChunkyFile pcfl, ChunkNumber *pcno = pvNil);
    bool FEnsureAutosave(PChunkyResourceFile *pcrf = pvNil);
    bool FCopyMsndFromPcfl(PChunkyFile pcfl, ChunkNumber cnoSrc, ChunkNumber *pcnoDest);
    bool FResolveSndTag(PTAG ptag, ChildChunkID chid, ChunkNumber cnoScen = cnoNil, PChunkyResourceFile pcrf = pvNil);
    bool FChidFromUserSndCno(ChunkNumber cno, ChildChunkID *pchid);
    void SetDocClosing(bool fClose)
    {
        _fDocClosing = fClose;
    }
    bool FQueryDocClosing(void)
    {
        return _fDocClosing;
    }
    bool FQueryGCSndsOnClose(void)
    {
        return _fGCSndsOnClose;
    }
    bool FUnusedSndsUser(bool *pfHaveValid = pvNil);

    //
    // Roll call
    //
    bool FGetArid(long iarid, long *parid, PString pstn, long *pcactRef,
                  PTAG ptagTmpl = pvNil); // return actors one by one
    bool FChooseArid(long arid);          // user chose arid in roll call
    long AridSelected(void);
    bool FGetName(long arid, PString pstn);      // Return the name of a specific actor.
    bool FNameActr(long arid, PString pstn);     // Set the name of this actor.
    void ChangeActrTag(long arid, PTAG ptag); // Change an actor's TMPL tag
    long CmactrMac(void)
    {
        AssertThis(0);
        return _pgstmactr->IvMac();
    }
    bool FIsPropBrwsIarid(long iarid); // Identify the roll call browser iarids
    bool FIsIaridTdt(long iarid);      // 3d spletter

    //
    // Overridden DocumentBase functions
    //
    bool FGetFni(Filename *pfni);                  // For saving to a file
    bool FSave(long cid);                     // For saving to a file, (calls FGetFni and FSaveToFni)
    bool FSaveToFni(Filename *pfni, bool fSetFni); // For doing a Save As or Save
    PDocumentMDIWindow PdmdNew(void);                       // Do not use!
    bool FGetFniSave(Filename *pfni);              // For saving via the portfolio.

    //
    // Drawing stuff
    //
    void InvalViews(void);       // Invalidates all views on the movie.
    void InvalViewsAndScb(void); // Invalidates all views of movie and scroll bars.
    void MarkViews(void);        // Marks all views on the movie.
    TRANS Trans(void)
    {
        return _trans;
    } // Current transition in effect.
    void SetTrans(TRANS trans)
    {
        _trans = trans;
    }                                                                 // Set transition
    void DoTrans(PGraphicsEnvironment pgnvDst, PGraphicsEnvironment pgnvSrc, RC *prcDst, RC *prcSrc); // Draw current transition into GNVs

    //
    // Sound stuff
    //
    bool FSoundsEnabled(void)
    {
        return (_fSoundsEnabled);
    }
    void SetFSoundsEnabled(bool f)
    {
        _fSoundsEnabled = f;
    }

    //
    // End client callable functions
    //

    //
    // Begin internal movie engine functions
    //

    //
    // Command handlers
    //
    bool FCmdAlarm(Command *pcmd);  // Called at timer expiration (playback).
    bool FCmdRender(Command *pcmd); // Called to render a frame during playback.

    //
    // Automated test APIs
    //
    long LwQueryExists(long lwType, long lwId);     // Called by other apps to find if an actor/tbox exists.
    long LwQueryLocation(long lwType, long lwId);   // Called by other apps to find where actor/tbox exists.
    long LwSetMoviePos(long lwScene, long lwFrame); // Called by other apps to set the movie position.

    //
    // Scene stuff
    //
    PScene Pscen(void)
    {
        return _pscenOpen;
    }                                           // The currently open scene.
    bool FInsScenCore(long iscen, Scene *pscen); // Insert this scene as scene number.
    bool FNewScenInsCore(long iscen);           // Inserts a blank scene before iscen
    bool FRemScenCore(long iscen);              // Removes a scene from the movie
    bool FPasteActr(PActor pactr);               // Pastes an actor into current scene.
    bool FPasteActrPath(PActor pactr);           // Pastes the path onto selected actor.
    PMSQ Pmsq(void)
    {
        return _pmsq;
    } // Sound queue

    //
    // Runtime Pausing
    //
    void DoPause(WIT wit, long dts) // Make the movie pause during run.
    {
        _wit = wit;
        _dts = dts;
    }

    //
    // Material stuff
    //
    bool FInsertMtrl(PMTRL pmtrl, PTAG ptag); // Inserts a material into this movie.

    //
    // 3-D Text stuff
    //
    bool FInsTdt(PString pstn, long tdts, PTAG ptagTdf); // Inserts a TDT into this movie.
    bool FChangeActrTdt(PActor pactr, PString pstn, long tdts, PTAG ptagTdf);

    //
    // Marking (overridden DocumentBase methods)
    //
    virtual bool FDirty(void) // Has the movie changed since last saved?
    {
        return _fAutosaveDirty || _fDirty;
    }
    virtual void SetDirty(bool fDirty = fTrue) // Mark the movie as changed.
    {
        _fAutosaveDirty = fDirty;
    }

    //
    // Roll call
    //
    bool FAddToRollCall(Actor *pactr, PString pstn);                    // Add an actor to the roll call
    void RemFromRollCall(Actor *pactr, bool fDelIfOnlyRef = fFalse); // Remove an actor from the roll call.
    void BuildActionMenu(void);                                     // Called when the selected actor has changed.

    //
    // Overridden DocumentBase functions
    //
    PDocumentDisplayGraphicsObject PddgNew(PGCB pgcb);    // For creating a view on a movie.
    bool FAddUndo(PMovieUndo pmunb); // Add an item to the undo list
    void ClearUndo(void);

    //
    // Accessors for MVUs only.
    //
#ifdef DEBUG
    void SetFPlaying(bool f)
    {
        _fPlaying = f;
        if (!f)
            SetFWriteBmps(fFalse);
    }  // Set the playing flag.
#else  // DEBUG
    void SetFPlaying(bool f)
    {
        _fPlaying = f;
    } // Set the playing flag.
#endif // !DEBUG
    void SetFStopPlaying(bool f)
    {
        _fStopPlaying = f;
    } // INTERNAL USE ONLY
    bool FStopPlaying(void)
    {
        return (_fStopPlaying);
    } // INTERNAL USE ONLY
    bool FPlaying(void)
    {
        return _fPlaying;
    } // Query the playing flag.
    PClock Pclok()
    {
        return &_clok;
    } // For getting the clock for playing
    void SetFIdleSeen(bool fIdle)
    {
        _fIdleSeen = fIdle;
    }
    bool FIdleSeen(void)
    {
        return _fIdleSeen;
    }

    //
    // Accessor for getting to the Brender world.
    //
    PWorld Pbwld(void)
    {
        return _pbwld;
    }

    //
    // Frame rate information
    //
    long Cnfrm(void)
    {
        return _cnfrm;
    }
    ulong TsStart(void)
    {
        return _tsStart;
    }

    //
    // Thumbnail stuff
    //
    PDynamicArray PglclrThumbPalette(void)
    {
        AssertThis(0);
        return _pglclrThumbPalette;
    }
    void SetThumbPalette(PDynamicArray pglclr)
    {
        ReleasePpo(&_pglclrThumbPalette);
        _pglclrThumbPalette = pglclr;
        pglclr->AddRef();
    }
};

#endif // !MOVIE_H
