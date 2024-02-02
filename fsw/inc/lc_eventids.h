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
 *   Specification for the CFS Limit Checker (LC) event identifiers.
 */
#ifndef LC_EVENTS_H
#define LC_EVENTS_H

/**
 * \defgroup cfslcevents CFS Limit Checker Event IDs
 * \{
 */

#define LC_RESERVED_EID 0 /**< \brief Reserved EID */

/**
 * \brief LC Application Fatal Termination Event ID
 *
 *  \par Type: CRITICAL
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker
 *  exits due to a fatal error condition.
 */
#define LC_TASK_EXIT_EID 1

/**
 * \brief LC Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker has
 *  completed initialization.
 */
#define LC_INIT_INF_EID 2

/**
 * \brief LC Create Command Pipe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker
 *  is unable to create its command pipe via the #CFE_SB_CreatePipe
 *  API
 */
#define LC_CR_PIPE_ERR_EID 3

/**
 * \brief LC Subscribe To Housekeeping Request Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker
 *  is unable to subscribe to its Housekeeping Request message
 *  via the #CFE_SB_Subscribe API
 */
#define LC_SUB_HK_REQ_ERR_EID 4

/**
 * \brief LC Subscribe To Ground Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker
 *  is unable to subscribe to its ground commands via the
 *  #CFE_SB_Subscribe API
 */
#define LC_SUB_GND_CMD_ERR_EID 5

/**
 * \brief LC Subscribe To Actionpoint Sample Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Limit Checker
 *  is unable to subscribe to its actionpoint sample command via the
 *  #CFE_SB_Subscribe API
 */
#define LC_SUB_SAMPLE_CMD_ERR_EID 6

/**
 * \brief LC Watchpoint Definition Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the watchpoint definition
 *  table (WDT) could not be registered.
 */
#define LC_WDT_REGISTER_ERR_EID 7

/**
 * \brief LC Watchpoint Definition Table Reregistration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the watchpoint definition
 *  table (WDT) could not be registered non-critical after first
 *  registering and then unregistering the table as critical.
 *  This sequence can only occur when the WDT succeeds and the
 *  ADT fails the attempt to register as a critical table. This
 *  error is extremely unlikely to occur.
 */
#define LC_WDT_REREGISTER_ERR_EID 8

/**
 * \brief LC Actionpoint Definition Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the actionpoint definition
 *  table (ADT) could not be registered.
 */
#define LC_ADT_REGISTER_ERR_EID 9

/**
 * \brief LC Watchpoint Results Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the watchpoint results
 *  table (WRT) could not be registered.
 */
#define LC_WRT_REGISTER_ERR_EID 10

/**
 * \brief LC Actionpoint Results Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the actionpoint results
 *  table (ART) could not be registered.
 */
#define LC_ART_REGISTER_ERR_EID 11

/**
 * \brief LC Watchpoint Result Table CDS Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CDS area for the
 *  watchpoint results table (WRT) data could not be registered.
 */
#define LC_WRT_CDS_REGISTER_ERR_EID 12

/**
 * \brief LC Actionpoint Result Table CDS Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CDS area for the
 *  actionpont results table (ART) data could not be registered.
 */
#define LC_ART_CDS_REGISTER_ERR_EID 13

/**
 * \brief LC Application Data CDS Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the CDS area for the
 *  LC application data could not be registered.
 */
#define LC_APP_CDS_REGISTER_ERR_EID 14

/**
 * \brief LC Watchpoint Definition Table Load From File Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an error is encountered
 *  loading the watchpoint definition table (WDT) from the default
 *  file image
 */
#define LC_WDT_LOAD_ERR_EID 15

/**
 * \brief LC Actionpoint Definition Table Load From File Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an error is encountered
 *  loading the actionpoint definition table (ADT) from the default
 *  file image
 */
#define LC_ADT_LOAD_ERR_EID 16

/**
 * \brief LC Watchpoint Result Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the address can't be obtained
 *  from table services for the watchpoint results table (WRT).
 */
#define LC_WRT_GETADDR_ERR_EID 17

/**
 * \brief LC Actionpoint Result Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the address can't be obtained
 *  from table services for the actionpoint results table (ART).
 */
#define LC_ART_GETADDR_ERR_EID 18

/**
 * \brief LC Watchpoint Definition Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the address can't be obtained
 *  from table services for the watchpoint definition table (WDT).
 */
#define LC_WDT_GETADDR_ERR_EID 19

/**
 * \brief LC Actionpoint Definition Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the address can't be obtained
 *  from table services for the actionpoint definition table (ADT).
 */
#define LC_ADT_GETADDR_ERR_EID 20

/**
 * \brief LC State Restored From CDS Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued following a processor reset when the
 *  entire LC execution state has been successfully restored from the
 *  Critical Data Store (CDS). Application global data, actionpoint
 *  results data and watchpoint results data were restored from CDS
 *  areas managed directly by LC. Actionpoint definition table data
 *  and watchpoint definition table data were restored from the CDS
 *  area managed by cFE Table Services.
 *
 *  Note that an attempt to use CDS is only made when the appropriate
 *  parameter is enabled in the LC platform configuration header file.
 *
 *  \sa LC_SAVE_TO_CDS, #LC_CDS_UPDATED_INF_EID, #LC_CDS_DISABLED_INF_EID
 */
#define LC_CDS_RESTORED_INF_EID 21

/**
 * \brief LC Default State Initialized and Stored to CDS Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued following LC startup initialization
 *  when the entire LC execution state has been set to default values
 *  successfully stored to the Critical Data Store (CDS). Application
 *  global data, actionpoint results data and watchpoint results data
 *  were stored to CDS areas managed directly by LC. Actionpoint and
 *  watchpoint definition table data are stored to the CDS area
 *  managed by cFE Table Services.
 *
 *  This event implies that the CDS areas were created successfully.
 *  The reason that previous data was not restored may be due to the
 *  reset type (warm vs cold) or to a failure to restore the entire
 *  set of data described above. If data from any CDS area cannot be
 *  restored, then all data is set to defaults and written to CDS.
 *
 *  Note that an attempt to use CDS is only made when the appropriate
 *  parameter is enabled in the LC platform configuration header file.
 *
 *  \sa LC_SAVE_TO_CDS, #LC_CDS_RESTORED_INF_EID, #LC_CDS_DISABLED_INF_EID
 */
#define LC_CDS_UPDATED_INF_EID 22

/**
 * \brief LC Use Of CDS Disabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued following LC startup initialization
 *  when the entire LC execution state has been set to default values
 *  successfully stored to the Critical Data Store (CDS). Application
 *  global data, actionpoint results data and watchpoint results data
 *  were stored to CDS areas managed directly by LC. Actionpoint and
 *  watchpoint definition table data are stored to the CDS area
 *  managed by cFE Table Services.
 *
 *  This event implies that the CDS areas were not created successfully
 *  or that LC was unable to write the default values to CDS.
 *
 *  Note that an attempt to use CDS is only made when the appropriate
 *  parameter is enabled in the LC platform configuration header file.
 *
 *  \sa LC_SAVE_TO_CDS, #LC_CDS_RESTORED_INF_EID, #LC_CDS_UPDATED_INF_EID
 */
#define LC_CDS_DISABLED_INF_EID 23

/**
 * \brief LC Command Code Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a software bus message is received
 *  with an invalid command code.
 */
#define LC_CC_ERR_EID 24

/**
 * \brief LC Sample Actionpoint Message Start Or End Number Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SAMPLE_AP_MID message
 *  has been received with an invalid actionpoint start or end number specified
 */
#define LC_APSAMPLE_APNUM_ERR_EID 25

/**
 * \brief LC No-op Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a NOOP command has been received.
 */
#define LC_NOOP_INF_EID 26

/**
 * \brief LC Reset Counters Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a reset counters command has
 *  been received.
 */
#define LC_RESET_INF_EID 27

/**
 * \brief LC Set Limit Checker State Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_LC_STATE_CC command
 *  has been successfully executed
 */
#define LC_LCSTATE_INF_EID 28

/**
 * \brief LC Set Limit Checker State, State Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_LC_STATE_CC command
 *  has been received with an invalid state argument specified
 */
#define LC_LCSTATE_ERR_EID 29

/**
 * \brief LC Set Actionpoint State New State Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_STATE_CC command
 *  has been received with an invalid state argument specified
 */
#define LC_APSTATE_NEW_ERR_EID 30

/**
 * \brief LC Set Actionpoint State Current State Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_STATE_CC command
 *  has been received and the current actionpoint state is either
 *  #LC_APSTATE_NOT_USED or #LC_APSTATE_PERMOFF which can only be changed
 *  with a table load.
 */
#define LC_APSTATE_CURR_ERR_EID 31

/**
 * \brief LC Set Actionpoint State Actionpoint Number Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_STATE_CC command
 *  has been received with an invalid actionpoint number specified
 */
#define LC_APSTATE_APNUM_ERR_EID 32

/**
 * \brief LC Set Actionpoint State Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_STATE_CC command
 *  has been successfully executed
 */
#define LC_APSTATE_INF_EID 33

/**
 * \brief LC Set Actionpoint Permanently Off Actionpoint Number Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_PERM_OFF_CC command
 *  has been received with an invalid actionpoint number specified
 */
#define LC_APOFF_APNUM_ERR_EID 34

/**
 * \brief LC Set Actionpoint Permanently Off Actionpoint Not Disabled Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_PERM_OFF_CC command
 *  has been received and the current actionpoint state is not
 *  #LC_APSTATE_DISABLED
 */
#define LC_APOFF_CURR_ERR_EID 35

/**
 * \brief LC Set Actionpoint Permanently Off Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SET_AP_PERM_OFF_CC command
 *  has been successfully executed
 */
#define LC_APOFF_INF_EID 36

/**
 * \brief LC Reset Actionpoint Stats Actionpoint Number Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_RESET_AP_STATS_CC command
 *  has been received with an invalid actionpoint number specified
 */
#define LC_APSTATS_APNUM_ERR_EID 37

/**
 * \brief LC Reset Actionpoint Stats Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_RESET_AP_STATS_CC command
 *  has been successfully executed
 */
#define LC_APSTATS_INF_EID 38

/**
 * \brief LC Reset Watchpoint Stats Watchpoint Number Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_RESET_WP_STATS_CC command
 *  has been received with an invalid watchpoint number specified
 */
#define LC_WPSTATS_WPNUM_ERR_EID 39

/**
 * \brief LC Reset Watchpoint Stats Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_RESET_WP_STATS_CC command
 *  has been successfully executed
 */
#define LC_WPSTATS_INF_EID 40

/**
 * \brief LC Housekeeping Request Message Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a housekeeping request is received
 *  with a message length that doesn't match the expected value.
 */
#define LC_HKREQ_LEN_ERR_EID 41

/**
 * \brief LC Actionpoint Sample Message Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint sample request is received
 *  with a message length that doesn't match the expected value.
 */
#define LC_APSAMPLE_LEN_ERR_EID 42

/**
 * \brief LC Command Message Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a ground command message is received
 *  with a message length that doesn't match the expected value.
 */
#define LC_CMD_LEN_ERR_EID 43

/**
 * \brief LC Watchpoint Message Unsubscribe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an error is encountered
 *  unsubscribing to a watchpoint message ID
 */
#define LC_UNSUB_WP_ERR_EID 44

/**
 * \brief LC Watchpoint Message Subscribe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an error is encountered
 *  when subscribing to a watchpoint message iD
 */
#define LC_SUB_WP_ERR_EID 45

/**
 * \brief LC Watchpoint Results Table Not Saved To CDS On Exit Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the watchpoint results
 *  table (WRT) data could not be saved to the CDS on application
 *  exit.
 */
#define LC_WRT_NO_SAVE_ERR_EID 46

/**
 * \brief LC Actionpoint Results Table Not Saved To CDS On Exit Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the actionpoint results
 *  table (ART) data could not be saved to the CDS on application
 *  exit.
 */
#define LC_ART_NO_SAVE_ERR_EID 47

/**
 * \brief LC Application Data Not Saved To CDS On Startup Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the LC application data
 *  could not be saved to the CDS on application startup.
 */
#define LC_APP_NO_SAVE_START_ERR_EID 48

/**
 * \brief LC Unreferenced Message ID Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a software bus message has
 *  been received that isn't a recognized LC message and has no
 *  defined watchpoints referencing it's message ID
 */
#define LC_MID_INF_EID 49

/**
 * \brief LC Watchpoint Data Type Undefined Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued whenever an undefined watchpoint data type
 *  identifier is detected
 */
#define LC_WP_DATATYPE_ERR_EID 50

/**
 * \brief LC Watchpoint Operator ID Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued whenever an undefined watchpoint
 *  operator identifier is detected
 */
#define LC_WP_OPERID_ERR_EID 51

/**
 * \brief LC Watchpoint Data Value Is Float NAN Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a watchpoint is defined as a float
 *  type, but the extracted value would equate to a floating point
 *  NAN (not-a-number) value
 */
#define LC_WP_NAN_ERR_EID 52

/**
 * \brief LC Watchpoint Offset Value Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a watchpoint offset value extends past
 *  the end of the message as reported by the #CFE_MSG_GetSize
 *  function
 */
#define LC_WP_OFFSET_ERR_EID 53

/**
 * \brief LC Watchpoint Definition Table Validation Floating Point Check Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued on the first error when a table validation
 *  fails for a watchpoint definition table (WDT) load and the error is
 *  a failed floating point check. This error is caused when the data type
 *  for a wachpoint definition is floating point and the comparison value
 *  equates to a floating point NAN (not-a-number) or infinite value.
 */
#define LC_WDTVAL_FPERR_EID 54

/**
 * \brief LC Watchpoint Definition Table Validation Failed (Not Floating Point Check) Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued on the first error when a table validation
 *  fails for a watchpoint definition table (WDT) load and the error is
 *  NOT a failed floating point check.
 */
#define LC_WDTVAL_ERR_EID 55

/**
 * \brief LC Watchpoint Definition Table Validation Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a table validation has been
 *  completed for a watchpoint definition table (WDT) load
 */
#define LC_WDTVAL_INF_EID 56

/**
 * \brief LC Sample Actionpoint Current Actionpoint State Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the #LC_SAMPLE_AP_MID message
 *  has been received and the current state for the specified
 *  actionpoint state is either #LC_APSTATE_NOT_USED or #LC_APSTATE_PERMOFF.
 */
#define LC_APSAMPLE_CURR_ERR_EID 57

/**
 * \brief LC Actionpoint State Change From PASS To FAIL Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint evaluation transitions
 *  from #LC_ACTION_PASS to #LC_ACTION_FAIL
 */
#define LC_AP_PASSTOFAIL_INF_EID 58

/**
 * \brief LC Actionpoint Failed While Limit Checker State Passive Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint fails evaluation while
 *  the LC task operating state is #LC_STATE_PASSIVE
 */
#define LC_PASSIVE_FAIL_DBG_EID 59

/**
 * \brief LC Actionpoint Failed While Actionpoint State Passive Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint fails evaluation while
 *  the actionpoint state is #LC_APSTATE_PASSIVE
 */
#define LC_AP_PASSIVE_FAIL_INF_EID 60

/**
 * \brief LC Actionpoint State Change From FAIL To PASS Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint evaluation transitions
 *  from #LC_ACTION_FAIL to #LC_ACTION_PASS
 */
#define LC_AP_FAILTOPASS_INF_EID 61

/**
 * \brief LC Actionpoint Evaluated To Error Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an actionpoint evaluation
 *  results in #LC_ACTION_ERROR
 */
#define LC_ACTION_ERROR_ERR_EID 62

/**
 * \brief LC Actionpoint RPN Expression Illegal Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an illegal Reverse Polish Notation
 *  (RPN) expression is detected during an actionpoint evaluation
 */
#define LC_INVALID_RPN_ERR_EID 63

/**
 * \brief LC Actionpoint Definition Table Validation RPN Equation Check Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued on the first error when a table validation
 *  fails for an actionpoint definition table (ADT) load and the error is
 *  a failed RPN equation check.
 */
#define LC_ADTVAL_RPNERR_EID 64

/**
 * \brief LC Actionpoint Definition Table Validation Failed (Not RPN Equation Check) Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued on the first error when a table validation
 *  fails for an actioinpoint definition table (ADT) load and the error is
 *  NOT a failed RPN equation check.
 */
#define LC_ADTVAL_ERR_EID 65

/**
 * \brief LC Actionpoint Definition Table Validation Result Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a table validation has been
 *  completed for an actionpoint definition table (ADT) load
 */
#define LC_ADTVAL_INF_EID 66

/**
 * \brief LC LC_CustomFunction Called With Unexpected Watchpoint ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the mission specific custom
 *  function #LC_CustomFunction is called with an unexpected
 *  watchpoint ID
 */
#define LC_CFCALL_ERR_EID 67

/**
 * \brief LC Watchdog Definition Table Critical Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the watchpoint definition
 *  table (WDT) could not be registered as a critical table.
 */
#define LC_WDT_REGISTER_CRIT_ERR_EID 68

/**
 * \brief LC Actionpoint Definition Table Critical Table Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the actionpoint definition
 *  table (ADT) could not be registered as a critical table.
 */
#define LC_ADT_REGISTER_CRIT_ERR_EID 69

/**
 * \brief LC Actionpoint Definition Table Base ID
 *
 *  \par Type: User defined in Actionpoint Definition Table
 *
 *  \par Cause:
 *
 *  The actionpoint base event ID is designed to avoid conflicts
 *  between the event ID's defined above for use by the LC application
 *  and the user defined event ID's in the actionpoint table.
 *
 *  These events are generated when the evaluation of an actionpoint
 *  results in sending a command to the stored command (SC) processor
 *  to start a real time command sequence (RTS).  The event text is
 *  user defined and specific to the particular actionpoint.
 *
 *  Note that user defined event ID's can be easily recognized if the
 *  base number is easily recognizable.  For example, using the value
 *  1000 for the base event ID and using the actionpoint table index as
 *  the offset portion creates an obvious correlation.  Thus, if an LC
 *  event ID is 1025 then it is immediately apparent that the event is
 *  the user defined event for actionpoint table index 25.
 */
#define LC_BASE_AP_EID 1000

/**\}*/

#endif
