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
 *   Specification for the CFS Limit Checker (LC) table related
 *   constant definitions.
 *
 * @note
 *   These Macro definitions have been put in this file (instead of
 *   lc_tbl.h) so this file can be included directly into ASIST build
 *   test scripts. ASIST RDL files can accept C language \#defines but
 *   can't handle type definitions. As a result: DO NOT PUT ANY
 *   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE!
 *   ADD THEM TO lc_tbl.h IF NEEDED!
 */
#ifndef LC_TBLDEFS_H
#define LC_TBLDEFS_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \name LC CDS Saved on Exit Identifiers
 * \{
 */
#define LC_CDS_SAVED     0xF0F0 /**< \brief CDS saved on exit */
#define LC_CDS_NOT_SAVED 0x0F0F /**< \brief CDS not saved on exit */
/**\}*/

/**
 * \name Watchpoint Definition Table (WDT) DataType Enumerated Types
 * \{
 */
#define LC_DATA_WATCH_NOT_USED  0xFF /**< \brief Use for empty entries */
#define LC_DATA_WATCH_BYTE      1    /**< \brief 8 bit signed byte */
#define LC_DATA_WATCH_UBYTE     2    /**< \brief 8 bit unsigned byte */
#define LC_DATA_WATCH_WORD_BE   3    /**< \brief 16 bit signed word big endian byte order */
#define LC_DATA_WATCH_WORD_LE   4    /**< \brief 16 bit signed word little endian byte order */
#define LC_DATA_WATCH_UWORD_BE  5    /**< \brief 16 bit unsigned word big endian byte order */
#define LC_DATA_WATCH_UWORD_LE  6    /**< \brief 16 bit unsigned word little endian byte order */
#define LC_DATA_WATCH_DWORD_BE  7    /**< \brief 32 bit signed double word big endian byte order */
#define LC_DATA_WATCH_DWORD_LE  8    /**< \brief 32 bit signed double word little endian byte order */
#define LC_DATA_WATCH_UDWORD_BE 9    /**< \brief 32 bit unsigned double word big endian byte order */
#define LC_DATA_WATCH_UDWORD_LE 10   /**< \brief 32 bit unsigned double word little endian byte order */
#define LC_DATA_WATCH_FLOAT_BE                                             \
    11 /**< \brief 32 bit single precision IEEE-754 floating point number, \
        *          big endian byte order                                   \
        */
#define LC_DATA_WATCH_FLOAT_LE                                             \
    12 /**< \brief 32 bit single precision IEEE-754 floating point number, \
        *          little endian byte order                                \
        */
/**\}*/

/**
 * \name Watchpoint Definition Table (WDT) OperatorID Enumerated Types
 * \{
 */
#define LC_OPER_NONE   0xFF /**< \brief Use for empty entries         */
#define LC_OPER_LT     1    /**< \brief Less Than (<)                 */
#define LC_OPER_LE     2    /**< \brief Less Than or Equal To (<=)    */
#define LC_OPER_NE     3    /**< \brief Not Equal (!=)                */
#define LC_OPER_EQ     4    /**< \brief Equal (==)                    */
#define LC_OPER_GE     5    /**< \brief Greater Than or Equal To (>=) */
#define LC_OPER_GT     6    /**< \brief Greater Than (>)              */
#define LC_OPER_CUSTOM 7    /**< \brief Use custom function           */
/**\}*/

/**
 * \name Watchpoint Definition Table (WDT) BitMask Enumerated Types
 * \{
 */
#define LC_BITMASK_NONE 0xFFFFFFFF /**< \brief Use for no masking       */
/**\}*/

/**
 * \name Actionpoint Definition Table (ADT) Reverse Polish Operators
 * \{
 */
#define LC_RPN_AND   0xFFF1 /**< \brief RPN and operator */
#define LC_RPN_OR    0xFFF2 /**< \brief RPN or operator */
#define LC_RPN_XOR   0xFFF3 /**< \brief RPN xor operator */
#define LC_RPN_NOT   0xFFF4 /**< \brief RPN not operator */
#define LC_RPN_EQUAL 0xFFF5 /**< \brief RPN equal operator */
/**\}*/

/**
 * \name Watchpoint Results Table (WRT) WatchResult Enumerated Types
 * \{
 */
#define LC_WATCH_STALE 0xFF /**< \brief Watchpoint stale */
#define LC_WATCH_FALSE 0    /**< \brief Watchpoint false, must be zero for RPN evaluation */
#define LC_WATCH_TRUE  1    /**< \brief Watchpoint true, must be one for RPN evaluation */
#define LC_WATCH_ERROR 2    /**< \brief Watchpoint error */
/**\}*/

/**
 * \name Actionpoint Results Table (ART) ActionResult Enumerated Types
 * \{
 */
#define LC_ACTION_STALE 0xFF /**< \brief Actionpoint stale */
#define LC_ACTION_PASS  0    /**< \brief Actionpoint pass */
#define LC_ACTION_FAIL  1    /**< \brief Actionpoint fail */
#define LC_ACTION_ERROR 2    /**< \brief Actionpoint error */
/**\}*/

/**
 * \name Watchpoint Definition Table (WDT) Validation Error Enumerated Types
 * \{
 */
#define LC_WDTVAL_ERR_NONE     0 /**< \brief No error                          */
#define LC_WDTVAL_ERR_DATATYPE 1 /**< \brief Invalid DataType                  */
#define LC_WDTVAL_ERR_OPER     2 /**< \brief Invalid OperatorID                */
#define LC_WDTVAL_ERR_MID      3 /**< \brief Invalid MessageID                 */
#define LC_WDTVAL_ERR_FPNAN    4 /**< \brief ComparisonValue is NAN float      */
#define LC_WDTVAL_ERR_FPINF    5 /**< \brief ComparisonValue is infinite float */
/**\}*/

/**
 * \name Actionpoint Definition Table (ADT) Validation Error Enumerated Types
 * \{
 */
#define LC_ADTVAL_ERR_NONE     0 /**< \brief No error                          */
#define LC_ADTVAL_ERR_DEFSTATE 1 /**< \brief Invalid DefaultState              */
#define LC_ADTVAL_ERR_RTSID    2 /**< \brief Invalid RTSId                     */
#define LC_ADTVAL_ERR_FAILCNT  3 /**< \brief MaxFailsBeforeRTS is zero         */
#define LC_ADTVAL_ERR_EVTTYPE  4 /**< \brief Invalid EventType                 */
#define LC_ADTVAL_ERR_RPN      5 /**< \brief Invalid Reverse Polish Expression */
/**\}*/

#ifndef LC_OMIT_DEPRECATED

#define LC_NO_OPER       LC_OPER_NONE
#define LC_NO_BITMASK    LC_BITMASK_NONE
#define LC_ADTVAL_NO_ERR LC_ADTVAL_ERR_NONE
#define LC_WDTVAL_NO_ERR LC_WDTVAL_ERR_NONE

#define LC_DATA_NOT_USED  LC_DATA_WATCH_NOT_USED
#define LC_DATA_BYTE      LC_DATA_WATCH_BYTE
#define LC_DATA_UBYTE     LC_DATA_WATCH_UBYTE
#define LC_DATA_WORD_BE   LC_DATA_WATCH_WORD_BE
#define LC_DATA_WORD_LE   LC_DATA_WATCH_WORD_LE
#define LC_DATA_UWORD_BE  LC_DATA_WATCH_UWORD_BE
#define LC_DATA_UWORD_LE  LC_DATA_WATCH_UWORD_LE
#define LC_DATA_DWORD_BE  LC_DATA_WATCH_DWORD_BE
#define LC_DATA_DWORD_LE  LC_DATA_WATCH_DWORD_LE
#define LC_DATA_UDWORD_BE LC_DATA_WATCH_UDWORD_BE
#define LC_DATA_UDWORD_LE LC_DATA_WATCH_UDWORD_LE
#define LC_DATA_FLOAT_BE  LC_DATA_WATCH_FLOAT_BE
#define LC_DATA_FLOAT_LE  LC_DATA_WATCH_FLOAT_LE

#endif

#endif
