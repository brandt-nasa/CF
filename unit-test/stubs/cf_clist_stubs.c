/************************************************************************
** File: cf_clist_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application circular list definition stubs file
**
**  This is a circular doubly-linked list implementation. It is used for
**  all data structures in CF.
**
**  This file is intended to be a generic class that can be used in other apps.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cf_verify.h"
#include "cf_clist.h"
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"
#include "cf_cfdp.h"

extern type_of_context_CF_CList_Traverse_t type_of_context_CF_CList_Traverse;

/************************************************************************/
/** \brief Initialize a clist node.
**
**  \par Assumptions, External Events, and Notes:
**       node must not be NULL.
**
*************************************************************************/
void CF_CList_InitNode(CF_CListNode_t *node)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InitNode), &node, sizeof(node));

    UT_DEFAULT_IMPL(CF_CList_InitNode);
}

/************************************************************************/
/** \brief Insert the given node into the front of a list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief Insert the given node into the back of a list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InsertBack), &head, sizeof(head));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InsertBack), &node, sizeof(node));

    UT_DEFAULT_IMPL(CF_CList_InsertBack);
}

/************************************************************************/
/** \brief Remove the first node from a list and return it.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL.
**
**  \returns
**  \retstmt The first node (now removed) in the list; NULL if list was empty. \endcode
**  \endreturns
**
*************************************************************************/
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head)
{
    CF_CListNode_t *forced_return;

    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Pop), &head, sizeof(head));

    UT_DEFAULT_IMPL(CF_CList_Pop);

    UT_Stub_CopyToLocal(UT_KEY(CF_CList_Pop), &forced_return, sizeof(forced_return));

    return forced_return;
}

/************************************************************************/
/** \brief Remove the given node from the list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Remove), &head, sizeof(head));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Remove), &node, sizeof(node));

    UT_DEFAULT_IMPL(CF_CList_Remove);
}

/************************************************************************/
/** \brief Insert the given node into the last after the given start node.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InsertAfter), &head, sizeof(head));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InsertAfter), &start, sizeof(start));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_InsertAfter), &after, sizeof(after));

    UT_DEFAULT_IMPL(CF_CList_InsertAfter);
}

/************************************************************************/
/** \brief Traverse the entire list, calling the given function on all nodes.
**
**  \par Assumptions, External Events, and Notes:
**       start must not be NULL. fn must be a valid function.
**
*************************************************************************/

void CF_CList_Traverse(CF_CListNode_t *start, CF_CListFn_t fn, void *context)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), &start, sizeof(start));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), &fn, sizeof(fn));

    UT_GenStub_AddParam(CF_CList_Traverse, void *, context);

    if (context != NULL)
    {
        void  *result_location;
        size_t fn_size;

        switch (type_of_context_CF_CList_Traverse)
        {
            case TRAV_ARG_T:
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), context, sizeof(int32));
                result_location = (uint8 *)context + sizeof(int32);
                UT_Stub_CopyToLocal(UT_KEY(CF_CList_Traverse), result_location, sizeof(int32));
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), (uint8 *)context + (sizeof(int32) * 2), sizeof(fn));
                break;

            case TRAVERSE_ALL_ARGS_T:
                // TODO: fn_size = sizeof(CF_CListFn_t ) is cheating and an
                // assumption the actual type I've seen as the context fn here is
                // CF_TraverseAllTransactions_fn_t and happens to be same size 8
                fn_size = sizeof(CF_CListFn_t);
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), context, fn_size);
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), (uint8 *)context + fn_size, sizeof(void *));
                // TODO: does the counter really need this behavior?  all it is
                // counting is number of times stub is called which is already
                // counted.
                result_location = (uint8 *)context + fn_size + sizeof(void *);
                ++(*(int32 *)result_location);             /* increase count */
                int32 count = *((int32 *)result_location); /* count val to copy */
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), &count, sizeof(int32));
                break;

            case POINTER:
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), &context, sizeof(void *));
                break;

            case FIND_T_BY_SEQ_NUM:
            {
                void *transaction_sequence_number = context;
                void *src_eid                     = (uint8 *)transaction_sequence_number + 4; /* uint32 - 4 bytes */
                void *t                           = (uint8 *)src_eid + 4; /* uint8 - 4 bytes, no packing in struct */

                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), transaction_sequence_number,
                                      sizeof(CF_TransactionSeq_t));
                UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), src_eid, sizeof(CF_EntityId_t));
                UT_Stub_CopyToLocal(UT_KEY(CF_CList_Traverse), t, sizeof(CF_Transaction_t *));
            }
            break;

            default:
                UtAssertEx(false, UTASSERT_CASETYPE_TSF, __FILE__, __LINE__,
                           "CF_CList_Traverse Stub type_of_context_CF_CList_Traverse = NOT_YET_SET or a value not in "
                           "case statement.\n"
                           "This means the context will not be used/saved correctly.\n"
                           "It should be set to a value equal to the expected context type.\n");
        } /* end switch */
    }
    else /* POINTER - NULL was passed in, if clause is for safeguarding against
          * accidentally case functionality using a NULL context inadvetently */
         /* CLOSE_FILES - Not included in case statements because it should have a null context */
    {
        UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse), &context, sizeof(void *));
    }

    UT_GenStub_Execute(CF_CList_Traverse, Basic, NULL);
}

/************************************************************************/
/** \brief Reverse list traversal, starting from end, calling given function on all nodes.
**
**  \par Assumptions, External Events, and Notes:
**       end must not be NULL. fn must be a valid function.
**
*************************************************************************/
#include "cf_cfdp.h" //TODO:this is to get CF_Transaction_t -- find a better way?

void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse_R), &end, sizeof(end));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CList_Traverse_R), &fn, sizeof(fn));

    UT_DEFAULT_IMPL(CF_CList_Traverse_R);

    // TODO:may need to understand what a context can be when this is used by mutltiple tests.
    if (context != NULL)
    {
        void *result_location = context;

        UT_Stub_CopyToLocal(UT_KEY(CF_CList_Traverse_R), result_location, sizeof(CF_Transaction_t *));
    }
}
