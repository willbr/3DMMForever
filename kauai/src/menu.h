/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Menu bar management.

***************************************************************************/
#ifndef MENU_H
#define MENU_H

// Menu Bar class
typedef class MenuBar *PMenuBar;
#define MenuBar_PAR BASE
#define kclsMenuBar 'MUB'
class MenuBar : public MenuBar_PAR
{
    RTCLASS_DEC
    MARKMEM

  private:
#ifdef MAC
    // System Menu
    typedef MenuInfo SMU;

    // System Menu Bar
    struct SMB
    {
        ushort cmid;
        ushort rgmid[1];
    };

    // Menu Item
    struct MNI
    {
        long cid;
        long lw0;
    };

    // Menu
    struct MNU
    {
        long mid;
        SMU **hnsmu;
        PDynamicArray pglmni;
    };

    // menu list
    struct MLST
    {
        long imnu;
        long imniBase;
        long cmni;
        long cid;
        bool fSeparator;
    };

    HN _hnmbar;
    PDynamicArray _pglmnu;
    PDynamicArray _pglmlst; // menu lists

    bool _FInsertMni(long imnu, long imni, long cid, long lw0, PString pstn);
    void _DeleteMni(long imnu, long imni);
    bool _FFindMlst(long imnu, long imni, MLST *pmlst = pvNil, long *pimlst = pvNil);
    bool _FGetCmdFromCode(long lwCode, Command *pcmd);
    void _Free(void);
    bool _FFetchRes(ulong ridMenuBar);
#endif // MAC

#ifdef WIN
    // menu list
    struct MLST
    {
        HMENU hmenu;
        long imniBase;
        long wcidList;
        long cid;
        bool fSeparator;
        PDynamicArray pgllw;
    };

    HMENU _hmenu; // the menu bar
    long _cmnu;   // number of menus on the menu bar
    PDynamicArray _pglmlst; // menu lists

    bool _FInitLists(void);
    bool _FFindMlst(long wcid, MLST *pmlst, long *pimlst = pvNil);
    bool _FGetCmdForWcid(long wcid, PCommand pcmd);
#endif // WIN

  protected:
    MenuBar(void)
    {
    }

  public:
    ~MenuBar(void);

    static PMenuBar PmubNew(ulong ridMenuBar);

    virtual void Set(void);
    virtual void Clean(void);

#ifdef MAC
    virtual bool FDoClick(EVT *pevt);
    virtual bool FDoKey(EVT *pevt);
#endif // MAC
#ifdef WIN
    virtual void EnqueueWcid(long wcid);
#endif // WIN

    virtual bool FAddListCid(long cid, long lw0, PString pstn);
    virtual bool FRemoveListCid(long cid, long lw0, PString pstn = pvNil);
    virtual bool FChangeListCid(long cid, long lwOld, PString pstnOld, long lwNew, PString pstnNew);
    virtual bool FRemoveAllListCid(long cid);
};

extern PMenuBar vpmubCur;

#endif //! MENU_H
