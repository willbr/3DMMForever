/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    Portfolio related includes.

    Primary Author: ******
    Review Status: Not yet reviewed

***************************************************************************/

// Top level portoflio routines.
bool FPortGetFniMovieOpen(Filename *pfni);
bool FPortDisplayWithIds(Filename *pfni, bool fOpen, long lFilterLabel, long lFilterExt, long lTitle, LPTSTR lpstrDefExt,
                         PString pstnDefFileName, Filename *pfniInitialDir, ulong grfPrevType, ChunkNumber cnoWave);
bool FPortGetFniOpen(Filename *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle, Filename *pfniInitialDir, ulong grfPrevType,
                     ChunkNumber cnoWave);
bool FPortGetFniSave(Filename *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle, LPTSTR lpstrDefExt, PString pstnDefFileName,
                     ulong grfPrevType, ChunkNumber cnoWave);

UINT CALLBACK OpenHookProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
void OpenPreview(HWND hwnd, PGraphicsEnvironment pgnvOff, RCS *prcsPreview);
void RepaintPortfolio(HWND hwndCustom);

static WNDPROC lpBtnProc;
LRESULT CALLBACK SubClassBtnProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC lpPreviewProc;
LRESULT CALLBACK SubClassPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC lpDlgProc;
LRESULT CALLBACK SubClassDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct dlginfo
{
    bool fIsOpen;      // fTrue if Open file, (ie not Save file)
    bool fDrawnBkgnd;  // fTrue if portfolio background bitmap has been displayed.
    RCS rcsDlg;        // Initial size of the portfolio common dlg window client area.
    ulong grfPrevType; // Bits for types of preview required, (eg movie, sound etc) == 0 if no preview
    ChunkNumber cnoWave;       // Wave file cno for audio when portfolio is invoked.
} DLGINFO;
typedef DLGINFO *PDLGINFO;

enum
{
    fpfNil = 0x0000,
    fpfPortPrevMovie = 0x0001,
    fpfPortPrevSound = 0x0002,
    fpfPortPrevTexture = 0x0004
};
