/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_cmd.c"

/*
 * In order to properly instantiate buffers to pass to functions that
 * accept a CFE_SB_Buffer_t, a union must be used to align the
 * data.
 */

typedef union
{
    CF_UnionArgsCmd_t ua;
    CFE_SB_Buffer_t   buf;
} CF_UT_cmd_unionargs_buf_t;

typedef union
{
    CF_SetParamCmd_t sp;
    CFE_SB_Buffer_t  buf;
} CF_UT_cmd_set_param_args_buf_t;

typedef union
{
    CF_GetParamCmd_t gp;
    CFE_SB_Buffer_t  buf;
} CF_UT_cmd_get_param_args_buf_t;

typedef union
{
    CF_TxFileCmd_t  tf;
    CFE_SB_Buffer_t buf;
} CF_UT_cmd_tx_file_buf_t;

typedef union
{
    CF_PlaybackDirCmd_t pd;
    CFE_SB_Buffer_t     buf;
} CF_UT_cmd_playback_dir_buf_t;

typedef union
{
    CF_TransactionCmd_t xact;
    CFE_SB_Buffer_t     buf;
} CF_UT_cmd_transaction_buf_t;

typedef union
{
    CF_WriteQueueCmd_t wq;
    CFE_SB_Buffer_t    buf;
} CF_UT_cmd_write_q_buf_t;

/* cf_cmd_tests globals */
extern type_of_context_CF_CList_Traverse_t type_of_context_CF_CList_Traverse;

CFE_MSG_GetSize_context_t context_CFE_MSG_GetSize;

/*******************************************************************************
**
**  cf_cmd_tests Setup and Teardown
**
*******************************************************************************/

void cf_cmd_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_cmd_tests_Setup */

void cf_cmd_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cmd_tests_Teardown */

/* end cf_cmd_tests Setup and Teardown */

/*******************************************************************************
**
**  cf_cmd_tests specific Any functions  NOTE:Some of these may be better as global
**
*******************************************************************************/

CF_CFDP_Class_t Any_cfdp_class_t(void)
{
    return (CF_CFDP_Class_t)AnyCoinFlip();
}

CF_EntityId_t Any_CF_EntityId_t(void)
{
    return (CF_EntityId_t)Any_uint8();
}

/* uint8 used for Any_cf_channel likely there will never be that many channels */
uint8 Any_cf_channel(void)
{
    return Any_uint8_LessThan(CF_NUM_CHANNELS);
}

/* uint8 used for Any_cf_polldir likely there will never be that many polldirs */
uint8 Any_cf_polldir(void)
{
    return Any_uint8_LessThan(CF_MAX_POLLING_DIR_PER_CHAN);
}

/* bool_arg_t_barg should only be 0 or 1 (Boolean) */
uint8 Any_bool_arg_t_barg(void)
{
    return AnyCoinFlip();
}

uint8 Any_queue_Except_q_pend(void)
{
    /* q_pend = 0, q_active = 1, q_history = 2, q_all = 3 */
    return (rand() % 2) + 1; /* 0-2, + 1 -> 1-3 */
}

CF_TransactionSeq_t Any_CF_TransactionSeq_t(void)
{
    return (CF_TransactionSeq_t)Any_uint32();
}

/* end cf_cmd_tests specific Any functions */

/*******************************************************************************
**
**  cf_cmd_tests dummy test functions
**
*******************************************************************************/

int Dummy_chan_action_fn_t(uint8 chan_num, void *context)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_chan_action_fn_t), &chan_num, sizeof(uint8));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_chan_action_fn_t), &context, sizeof(void *));

    return UT_DEFAULT_IMPL(Dummy_chan_action_fn_t);
}

void Dummy_CF_TsnChanAction_fn_t(CF_Transaction_t *t, void *context)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_CF_TsnChanAction_fn_t), &t, sizeof(CF_Transaction_t *));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_CF_TsnChanAction_fn_t), &context, sizeof(void *));

    UT_DEFAULT_IMPL(Dummy_CF_TsnChanAction_fn_t);
}
typedef struct
{
    CF_Transaction_t *t;
    void             *context;
} CF_PACK Dummy_CF_TsnChanAction_fn_t_context_t;

/*******************************************************************************
**
**  CF_CmdAcc tests
**
*******************************************************************************/

void Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter(void)
{
    /* Arrange */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdAcc();

    /* Assert */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter */

/* end CF_CmdAcc tests */

/*******************************************************************************
**
**  CF_CmdRej tests
**
*******************************************************************************/
void Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter(void)
{
    /* Arrange */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdRej();

    /* Assert */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter */

/* end CF_CmdRej tests */

/*******************************************************************************
**
**  CF_CmdCond tests
**
*******************************************************************************/

void Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc(void)
{
    /* Arrange */
    int arg_cond = 0;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdCond(arg_cond);

    /* Assert */
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc */

void Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej(void)
{
    /* Arrange */
    int arg_cond = Any_int_Except(0);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdCond(arg_cond);

    /* Assert */
    /* Assert for CF-CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej */

/* end CF_CmdCond tests */

/*******************************************************************************
**
**  CF_CmdNoop tests
**
*******************************************************************************/

void Test_CF_CmdNoop_SendNoopEventAndAcceptCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdNoop(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_NOOP,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_NOOP)", EventID,
                  CF_EID_INF_CMD_NOOP);
    /* Assert to show CF_CmdAcc was called */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* Test_CF_CmdNoop_SendNoopEventAndAcceptCommand */

/* end CF_CmdNoop tests */

/*******************************************************************************
**
**  CF_CmdReset tests
**
*******************************************************************************/

void Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 5; /* 5 is size of 'names' */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_RESET_INVALID,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_ERR_CMD_RESET_INVALID)",
                  EventID, CF_EID_ERR_CMD_RESET_INVALID);
    /* Assert to show CF_CmdRej was called */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = Any_uint8_GreaterThan(5); /* 5 is size of 'names' */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_RESET_INVALID,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_ERR_CMD_RESET_INVALID)",
                  EventID, CF_EID_ERR_CMD_RESET_INVALID);
    /* Assert to show CF_CmdRej was called */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 1; /* 1 is 'cmd' */

    CF_AppData.hk.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.counters.err = Any_uint16_Except(0);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_RESET,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_RESET)", EventID,
                  CF_EID_INF_CMD_RESET);
    UtAssert_True(CF_AppData.hk.counters.cmd == 0, "CF_AppData.hk.counters.cmd is %hu and should be 0 (was reset)",
                  CF_AppData.hk.counters.cmd);
    UtAssert_True(CF_AppData.hk.counters.err == 0, "CF_AppData.hk.counters.err is %hu and should be 0 (was reset)",
                  CF_AppData.hk.counters.err);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent */

void Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 2; /* 2 is 'fault' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.spare              = Any_uint16_Except(0);
    }

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_RESET,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_RESET)", EventID,
                  CF_EID_INF_CMD_RESET);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_open == 0,
                      "fault.file_open for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_open);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_read == 0,
                      "fault.file_read for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_read);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_seek == 0,
                      "fault.file_seek for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_seek);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_write == 0,
                      "fault.file_write for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_write);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_rename == 0,
                      "fault.file_rename for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_rename);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.directory_read == 0,
                      "fault.directory_read for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.directory_read);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch == 0,
                      "fault.crc_mismatch for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch == 0,
                      "fault.file_size_mismatch for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.nak_limit == 0,
                      "fault.nak_limit for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.nak_limit);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.ack_limit == 0,
                      "fault.ack_limit for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.ack_limit);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer == 0,
                      "fault.inactivity_timer for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.fault.spare == 0,
                      "fault.spare for channel %hu is %hu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.fault.spare);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 3; /* 3 is 'up' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_RESET,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_RESET)", EventID,
                  CF_EID_INF_CMD_RESET);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes == 0,
                      "recv.file_data_bytes for channel %u is %lu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.pdu == 0,
                      "recv.pdu for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.pdu);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.error == 0,
                      "recv.error for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.error);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.spurious == 0,
                      "recv.spurious for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.spurious);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.pdu == 0,
                      "recv.pdu for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.pdu);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests == 0,
                      "recv.nak_segment_requests for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand */

void Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    uint8                     i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 4; /* 4 is 'down' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_RESET,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_RESET)", EventID,
                  CF_EID_INF_CMD_RESET);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes == 0,
                      "sent.file_data_bytes for channel %u is %lu and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests == 0,
                      "sent.nak_segment_requests for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests);
        UtAssert_True(CF_AppData.hk.channel_hk[i].counters.sent.pdu == 0,
                      "sent.pdu for channel %u is %u and should be 0", i,
                      CF_AppData.hk.channel_hk[i].counters.sent.pdu);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 0; /* 0 is 'all' */

    CF_AppData.hk.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.counters.err = Any_uint16_Except(0);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.spare              = Any_uint16_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_RESET,
                  "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_INF_CMD_RESET)", EventID,
                  CF_EID_INF_CMD_RESET);
    UtAssert_True(CF_AppData.hk.counters.cmd == 0, "CF_AppData.hk.counters.cmd is %hu and should be 0 (was reset)",
                  CF_AppData.hk.counters.cmd);
    UtAssert_True(CF_AppData.hk.counters.err == 0, "CF_AppData.hk.counters.err is %hu and should be 0 (was reset)",
                  CF_AppData.hk.counters.err);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent */

/* end CF_CmdReset tests */

/*******************************************************************************
**
**  CF_CmdTxFile tests
**
*******************************************************************************/

void Test_CF_CmdTxFile_WhenCallTo_CF_CFDP_TxFile_Returns_0_AcceptCommand(void)
{
    /* Arrange */
    int32                    dummy_CF_CFDP_TxFile_return = 0;
    CF_UT_cmd_tx_file_buf_t  utbuf;
    CF_TxFileCmd_t          *dummy_msg = &utbuf.tf;
    CFE_SB_Buffer_t         *arg_msg   = &utbuf.buf;
    CF_CFDP_TxFile_context_t context_CF_CFDP_TxFile;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxFile), dummy_CF_CFDP_TxFile_return);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_TxFile), &context_CF_CFDP_TxFile, sizeof(context_CF_CFDP_TxFile), false);

    memcpy(dummy_msg->src_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    memcpy(dummy_msg->dst_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    dummy_msg->cfdp_class = Any_cfdp_class_t();
    dummy_msg->keep       = Any_uint8();
    dummy_msg->chan_num   = Any_uint8();
    dummy_msg->priority   = Any_uint8();
    dummy_msg->dest_id    = Any_CF_EntityId_t();

    /* Arrange unstubbable: CF_CmdAcc via CF_CmdCond */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;
    /* Act */
    CF_CmdTxFile(arg_msg);

    /* Assert */
    UtAssert_StrCmp(context_CF_CFDP_TxFile.src_filename, dummy_msg->src_filename,
                    "CF_CFDP_TxFile received source_filename %s and should be %s (tx->src_filename)",
                    context_CF_CFDP_TxFile.src_filename, dummy_msg->src_filename);
    UtAssert_StrCmp(context_CF_CFDP_TxFile.dst_filename, dummy_msg->dst_filename,
                    "CF_CFDP_TxFile received source_filename %s and should be %s (tx->dst_filename)",
                    context_CF_CFDP_TxFile.dst_filename, dummy_msg->dst_filename);
    UtAssert_True(context_CF_CFDP_TxFile.cfdp_class == dummy_msg->cfdp_class,
                  "CF_CFDP_TxFile received cfdp_class %u and should be %u (tx->cfdp_class)",
                  context_CF_CFDP_TxFile.cfdp_class, dummy_msg->cfdp_class);
    UtAssert_True(context_CF_CFDP_TxFile.keep == dummy_msg->keep,
                  "CF_CFDP_TxFile received keep %u and should be %u (tx->keep)", context_CF_CFDP_TxFile.keep,
                  dummy_msg->keep);
    /* chan vs. chan_num -> CF_TxFileCmd_t uses chan_num, CUT uses chan */
    UtAssert_True(context_CF_CFDP_TxFile.chan == dummy_msg->chan_num,
                  "CF_CFDP_TxFile received chan %u and should be %u (tx->chan_num)", context_CF_CFDP_TxFile.chan,
                  dummy_msg->chan_num);
    UtAssert_True(context_CF_CFDP_TxFile.priority == dummy_msg->priority,
                  "CF_CFDP_TxFile received priority %u and should be %u (tx->priority)",
                  context_CF_CFDP_TxFile.priority, dummy_msg->priority);
    UtAssert_True(context_CF_CFDP_TxFile.dest_id == dummy_msg->dest_id,
                  "CF_CFDP_TxFile received dest_id %u and should be %u (tx->dest_id)", context_CF_CFDP_TxFile.dest_id,
                  dummy_msg->dest_id);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* Test_CF_CmdTxFile_WhenCallTo_CF_CFDP_TxFile_Returns_0_AcceptCommand */

void Test_CF_CmdTxFile_WhenCAllTo_CF_CFDP_TxFile_Returns_Non0_RejectCommand(void)
{
    /* Arrange */
    int32                    dummy_CF_CFDP_TxFile_return = Any_int32_Except(0);
    CF_UT_cmd_tx_file_buf_t  utbuf;
    CF_TxFileCmd_t          *dummy_msg = &utbuf.tf;
    CFE_SB_Buffer_t         *arg_msg   = &utbuf.buf;
    CF_CFDP_TxFile_context_t context_CF_CFDP_TxFile;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxFile), dummy_CF_CFDP_TxFile_return);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_TxFile), &context_CF_CFDP_TxFile, sizeof(context_CF_CFDP_TxFile), false);

    memcpy(dummy_msg->src_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    memcpy(dummy_msg->dst_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    dummy_msg->cfdp_class = Any_cfdp_class_t();
    dummy_msg->keep       = Any_uint8();
    dummy_msg->chan_num   = Any_uint8();
    dummy_msg->priority   = Any_uint8();
    dummy_msg->dest_id    = Any_CF_EntityId_t();

    /* Arrange unstubbable: CF_CmdAcc via CF_CmdCond */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdTxFile(arg_msg);

    /* Assert */
    UtAssert_StrCmp(context_CF_CFDP_TxFile.src_filename, dummy_msg->src_filename,
                    "CF_CFDP_TxFile received source_filename %s and should be %s (tx->src_filename)",
                    context_CF_CFDP_TxFile.src_filename, dummy_msg->src_filename);
    UtAssert_StrCmp(context_CF_CFDP_TxFile.dst_filename, dummy_msg->dst_filename,
                    "CF_CFDP_TxFile received source_filename %s and should be %s (tx->dst_filename)",
                    context_CF_CFDP_TxFile.dst_filename, dummy_msg->dst_filename);
    UtAssert_True(context_CF_CFDP_TxFile.cfdp_class == dummy_msg->cfdp_class,
                  "CF_CFDP_TxFile received cfdp_class %u and should be %u (tx->cfdp_class)",
                  context_CF_CFDP_TxFile.cfdp_class, dummy_msg->cfdp_class);
    UtAssert_True(context_CF_CFDP_TxFile.keep == dummy_msg->keep,
                  "CF_CFDP_TxFile received keep %u and should be %u (tx->keep)", context_CF_CFDP_TxFile.keep,
                  dummy_msg->keep);
    /* chan vs. chan_num -> CF_TxFileCmd_t uses chan_num, CUT uses chan */
    UtAssert_True(context_CF_CFDP_TxFile.chan == dummy_msg->chan_num,
                  "CF_CFDP_TxFile received chan %u and should be %u (tx->chan_num)", context_CF_CFDP_TxFile.chan,
                  dummy_msg->chan_num);
    UtAssert_True(context_CF_CFDP_TxFile.priority == dummy_msg->priority,
                  "CF_CFDP_TxFile received priority %u and should be %u (tx->priority)",
                  context_CF_CFDP_TxFile.priority, dummy_msg->priority);
    UtAssert_True(context_CF_CFDP_TxFile.dest_id == dummy_msg->dest_id,
                  "CF_CFDP_TxFile received dest_id %u and should be %u (tx->dest_id)", context_CF_CFDP_TxFile.dest_id,
                  dummy_msg->dest_id);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* Test_CF_CmdTxFile_WhenCAllTo_CF_CFDP_TxFile_Returns_Non0_RejectCommand */

/* end CF_CmdTxFile tests */

/*******************************************************************************
**
**  CF_CmdPlaybackDir tests
**
*******************************************************************************/

void Test_CF_CmdPlaybackDir_WhenCAllTo_CF_CFDP_PlaybackDir_Returns_0_AcceptCommand(void)
{
    /* Arrange */
    int32                         dummy_CF_CFDP_PlaybackDir_return = 0;
    CF_UT_cmd_playback_dir_buf_t  utbuf;
    CF_PlaybackDirCmd_t          *dummy_msg = &utbuf.pd;
    CFE_SB_Buffer_t              *arg_msg   = &utbuf.buf;
    CF_CFDP_PlaybackDir_context_t context_CF_CFDP_PlaybackDir;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_PlaybackDir), dummy_CF_CFDP_PlaybackDir_return);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_PlaybackDir), &context_CF_CFDP_PlaybackDir, sizeof(context_CF_CFDP_PlaybackDir),
                     false);

    memcpy(dummy_msg->src_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    memcpy(dummy_msg->dst_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    dummy_msg->cfdp_class = Any_cfdp_class_t();
    dummy_msg->keep       = Any_uint8();
    dummy_msg->chan_num   = Any_uint8();
    dummy_msg->priority   = Any_uint8();
    dummy_msg->dest_id    = Any_CF_EntityId_t();

    /* Arrange unstubbable: CF_CmdAcc via CF_CmdCond */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;
    /* Act */
    CF_CmdPlaybackDir(arg_msg);

    /* Assert */
    UtAssert_True(UtAssert_StrCmp(context_CF_CFDP_PlaybackDir.src_filename, dummy_msg->src_filename, "Strings matched"),
                  "CF_CmdPlaybackDir received source_filename %s and should be %s (tx->src_filename)",
                  context_CF_CFDP_PlaybackDir.src_filename, dummy_msg->src_filename);
    UtAssert_True(UtAssert_StrCmp(context_CF_CFDP_PlaybackDir.dst_filename, dummy_msg->dst_filename, "Strings matched"),
                  "CF_CmdPlaybackDir received source_filename %s and should be %s (tx->dst_filename)",
                  context_CF_CFDP_PlaybackDir.dst_filename, dummy_msg->dst_filename);
    UtAssert_True(context_CF_CFDP_PlaybackDir.cfdp_class == dummy_msg->cfdp_class,
                  "CF_CmdPlaybackDir received cfdp_class %u and should be %u (tx->cfdp_class)",
                  context_CF_CFDP_PlaybackDir.cfdp_class, dummy_msg->cfdp_class);
    UtAssert_True(context_CF_CFDP_PlaybackDir.keep == dummy_msg->keep,
                  "CF_CmdPlaybackDir received keep %u and should be %u (tx->keep)", context_CF_CFDP_PlaybackDir.keep,
                  dummy_msg->keep);
    /* chan vs. chan_num -> CF_TxFileCmd_t uses chan_num, CUT uses chan */
    UtAssert_True(context_CF_CFDP_PlaybackDir.chan == dummy_msg->chan_num,
                  "CF_CmdPlaybackDir received chan %u and should be %u (tx->chan_num)",
                  context_CF_CFDP_PlaybackDir.chan, dummy_msg->chan_num);
    UtAssert_True(context_CF_CFDP_PlaybackDir.priority == dummy_msg->priority,
                  "CF_CmdPlaybackDir received priority %u and should be %u (tx->priority)",
                  context_CF_CFDP_PlaybackDir.priority, dummy_msg->priority);
    UtAssert_True(context_CF_CFDP_PlaybackDir.dest_id == dummy_msg->dest_id,
                  "CF_CmdPlaybackDir received dest_id %u and should be %u (tx->dest_id)",
                  context_CF_CFDP_PlaybackDir.dest_id, dummy_msg->dest_id);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdPlaybackDir_WhenCAllTo_CF_CFDP_PlaybackDir_Returns_0_AcceptCommand */

void Test_CF_CmdPlaybackDir_WhenCallTo_CF_CmdPlaybackDir_Returns_non0_RejectCommand(void)
{
    /* Arrange */
    int32                         dummy_CF_CFDP_PlaybackDir_return = Any_int32_Except(0);
    CF_UT_cmd_playback_dir_buf_t  utbuf;
    CF_PlaybackDirCmd_t          *dummy_msg = &utbuf.pd;
    CFE_SB_Buffer_t              *arg_msg   = &utbuf.buf;
    CF_CFDP_PlaybackDir_context_t context_CF_CFDP_PlaybackDir;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_PlaybackDir), dummy_CF_CFDP_PlaybackDir_return);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_PlaybackDir), &context_CF_CFDP_PlaybackDir, sizeof(context_CF_CFDP_PlaybackDir),
                     false);

    memcpy(dummy_msg->src_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    memcpy(dummy_msg->dst_filename, AnyFilenameOfLength(10), CF_FILENAME_MAX_LEN); // 10 is arbitrary
    dummy_msg->cfdp_class = Any_cfdp_class_t();
    dummy_msg->keep       = Any_uint8();
    dummy_msg->chan_num   = Any_uint8();
    dummy_msg->priority   = Any_uint8();
    dummy_msg->dest_id    = Any_CF_EntityId_t();

    /* Arrange unstubbable: CF_CmdAcc via CF_CmdCond */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdPlaybackDir(arg_msg);

    /* Assert */
    UtAssert_True(UtAssert_StrCmp(context_CF_CFDP_PlaybackDir.src_filename, dummy_msg->src_filename, "Strings matched"),
                  "CF_CmdPlaybackDir received source_filename %s and should be %s (tx->src_filename)",
                  context_CF_CFDP_PlaybackDir.src_filename, dummy_msg->src_filename);
    UtAssert_True(UtAssert_StrCmp(context_CF_CFDP_PlaybackDir.dst_filename, dummy_msg->dst_filename, "Strings matched"),
                  "CF_CmdPlaybackDir received source_filename %s and should be %s (tx->dst_filename)",
                  context_CF_CFDP_PlaybackDir.dst_filename, dummy_msg->dst_filename);
    UtAssert_True(context_CF_CFDP_PlaybackDir.cfdp_class == dummy_msg->cfdp_class,
                  "CF_CmdPlaybackDir received cfdp_class %u and should be %u (tx->cfdp_class)",
                  context_CF_CFDP_PlaybackDir.cfdp_class, dummy_msg->cfdp_class);
    UtAssert_True(context_CF_CFDP_PlaybackDir.keep == dummy_msg->keep,
                  "CF_CmdPlaybackDir received keep %u and should be %u (tx->keep)", context_CF_CFDP_PlaybackDir.keep,
                  dummy_msg->keep);
    /* chan vs. chan_num -> CF_TxFileCmd_t uses chan_num, CUT uses chan */
    UtAssert_True(context_CF_CFDP_PlaybackDir.chan == dummy_msg->chan_num,
                  "CF_CmdPlaybackDir received chan %u and should be %u (tx->chan_num)",
                  context_CF_CFDP_PlaybackDir.chan, dummy_msg->chan_num);
    UtAssert_True(context_CF_CFDP_PlaybackDir.priority == dummy_msg->priority,
                  "CF_CmdPlaybackDir received priority %u and should be %u (tx->priority)",
                  context_CF_CFDP_PlaybackDir.priority, dummy_msg->priority);
    UtAssert_True(context_CF_CFDP_PlaybackDir.dest_id == dummy_msg->dest_id,
                  "CF_CmdPlaybackDir received dest_id %u and should be %u (tx->dest_id)",
                  context_CF_CFDP_PlaybackDir.dest_id, dummy_msg->dest_id);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* Test_CF_CmdPlaybackDir_WhenCallTo_CF_CmdPlaybackDir_Returns_non0_RejectCommand */

/* end CF_CmdTxFile tests */

/*******************************************************************************
**
**  CF_DoChanAction tests
**
*******************************************************************************/

void Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context    = &dummy_context;
    uint8                     random_fn_call = Any_uint8_LessThan(CF_NUM_CHANNELS) + 1;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDeferredRetcode(UT_KEY(Dummy_chan_action_fn_t), random_fn_call, 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (an fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1 */

void Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (an fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1 */

void Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoChanAction returned %d and should be 0 (all fn returned 0)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0 */

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1 */

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoChanAction returned %d and should be 0 (fn returned 0)", local_result);

} /* end Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1 */

void Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = CF_NUM_CHANNELS;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_CHAN_PARAM,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_CHAN_PARAM)", EventID,
                  CF_EID_ERR_CMD_CHAN_PARAM);
    UtAssert_True(local_result == -1,
                  "CF_DoChanAction returned %d and should be -1 (cmd->data.byte[0] >= CF_NUM_CHANNELS)", local_result);

} /* end Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_ */

void Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    chan_action_fn_t          arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    /* force ALL_CHANNELS to not be a selection possibility */
    arg_cmd->data.byte[0] = ALL_CHANNELS;
    int catastrophe_count = 0;
    while (arg_cmd->data.byte[0] == ALL_CHANNELS)
    {
        if (catastrophe_count == 10) // 10 is arbitrary
        {
            UtAssert_Message(UTASSERT_CASETYPE_ABORT, __FILE__, __LINE__,
                             "CANNOT make arg_cmd->data.byte[0] != ALL_CHANNELS in 10 tries");
        }

        arg_cmd->data.byte[0] = Any_uint8_GreaterThan_or_EqualTo(CF_NUM_CHANNELS);
        ++catastrophe_count;
    }

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_CHAN_PARAM,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_CHAN_PARAM)", EventID,
                  CF_EID_ERR_CMD_CHAN_PARAM);
    UtAssert_True(local_result == -1,
                  "CF_DoChanAction returned %d and should be -1 (cmd->data.byte[0] >= CF_NUM_CHANNELS)", local_result);

} /* end Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent */

/* end CF_DoChanAction tests */

/*******************************************************************************
**
**  CF_DoFreezeThaw tests
**
*******************************************************************************/

void Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0(void)
{
    /* Arrange */
    uint8             arg_chan_num = Any_cf_channel();
    bool_arg_t        dummy_context;
    const bool_arg_t *arg_context;
    int               local_result;

    dummy_context.barg = Any_bool_arg_t_barg();

    arg_context = &dummy_context;

    /* set frozen to opposite to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    CF_AppData.hk.channel_hk[arg_chan_num].frozen = !dummy_context.barg;

    /* Act */
    local_result = CF_DoFreezeThaw(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_chan_num].frozen == dummy_context.barg,
                  "CF_DoFreezeThaw set frozen to %d and should be %d (context->barg))",
                  CF_AppData.hk.channel_hk[arg_chan_num].frozen, dummy_context.barg);
    UtAssert_True(local_result == 0, "CF_DoFreezeThaw returned %d and should be 0 (only returns 0)", local_result);

} /* end Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0 */

/* end CF_DoFreezeThaw tests */

/**************************************************************************
**
**  CF_CmdFreeze tests
**
*******************************************************************************/

void Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdFreeze(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].frozen == 1,
                  "CF_DoFreezeThaw set frozen to %d and should be 1 (freeze = 1))",
                  CF_AppData.hk.channel_hk[dummy_chan_num].frozen);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand */

/*******************************************************************************
**
**  CF_CmdThaw tests
**
*******************************************************************************/

void Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdThaw(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].frozen == 0,
                  "CF_DoFreezeThaw set frozen to %d and should be 0 (thaw = 0))",
                  CF_AppData.hk.channel_hk[dummy_chan_num].frozen);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand */

/*******************************************************************************
**
**  CF_CFDP_FindTransactionBySequenceNumberAllChannels tests
**
*******************************************************************************/

void Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL(void)
{
    /* Arrange */
    CF_TransactionSeq_t arg_ts  = Any_CF_TransactionSeq_t();
    CF_EntityId_t       arg_eid = Any_CF_EntityId_t();
    CF_Transaction_t   *local_result;
    CF_Transaction_t   *expected_result = NULL;

    CF_CFDP_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    context_CF_CFDP_FTBSN.forced_return = NULL;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN,
                     sizeof(context_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_FindTransactionBySequenceNumber, CF_NUM_CHANNELS);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_FTBSN.c, CF_AppData.engine.channels);
    UtAssert_True(context_CF_CFDP_FTBSN.transaction_sequence_number == arg_ts,
                  "CF_CFDP_FTBSN (abbr.) received transaction_sequence_number %u and should be %u (ts)",
                  context_CF_CFDP_FTBSN.transaction_sequence_number, arg_ts);
    UtAssert_True(context_CF_CFDP_FTBSN.src_eid == arg_eid,
                  "CF_CFDP_FTBSN (abbr.) received src_eid %u and should be %u (eid)", context_CF_CFDP_FTBSN.src_eid,
                  arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);

} /* end Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL */

void Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound(void)
{
    /* Arrange */
    CF_TransactionSeq_t arg_ts                   = Any_CF_TransactionSeq_t();
    CF_EntityId_t       arg_eid                  = Any_CF_EntityId_t();
    uint8               number_transaction_match = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_Transaction_t    dummy_return_value;
    CF_Transaction_t   *local_result;
    CF_Transaction_t   *expected_result = &dummy_return_value;

    CF_CFDP_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    /* set non-matching transactions */
    int i = 0;
    for (i = 0; i < number_transaction_match; ++i)
    {
        contexts_CF_CFDP_FTBSN[i].forced_return = NULL;
    }
    /* set matching transaction */
    contexts_CF_CFDP_FTBSN[i].forced_return = &dummy_return_value;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &contexts_CF_CFDP_FTBSN,
                     sizeof(contexts_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_FindTransactionBySequenceNumber, number_transaction_match + 1);
    for (i = 0; i < number_transaction_match; ++i)
    {
        UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].c, CF_AppData.engine.channels + i);
        UtAssert_True(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number == arg_ts,
                      "CF_CFDP_FTBSN (abbr.) received transaction_sequence_number %u and should be %u (ts)",
                      contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
        UtAssert_True(contexts_CF_CFDP_FTBSN[i].src_eid == arg_eid,
                      "CF_CFDP_FTBSN (abbr.) received src_eid %u and should be %u (eid)",
                      contexts_CF_CFDP_FTBSN[i].src_eid, arg_eid);
    }
    UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].c, CF_AppData.engine.channels + i);
    UtAssert_True(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number == arg_ts,
                  "CF_CFDP_FTBSN (abbr.) received transaction_sequence_number %u and should be %u (ts)",
                  contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
    UtAssert_True(contexts_CF_CFDP_FTBSN[i].src_eid == arg_eid,
                  "CF_CFDP_FTBSN (abbr.) received src_eid %u and should be %u (eid)", contexts_CF_CFDP_FTBSN[i].src_eid,
                  arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);

} /* end Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound */

/*******************************************************************************
**
**  CF_TsnChanAction tests
**
*******************************************************************************/

void Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd = &utbuf.xact;
    char                        dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                  arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t       arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                         dummy_context;
    void                       *arg_context = &dummy_context;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    arg_cmd->chan = COMPOUND_KEY;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CFDP_FindTransactionBySequenceNumberAllChannels */
    CF_CFDP_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    /* set non-matching transactions */
    int i = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        contexts_CF_CFDP_FTBSN[i].forced_return = NULL;
    }

    UT_SetDataBuffer(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &contexts_CF_CFDP_FTBSN,
                     sizeof(contexts_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_CF_TsnChanAction_fn_t));

    /* Assert */
    UtAssert_True(local_result == -1, "CF_TsnChanAction returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_TRANS_NOT_FOUND,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_TRANS_NOT_FOUND)", EventID,
                  CF_EID_ERR_CMD_TRANS_NOT_FOUND);
    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 0);
} /* end Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail */

void Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t           utbuf;
    CF_TransactionCmd_t                  *arg_cmd = &utbuf.xact;
    char                                  dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                            arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t                 arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                                   dummy_context;
    void                                 *arg_context = &dummy_context;
    CF_Transaction_t                      dummy_t;
    int                                   local_result;
    Dummy_CF_TsnChanAction_fn_t_context_t context_Dummy_CF_TsnChanAction_fn_t;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    arg_cmd->chan = COMPOUND_KEY;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    UT_SetDataBuffer(UT_KEY(Dummy_CF_TsnChanAction_fn_t), &context_Dummy_CF_TsnChanAction_fn_t,
                     sizeof(context_Dummy_CF_TsnChanAction_fn_t), false);

    /* Arrange unstubbable: CF_CFDP_FindTransactionBySequenceNumberAllChannels */
    CF_CFDP_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    /* set matching transaction */
    context_CF_CFDP_FTBSN.forced_return = &dummy_t;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN,
                     sizeof(context_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_CF_TsnChanAction_fn_t));

    /* Assert */
    UtAssert_True(local_result == CFE_SUCCESS, "CF_TsnChanAction returned %d and should be %d (CFE_SUCCESS)",
                  local_result, CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 1);
    UtAssert_True(context_Dummy_CF_TsnChanAction_fn_t.t == &dummy_t,
                  "context_Dummy_CF_TsnChanAction_fn_t.t ==  &dummy_t");
    UtAssert_True(context_Dummy_CF_TsnChanAction_fn_t.context == arg_context,
                  "context_Dummy_CF_TsnChanAction_fn_t.context ==  arg_context");
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS */

void Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t                       utbuf;
    CF_TransactionCmd_t                              *arg_cmd = &utbuf.xact;
    char                                              dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                                        arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t                             arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                                               dummy_context;
    void                                             *arg_context     = &dummy_context;
    int                                               expected_result = Any_int();
    int                                               local_result;
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TATAC;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    context_CF_TATAC.forced_return = expected_result;

    arg_cmd->chan = ALL_CHANNELS;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TATAC, sizeof(context_CF_TATAC),
                     false);

    /* Act */
    local_result = CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context);

    /* Assert */
    UtAssert_True(local_result == expected_result,
                  "CF_TsnChanAction returned %d and should be %d (ret from CF_TraverseAllTransactions_All_Channels)",
                  local_result, expected_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels */

void Test_CF_TsnChanAction_cmd_chan_IsASingleChannel(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t          utbuf;
    CF_TransactionCmd_t                 *arg_cmd       = &utbuf.xact;
    const char                           arg_cmdstr[1] = "";
    CF_TsnChanAction_fn_t                arg_fn        = &Dummy_CF_TsnChanAction_fn_t;
    int                                  dummy_context;
    void                                *arg_context     = &dummy_context;
    int                                  expected_result = Any_int();
    int                                  local_result;
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));
    arg_cmd->chan = Any_cf_channel();

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), expected_result);

    /* Act */
    local_result = CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context);

    /* Assert */
    UtAssert_True(local_result == expected_result,
                  "CF_TsnChanAction returned %d and should be %d (ret from CF_TraverseAllTransactions)", local_result,
                  expected_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* TODO: values for context_CF_TraverseAllTransactions need checked!! */
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels */

void Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd       = &utbuf.xact;
    const char                  arg_cmdstr[1] = "";
    CF_TsnChanAction_fn_t       arg_fn        = &Dummy_CF_TsnChanAction_fn_t;
    int                         dummy_context;
    void                       *arg_context = &dummy_context;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));
    arg_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_TsnChanAction returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_TSN_CHAN_INVALID,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_TSN_CHAN_INVALID)",
                  EventID, CF_EID_ERR_CMD_TSN_CHAN_INVALID);
} /* end Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid */

/*******************************************************************************
**
**  CF_DoSuspRes_ tests
**
*******************************************************************************/

void Test_CF_DoSuspRes__Asserts_t_Is_NULL(void)
{
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_DoSuspRes__Asserts_t_Is_NULL */

void Test_CF_DoSuspRes__Set_context_same_To_1_suspended_Eq_action(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    susp_res_arg_t    dummy_context;
    susp_res_arg_t   *arg_context = &dummy_context;

    /* set same to 0 to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    arg_context->same   = 0;
    arg_context->action = AnyCoinFlip();

    arg_t->flags.com.suspended = arg_context->action;

    /* Act */
    CF_DoSuspRes_(arg_t, arg_context);

    /* Assert */
    UtAssert_True(arg_context->same == 1, "CF_DoSuspRes_ set context->same to %d and should be 1 (direct set)",
                  arg_context->same);
} /* end Test_CF_DoSuspRes__Set_context_same_To_1_suspended_Eq_action */

void Test_CF_DoSuspRes__When_suspended_NotEqTo_action_Set_suspended_To_action(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    susp_res_arg_t    dummy_context;
    susp_res_arg_t   *arg_context = &dummy_context;

    /* set same to 0 to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    arg_context->same   = 0;
    arg_context->action = AnyCoinFlip();

    arg_t->flags.com.suspended = !arg_context->action;

    /* Act */
    CF_DoSuspRes_(arg_t, arg_context);

    /* Assert */
    UtAssert_True(arg_t->flags.com.suspended == arg_context->action,
                  "CF_DoSuspRes_ set arg_t->flags.com.suspended to %d and should be %d (context->action)",
                  arg_t->flags.com.suspended, arg_context->action);
} /* end Test_CF_DoSuspRes__When_suspended_NotEqTo_action_Set_suspended_To_action */

/*******************************************************************************
**
**  CF_DoSuspRes tests
**
*******************************************************************************/

void Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasChangedTo_1_SendEventAndRejectCmd(void)
{
    /* Arrange */
    CF_Transaction_t            dummy_t_instance;
    CF_Transaction_t           *dummy_t = &dummy_t_instance;
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd    = &utbuf.xact;
    uint8                       arg_action = 1; // 0 is a WAG

    memset(&utbuf, 0, sizeof(utbuf));
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    arg_cmd->chan = COMPOUND_KEY;

    /* Arrange unstubbable: CF_CFDP_FindTransactionBySequenceNumberAllChannels */
    CF_CFDP_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FindTransactionBySequenceNumber;

    context_CF_CFDP_FindTransactionBySequenceNumber.forced_return = dummy_t;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &context_CF_CFDP_FindTransactionBySequenceNumber,
                     sizeof(context_CF_CFDP_FindTransactionBySequenceNumber), false);

    /* Arrange unstubbable: CF_DoSuspRes_ */
    dummy_t->flags.com.suspended = arg_action;

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DoSuspRes(arg_cmd, arg_action);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_SUSPRES_SAME,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_SUSPRES_SAME)", EventID,
                  CF_EID_ERR_CMD_SUSPRES_SAME);
    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasChangedTo_1_SendEventAndRejectCmd */

/* TODO: Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasChangedTo_1_SendEventAndRejectCmd
** and Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasNotChangedFrom_0_SendEventAndRejectCmd
** are really unable to be told apart because they have the exact same results, there is no "actual" way to know these
** test's results are indicative of what is actually occurring except by examining the actual tests */
void Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasChangedTo_1_SendEventAndRejectCmd(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd    = &utbuf.xact;
    uint8                       arg_action = 1; // 0 is a WAG

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    arg_cmd->chan = ALL_CHANNELS;

    /* Arrange unstubbable: CF_TraverseAllTransactions_All_Channels */
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TraverseAllTransactions_All_Channels;

    context_CF_TraverseAllTransactions_All_Channels.forced_return =
        0xDCDCDCDC; /* TODO: 0xDCDCDCDC is a special value that tells stub what to do, this is TEMPORARY until the hook
                       is implemented */
    /* TODO: implement hook function */
    // UT_SetHookFunction(UT_KEY(CF_TraverseAllTransactions_All_Channels), Hook_CF_TraverseAllTransactions_All_Channels,
    // &context_CF_TraverseAllTransactions_All_Channels);
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TraverseAllTransactions_All_Channels,
                     sizeof(context_CF_TraverseAllTransactions_All_Channels), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DoSuspRes(arg_cmd, arg_action);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_SUSPRES_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_SUSPRES_CHAN)", EventID,
                  CF_EID_ERR_CMD_SUSPRES_CHAN);
    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasChangedTo_1_SendEventAndRejectCmd */

void Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasNotChangedFrom_0_SendEventAndRejectCmd(
    void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd    = &utbuf.xact;
    uint8                       arg_action = 1; // 0 is a WAG

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    arg_cmd->chan = ALL_CHANNELS;

    /* Arrange unstubbable: CF_TraverseAllTransactions_All_Channels */
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TraverseAllTransactions_All_Channels;

    context_CF_TraverseAllTransactions_All_Channels.forced_return =
        0xDC0000DC; /* TODO: 0xDC0000DC is a special value that tells stub what to do, this is TEMPORARY until the hook
                       is implemented */
    /* TODO: implement hook function */
    // UT_SetHookFunction(UT_KEY(CF_TraverseAllTransactions_All_Channels), Hook_CF_TraverseAllTransactions_All_Channels,
    // &context_CF_TraverseAllTransactions_All_Channels);
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TraverseAllTransactions_All_Channels,
                     sizeof(context_CF_TraverseAllTransactions_All_Channels), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DoSuspRes(arg_cmd, arg_action);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_SUSPRES_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_SUSPRES_CHAN)", EventID,
                  CF_EID_ERR_CMD_SUSPRES_CHAN);
    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasNotChangedFrom_0_SendEventAndRejectCmd
   */

void Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasNotChangedFrom_0_AcceptCmd(void)
{
    /* Arrange */
    CF_Transaction_t            dummy_t_instance;
    CF_Transaction_t           *dummy_t = &dummy_t_instance;
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd    = &utbuf.xact;
    uint8                       arg_action = 1; // 0 is a WAG

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    arg_cmd->chan = ALL_CHANNELS;

    /* Arrange unstubbable: CF_TraverseAllTransactions_All_Channels */
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TraverseAllTransactions_All_Channels;

    context_CF_TraverseAllTransactions_All_Channels.forced_return = 0;
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TraverseAllTransactions_All_Channels,
                     sizeof(context_CF_TraverseAllTransactions_All_Channels), false);

    /* Arrange unstubbable: CF_DoSuspRes_ */
    dummy_t->flags.com.suspended = arg_action;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DoSuspRes(arg_cmd, arg_action);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasNotChangedFrom_0_AcceptCmd */

/*******************************************************************************
**
**  CF_CmdSuspend tests
**
*******************************************************************************/
/* TODO: SendEvent needs to be able to check all values before action = 1 can be verified in
 * Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1 */
void Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_cmd = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_cmd   = &utbuf.buf;
    uint16                      local_EventID[2];

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoSuspRes */
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), local_EventID, sizeof(local_EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    dummy_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdSuspend(arg_cmd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    /* Assert CF_TsnChanAction */
    UtAssert_True(local_EventID[0] == CF_EID_ERR_CMD_TSN_CHAN_INVALID,
                  "CFE_EVS_SendEvent call 1 received %u and should have received %u (CF_EID_ERR_CMD_TSN_CHAN_INVALID)",
                  local_EventID[0], CF_EID_ERR_CMD_TSN_CHAN_INVALID);
    /* Assert CF_DoSuspRes */
    UtAssert_True(local_EventID[1] == CF_EID_ERR_CMD_SUSPRES_CHAN,
                  "CFE_EVS_SendEvent call 2 received %u and should have received %u (CF_EID_ERR_CMD_SUSPRES_CHAN)",
                  local_EventID[1], CF_EID_ERR_CMD_SUSPRES_CHAN);
    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1 */

/* end CF_CmdSuspend tests */

/*******************************************************************************
**
**  CF_CmdResume tests
**
*******************************************************************************/
/* TODO: SendEvent needs to be able to check all values before action = 0 can be verified in
 * Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0 */
void Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_cmd = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_cmd   = &utbuf.buf;
    uint16                      local_EventID[2];

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoSuspRes */
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), local_EventID, sizeof(local_EventID), false);

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    dummy_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdResume(arg_cmd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    /* Assert CF_TsnChanAction */
    UtAssert_True(local_EventID[0] == CF_EID_ERR_CMD_TSN_CHAN_INVALID,
                  "CFE_EVS_SendEvent call 1 received %u and should have received %u (CF_EID_ERR_CMD_TSN_CHAN_INVALID)",
                  local_EventID[0], CF_EID_ERR_CMD_TSN_CHAN_INVALID);
    /* Assert CF_DoSuspRes */
    UtAssert_True(local_EventID[1] == CF_EID_ERR_CMD_SUSPRES_CHAN,
                  "CFE_EVS_SendEvent call 2 received %u and should have received %u (CF_EID_ERR_CMD_SUSPRES_CHAN)",
                  local_EventID[1], CF_EID_ERR_CMD_SUSPRES_CHAN);
    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0 */

/* end CF_CmdResume tests */

/*******************************************************************************
**
**  CF_CmdCancel_ tests
**
*******************************************************************************/

void Test_CF_CmdCancel__Call_CF_CFDP_CancelTransaction_WithGiven_t(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t       = &dummy_t;
    void             *arg_ignored = NULL;
    CF_Transaction_t *context_CF_CFDP_CancelTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_CancelTransaction), &context_CF_CFDP_CancelTransaction,
                     sizeof(context_CF_CFDP_CancelTransaction), false);

    /* Act */
    CF_CmdCancel_(arg_t, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_CancelTransaction, arg_t);

} /* end Test_CF_CmdCancel__Call_CF_CFDP_CancelTransaction_WithGiven_t */

/*******************************************************************************
**
**  CF_CmdCancel tests
**
*******************************************************************************/

void Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_msg = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_msg   = &utbuf.buf;

    // TODO: CF_TsnChanAction result is default with no setup; it should be controlled, but in the essence of time this
    // default value is used to show functionality of the code
    /* Arrange unstubbable: CF_TsnChanAction */
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->chan = Any_cf_chan_num();

    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), Any_int_Except(0));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdCancel(arg_msg);

    /* Assert */
    /* Assert for CF_TsnChanAction */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.c, CF_AppData.engine.channels + dummy_msg->chan);
    UtAssert_True(context_CF_TraverseAllTransactions.fn == CF_CmdCancel_,
                  "context_CF_TraverseAllTransactions.fn ==  CF_CmdCancel_");
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.context, NULL);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction */

/*******************************************************************************
**
**  CF_CmdAbandon_ tests
**
*******************************************************************************/

void Test_CF_CmdAbandon__Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0(void)
{
    /* Arrange */
    CF_Transaction_t                   dummy_t;
    CF_Transaction_t                  *arg_t       = &dummy_t;
    void                              *arg_ignored = NULL;
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CmdAbandon_(arg_t, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 0,
                  "CF_CFDP_CancelTransaction was called with int %d and should be 0 (constant in call)",
                  context_CF_CFDP_ResetTransaction.keep_history);

} /* end Test_CF_CmdAbandon__Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0 */

/*******************************************************************************
**
**  CF_CmdAbandon tests
**
*******************************************************************************/

void Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_msg = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_msg   = &utbuf.buf;

    // TODO: CF_TsnChanAction result is default with no setup; it should be controlled, but in the essence of time this
    // default value is used to show functionality of the code
    /* Arrange unstubbable: CF_TsnChanAction */
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->chan = Any_cf_chan_num();

    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), Any_int_Except(0));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdAbandon(arg_msg);

    /* Assert */
    /* Assert for CF_TsnChanAction */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.c, CF_AppData.engine.channels + dummy_msg->chan);
    UtAssert_True(context_CF_TraverseAllTransactions.fn == CF_CmdAbandon_,
                  "context_CF_TraverseAllTransactions.fn ==  CF_CmdAbandon_");
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.context, NULL);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction */

/*******************************************************************************
**
**  CF_DoEnableDisableDequeue tests
**
*******************************************************************************/

void Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_chan_num = Any_cf_channel();
    bool_arg_t       dummy_context;
    bool_arg_t      *arg_context = &dummy_context;

    CF_AppData.config_table = &dummy_config_table;
    dummy_context.barg      = Any_bool_arg_t_barg();

    /* Act */
    CF_DoEnableDisableDequeue(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled == dummy_context.barg,
                  "Channel %u dequeue_enabled is %u and should be %u (context->barg)", arg_chan_num,
                  CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled, dummy_context.barg);

} /* end Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg */

/*******************************************************************************
**
**  CF_CmdEnableDequeue tests
**
*******************************************************************************/

void Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t dummy_config_table;
    uint8            dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnableDequeue(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled == 1,
                  "CF_CmdEnableDequeue set dequeue_enabled to %d and should be 1 (barg = 1))",
                  CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d\nACCEPTANCE OF COMMAND (+1) SHOULD BE "
                  "THE BEHAVIOR BUT IT IS NOT",
                  CF_AppData.hk.counters.cmd, initial_hk_cmd_counter);
} /* end Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction */

/*******************************************************************************
**
**  CF_CmdDisableDequeue tests
**
*******************************************************************************/

void Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t dummy_config_table;
    uint8            dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisableDequeue(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled == 0,
                  "CF_CmdEnableDequeue set dequeue_enabled to %d and should be 0 (barg = 0))",
                  CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction */

/*******************************************************************************
**
**  CF_DoEnableDisablePolldir tests
**
*******************************************************************************/

void Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    bool_msg_arg_t            dummy_context;
    bool_msg_arg_t           *arg_context = &dummy_context;
    CF_ConfigTable_t          dummy_config_table;
    uint8                     expected_enabled;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = ALL_CHANNELS;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();
    expected_enabled   = dummy_context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    uint8 current_polldir = 0;

    for (current_polldir = 0; current_polldir < CF_MAX_POLLING_DIR_PER_CHAN; ++current_polldir)
    {
        UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled == expected_enabled,
                      "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, current_polldir,
                      CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled, expected_enabled);
    }
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoEnableDisablePolldir returned %d and should be 0", local_result);
} /* end Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg */

void Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    uint8                     arg_chan_num  = Any_cf_channel();
    uint8                     dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    bool_msg_arg_t            dummy_context;
    bool_msg_arg_t           *arg_context = &dummy_context;
    CF_ConfigTable_t          dummy_config_table;
    uint8                     expected_enabled;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = dummy_polldir;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();
    expected_enabled   = dummy_context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[dummy_polldir].enabled == expected_enabled,
                  "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, dummy_polldir,
                  CF_AppData.config_table->chan[arg_chan_num].polldir[dummy_polldir].enabled, expected_enabled);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoEnableDisablePolldir returned %d and should be 0", local_result);
} /* end Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg */

void Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    bool_msg_arg_t            dummy_context;
    bool_msg_arg_t           *arg_context = &dummy_context;
    CF_ConfigTable_t          dummy_config_table;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(
        EventID == CF_EID_ERR_CMD_POLLDIR_INVALID,
        "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_ERR_CMD_POLLDIR_INVALID)", EventID,
        CF_EID_ERR_CMD_POLLDIR_INVALID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(local_result == -1, "CF_DoEnableDisablePolldir returned %d and should be -1", local_result);
} /* end Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent */

void Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    bool_msg_arg_t            dummy_context;
    bool_msg_arg_t           *arg_context = &dummy_context;
    CF_ConfigTable_t          dummy_config_table;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(
        EventID == CF_EID_ERR_CMD_POLLDIR_INVALID,
        "CFE_EVS_SendEvent receive event id 0x%04X and should receive 0x%04X (CF_EID_ERR_CMD_POLLDIR_INVALID)", EventID,
        CF_EID_ERR_CMD_POLLDIR_INVALID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(local_result == -1, "CF_DoEnableDisablePolldir returned %d and should be -1", local_result);
} /* end Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent */

/*******************************************************************************
**
**  CF_CmdEnablePolldir tests
**
*******************************************************************************/

void Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t          dummy_config_table;
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = dummy_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled == 1,
                  "Channel %u Polldir %u set to %u and should be 1 (context->barg)", dummy_channel, dummy_polldir,
                  CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled);
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess */

void Test_CF_CmdEnablePolldir_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, ALL_CHANNELS - 1);
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = error_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdEnablePolldir_FailWhenActionFail */

/* end */

/*******************************************************************************
**
**  CF_CmdDisablePolldir tests
**
*******************************************************************************/

void Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t          dummy_config_table;
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = dummy_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled == 0,
                  "Channel %u Polldir %u set to %u and should be 0 (context->barg)", dummy_channel, dummy_polldir,
                  CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled);
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess */

void Test_CF_CmdDisablePolldir_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, ALL_CHANNELS - 1);
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = error_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdDisablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdDisablePolldir_FailWhenActionFail */

/*******************************************************************************
**
**  CF_PurgeHistory tests
**
*******************************************************************************/

void Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_History_t                   dummy_h;
    CF_CListNode_t                *arg_n = &dummy_h.cl_node;
    CF_Channel_t                   dummy_c;
    CF_Channel_t                  *arg_c = &dummy_c;
    int                            local_result;
    CF_CFDP_ResetHistory_context_t context_CF_CFDP_ResetHistory;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetHistory), &context_CF_CFDP_ResetHistory, sizeof(context_CF_CFDP_ResetHistory),
                     false);

    /* Act */
    local_result = CF_PurgeHistory(arg_n, arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.c, arg_c);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.h, &dummy_h);
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_PurgeHistory returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);

} /* end Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT */

/*******************************************************************************
**
**  CF_PurgeTransaction tests
**
*******************************************************************************/

void Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t                   dummy_t;
    CF_CListNode_t                    *arg_n = &dummy_t.cl_node;
    int                                dummy_ignored;
    void                              *arg_ignored = &dummy_ignored;
    int                                local_result;
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    local_result = CF_PurgeTransaction(arg_n, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, &dummy_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 0,
                  "CF_CFDP_ResetTransaction received keep_history %u and should be 0 (constant)",
                  context_CF_CFDP_ResetTransaction.keep_history);
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_PurgeHistory returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);

} /* end Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT */

/*******************************************************************************
**
**  CF_DoPurgeQueue tests
**
*******************************************************************************/

void Test_CF_DoPurgeQueue_PendOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_start;
    CF_CListNode_t                     *expected_start = &dummy_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 0; /* pend */

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* set correct context type for CF_CList_Traverse stub */
    type_of_context_CF_CList_Traverse = POINTER;

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_PEND] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == CF_PurgeTransaction,
                  "context_CF_CList_Traverse.fn ==  CF_PurgeTransaction");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, NULL);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_PendOnly */

void Test_CF_DoPurgeQueue_HistoryOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_start;
    CF_CListNode_t                     *expected_start = &dummy_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 1; /* history */

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* set correct context type for CF_CList_Traverse stub */
    type_of_context_CF_CList_Traverse = POINTER;

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_HIST] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == (CF_CListFn_t)CF_PurgeHistory,
                  "context_CF_CList_Traverse.fn ==  (CF_CListFn_t )CF_PurgeHistory");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, dummy_c);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_HistoryOnly */

void Test_CF_DoPurgeQueue_Both(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_pend_start;
    CF_CListNode_t                     *expected_pend_start = &dummy_pend_start;
    CF_CListNode_t                      dummy_history_start;
    CF_CListNode_t                     *expected_history_start = &dummy_history_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse[2];

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 2; /* both */

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* set correct context type for CF_CList_Traverse stub */
    type_of_context_CF_CList_Traverse = POINTER;

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_PEND] = expected_pend_start;
    dummy_c->qs[CF_QueueIdx_HIST] = expected_history_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 2);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[0].start, expected_pend_start);
    UtAssert_True(context_CF_CList_Traverse[0].fn == CF_PurgeTransaction,
                  "context_CF_CList_Traverse[0].fn ==  CF_PurgeTransaction");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[0].context, NULL);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[1].start, expected_history_start);
    UtAssert_True(context_CF_CList_Traverse[1].fn == (CF_CListFn_t)CF_PurgeHistory,
                  "context_CF_CList_Traverse[1].fn ==  (CF_CListFn_t )CF_PurgeHistory");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[1].context, dummy_c);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_Both */

void Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd = &utbuf.ua;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 3; /* 3 is first default value */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_True(local_result == -1, "CF_DoPurgeQueue returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_PURGE_ARG,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_PURGE_ARG)", EventID,
                  CF_EID_ERR_CMD_PURGE_ARG);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
} /* end Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1 */

void Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd = &utbuf.ua;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = Any_uint8_GreaterThan_or_EqualTo(3);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_True(local_result == -1, "CF_DoPurgeQueue returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_PURGE_ARG,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_PURGE_ARG)", EventID,
                  CF_EID_ERR_CMD_PURGE_ARG);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
} /* end Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1 */

/*******************************************************************************
**
**  CF_CmdPurgeQueue tests
**
*******************************************************************************/

void Test_CF_CmdPurgeQueue_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_purge   = 3; /* Shortest return from CF_DoPurgeQueue */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoPurgeQueue */
    dummy_msg->data.byte[1] = error_purge;

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdPurgeQueue(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
}

/*******************************************************************************
**
**  CF_CmdWriteQueue tests
**
*******************************************************************************/

void Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    dummy_wq->chan = CF_NUM_CHANNELS;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_CHAN)", EventID,
                  CF_EID_ERR_CMD_WQ_CHAN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    dummy_wq->chan = Any_uint8_GreaterThan(CF_NUM_CHANNELS);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_CHAN)", EventID,
                  CF_EID_ERR_CMD_WQ_CHAN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* invalid combination up direction, pending queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* 0 is q_pend */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_ARGS,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_ARGS)", EventID,
                  CF_EID_ERR_CMD_WQ_ARGS);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination up direction, NOT pending queue */
    dummy_wq->type  = 1;                         /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = Any_queue_Except_q_pend(); /* 0 is q_pend */

    /* invalid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_WRITE_ONLY,
                  "CF_WrappedCreat received access %d and should be %d (OS_WRITE_ONLY)",
                  context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_OPEN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_OPEN)", EventID,
                  CF_EID_ERR_CMD_WQ_OPEN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination down direction, pending queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* 0 is q_pend */

    /* invalid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_WRITE_ONLY,
                  "CF_WrappedCreat received access %d and should be %d (OS_WRITE_ONLY)",
                  context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_OPEN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_OPEN)", EventID,
                  CF_EID_ERR_CMD_WQ_OPEN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteQueueDataToFile_Fails(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = 0;

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteQueueDataToFile */
    int32                             forced_return_CF_WriteQueueDataToFile = Any_int32_Except(0);
    CF_WriteQueueDataToFile_context_t context_CF_WriteQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteQueueDataToFile), &context_CF_WriteQueueDataToFile,
                     sizeof(context_CF_WriteQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEQ_RX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEQ_RX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEQ_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteQueueDataToFile_Fails
   */

void Test_CF_CmdWriteQueue_When_CF_WriteQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteQueueDataToFile */
    int32                             forced_return_CF_WriteQueueDataToFile = Any_int32_Except(0);
    CF_WriteQueueDataToFile_context_t context_CF_WriteQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteQueueDataToFile), &context_CF_WriteQueueDataToFile,
                     sizeof(context_CF_WriteQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEQ_RX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEQ_RX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEQ_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEHIST_RX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEHIST_RX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEHIST_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteQueueDataToFile */
    int32                             forced_return_CF_WriteQueueDataToFile = Any_int32_Except(0);
    CF_WriteQueueDataToFile_context_t context_CF_WriteQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteQueueDataToFile), &context_CF_WriteQueueDataToFile,
                     sizeof(context_CF_WriteQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEQ_TX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEQ_TX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEQ_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteQueueDataToFile */
    int32                             forced_return_CF_WriteQueueDataToFile_1st_call = 0;
    int32                             forced_return_CF_WriteQueueDataToFile_2nd_call = Any_int32_Except(0);
    CF_WriteQueueDataToFile_context_t context_CF_WriteQueueDataToFile[2];

    UT_SetDataBuffer(UT_KEY(CF_WriteQueueDataToFile), &context_CF_WriteQueueDataToFile,
                     sizeof(context_CF_WriteQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile_1st_call);
    UT_SetDeferredRetcode(UT_KEY(CF_WriteQueueDataToFile), SECOND_CALL, forced_return_CF_WriteQueueDataToFile_2nd_call);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEQ_TX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEQ_TX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEQ_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteQueueDataToFile */
    int32                             forced_return_CF_WriteQueueDataToFile = Any_int32_Except(0);
    CF_WriteQueueDataToFile_context_t context_CF_WriteQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteQueueDataToFile), &context_CF_WriteQueueDataToFile,
                     sizeof(context_CF_WriteQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEQ_PEND,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEQ_PEND)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEQ_PEND);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_WQ_WRITEHIST_TX,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_WQ_WRITEHIST_TX)", EventID,
                  CF_EID_ERR_CMD_WQ_WRITEHIST_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 4);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active */

/* Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Pend IS an error and is handled by a previous test */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteQueueDataToFile */
    int32 forced_return_CF_WriteQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteQueueDataToFile), forced_return_CF_WriteQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend */

/* end CF_CmdWriteQueue tests */

/*******************************************************************************
**
**  CF_CmdSendCfgParams tests
**
*******************************************************************************/

void Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg = &utbuf;
    CF_ConfigTable_t             dummy_config_table;
    CFE_TIME_SysTime_t           fake_time;
    CFE_SB_TransmitMsg_context_t context_CFE_SB_TransmitMsg;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_config_table.ticks_per_second             = Any_uint32();
    dummy_config_table.rx_crc_calc_bytes_per_wakeup = Any_uint32();
    dummy_config_table.ack_timer_s                  = Any_uint32();
    dummy_config_table.nak_timer_s                  = Any_uint32();
    dummy_config_table.inactivity_timer_s           = Any_uint32();
    dummy_config_table.outgoing_file_chunk_size     = Any_uint16();
    dummy_config_table.ack_limit                    = Any_uint8();
    dummy_config_table.nak_limit                    = Any_uint8();
    dummy_config_table.local_eid                    = Any_uint8();

    CF_AppData.config_table = &dummy_config_table;

    Any_CFE_TIME_SysTime_Set(&fake_time);
    UT_SetDataBuffer(UT_KEY(CFE_TIME_GetTime), &fake_time, sizeof(fake_time), false);

    UT_SetHookFunction(UT_KEY(CFE_MSG_SetMsgTime), stub_reporter, &context_CFE_MSG_SetMsgTime);
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), stub_reporter, &context_CFE_SB_TransmitMsg);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdSendCfgParams(arg_msg);

    /* Assert */
    UtAssert_True(CF_AppData.cfg.ticks_per_second == CF_AppData.config_table->ticks_per_second,
                  "cfg.ticks_per_second is %u and should be %u (config_table->ticks_per_second)",
                  CF_AppData.cfg.ticks_per_second, CF_AppData.config_table->ticks_per_second);
    UtAssert_True(
        CF_AppData.cfg.rx_crc_calc_bytes_per_wakeup == CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup,
        "cfg.rx_crc_calc_bytes_per_wakeup is %u and should be %u (config_table->rx_crc_calc_bytes_per_wakeup)",
        CF_AppData.cfg.rx_crc_calc_bytes_per_wakeup, CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup);
    UtAssert_True(CF_AppData.cfg.ack_timer_s == CF_AppData.config_table->ack_timer_s,
                  "cfg.ack_timer_s is %u and should be %u (config_table->ack_timer_s)", CF_AppData.cfg.ack_timer_s,
                  CF_AppData.config_table->ack_timer_s);
    UtAssert_True(CF_AppData.cfg.nak_timer_s == CF_AppData.config_table->nak_timer_s,
                  "cfg.nak_timer_s is %u and should be %u (config_table->nak_timer_s)", CF_AppData.cfg.nak_timer_s,
                  CF_AppData.config_table->nak_timer_s);
    UtAssert_True(CF_AppData.cfg.inactivity_timer_s == CF_AppData.config_table->inactivity_timer_s,
                  "cfg.inactivity_timer_s is %u and should be %u (config_table->inactivity_timer_s)",
                  CF_AppData.cfg.inactivity_timer_s, CF_AppData.config_table->inactivity_timer_s);
    UtAssert_True(CF_AppData.cfg.outgoing_file_chunk_size == CF_AppData.config_table->outgoing_file_chunk_size,
                  "cfg.outgoing_file_chunk_size is %u and should be %u (config_table->outgoing_file_chunk_size)",
                  CF_AppData.cfg.outgoing_file_chunk_size, CF_AppData.config_table->outgoing_file_chunk_size);
    UtAssert_True(CF_AppData.cfg.ack_limit == CF_AppData.config_table->ack_limit,
                  "cfg.ack_limit is %u and should be %u (config_table->ack_limit)", CF_AppData.cfg.ack_limit,
                  CF_AppData.config_table->ack_limit);
    UtAssert_True(CF_AppData.cfg.nak_limit == CF_AppData.config_table->nak_limit,
                  "cfg.nak_limit is %u and should be %u (config_table->nak_limit)", CF_AppData.cfg.nak_limit,
                  CF_AppData.config_table->nak_limit);
    UtAssert_True(CF_AppData.cfg.local_eid == CF_AppData.config_table->local_eid,
                  "cfg.local_eid is %u and should be %u (config_table->local_eid)", CF_AppData.cfg.local_eid,
                  CF_AppData.config_table->local_eid);
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_SetMsgTime.MsgPtr, &CF_AppData.cfg.tlm_header.Msg);
    UtAssert_True(context_CFE_MSG_SetMsgTime.Time.Seconds == fake_time.Seconds,
                  "CFE_MSG_SetMsgTime received Time.Seconds %u and should be %u (call to CFE_TIME_GetTime Seconds)",
                  context_CFE_MSG_SetMsgTime.Time.Seconds, fake_time.Seconds);
    UtAssert_True(
        context_CFE_MSG_SetMsgTime.Time.Subseconds == fake_time.Subseconds,
        "CFE_MSG_SetMsgTime received Time.Subseconds %u and should be %u (call to CFE_TIME_GetTime Subseconds)",
        context_CFE_MSG_SetMsgTime.Time.Subseconds, fake_time.Subseconds);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_ADDRESS_EQ(context_CFE_SB_TransmitMsg.MsgPtr, &CF_AppData.cfg.tlm_header.Msg);
    UtAssert_True(context_CFE_SB_TransmitMsg.IncrementSequenceCount == true,
                  "CFE_SB_TransmitMsg received IncrementSequenceCount '%s' and should be 'true'",
                  context_CFE_SB_TransmitMsg.IncrementSequenceCount ? "true" : "false");
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand */

/*******************************************************************************
**
**  CF_CmdValidateChunkSize tests
**
*******************************************************************************/

void Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t) + 1;
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateChunkSize returned %d and should be 1 (failed)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1 */

void Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = Any_uint32_GreaterThan(sizeof(CF_CFDP_PduFileDataContent_t));
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateChunkSize returned %d and should be 1 (failed)", local_result);

} /* end Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1 */

void Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t);
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateChunkSize returned %d and should be 0 (success)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0 */

void Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = Any_uint32_LessThan_or_EqualTo(sizeof(CF_CFDP_PduFileDataContent_t));
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateChunkSize returned %d and should be 0 (success)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0 */

/*******************************************************************************
**
**  CF_CmdValidateMaxOutgoing tests
**
*******************************************************************************/

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = Any_uint32_Except(0);
    uint8  arg_chan_num = Any_uint8(); /* Any_uint8() used here because it shows value does not matter in this test */
    int    local_result;

    // CF_AppData.config_table = &dummy_config_table;
    //  memcpy(CF_AppData.config_table->chan[arg_chan_num].sem_name, dummy_sem_name, 20);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateMaxOutgoing returned %d and should be 0 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success */

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t dummy_config_table;
    int              local_result;

    CF_AppData.config_table = &dummy_config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)Any_uint8_Except(0), 1);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateMaxOutgoing returned %d and should be 0 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success */

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t dummy_config_table;
    int              local_result;

    CF_AppData.config_table = &dummy_config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)0, 1);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateMaxOutgoing returned %d and should be 1 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail */

/* end CF_CmdValidateMaxOutgoing tests */

/*******************************************************************************
**
**  CF_CmdGetSetParam tests
**
*******************************************************************************/

void Test_CF_CmdGetSetParam_When_param_id_Eq_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = Any_uint8();
    uint8            arg_param_id = CF_NUM_CFG_PACKET_ITEMS;
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = Any_uint8();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_GETSET_PARAM,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_GETSET_PARAM)", EventID,
                  CF_EID_ERR_CMD_GETSET_PARAM);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdGetSetParam_When_param_id_Eq_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd */

void Test_CF_CmdGetSetParam_When_param_id_AnyGreaterThan_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = Any_uint8();
    uint8            arg_param_id = Any_uint8_GreaterThan(CF_NUM_CFG_PACKET_ITEMS);
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = Any_uint8();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_GETSET_PARAM,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_GETSET_PARAM)", EventID,
                  CF_EID_ERR_CMD_GETSET_PARAM);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %u and should be 1 more than %u", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdGetSetParam_When_param_id_AnyGreaterThan_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd */

void Test_CF_CmdGetSetParam_Given_chan_num_IsEqTo_CF_NUM_CHANNELS_ErrorOutAndCountError(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = Any_uint8();
    uint8            arg_param_id = Any_uint8_LessThan(CF_NUM_CFG_PACKET_ITEMS);
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = CF_NUM_CHANNELS;

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_GETSET_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_GETSET_CHAN)", EventID,
                  CF_EID_ERR_CMD_GETSET_CHAN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %u and should be 1 more than %u", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdGetSetParam_Given_chan_num_IsEqTo_CF_NUM_CHANNELS_ErrorOutAndCountError */

void Test_CF_CmdGetSetParam_Given_chan_num_IsGreaterThan_CF_NUM_CHANNELS_ErrorOutAndCountError(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = Any_uint8();
    uint8            arg_param_id = Any_uint8_LessThan(CF_NUM_CFG_PACKET_ITEMS);
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = Any_uint8_GreaterThan(CF_NUM_CHANNELS);

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_GETSET_CHAN,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_GETSET_CHAN)", EventID,
                  CF_EID_ERR_CMD_GETSET_CHAN);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %u and should be 1 more than %u", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdGetSetParam_Given_chan_num_IsGreaterThan_CF_NUM_CHANNELS_ErrorOutAndCountError */

void Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = 0;
    uint8            arg_param_id = 0;
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = Any_cf_chan_num();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    /* TODO: CANNOT test memcpy because copies it to a local value (the arg value, but it was not passed by ref) and
     * then only used in the SendEvent function which does not track that value as of the writing of this comment. */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_GETSET2,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_INF_CMD_GETSET2)", EventID,
                  CF_EID_INF_CMD_GETSET2);
    /* Assert for CF_CmdAcc() */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand */

void Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_SendEventAndAcceptCommand(
    void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = 1;
    uint8            arg_param_id = 0;
    uint32           arg_value    = Any_uint32();
    uint8            arg_chan_num = Any_cf_chan_num();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->ticks_per_second == arg_value,
                  "ticks_per_second is %u and should be %u (value)", CF_AppData.config_table->ticks_per_second,
                  arg_value);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_GETSET1,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_INF_CMD_GETSET1)", EventID,
                  CF_EID_INF_CMD_GETSET1);
    /* Assert for CF_CmdAcc() */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end
     Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_SendEventAndAcceptCommand
   */

/* TODO: Add randomized test that will check any SPTR value, but that will take extra time for now. */

void Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_SendEventBecause_fn_Returned_1_AndRejectCommand(
    void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = 1;
    uint8            arg_param_id = 5;
    uint32           arg_value    = Any_uint32_GreaterThan(sizeof(
        CF_CFDP_PduFileDataContent_t)); /* Arrange unstubbable: CF_CmdValidateChunkSize - specific value needed */
    uint8            arg_chan_num = Any_cf_chan_num();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    /* TODO: CANNOT test memcpy because copies it to a local value (the arg value, but it was not passed by ref) and
     * then only used in the SendEvent function which does not track that value as of the writing of this comment. */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_GETSET_VALIDATE,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_GETSET_VALIDATE)", EventID,
                  CF_EID_ERR_CMD_GETSET_VALIDATE);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %u and should be 1 more than %u", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end
     Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_SendEventBecause_fn_Returned_1_AndRejectCommand
   */

void Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_ThenCopy_value_To_ptr_Because_fn_Returned_0_SendEventAndAcceptCommand(
    void)
{
    /* Arrange */
    CF_ConfigTable_t dummy_config_table;
    uint8            arg_is_set   = 1;
    uint8            arg_param_id = 5;
    uint32           arg_value    = Any_uint32_LessThan_or_EqualTo(sizeof(
        CF_CFDP_PduFileDataContent_t)); /* Arrange unstubbable: CF_CmdValidateChunkSize - specific value needed */
    uint8            arg_chan_num = Any_cf_chan_num();

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdGetSetParam(arg_is_set, arg_param_id, arg_value, arg_chan_num);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->outgoing_file_chunk_size == arg_value,
                  "outgoing_file_chunk_size is %u and should be %u (value)",
                  CF_AppData.config_table->outgoing_file_chunk_size, arg_value);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_GETSET1,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_INF_CMD_GETSET1)", EventID,
                  CF_EID_INF_CMD_GETSET1);
    /* Assert for CF_CmdAcc() */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end
     Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_ThenCopy_value_To_ptr_Because_fn_Returned_0_SendEventAndAcceptCommand
   */

/* end CF_CmdGetSetParam tests */

/*******************************************************************************
**
**  CF_CmdSetParam tests
**
*******************************************************************************/

/* TODO: Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value only uses 0 as key, there could be more
 * tests here, but only because CF_CmdGetSetParam is unstubbable, this test mimics
 * Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_SendEventAndAcceptCommand*/
void Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value(void)
{
    /* Arrange */
    CF_UT_cmd_set_param_args_buf_t utbuf;
    CF_SetParamCmd_t              *dummy_msg = &utbuf.sp;
    CFE_SB_Buffer_t               *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_CmdGetSetParam */
    CF_ConfigTable_t dummy_config_table;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->key      = 0;
    dummy_msg->value    = Any_uint32();
    dummy_msg->chan_num = 0;

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdSetParam(arg_msg);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->ticks_per_second == dummy_msg->value,
                  "ticks_per_second is %u and should be %u (msg->value)", CF_AppData.config_table->ticks_per_second,
                  dummy_msg->value);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_GETSET1,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_INF_CMD_GETSET1)", EventID,
                  CF_EID_INF_CMD_GETSET1);
    /* Assert for CF_CmdAcc() */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value */

/* end CF_CmdSetParam tests */

/*******************************************************************************
**
**  CF_CmdGetParam tests
**
*******************************************************************************/

/* TODO: Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_And_0 only uses 0 as key, there could be more
 * tests here, but only because CF_CmdGetSetParam is unstubbable, this test mimics
 * Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand*/
void Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0(void)
{
    /* Arrange */
    CF_UT_cmd_get_param_args_buf_t utbuf;
    CF_GetParamCmd_t              *dummy_msg = &utbuf.gp;
    CFE_SB_Buffer_t               *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->key      = 0;
    dummy_msg->chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CmdGetSetParam */
    CF_ConfigTable_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdGetParam(arg_msg);

    /* Assert */
    /* Assert for CF_CmdGetSetParam */
    /* TODO: CANNOT test memcpy because copies it to a local value (the arg value, but it was not passed by ref) and
     * then only used in the SendEvent function which does not track that value as of the writing of this comment. */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CMD_GETSET2,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_INF_CMD_GETSET2)", EventID,
                  CF_EID_INF_CMD_GETSET2);
    /* Assert for CF_CmdAcc() */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0 */

/*******************************************************************************
**
**  CF_CmdEnableEngine tests
**
*******************************************************************************/

void Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg                          = NULL;
    uint32           forced_return_CF_CFDP_InitEngine = CFE_SUCCESS;

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter */

void Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg                          = NULL;
    uint32           forced_return_CF_CFDP_InitEngine = Any_uint32_Except(CFE_SUCCESS);

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_ENABLE_ENGINE,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_ENABLE_ENGINE)", EventID,
                  CF_EID_ERR_CMD_ENABLE_ENGINE);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter */

void Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 1; /* 1 is enabled */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_ENG_ALREADY_ENA,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_ENG_ALREADY_ENA)", EventID,
                  CF_EID_ERR_CMD_ENG_ALREADY_ENA);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter */

/*******************************************************************************
**
**  CF_CmdDisableEngine tests
**
*******************************************************************************/

void Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 1; /* 1 is enabled */

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisableEngine(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter */

void Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdDisableEngine(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CMD_ENG_ALREADY_DIS,
                  "CFE_EVS_SendEvent received %u and should have received %u (CF_EID_ERR_CMD_ENG_ALREADY_DIS)", EventID,
                  CF_EID_ERR_CMD_ENG_ALREADY_DIS);
    /* Assert for CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail */

/* end CF_CmdDisableEngine tests */

/*******************************************************************************
**
**  CF_ProcessGroundCommand tests
**
*******************************************************************************/

void Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = CF_NUM_COMMANDS;
    const char                  *expected_Spec                    = "CF: invalid ground command packet cmd_code=0x%02x";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), stub_reporter, &context_CFE_MSG_GetFcnCode);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CMD_GCMD_CC,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CMD_GCMD_CC)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CMD_GCMD_CC);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */ /* TODO: This will fail if initial_hk_err_counter is MAX value, but left this way to show
                                  this later and decide on a permanent fix */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent */

void Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = Any_uint8_GreaterThan(CF_NUM_COMMANDS);
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    const char                  *expected_Spec = "CF: invalid ground command packet cmd_code=0x%02x";

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), stub_reporter, &context_CFE_MSG_GetFcnCode);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CMD_GCMD_CC,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CMD_GCMD_CC)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CMD_GCMD_CC);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */ /* TODO: This will fail if initial_hk_err_counter is MAX value, but left this way to show
                                  this later and decide on a permanent fix */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent */

void Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej(
    void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_SB_Buffer_t  *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0x00; /* 0x00 forces fns[0] which is CF_CmdNoop */
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = Any_uint16_Except(
              sizeof(CF_NoArgsCmd_t)); /* sizeof(CF_NoArgsCmd_t) is expected size of CF_CmdNoop, using uint16 as a
                                           reasonable size constraint here as size_t is at least 16 bit */
    const char *expected_Spec = "CF: invalid ground command length for command 0x%02x, expected %d got %zd";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), stub_reporter, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), stub_reporter, &context_CFE_MSG_GetSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CMD_GCMD_LEN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CMD_GCMD_LEN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CMD_GCMD_LEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */ /* TODO: This will fail if initial_hk_err_counter is MAX value, but left this way to show
                                  this later and decide on a permanent fix */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej
   */

void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_SB_Buffer_t  *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0x00; /* 0x00 forces fns[0] which is CF_CmdNoop */
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize =
        sizeof(CF_NoArgsCmd_t); /* sizeof(CF_NoArgsCmd_t) is expected size of CF_CmdNoop */
    const char                  *expected_Spec = "CF: No-Op received, Version %d.%d.%d";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), stub_reporter, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), stub_reporter, &context_CFE_MSG_GetSize);

    /* Arrange unstubbable: CF_CmdNoop */
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    /* Assert for CF_CmdNoop */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_INF_CMD_NOOP,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_INF_CMD_NOOP)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_INF_CMD_NOOP);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_INFORMATION,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_INFORMATION)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdAcc */ /* TODO: This will fail if intial_hk_cmd_counter is MAX value, but left this way to show
                                  this later and decide on a permanent fix */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.counters.cmd, initial_hk_cmd_counter);
} /* end Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg */

/* TODO: Not sure if it is even possible to have a NULL get to this check, but
** Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL is forcing it
** to happen.  The reason is, I'm not sure CFE_SB_GetTotalMsgLength can ever actually return 0. */
void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = 0x0C; /* 0x0C forces a null slot */
    CFE_MSG_Size_t               forced_return_CFE_MSG_GetSize    = 0;
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), stub_reporter, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), stub_reporter, &context_CFE_MSG_GetSize);

    /* Arrange unstubbable: CF_CmdAcc, CF_CmdRej */ /* technically these are NOT called, but makes sense when looking at
                                                       other tests for CF_ProcessGroundCommand */
    uint16 initial_hk_cmd_counter = Any_uint16();
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;
    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CmdAcc */ /* TODO: just to note because of other CF_ProcessGroundCommand tests note, no problem
                                  here with overflow because no values should have been altered */
    UtAssert_True(CF_AppData.hk.counters.cmd == initial_hk_cmd_counter,
                  "CF_AppData.hk.counters.cmd is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.counters.cmd, initial_hk_cmd_counter);
    UtAssert_True(CF_AppData.hk.counters.err == initial_hk_err_counter,
                  "CF_AppData.hk.counters.err is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.counters.err, initial_hk_err_counter);
} /* end Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL */

/* end CF_ProcessGroundCommand tests */

/*******************************************************************************
**
**  cf_cmd tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CmdAcc_tests(void)
{
    UtTest_Add(Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter");
} /* end add_CF_CmdAcc_tests */

void add_CF_CmdRej_tests(void)
{
    UtTest_Add(Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter");
} /* end add_CF_CmdRej_tests */

void add_CF_CmdCond_tests(void)
{
    UtTest_Add(Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc");
    UtTest_Add(Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej");
} /* end add_CF_CmdCond_tests */

void add_CF_CmdNoop_tests(void)
{
    UtTest_Add(Test_CF_CmdNoop_SendNoopEventAndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdNoop_SendNoopEventAndAcceptCommand");
} /* end add_CF_CmdNoop_tests */

void add_CF_CmdReset_tests(void)
{
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent");
} /* end add_CF_CmdReset_tests */

void add_CF_CmdTxFile_tests(void)
{
    UtTest_Add(Test_CF_CmdTxFile_WhenCallTo_CF_CFDP_TxFile_Returns_0_AcceptCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdTxFile_WhenCallTo_CF_CFDP_TxFile_Returns_0_AcceptCommand");
    UtTest_Add(Test_CF_CmdTxFile_WhenCAllTo_CF_CFDP_TxFile_Returns_Non0_RejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdTxFile_WhenCAllTo_CF_CFDP_TxFile_Returns_Non0_RejectCommand");
} /* end add_CF_CmdTxFile_tests */

void add_CF_CmdPlaybackDir_tests(void)
{
    UtTest_Add(Test_CF_CmdPlaybackDir_WhenCAllTo_CF_CFDP_PlaybackDir_Returns_0_AcceptCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdPlaybackDir_WhenCAllTo_CF_CFDP_PlaybackDir_Returns_0_AcceptCommand");
    UtTest_Add(Test_CF_CmdPlaybackDir_WhenCallTo_CF_CmdPlaybackDir_Returns_non0_RejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdPlaybackDir_WhenCallTo_CF_CmdPlaybackDir_Returns_non0_RejectCommand");
} /* end add_CF_CmdPlaybackDir_tests */

void add_CF_DoChanAction_tests(void)
{
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_");
    UtTest_Add(Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent");
} /* end add_CF_DoChanAction_tests */

void add_CF_DoFreezeThaw_tests(void)
{
    UtTest_Add(Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0");
} /* end add_CF_DoFreezeThaw_tests */

void add_CF_CmdFreeze_tests(void)
{
    UtTest_Add(Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand");
} /* end add_CF_CmdFreeze_tests */

void add_CF_CmdThaw_tests(void)
{
    UtTest_Add(Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand");
} /* end add_CF_CmdThaw_tests */

void add_CF_CFDP_FindTransactionBySequenceNumberAllChannels_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL");
    UtTest_Add(Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CFDP_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound");
} /* end add_CF_CFDP_FindTransactionBySequenceNumberAllChannels_tests */

void add_CF_TsnChanAction_tests(void)
{
    UtTest_Add(Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_IsASingleChannel, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_IsASingleChannel");
    UtTest_Add(Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid");
} /* end add_CF_TsnChanAction_tests */

void add_CF_DoSuspRes__tests(void)
{
    UtTest_Add(Test_CF_DoSuspRes__Asserts_t_Is_NULL, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoSuspRes__Asserts_t_Is_NULL");
    UtTest_Add(Test_CF_DoSuspRes__Set_context_same_To_1_suspended_Eq_action, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoSuspRes__Set_context_same_To_1_suspended_Eq_action");
    UtTest_Add(Test_CF_DoSuspRes__When_suspended_NotEqTo_action_Set_suspended_To_action, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoSuspRes__When_suspended_NotEqTo_action_Set_suspended_To_action");
} /* end add_CF_DoSuspRes__tests */

void add_CF_DoSuspRes_tests(void)
{
    UtTest_Add(
        Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasChangedTo_1_SendEventAndRejectCmd,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasChangedTo_1_SendEventAndRejectCmd");
    UtTest_Add(
        Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasChangedTo_1_SendEventAndRejectCmd,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasChangedTo_1_SendEventAndRejectCmd");
    UtTest_Add(
        Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasNotChangedFrom_0_SendEventAndRejectCmd,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_neg1_And_args_same_WasNotChangedFrom_0_"
        "SendEventAndRejectCmd");
    UtTest_Add(Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasNotChangedFrom_0_AcceptCmd,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoSuspRes_CallTo_CF_TsnChanAction_Returns_0_And_args_same_WasNotChangedFrom_0_AcceptCmd");
} /* end add_CF_DoSuspRes_tests */

void add_CF_CmdSuspend_tests(void)
{
    UtTest_Add(Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1");
} /* end add_CF_CmdSuspend_tests */

void add_CF_CmdResume_tests(void)
{
    UtTest_Add(Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0");
} /* end add_CF_CmdResume_tests */

void add_CF_CmdCancel__tests(void)
{
    UtTest_Add(Test_CF_CmdCancel__Call_CF_CFDP_CancelTransaction_WithGiven_t, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCancel__Call_CF_CFDP_CancelTransaction_WithGiven_t");
} /* end add_CF_CF_CmdCancel__tests */

void add_CF_CmdCancel_tests(void)
{
    UtTest_Add(Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction");
} /* end add_CF_CmdCancel_tests */

void add_CF_CmdAbandon__tests(void)
{
    UtTest_Add(Test_CF_CmdAbandon__Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdAbandon__Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0");
} /* end add_CF_CmdAbandon__tests */

void add_CF_CmdAbandon_tests(void)
{
    UtTest_Add(Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction");
} /* end add_CF_CmdAbandon__tests */

void add_CF_DoEnableDisableDequeue_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg");
} /* end add_CF_DoEnableDisableDequeue_tests */

void add_CF_CmdEnableDequeue_tests(void)
{
    UtTest_Add(Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction");
} /* end add_CF_CmdEnableDequeue_tests */

void add_CF_CmdDisableDequeue_tests(void)
{
    UtTest_Add(Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction");
} /* end add_CF_CmdDisableDequeue_tests */

void add_CF_DoEnableDisablePolldir_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg");
    UtTest_Add(
        Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent");
} /* end add_CF_DoEnableDisablePolldir_tests */

void add_CF_CmdEnablePolldir_tests(void)
{
    UtTest_Add(Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_CmdEnablePolldir_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnablePolldir_FailWhenActionFail");
} /* end add_CF_CmdEnablePolldir_tests */

void add_CF_CmdDisablePolldir_tests(void)
{
    UtTest_Add(Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_CmdDisablePolldir_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdDisablePolldir_FailWhenActionFail");
} /* end add_CF_CmdDisablePolldir_tests */

void add_CF_PurgeHistory_tests(void)
{
    UtTest_Add(Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT");
} /* end add_CF_PurgeHistory_tests */

void add_CF_PurgeTransaction_tests(void)
{
    UtTest_Add(Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT");
} /* end add_CF_PurgeTransaction_tests */

void add_CF_DoPurgeQueue_tests(void)
{
    UtTest_Add(Test_CF_DoPurgeQueue_PendOnly, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoPurgeQueue_PendOnly");
    UtTest_Add(Test_CF_DoPurgeQueue_HistoryOnly, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoPurgeQueue_HistoryOnly");
    UtTest_Add(Test_CF_DoPurgeQueue_Both, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_Both");
    UtTest_Add(Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1");
    UtTest_Add(Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1");
} /* end add_CF_DoPurgeQueue_tests */

void add_CF_CmdPurgeQueue_tests(void)
{
    UtTest_Add(Test_CF_CmdPurgeQueue_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdPurgeQueue_FailWhenActionFail");
} /* end add_CF_CmdPurgeQueue_tests */

void add_CF_CmdWriteQueue_tests(void)
{
    UtTest_Add(Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteQueueDataToFile_Fails,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_"
        "SendEventCloseAndRejectCommandWhen_CF_WriteQueueDataToFile_Fails");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_"
        "WrappedClose_SendEventClosesAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active");
    /* see Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand */
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend");
} /* end add_CF_CmdWriteQueue_tests */

void add_CF_CmdSendCfgParams_tests(void)
{
    UtTest_Add(Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand");
} /* end add_CF_CmdSendCfgParams_tests */

void add_CF_CmdValidateChunkSize_tests(void)
{
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0");
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0");
} /* end add_CF_CmdValidateChunkSize_tests */

void add_CF_CmdValidateMaxOutgoing_tests(void)
{
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success");
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success");
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail");
} /* end add_CF_CmdValidateMaxOutgoing_tests */

void add_CF_CmdGetSetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdGetSetParam_When_param_id_Eq_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdGetSetParam_When_param_id_Eq_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd");
    UtTest_Add(Test_CF_CmdGetSetParam_When_param_id_AnyGreaterThan_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdGetSetParam_When_param_id_AnyGreaterThan_CF_NUM_CFG_PACKET_ITEMS_FailSendEventAndRejectCmd");
    UtTest_Add(Test_CF_CmdGetSetParam_Given_chan_num_IsEqTo_CF_NUM_CHANNELS_ErrorOutAndCountError, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdGetSetParam_Given_chan_num_IsEqTo_CF_NUM_CHANNELS_ErrorOutAndCountError");
    UtTest_Add(Test_CF_CmdGetSetParam_Given_chan_num_IsGreaterThan_CF_NUM_CHANNELS_ErrorOutAndCountError,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdGetSetParam_Given_chan_num_IsGreaterThan_CF_NUM_CHANNELS_ErrorOutAndCountError");
    UtTest_Add(Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand");
    UtTest_Add(
        Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_SendEventAndAcceptCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_"
        "SendEventAndAcceptCommand");
    UtTest_Add(
        Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_SendEventBecause_fn_Returned_1_AndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_SendEventBecause_fn_Returned_1_"
        "AndRejectCommand");
    UtTest_Add(
        Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_ThenCopy_value_To_ptr_Because_fn_Returned_0_SendEventAndAcceptCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_5_Uses_SPTRFN_ThenCopy_value_To_ptr_Because_fn_"
        "Returned_0_SendEventAndAcceptCommand");
} /* end add_CF_CmdGetSetParam_tests */

void add_CF_CmdSetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value");
} /* end add_CF_CmdSetParam_tests */

void add_CF_CmdGetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0");
} /* end add_CF_CmdGetParam_tests */

void add_CF_CmdEnableEngine_tests(void)
{
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter");
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter");
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter");
} /* end add_CF_CmdEnableEngine_tests */

void add_CF_CmdDisableEngine_tests(void)
{
    UtTest_Add(Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter");
    UtTest_Add(Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail");
} /* end add_CF_CmdDisableEngine_tests */

void add_CF_ProcessGroundCommand_tests(void)
{
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(
        Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_"
        "CmdRej");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL");
} /* end add_CF_ProcessGroundCommand_tests */

/* end cf_cmd tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_cmd_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CmdAcc_tests();

    add_CF_CmdRej_tests();

    add_CF_CmdCond_tests();

    add_CF_CmdNoop_tests();

    add_CF_CmdReset_tests();

    add_CF_CmdTxFile_tests();

    add_CF_CmdPlaybackDir_tests();

    add_CF_DoChanAction_tests();

    add_CF_DoFreezeThaw_tests();

    add_CF_CmdFreeze_tests();

    add_CF_CmdThaw_tests();

    add_CF_CFDP_FindTransactionBySequenceNumberAllChannels_tests();

    add_CF_TsnChanAction_tests();

    add_CF_DoSuspRes__tests();

    add_CF_DoSuspRes_tests();

    add_CF_CmdSuspend_tests();

    add_CF_CmdResume_tests();

    add_CF_CmdCancel__tests();

    add_CF_CmdCancel_tests();

    add_CF_CmdAbandon__tests();

    add_CF_CmdAbandon_tests();

    add_CF_DoEnableDisableDequeue_tests();

    add_CF_CmdEnableDequeue_tests();

    add_CF_CmdDisableDequeue_tests();

    add_CF_DoEnableDisablePolldir_tests();

    add_CF_CmdEnablePolldir_tests();

    add_CF_CmdDisablePolldir_tests();

    add_CF_PurgeHistory_tests();

    add_CF_PurgeTransaction_tests();

    add_CF_DoPurgeQueue_tests();

    add_CF_CmdPurgeQueue_tests();

    add_CF_CmdWriteQueue_tests();

    add_CF_CmdSendCfgParams_tests();

    add_CF_CmdValidateChunkSize_tests();

    add_CF_CmdValidateMaxOutgoing_tests();

    add_CF_CmdGetSetParam_tests();

    add_CF_CmdSetParam_tests();

    add_CF_CmdGetParam_tests();

    add_CF_CmdEnableEngine_tests();

    add_CF_CmdDisableEngine_tests();

    add_CF_ProcessGroundCommand_tests();

} /* end UtTest_Setup for cf_cmd_tests.c */

/* end cf_cmd_tests.c */
