/************************************************************************
** File: cf_chunk_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  Stubs file for the CF Application chunks (sparse gap tracking) logic file
**
**  This class handles the complexity of sparse gap tracking so that
**  the CFDP engine doesn't need to worry about it. Information is given
**  to the class and when needed calculations are made internally to
**  help the engine build NAK packets. Received NAK segmnent requests
**  are stored in this class as well and used for re-transmit processing.
**
**  This is intended to be mostly a generic purpose class used by CF.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

/* Most of this was originally written by Stephen Newell stephen@sjnewell.com
 * who wrote it responding to my asking him about the problem. He wrote it
 * in C++ and I (Steven Seeger) ported it to C and fixed a couple bugs and
 * added some stuff.
 *
 * This is a pretty generic implemenation of a solution to the problem of
 * sparse gap tracking over a linear range. */
#include <string.h>
#include "cf_verify.h"
#include "cf_assert.h"
#include "cf_chunk.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "uttools.h"
#include "utgenstub.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/************************************************************************/
/** \brief Erase a range of chunks.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
// static void CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Erase a single chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
// static void CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Insert a chunk before index_before.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
*************************************************************************/
// static void CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Finds where a chunk should be inserted in the chunks.
**
**  \par Description
**       This is a C version of std::lower_bound from C++ algorithms.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns an index to the first chunk that is greater than or equal to the requested's offset. \endcode
**  \endreturns
**
*************************************************************************/
// static CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Possibly combines the given chunk with the previous chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns 1 if combined with another chunk; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Possibly combines the given chunk with the next chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns 1 if combined with another chunk; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Finds the smallest size out of all chunks.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
**  \returns
**  \retstmt The smallest size out of all the chunks. \endcode
**  \endreturns
**
*************************************************************************/
// static CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Insert a chunk.
**
**  \par Description
**       Finds the correct insertion point for a chunk. May combine with
**       an existing chunk if contiguous.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
*************************************************************************/
// static void CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Public function to add a chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkListAdd), &chunks, sizeof(chunks));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkListAdd), &offset, sizeof(offset));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkListAdd), &size, sizeof(size));

    UT_DEFAULT_IMPL(CF_ChunkListAdd);
}

/************************************************************************/
/** \brief Public function to remove some amount of size from the first chunk.
**
**  \par Description
**       This may remove the chunk entirely. This function is to satisfy the
**       use-case where data is retrieved from the structure in-order and
**       once consumed should be removed.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_RemoveFromFirst), &chunks, sizeof(chunks));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_RemoveFromFirst), &size, sizeof(size));

    UT_DEFAULT_IMPL(CF_ChunkList_RemoveFromFirst);
}

/************************************************************************/
/** \brief Public function to remove some amount of size from the first chunk.
**
**  \par Description
**       This may remove the chunk entirely. This function is to satisfy the
**       use-case where data is retrieved from the structure in-order and
**       once consumed should be removed.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks)
{
    CF_Chunk_t *forced_return;

    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_GetFirstChunk), &chunks, sizeof(chunks));

    UT_DEFAULT_IMPL(CF_ChunkList_GetFirstChunk);

    UT_Stub_CopyToLocal(UT_KEY(CF_ChunkList_GetFirstChunk), &forced_return, sizeof(forced_return));

    return forced_return;
}

/************************************************************************/
/** \brief Initialize a chunks structure.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunks_mem must not be NULL.
**
*************************************************************************/
void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t CF_max_chunks, CF_Chunk_t *chunks_mem)
{
    UT_GenStub_AddParam(CF_ChunkListInit, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_ChunkListInit, CF_ChunkIdx_t, CF_max_chunks);
    UT_GenStub_AddParam(CF_ChunkListInit, CF_Chunk_t *, chunks_mem);

    UT_GenStub_Execute(CF_ChunkListInit, Basic, NULL);
}

/************************************************************************/
/** \brief Resets a chunks structure.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_ChunkListReset(CF_ChunkList_t *chunks)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief Compute gaps between chunks, and call a callback for each.
**
**  \par Description
**       This function walks over all chunks and computes the gaps between.
**       It can exit early if the calculated gap start is larger than the
**       desired total.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. compute_gap_fn is a valid function address.
**
**  \returns
**  \retstmt The number of computed gaps. \endcode
**  \endreturns
**
*************************************************************************/
uint32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_ComputeGaps), &chunks, sizeof(chunks));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_ComputeGaps), &max_gaps, sizeof(max_gaps));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_ComputeGaps), &total, sizeof(total));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_ComputeGaps), &compute_gap_fn, sizeof(compute_gap_fn));
    UT_Stub_CopyFromLocal(UT_KEY(CF_ChunkList_ComputeGaps), &opaque, sizeof(opaque));

    return UT_DEFAULT_IMPL(CF_ChunkList_ComputeGaps);
}
