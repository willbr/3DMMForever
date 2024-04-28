/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
    Author: ShonK
    Project: Kauai
    Reviewed:
    Copyright (c) Microsoft Corporation

    Core routines for the utility layer.

***************************************************************************/
#include "util.h"
ASSERTNAME

#ifdef DEBUG
/***************************************************************************
    Mark all util-level memory and objects.
***************************************************************************/
void MarkUtilMem(void)
{
    PChunkyFile pcfl;
    PFileObject pfil;

    MarkMemObj(&vsflUtil);
    MarkMemObj(&vrndUtil);
    MarkMemObj(&vkcdcUtil);
    MarkMemObj(&vcodmUtil);
    MarkMemObj(vpcodmUtil);

    for (pcfl = ChunkyFile::PcflFirst(); pcfl != pvNil; pcfl = pcfl->PcflNext())
        MarkMemObj(pcfl);

    for (pfil = FileObject::PfilFirst(); pfil != pvNil; pfil = pfil->PfilNext())
        MarkMemObj(pfil);
}
#endif // DEBUG
