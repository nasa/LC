/************************************************************************
 * NASA Docket No. GSC-18,921-1, and identified as “CFS Limit Checker
 * Application version 2.2.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   The CFS Limit Checker (LC) is a table-driven application
 *   that provides telemetry monitoring and autonomous response
 *   capabilities to Core Flight Executive (cFE) based systems.
 */

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_eventids.h"
#include "lc_msgids.h"
#include "lc_perfids.h"
#include "lc_version.h"
#include "lc_cmds.h"
#include "lc_dispatch.h"
#include "lc_action.h"
#include "lc_watch.h"
#include "lc_utils.h"
#include "lc_platform_cfg.h"
#include "lc_mission_cfg.h" /* Leave these two last to make sure all   */
#include "lc_verify.h"      /* LC configuration parameters are checked */

/************************************************************************
** LC Global Data
*************************************************************************/
LC_OperData_t LC_OperData;
LC_AppData_t  LC_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void LC_AppMain(void)
{
    int32            Status      = CFE_SUCCESS;
    bool             Initialized = false;
    uint32           RunStatus   = CFE_ES_RunStatus_APP_RUN;
    CFE_SB_Buffer_t *BufPtr      = NULL;

    /*
    ** Performance Log, Start
    */
    CFE_ES_PerfLogEntry(LC_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization
    */
    Status = LC_AppInit();

    /*
    ** Check for start-up error...
    */
    if (Status != CFE_SUCCESS)
    {
        /*
        ** Set run status to terminate main loop
        */
        RunStatus   = CFE_ES_RunStatus_APP_ERROR;
        Initialized = false;
    }
    else
    {
        Initialized = true;
    }

    /*
    ** Application main loop
    */
    while (CFE_ES_RunLoop(&RunStatus) == true)
    {
        /*
        ** Performance Log, Stop
        */
        CFE_ES_PerfLogExit(LC_APPMAIN_PERF_ID);

        /*
        ** Pend on the arrival of the next Software Bus message
        */
        Status = CFE_SB_ReceiveBuffer(&BufPtr, LC_OperData.CmdPipe, LC_SB_TIMEOUT);

        /*
        ** Performance Log, Start
        */
        CFE_ES_PerfLogEntry(LC_APPMAIN_PERF_ID);

        /*
        ** Process the software bus message
        */
        if (Status == CFE_SB_TIME_OUT)
        {
            /* Note: these routine actions are generally done in the
             * housekeeping cycle.  If we are not getting messages as
             * expected, the routine actions are done here instead. */
            Status = LC_PerformMaintenance();
        }
        else if (Status == CFE_SUCCESS)
        {
            Status = LC_AppPipe(BufPtr);
        }

        /*
        ** Note: If there were some reason to exit the task
        **       normally (without error) then we would set
        **       RunStatus = CFE_ES_APP_EXIT
        */
        if (Status != CFE_SUCCESS)
        {
            /*
            ** Set request to terminate main loop
            */
            RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

    } /* end CFS_ES_RunLoop while */

    /*
    ** Check for "fatal" process error...
    */
    if (Status != CFE_SUCCESS)
    {
        /*
        ** Send an event describing the reason for the termination
        */
        CFE_EVS_SendEvent(LC_TASK_EXIT_EID, CFE_EVS_EventType_CRITICAL, "Task terminating, err = 0x%08X",
                          (unsigned int)Status);

        /*
        ** In case cFE Event Services is not working
        */
        CFE_ES_WriteToSysLog("LC task terminating, err = 0x%08X\n", (unsigned int)Status);
    }

    /*
    ** Performance Log, Stop
    */
    CFE_ES_PerfLogExit(LC_APPMAIN_PERF_ID);

    /*
    ** Do not update CDS if inactive or startup was incomplete
    */
    if ((Initialized == true) && (LC_OperData.HaveActiveCDS) && (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED))
    {
        LC_UpdateTaskCDS();
    }

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC initialization                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_AppInit(void)
{
    CFE_Status_t Status = CFE_SUCCESS;

    /*
    ** Zero out the global data structures...
    */
    memset(&LC_OperData, 0, sizeof(LC_OperData));
    memset(&LC_AppData, 0, sizeof(LC_AppData));

    /*
    ** Initialize event services
    */
    Status = LC_EvsInit();

    /*
    ** Initialize software bus
    */
    if (Status == CFE_SUCCESS)
    {
        Status = LC_SbInit();
    }

    /*
    ** Initialize table services
    */
    if (Status == CFE_SUCCESS)
    {
        Status = LC_TableInit();
    }

    if (Status == CFE_SUCCESS)
    {
        /*
        ** If we get here, all is good
        ** Issue the application startup event message
        */
        CFE_EVS_SendEvent(LC_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "LC Initialized. Version %d.%d.%d.%d",
                          LC_MAJOR_VERSION, LC_MINOR_VERSION, LC_REVISION, LC_MISSION_REV);
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize event services interface                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_EvsInit(void)
{
    CFE_Status_t Status = CFE_SUCCESS;

    /*
    ** If an application event filter table is added
    ** in the future, initialize it here
    */

    /*
    **  Register for event services
    */
    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("LC App: Error Registering For Event Services, RC = 0x%08X\n", (unsigned int)Status);
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize the software bus interface                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_SbInit(void)
{
    CFE_Status_t Status = CFE_SUCCESS;

    /*
    ** Initialize housekeeping packet...
    */
    CFE_MSG_Init(CFE_MSG_PTR(LC_OperData.HkPacket.TelemetryHeader), CFE_SB_ValueToMsgId(LC_HK_TLM_MID),
                 sizeof(LC_HkPacket_t));

    /*
    ** Create Software Bus message pipe...
    */
    Status = CFE_SB_CreatePipe(&LC_OperData.CmdPipe, LC_PIPE_DEPTH, LC_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "Error Creating LC Pipe, RC=0x%08X",
                          (unsigned int)Status);
    }

    if (Status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to Housekeeping request messages...
        */
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(LC_SEND_HK_MID), LC_OperData.CmdPipe);
        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_SUB_HK_REQ_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error Subscribing to HK Request, MID=0x%08X, RC=0x%08X", LC_SEND_HK_MID,
                              (unsigned int)Status);
        }
    }

    if (Status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to LC ground command messages...
        */
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(LC_CMD_MID), LC_OperData.CmdPipe);
        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_SUB_GND_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error Subscribing to GND CMD, MID=0x%08X, RC=0x%08X", LC_CMD_MID, (unsigned int)Status);
        }
    }

    if (Status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to LC internal actionpoint sample messages...
        */
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(LC_SAMPLE_AP_MID), LC_OperData.CmdPipe);
        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_SUB_SAMPLE_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error Subscribing to Sample CMD, MID=0x%08X, RC=0x%08X", LC_SAMPLE_AP_MID,
                              (unsigned int)Status);
        }
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Table initialization - includes Critical Data Store (CDS)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_TableInit(void)
{
    CFE_Status_t Result;

/*
** LC task use of Critical Data Store (CDS)
**
**    Global application data (LC_AppData)
**    Watchpoint results dump only table data
**    Actionpoint results dump only table data
**
** cFE Table Services use of CDS for LC task
**
**    Watchpoint definition loadable table data
**    Actionpoint definition loadable table data
**
** LC table initialization logic re CDS
**
**    If LC cannot create all the CDS storage at startup, then LC
**    will disable LC use of CDS and continue.
**
**    If LC cannot register definition tables as critical, then LC
**    will disable LC use of CDS and re-register tables as non-critical.
**
**    If LC cannot register definition and results tables at startup,
**    then LC will terminate - table use is a required function.
**
**    If LC can create all the CDS storage and register definition
**    tables as critical, then LC will write to CDS regardless of
**    whether LC was able to read from CDS at startup.
**
**    If LC cannot restore everything from CDS at startup, then LC
**    will initialize everything - load default definition tables,
**    init results table contents, init global application data.
*/

/* lc_platform_cfg.h */
#ifdef LC_SAVE_TO_CDS
    LC_OperData.HaveActiveCDS = true;
#endif

    /*
    ** Maintain a detailed record of table initialization results
    */
    if (LC_OperData.HaveActiveCDS)
    {
        LC_OperData.TableResults |= LC_CDS_ENABLED;
    }

    /*
    ** Create watchpoint and actionpoint result tables
    */
    Result = LC_CreateResultTables();

    /*
    ** If CDS is enabled - create the 3 CDS areas managed by the LC task
    **  (continue with init, but disable CDS if unable to create all 3)
    */
    if (Result == CFE_SUCCESS)
    {
        if (LC_OperData.HaveActiveCDS)
        {
            Result = LC_CreateTaskCDS();
            if (Result != CFE_SUCCESS)
            {
                LC_OperData.HaveActiveCDS = false;
            }
        }
        /*
         ** Create wp/ap definition tables - critical if CDS enabled
         */
        Result = LC_CreateDefinitionTables();
    }

    /*
    ** CDS still active only if we created 3 CDS areas and 2 critical tables
    */
    if ((Result == CFE_SUCCESS) && (LC_OperData.HaveActiveCDS))
    {
        LC_OperData.TableResults |= LC_CDS_CREATED;
    }

    /*
    ** If any CDS area or critical table is not restored - initialize everything.
    **  (might be due to reset type, CDS disabled or corrupt, table restore error)
    */
    if (Result == CFE_SUCCESS)
    {
        if (((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == LC_WRT_CDS_RESTORED) &&
            ((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == LC_ART_CDS_RESTORED) &&
            ((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == LC_APP_CDS_RESTORED) &&
            ((LC_OperData.TableResults & LC_WDT_TBL_RESTORED) == LC_WDT_TBL_RESTORED) &&
            ((LC_OperData.TableResults & LC_ADT_TBL_RESTORED) == LC_ADT_TBL_RESTORED))
        {
            LC_OperData.TableResults |= LC_CDS_RESTORED;

            /*
            ** Get a pointer to the watchpoint definition table data...
            */
            Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

            if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
            {
                CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Error getting WDT address, RC=0x%08X", (unsigned int)Result);
            }

            /*
            ** Get a pointer to the actionpoint definition table data
            */
            if ((Result == CFE_SUCCESS) || (Result == CFE_TBL_INFO_UPDATED))
            {
                Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

                if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
                {
                    CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Error getting ADT address, RC=0x%08X", (unsigned int)Result);
                }
            }
        }
        else
        {
            Result = LC_LoadDefaultTables();
        }

        if (Result == CFE_TBL_INFO_UPDATED)
        {
            Result = CFE_SUCCESS;
        }
    }

    /*
    ** Create watchpoint hash tables -- also subscribes to watchpoint packets
    */
    if (Result == CFE_SUCCESS)
    {
        LC_CreateHashTable();

        /*
        ** Display results of CDS initialization (if enabled at startup)
        */
        if ((LC_OperData.TableResults & LC_CDS_ENABLED) == LC_CDS_ENABLED)
        {
            if ((LC_OperData.TableResults & LC_CDS_RESTORED) == LC_CDS_RESTORED)
            {
                CFE_EVS_SendEvent(LC_CDS_RESTORED_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Previous state restored from Critical Data Store");
            }
            else if ((LC_OperData.TableResults & LC_CDS_UPDATED) == LC_CDS_UPDATED)
            {
                CFE_EVS_SendEvent(LC_CDS_UPDATED_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Default state loaded and written to CDS, activity mask = 0x%08X",
                                  (unsigned int)LC_OperData.TableResults);
            }
        }
        else
        {
            CFE_EVS_SendEvent(LC_CDS_DISABLED_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "LC use of Critical Data Store disabled, activity mask = 0x%08X",
                              (unsigned int)LC_OperData.TableResults);
        }
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create WP and AP results tables        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_CreateResultTables(void)
{
    CFE_Status_t Result;
    uint32       DataSize;
    uint32       OptionFlags;

    /*
    ** Set "dump only" table option flags
    */
    OptionFlags = CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY;

    /*
    ** Register the Watchpoint Results Table (WRT) - "dump only" tables
    ** cannot be critical with CDS use managed by CFE Table Services.
    */
    DataSize = LC_MAX_WATCHPOINTS * sizeof(LC_WRTEntry_t);
    Result   = CFE_TBL_Register(&LC_OperData.WRTHandle, LC_WRT_TABLENAME, DataSize, OptionFlags, NULL);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WRT_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, "Error registering WRT, RC=0x%08X",
                          (unsigned int)Result);
    }
    else
    {
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.WRTPtr, LC_OperData.WRTHandle);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_WRT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting WRT address, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        LC_OperData.TableResults |= LC_WRT_TBL_CREATED;

        /*
        ** Register the Actionpoint Results Table (ART) - "dump only" tables
        ** cannot be critical with CDS use managed by CFE Table Services.
        */
        DataSize = LC_MAX_ACTIONPOINTS * sizeof(LC_ARTEntry_t);
        Result   = CFE_TBL_Register(&LC_OperData.ARTHandle, LC_ART_TABLENAME, DataSize, OptionFlags, NULL);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_ART_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, "Error registering ART, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.ARTPtr, LC_OperData.ARTHandle);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(LC_ART_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting ART address, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        LC_OperData.TableResults |= LC_ART_TBL_CREATED;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create WP and AP definition tables     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_CreateDefinitionTables(void)
{
    CFE_Status_t Result;
    uint32       DataSize;
    uint32       OptionFlags = CFE_TBL_OPT_DEFAULT;

    /*
    ** If CDS is still enabled, try to register the 2 definition tables as critical
    **  (if error, continue with init - but disable CDS and re-register as non-critical)
    */
    if (LC_OperData.HaveActiveCDS)
    {
        OptionFlags = CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL;
    }

    /*
    ** Register the Watchpoint Definition Table (WDT)
    */
    DataSize = LC_MAX_WATCHPOINTS * sizeof(LC_WDTEntry_t);
    Result   = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME, DataSize, OptionFlags, LC_ValidateWDT);

    if ((LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS)))
    {
        LC_OperData.HaveActiveCDS = false;
        OptionFlags               = CFE_TBL_OPT_DEFAULT;

        CFE_EVS_SendEvent(LC_WDT_REGISTER_CRIT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error registering WDT as critical table, retrying with default options, RC=0x%08X",
                          (unsigned int)Result);

        /*
        ** Re-register the Watchpoint Definition Table (WDT) non-critical
        */
        Result = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME, DataSize, OptionFlags, LC_ValidateWDT);
    }

    if (Result == CFE_TBL_INFO_RECOVERED_TBL)
    {
        LC_OperData.TableResults |= LC_WDT_CRITICAL_TBL;
        LC_OperData.TableResults |= LC_WDT_TBL_RESTORED;
        Result = CFE_SUCCESS;
    }
    else if (Result == CFE_SUCCESS)
    {
        if (LC_OperData.HaveActiveCDS)
        {
            LC_OperData.TableResults |= LC_WDT_CRITICAL_TBL;
        }
        else
        {
            LC_OperData.TableResults |= LC_WDT_NOT_CRITICAL;
        }
    }
    else
    {
        /*
        ** Task initialization fails without this table
        **
        */
        CFE_EVS_SendEvent(LC_WDT_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, "Error registering WDT, RC=0x%08X",
                          (unsigned int)Result);
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Register the Actionpoint Definition Table (ADT)
        */
        DataSize = LC_MAX_ACTIONPOINTS * sizeof(LC_ADTEntry_t);
        Result   = CFE_TBL_Register(&LC_OperData.ADTHandle, LC_ADT_TABLENAME, DataSize, OptionFlags, LC_ValidateADT);

        if ((LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS)))
        {
            LC_OperData.HaveActiveCDS = false;
            OptionFlags               = CFE_TBL_OPT_DEFAULT;

            CFE_EVS_SendEvent(LC_ADT_REGISTER_CRIT_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error registering ADT as critical table, retrying with default options, RC=0x%08X",
                              (unsigned int)Result);

            /*
            ** Re-register the Actionpoint Definition Table (ADT) non-critical
            */
            Result = CFE_TBL_Register(&LC_OperData.ADTHandle, LC_ADT_TABLENAME, DataSize, OptionFlags, LC_ValidateADT);
        }

        if (Result == CFE_TBL_INFO_RECOVERED_TBL)
        {
            LC_OperData.TableResults |= LC_ADT_CRITICAL_TBL;
            LC_OperData.TableResults |= LC_ADT_TBL_RESTORED;
            Result = CFE_SUCCESS;
        }
        else if (Result == CFE_SUCCESS)
        {
            if (LC_OperData.HaveActiveCDS)
            {
                LC_OperData.TableResults |= LC_ADT_CRITICAL_TBL;
            }
            else
            {
                LC_OperData.TableResults |= LC_ADT_NOT_CRITICAL;
            }
        }
        else
        {
            /*
            ** Task initialization fails without this table
            */
            CFE_EVS_SendEvent(LC_ADT_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, "Error registering ADT, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** In case we created a critical WDT and then created a non-critical ADT
        */
        if (((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) &&
            ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL))
        {
            /*
            ** Un-register the critical watchpoint Definition Table (WDT)
            */
            CFE_TBL_Unregister(LC_OperData.WDTHandle);

            /*
            ** Re-register the Watchpoint Definition Table (WDT) non-critical
            */
            DataSize    = LC_MAX_WATCHPOINTS * sizeof(LC_WDTEntry_t);
            OptionFlags = CFE_TBL_OPT_DEFAULT;
            Result = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME, DataSize, OptionFlags, LC_ValidateWDT);
            if (Result == CFE_SUCCESS)
            {
                LC_OperData.TableResults |= LC_WDT_NOT_CRITICAL;
            }
            else
            {
                /*
                ** Task initialization fails without this table
                */
                CFE_EVS_SendEvent(LC_WDT_REREGISTER_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Error re-registering WDT, RC=0x%08X", (unsigned int)Result);
            }
        }
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create Critical Data Store (CDS)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_CreateTaskCDS(void)
{
    CFE_Status_t Result;
    uint32       DataSize;

    /*
    ** Create CDS and try to restore Watchpoint Results Table (WRT) data
    */
    DataSize = LC_MAX_WATCHPOINTS * sizeof(LC_WRTEntry_t);
    Result   = CFE_ES_RegisterCDS(&LC_OperData.WRTDataCDSHandle, DataSize, LC_WRT_CDSNAME);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Normal result after a power on reset (cold boot) - continue with next CDS area
        */
        LC_OperData.TableResults |= LC_WRT_CDS_CREATED;
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /*
        ** Normal result after a processor reset (warm boot) - try to restore previous data
        */
        LC_OperData.TableResults |= LC_WRT_CDS_CREATED;

        Result = CFE_ES_RestoreFromCDS(LC_OperData.WRTPtr, LC_OperData.WRTDataCDSHandle);

        if (Result == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_WRT_CDS_RESTORED;
        }

        Result = CFE_SUCCESS; /* Continue to next part of restore process */
    }
    else
    {
        CFE_EVS_SendEvent(LC_WRT_CDS_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error registering WRT CDS Area, RC=0x%08X", (unsigned int)Result);
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Create CDS and try to restore Actionpoint Results Table (ART) data
        */
        DataSize = LC_MAX_ACTIONPOINTS * sizeof(LC_ARTEntry_t);
        Result   = CFE_ES_RegisterCDS(&LC_OperData.ARTDataCDSHandle, DataSize, LC_ART_CDSNAME);

        if (Result == CFE_SUCCESS)
        {
            /*
            ** Normal result after a power on reset (cold boot) - continue with next CDS area
            */
            LC_OperData.TableResults |= LC_ART_CDS_CREATED;
        }
        else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
        {
            /*
            ** Normal result after a processor reset (warm boot) - try to restore previous data
            */
            LC_OperData.TableResults |= LC_ART_CDS_CREATED;

            Result = CFE_ES_RestoreFromCDS(LC_OperData.ARTPtr, LC_OperData.ARTDataCDSHandle);

            if (Result == CFE_SUCCESS)
            {
                LC_OperData.TableResults |= LC_ART_CDS_RESTORED;
            }

            Result = CFE_SUCCESS; /* Continue to next part of restore process */
        }
        else
        {
            CFE_EVS_SendEvent(LC_ART_CDS_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error registering ART CDS Area, RC=0x%08X", (unsigned int)Result);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Create CDS and try to restore Application (APP) data
        */
        DataSize = sizeof(LC_AppData_t);
        Result   = CFE_ES_RegisterCDS(&LC_OperData.AppDataCDSHandle, DataSize, LC_APPDATA_CDSNAME);

        if (Result == CFE_SUCCESS)
        {
            /*
            ** Normal result after a power on reset (cold boot) - continue with next CDS area
            */
            LC_OperData.TableResults |= LC_APP_CDS_CREATED;
        }
        else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
        {
            /*
            ** Normal result after a processor reset (warm boot) - try to restore previous data
            */
            LC_OperData.TableResults |= LC_APP_CDS_CREATED;

            Result = CFE_ES_RestoreFromCDS(&LC_AppData, LC_OperData.AppDataCDSHandle);

            if ((Result == CFE_SUCCESS) && (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED))
            {
                /*
                ** Success - only if previous session saved CDS data at least once
                */
                LC_OperData.TableResults |= LC_APP_CDS_RESTORED;

                /*
                ** May need to override the restored application state
                */

#if LC_STATE_WHEN_CDS_RESTORED != LC_STATE_FROM_CDS
                LC_AppData.CurrentLCState = LC_STATE_WHEN_CDS_RESTORED;
#endif
            }

            Result = CFE_SUCCESS; /* Return CFE_SUCCESS */
        }
        else
        {
            CFE_EVS_SendEvent(LC_APP_CDS_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error registering application data CDS Area, RC=0x%08X", (unsigned int)Result);
        }
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - load default WP/AP definition tables   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_LoadDefaultTables(void)
{
    CFE_Status_t Result;

    /*
    ** Load default watchpoint definition table (WDT)
    */
    Result = CFE_TBL_Load(LC_OperData.WDTHandle, CFE_TBL_SRC_FILE, LC_WDT_FILENAME);

    if (Result == CFE_SUCCESS)
    {
        LC_OperData.TableResults |= LC_WDT_DEFAULT_TBL;
    }
    else
    {
        /*
        ** Task initialization fails without this table
        */
        CFE_EVS_SendEvent(LC_WDT_LOAD_ERR_EID, CFE_EVS_EventType_ERROR, "Error (RC=0x%08X) Loading WDT with '%s'",
                          (unsigned int)Result, LC_WDT_FILENAME);
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Get a pointer to the watchpoint definition table data...
        */
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

        if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
        {
            CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting WDT address, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if ((Result == CFE_SUCCESS) || (Result == CFE_TBL_INFO_UPDATED))
    {
        /*
        ** Load default actionpoint definition table (ADT)
        */
        Result = CFE_TBL_Load(LC_OperData.ADTHandle, CFE_TBL_SRC_FILE, LC_ADT_FILENAME);

        if (Result == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_ADT_DEFAULT_TBL;
        }
        else
        {
            /*
            ** Task initialization fails without this table
            */
            CFE_EVS_SendEvent(LC_ADT_LOAD_ERR_EID, CFE_EVS_EventType_ERROR, "Error (RC=0x%08X) Loading ADT with '%s'",
                              (unsigned int)Result, LC_ADT_FILENAME);
        }
    }

    if (Result == CFE_SUCCESS)
    {
        /*
        ** Get a pointer to the actionpoint definition table data
        */
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

        if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
        {
            CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting ADT address, RC=0x%08X",
                              (unsigned int)Result);
        }
    }

    if ((Result == CFE_SUCCESS) || (Result == CFE_TBL_INFO_UPDATED))
    {
        /*
        ** Initialize the watchpoint and actionpoint result table data
        */
        LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, false);
        LC_OperData.TableResults |= LC_WRT_DEFAULT_DATA;

        LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, false);
        LC_OperData.TableResults |= LC_ART_DEFAULT_DATA;

        /*
        ** Reset application data counters reported in housekeeping
        */
        LC_ResetCounters();

        /*
        ** Set LC operational state to configured startup value
        */
        LC_AppData.CurrentLCState = LC_STATE_POWER_ON_RESET;
        LC_OperData.TableResults |= LC_APP_DEFAULT_DATA;

        /*
        ** If CDS is enabled - try to update the 3 CDS areas managed by the LC task
        **  (continue, but disable CDS if unable to update all 3)
        */
        if (LC_OperData.HaveActiveCDS)
        {
            if (LC_UpdateTaskCDS() == CFE_SUCCESS)
            {
                LC_OperData.TableResults |= LC_CDS_UPDATED;
            }
            else
            {
                LC_OperData.HaveActiveCDS = false;
            }
        }
    }

    return Result;
}
