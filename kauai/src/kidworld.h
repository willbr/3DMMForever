/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Copyright (c) Microsoft Corporation

    This is a class that knows how to create GOKs, Help Balloons and
    Kidspace script interpreters. It exists so an app can customize
    default behavior.

***************************************************************************/
#ifndef KIDWORLD_H
#define KIDWORLD_H

using namespace Help;
using namespace ScriptCompiler;
using GraphicalObjectRepresentation::PKidspaceGraphicObject;

/***************************************************************************
    Base KidspaceGraphicObject descriptor.
***************************************************************************/
// location from parent map structure
struct LOP
{
    long hidPar;
    long xp;
    long yp;
    long zp; // the z-plane number used for placing the KidspaceGraphicObject in the GraphicsObject tree
};

// cursor map entry
struct CursorMapEntry
{
    ulong grfcustMask; // what cursor states this CursorMapEntry is good for
    ulong grfcust;
    ulong grfbitSno; // what button states this CursorMapEntry is good for
    ChunkNumber cnoCurs;     // the cursor to use
    ChildChunkID chidScript; // execution script (absolute)
    long cidDefault; // default command
    ChunkNumber cnoTopic;    // tool tip topic
};

typedef class KidspaceGraphicObjectDescriptor *PKidspaceGraphicObjectDescriptor;
#define KidspaceGraphicObjectDescriptor_PAR BaseCacheableObject
#define kclsKidspaceGraphicObjectDescriptor 'GOKD'
class KidspaceGraphicObjectDescriptor : public KidspaceGraphicObjectDescriptor_PAR
{
    RTCLASS_DEC

  protected:
    KidspaceGraphicObjectDescriptor(void)
    {
    }

  public:
    virtual long Gokk(void) = 0;
    virtual bool FGetCume(ulong grfcust, long sno, CursorMapEntry *pcume) = 0;
    virtual void GetLop(long hidPar, LOP *plop) = 0;
};

/***************************************************************************
    Standard KidspaceGraphicObject descriptor. Contains location information and cursor
    map stuff.
***************************************************************************/
// KidspaceGraphicObject construction descriptor on file - these are stored in chunky resource files
struct GOKDF
{
    short bo;
    short osk;
    long gokk;
    // LOP rglop[];		ends with a default entry (hidPar == hidNil)
    // CursorMapEntry rgcume[];	the cursor map
};
const ByteOrderMask kbomGokdf = 0x0C000000;

typedef class KidspaceGraphicObjectDescriptorLocation *PKidspaceGraphicObjectDescriptorLocation;
#define KidspaceGraphicObjectDescriptorLocation_PAR KidspaceGraphicObjectDescriptor
#define kclsKidspaceGraphicObjectDescriptorLocation 'GKDS'
class KidspaceGraphicObjectDescriptorLocation : public KidspaceGraphicObjectDescriptorLocation_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    HQ _hqData;
    long _gokk;
    long _clop;
    long _ccume;

    KidspaceGraphicObjectDescriptorLocation(void)
    {
    }

  public:
    // An object reader for a KidspaceGraphicObjectDescriptor.
    static bool FReadGkds(PChunkyResourceFile pcrf, ChunkTagOrType ctg, ChunkNumber cno, DataBlock *pblck, PBaseCacheableObject *ppbaco, long *pcb);
    ~KidspaceGraphicObjectDescriptorLocation(void);

    virtual long Gokk(void);
    virtual bool FGetCume(ulong grfcust, long sno, CursorMapEntry *pcume);
    virtual void GetLop(long hidPar, LOP *plop);
};

/***************************************************************************
    World of Kidspace class.
***************************************************************************/
typedef class WorldOfKidspace *PWorldOfKidspace;
#define WorldOfKidspace_PAR GraphicsObject
#define kclsWorldOfKidspace 'WOKS'
class WorldOfKidspace : public WorldOfKidspace_PAR
{
    RTCLASS_DEC
    ASSERT
    MARKMEM

  protected:
    PStringRegistry _pstrg;
    StringRegistry _strg;
    ulong _grfcust;

    Clock _clokAnim;
    Clock _clokNoSlip;
    Clock _clokGen;
    Clock _clokReset;

  public:
    WorldOfKidspace(GraphicsObjectBlock *pgcb, PStringRegistry pstrg = pvNil);
    ~WorldOfKidspace(void);

    PStringRegistry Pstrg(void)
    {
        return _pstrg;
    }

    virtual bool FGobIn(PGraphicsObject pgob);
    virtual PKidspaceGraphicObjectDescriptor PgokdFetch(ChunkTagOrType ctg, ChunkNumber cno, PResourceCache prca);
    virtual PKidspaceGraphicObject PgokNew(PGraphicsObject pgobPar, long hid, ChunkNumber cno, PResourceCache prca);
    virtual PGraphicsObjectInterpreter PscegNew(PResourceCache prca, PGraphicsObject pgob);
    virtual PBalloon PhbalNew(PGraphicsObject pgobPar, PResourceCache prca, ChunkNumber cnoTopic, Help::PTopic phtop = pvNil);
    virtual PCommandHandler PcmhFromHid(long hid);
    virtual PGraphicsObject PgobParGob(PGraphicsObject pgob);
    virtual bool FFindFile(PString pstnSrc, PFilename pfni);
    virtual tribool TGiveAlert(PString pstn, long bk, long cok);
    virtual void Print(PString pstn);

    virtual ulong GrfcustCur(bool fAsynch = fFalse);
    virtual void ModifyGrfcust(ulong grfcustOr, ulong grfcustXor);
    virtual ulong GrfcustAdjust(ulong grfcust);

    virtual bool FModalTopic(PResourceCache prca, ChunkNumber cnoTopic, long *plwRet);
    virtual PClock PclokAnim(void)
    {
        return &_clokAnim;
    }
    virtual PClock PclokNoSlip(void)
    {
        return &_clokNoSlip;
    }
    virtual PClock PclokGen(void)
    {
        return &_clokGen;
    }
    virtual PClock PclokReset(void)
    {
        return &_clokReset;
    }
    virtual PClock PclokFromHid(long hid);
};

#endif //! KIDWORLD_H
