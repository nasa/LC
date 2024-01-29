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
 *   CFS Limit Checker (LC) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef LC_INTERNAL_CFG_H
#define LC_INTERNAL_CFG_H

/**
 * \defgroup cfslcplatformcfg CFS Limit Checker Platform Configuration
 * \{
 */

/**
 * \brief Application Name
 *
 *  \par Description:
 *       This definition must match the name used at startup by the cFE
 *       Executive Services when creating the LC application.  Note that
 *       application names are also an argument to certain cFE commands.
 *       For example, the application name is needed to access tables
 *       via cFE Table Services commands.
 *
 *  \par Limits:
 *       LC requires that this name be defined, but otherwise places
 *       no limits on the definition.  Refer to CFE Executive Services
 *       for specific information on limits related to application names.
 */
#define LC_APP_NAME "LC"

/**
 * \brief Command Pipe Depth
 *
 *  \par Description:
 *       Maximum number of messages that will be allowed in the
 *       LC command pipe at one time. Used during initialization
 *       in the call to #CFE_SB_CreatePipe
 *
 *  \par Limits:
 *       This parameter can't be larger than an unsigned 16 bit
 *       integer (65535).
 */
#define LC_PIPE_DEPTH 12

/**
 * \brief LC state after power-on reset
 *
 *  \par Description:
 *       What operating state LC should initialize to after a power-on
 *       reset.
 *
 *  \par Limits:
 *       This parameter must be one of the following:
 *       #LC_STATE_ACTIVE
 *       #LC_STATE_PASSIVE
 *       #LC_STATE_DISABLED
 */
#define LC_STATE_POWER_ON_RESET LC_STATE_DISABLED

/**
 * \brief Save data to CDS compiler switch
 *
 *  \par Description:
 *       Compile switch that tells LC that we should save data
 *       over a processor or application reset by using the
 *       Critical Data Store (CDS).
 *       Comment out or \#undef to force LC to do a default (power-on)
 *       initialization sequence on all restarts (this is the
 *       default case).
 *
 *  \par Limits:
 *       n/a
 */
/* #define LC_SAVE_TO_CDS */

/**
 * \brief LC state when CDS is restored
 *
 *  \par Description:
 *       What operating state LC should initialize to after successfully
 *       restoring information from the CDS after a processor or
 *       application reset. This is only used when LC_SAVE_TO_CDS
 *       is set to true, and provides a way to override any state LC
 *       may have been operating in prior to the reset occurring.
 *
 *       If this is set to LC_STATE_FROM_CDS and LC_SAVE_TO_CDS is set
 *       to true, then the LC state will be preserved across resets and
 *       restored.  If this is not set to LC_STATE_FROM_CDS, the state
 *       saved in the CDS is overwritten by the state assigned here.
 *
 *  \par Limits:
 *       This parameter must be one of the following:
 *       #LC_STATE_ACTIVE
 *       #LC_STATE_PASSIVE
 *       #LC_STATE_DISABLED
 *       #LC_STATE_FROM_CDS
 */
#define LC_STATE_WHEN_CDS_RESTORED LC_STATE_FROM_CDS

/**
 * \brief Watchpoint Definition Table (WDT) filename
 *
 *  \par Description:
 *       Default file to load the watchpoint definition table from
 *       during a power-on reset sequence
 *
 *  \par Limits:
 *       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
 *       target platform in question
 */
#define LC_WDT_FILENAME "/cf/lc_def_wdt.tbl"

/**
 * \brief Actionpoint Definition Table (ADT) filename
 *
 *  \par Description:
 *       Default file to load the actionpoint definition table from
 *       during a power-on reset sequence
 *
 *  \par Limits:
 *       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
 *       target platform in question
 */
#define LC_ADT_FILENAME "/cf/lc_def_adt.tbl"

/**
 * \brief Maximum valid ADT RTS ID
 *
 *  \par Description:
 *       The maximum RTS ID that LC will allow during table
 *       validation in an Actionpoint Definition Table (ADT) entry
 *
 *  \par Limits:
 *       This parameter can't be larger than an unsigned 16 bit
 *       integer (65535).
 */
#define LC_MAX_VALID_ADT_RTSID 0xFFF0

/**
 * \brief Floating Point Compare Tolerance
 *
 *  \par Description:
 *       Difference between 2 floats that will still compare as
 *       equal. The default value of (1.0e-25) was taken from
 *       the GNC file mathconstants.h
 *
 *  \par Limits:
 *       The LC app does not place a limit on this parameter.
 */
#define LC_FLOAT_TOLERANCE (1.0e-25)

/**
 * \name LC Table Name Strings
 * \{
 */
#define LC_WDT_TABLENAME "LC_WDT"
#define LC_ADT_TABLENAME "LC_ADT"
#define LC_WRT_TABLENAME "LC_WRT"
#define LC_ART_TABLENAME "LC_ART"
/**\}*/

/**
 * \brief Mission specific version number for LC application
 *
 *  \par Description:
 *       An application version number consists of four parts:
 *       major version number, minor version number, revision
 *       number and mission specific revision number. The mission
 *       specific revision number is defined here and the other
 *       parts are defined in "lc_version.h".
 *
 *  \par Limits:
 *       Must be defined as a numeric value that is greater than
 *       or equal to zero.
 */
#define LC_MISSION_REV 0

/**\}*/

#endif
