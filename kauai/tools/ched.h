/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    Chunky editor main header file

***************************************************************************/
#ifndef CHED_H
#define CHED_H

#include "kidframe.h"
#include "chedres.h"

// creator type for the chunky editor
#define kctgChed 'CHED'

#include "chdoc.h"

#define APP_PAR ApplicationBase
#define kclsAPP 'APP'
class APP : public APP_PAR
{
    RTCLASS_DEC
    CMD_MAP_DEC(APP)

  protected:
    virtual bool _FInit(ulong grfapp, ulong grfgob, long ginDef);
    virtual void _FastUpdate(PGraphicsObject pgob, PRegion pregnClip, ulong grfapp = fappNil, PGraphicsPort pgpt = pvNil);

  public:
    virtual void GetStnAppName(PString pstn);
    virtual void UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp = fappNil);

    virtual bool FCmdOpen(PCommand pcmd);
};

#endif //! CHED_H
