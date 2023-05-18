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
 *   Unit specification for the Core Flight System (CFS)
 *   Limit Checker (LC) Application.
 */
#ifndef LC_APP_H
#define LC_APP_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cfe.h"
#include "lc_msg.h"
#include "lc_tbl.h"

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \name LC CDS Buffer Strings
 * \{
 */
#define LC_WRT_CDSNAME     "LC_CDS_WRT"
#define LC_ART_CDSNAME     "LC_CDS_ART"
#define LC_APPDATA_CDSNAME "LC_CDS_AppData"
/**\}*/

/**
 * \name LC Command Pipe Parameters
 * \{
 */
#define LC_PIPE_NAME "LC_CMD_PIPE"
/**\}*/

/**
 * \name Table and CDS Initialization Results
 * \{
 */
#define LC_CDS_ENABLED  0x00000001
#define LC_CDS_CREATED  0x00000002
#define LC_CDS_RESTORED 0x00000004
#define LC_CDS_UPDATED  0x00000008

#define LC_WRT_DEFAULT_DATA 0x00000010
#define LC_WRT_CDS_CREATED  0x00000020
#define LC_WRT_CDS_RESTORED 0x00000040
#define LC_WRT_TBL_CREATED  0x00000080

#define LC_ART_DEFAULT_DATA 0x00000100
#define LC_ART_CDS_CREATED  0x00000200
#define LC_ART_CDS_RESTORED 0x00000400
#define LC_ART_TBL_CREATED  0x00000800

#define LC_APP_DEFAULT_DATA 0x00001000
#define LC_APP_CDS_CREATED  0x00002000
#define LC_APP_CDS_RESTORED 0x00004000

#define LC_WDT_DEFAULT_TBL  0x00010000
#define LC_WDT_CRITICAL_TBL 0x00020000
#define LC_WDT_TBL_RESTORED 0x00040000
#define LC_WDT_NOT_CRITICAL 0x00080000

#define LC_ADT_DEFAULT_TBL  0x00100000
#define LC_ADT_CRITICAL_TBL 0x00200000
#define LC_ADT_TBL_RESTORED 0x00400000
#define LC_ADT_NOT_CRITICAL 0x00800000
/**\}*/

/**
 * \brief Wakeup for LC
 *
 * \par Description
 *      Wakes up LC every 1 second for routine maintenance whether a
 *      message was received or not.
 */
#define LC_SB_TIMEOUT 1000

/**
 * \name Hash table definitions - presumes MessageID as hash function input
 * \{
 */
#define LC_HASH_TABLE_ENTRIES 256    /**< \brief Hash table number of entries */
#define LC_HASH_TABLE_MASK    0x00FF /**< \brief Hash table mask */
/**\}*/

/** \brief Linked list of Watchpoints that reference the same MessageID */
typedef struct LC_WListTag
{
    struct LC_WListTag *Next; /**< \brief Next linked list element */

    uint16 WatchIndex; /**< \brief Watchpoint table index   */
    uint16 Spare;      /**< \brief Structure alignment pad  */
} LC_WatchPtList_t;

/** \brief Linked list of MessageID's with same hash function result */
typedef struct LC_MListTag
{
    struct LC_MListTag *Next; /**< \brief Next linked list element */

    CFE_SB_MsgId_t MessageID; /**< \brief MessageID for this link  */
    uint16         Spare;     /**< \brief Structure alignment pad  */

    LC_WatchPtList_t *WatchPtList; /**< \brief Watchpoint list for this MessageID */
} LC_MessageList_t;

/************************************************************************
 * Type Definitions
 ************************************************************************/

/**
 *  \brief LC Operational Data Structure
 *
 *  The operational data is not saved to the CDS
 */
typedef struct
{
    CFE_SB_PipeId_t CmdPipe; /**< \brief Command pipe ID                      */

    LC_WDTEntry_t *WDTPtr; /**< \brief Watchpoint  Definition Table Pointer */
    LC_ADTEntry_t *ADTPtr; /**< \brief Actionpoint Definition Table Pointer */

    LC_WRTEntry_t *WRTPtr; /**< \brief Watchpoint  Results Table Pointer    */
    LC_ARTEntry_t *ARTPtr; /**< \brief Actionpoint Results Table Pointer    */

    CFE_TBL_Handle_t WDTHandle; /**< \brief Watchpoint  Definition Table Handle  */
    CFE_TBL_Handle_t ADTHandle; /**< \brief Actionpoint Definition Table Handle  */

    CFE_TBL_Handle_t WRTHandle; /**< \brief Watchpoint  Results Table Handle     */
    CFE_TBL_Handle_t ARTHandle; /**< \brief Actionpoint Results Table Handle     */

    CFE_ES_CDSHandle_t WRTDataCDSHandle; /**< \brief Watchpoint  Results Table CDS Handle */
    CFE_ES_CDSHandle_t ARTDataCDSHandle; /**< \brief Actionpoint Results Table CDS Handle */

    CFE_ES_CDSHandle_t AppDataCDSHandle; /**< \brief Application Data CDS Handle          */

    uint16 WatchpointCount; /**< \brief Count of in-use watchpoints defined
                                        in the Watchpoint Definition Table   */

    uint16 MessageIDsCount; /**< \brief Count of unique MessageIDs referenced
                                        in the Watchpoint Definition Table   */

    LC_HkPacket_t HkPacket; /**< \brief Housekeeping telemetry packet        */

    uint32 TableResults; /**< \brief Table and CDS initialization results */

    LC_MessageList_t *HashTable[LC_HASH_TABLE_ENTRIES]; /**< \brief Each entry in the hash
                                                                   table is a linked list
                                                                   of all the MessageID's
                                                                   that the hash function
                                                                   converts to each index  */

    LC_MessageList_t MessageLinks[LC_MAX_WATCHPOINTS]; /**< \brief Message linked list elements */
    LC_WatchPtList_t WatchPtLinks[LC_MAX_WATCHPOINTS]; /**< \brief WatchPoint linked list elements */

    bool HaveActiveCDS; /**< \brief Critical Data Store in use flag      */
} LC_OperData_t;

/**
 *  \brief LC Application Data Structure
 *
 *  The application data is saved and restored from the CDS
 *  when the app is configured to use it
 */
typedef struct
{
    uint16 CmdCount;    /**< \brief Command Counter                        */
    uint16 CmdErrCount; /**< \brief Command Error Counter                  */

    uint32 APSampleCount;       /**< \brief Total count of Actionpoints sampled    */
    uint32 MonitoredMsgCount;   /**< \brief Total count of messages monitored      */
    uint32 RTSExecCount;        /**< \brief Total count of RTS sequences initiated */
    uint16 PassiveRTSExecCount; /**< \brief Total count of RTS sequences not
                                            initiated because the LC state is
                                            set to #LC_STATE_PASSIVE or the state
                                            of the actionpoint that failed is set to
                                            #LC_APSTATE_PASSIVE                    */

    uint16 CDSSavedOnExit; /**< \brief Variable that tells us if we exited clean or not */
    uint8  CurrentLCState; /**< \brief Current LC application operating state */
} LC_AppData_t;

/************************************************************************
 * Exported Data
 ************************************************************************/

extern LC_OperData_t LC_OperData; /**< \brief Operational data */
extern LC_AppData_t  LC_AppData;  /**< \brief Application data */

/************************************************************************
 * Exported Functions
 ************************************************************************/

/**
 * \brief CFS Limit Checker (LC) application entry point
 *
 *  \par Description
 *       Limit Checker application entry point and main process loop.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 */
void LC_AppMain(void);

/**
 * \brief Initialize the CFS Limit Checker (LC) application
 *
 *  \par Description
 *       Limit Checker application initialization routine. This
 *       function performs all the required startup steps to
 *       initialize (or restore from CDS) LC data structures and get
 *       the application registered with the cFE services so it can
 *       begin to receive command messages.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t LC_AppInit(void);

/**
 * \brief Initialize Event Services
 *
 *  \par Description
 *       This function performs the steps required to setup
 *       cFE Events Services for use by the LC application
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t LC_EvsInit(void);

/**
 * \brief Initialize Software Bus
 *
 *  \par Description
 *       This function performs the steps required to setup the
 *       cFE software bus for use by the LC application
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t LC_SbInit(void);

/**
 * \brief Initialize Table Services (includes CDS)
 *
 *  \par Description
 *       This function creates the tables used by the LC application and
 *       establishes the initial table values based on the configuration
 *       setting that enables the use of Critical Data Store (CDS) and
 *       the availability of stored data to restore.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa LC_SAVE_TO_CDS
 */
CFE_Status_t LC_TableInit(void);

/**
 * \brief Create Watchpoint and Actionpoint Result Tables
 *
 *  \par Description
 *       This function creates the dump only result tables used by the LC
 *       application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa #LC_TableInit
 */
CFE_Status_t LC_CreateResultTables(void);

/**
 * \brief Create Watchpoint and Actionpoint Definition Tables
 *
 *  \par Description
 *       This function creates the loadable definition tables used by the
 *       LC application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa #LC_TableInit
 */
CFE_Status_t LC_CreateDefinitionTables(void);

/**
 * \brief Create Result Table and Application Data CDS Areas
 *
 *  \par Description
 *       This function creates the loadable definition tables used by the
 *       LC application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa #LC_TableInit
 */
CFE_Status_t LC_CreateTaskCDS(void);

/**
 * \brief Load Default Table Values
 *
 *  \par Description
 *       This function loads the definition tables from table files named
 *       in the LC platform configuration header file.  The function also
 *       initializes the contents of the dump only results tables and
 *       initializes the global application data structure.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 *
 *  \sa #LC_TableInit
 */
CFE_Status_t LC_LoadDefaultTables(void);

#endif
