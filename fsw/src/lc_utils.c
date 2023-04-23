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
 *   CFS Limit Checker (LC) utility functions
 */

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_cmds.h"
#include "lc_msgids.h"
#include "lc_eventids.h"
#include "lc_version.h"
#include "lc_action.h"
#include "lc_watch.h"
#include "lc_platform_cfg.h"
#include "lc_utils.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Manage tables - chance to be dumped, reloaded, etc.             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_ManageTables(void)
{
    CFE_Status_t Result;

    /*
    ** It is not necessary to release dump only table pointers before
    **  calling cFE Table Services to manage the table
    */
    CFE_TBL_Manage(LC_OperData.WRTHandle);
    CFE_TBL_Manage(LC_OperData.ARTHandle);

    /*
    ** Must release loadable table pointers before allowing updates
    */
    CFE_TBL_ReleaseAddress(LC_OperData.WDTHandle);
    CFE_TBL_ReleaseAddress(LC_OperData.ADTHandle);

    CFE_TBL_Manage(LC_OperData.WDTHandle);
    CFE_TBL_Manage(LC_OperData.ADTHandle);

    /*
    ** Re-acquire the pointers and check for new table data
    */
    Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /*
        ** Clear watchpoint results for previous table
        */
        LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, false);

        /*
        ** Create watchpoint hash tables -- also subscribes to watchpoint packets
        */
        LC_CreateHashTable();
    }
    else if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting WDT address, RC=0x%08X",
                          (unsigned int)Result);
        return Result;
    }

    Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /*
        ** Clear actionpoint results for previous table
        */
        LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, false);
    }
    else if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR, "Error getting ADT address, RC=0x%08X",
                          (unsigned int)Result);
        return Result;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Update Critical Data Store (CDS)                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_UpdateTaskCDS(void)
{
    CFE_Status_t Result;

    /*
    ** Copy the watchpoint results table (WRT) data to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.WRTDataCDSHandle, LC_OperData.WRTPtr);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WRT_NO_SAVE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to update watchpoint results in CDS, RC=0x%08X", (unsigned int)Result);
        return Result;
    }

    /*
    ** Copy the actionpoint results table (ART) data to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.ARTDataCDSHandle, LC_OperData.ARTPtr);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ART_NO_SAVE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to update actionpoint results in CDS, RC=0x%08X", (unsigned int)Result);
        return Result;
    }

    /*
    ** Set the "data has been saved" indicator
    */
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /*
    ** Copy the global application data structure to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.AppDataCDSHandle, &LC_AppData);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_APP_NO_SAVE_START_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to update application data in CDS, RC=0x%08X", (unsigned int)Result);
        return Result;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Perform Background Maintenance Tasks                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t LC_PerformMaintenance(void)
{
    CFE_Status_t Result;

    /*
    ** Manage tables - allow cFE to perform dump, update, etc.
    **  (an error here is fatal - LC must be able to access its tables)
    */
    Result = LC_ManageTables();

    if (Result == CFE_SUCCESS)
    {
        if (LC_OperData.HaveActiveCDS)
        {
            /*
            ** If CDS is enabled - update the 3 CDS areas managed by LC
            **  (continue, but disable CDS if unable to update all 3)
            */
            if (LC_UpdateTaskCDS() != CFE_SUCCESS)
            {
                LC_OperData.HaveActiveCDS = false;
            }
        }
    }

    return Result;
}
