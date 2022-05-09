/************************************************************************
** File: lc_utils.h
**
**  Copyright (c) 2007-2020 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be
**  used, distributed and modified only pursuant to the terms of that
**  agreement.
**
** Purpose:
**   CFS Limit Checker (LC) utility functions
**
**
*************************************************************************/
#ifndef _lc_utils_h_
#define _lc_utils_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************/
/** \brief Verify message length
**
**  \par Description
**       Checks if the actual length of a software bus message matches
**       the expected length and sends an error event if a mismatch
**       occures
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MsgPtr           A #CFE_MSG_Message_t* pointer that
**                                 references the software bus message
**
**  \param [in]   ExpectedLength   The expected length of the message
**                                 based upon the command code
**
**  \returns
**  \retstmt Returns TRUE if the length is as expected      \endcode
**  \retstmt Returns FALSE if the length is not as expected \endcode
**  \endreturns
**
**  \sa #LC_LEN_ERR_EID
**
*************************************************************************/
bool LC_VerifyMsgLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

/************************************************************************/
/** \brief Manage LC application tables
**
**  \par Description
**       Checks the status of the LC application tables and provides
**       an opportunity to dump or reload tables.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt Returns CFE_SUCCESS on success
**  \retstmt Return codes from #CFE_TBL_GetAddress     \endcode
**  \endreturns
**
**  \sa #LC_WDT_GETADDR_ERR_EID, #LC_ADT_GETADDR_ERR_EID
**
*************************************************************************/
int32 LC_ManageTables(void);

/************************************************************************/
/** \brief Write to Critical Data Store (CDS)
**
**  \par Description
**       This function updates the CDS areas containing the watchpoint
**       results table, the actionpoint results table and the LC
**       application global data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_CopyToCDS  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_UpdateTaskCDS(void);

/************************************************************************/
/** \brief Perform Routine Maintenance
**
**  \par Description
**       This function manages LC tables and updates the Task CDS.
**
**  \par Assumptions, External Events, and Notes:
**       This function is called during the housekeeping cycle and
**       if the software bus times out receiving a message.  The CDS
**       is updated only at these times.  One alternative was to do
**       the CDS update every time the results tables were modified
**       but that would result in the update ocurring several times
**       per second.  By doing the update in the housekeeping and
**       timeout cases, we cut down on the update frequency at the
**       cost of the stored data being a couple of seconds old when
**       a processor reset does occur.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_ManageTables  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_PerformMaintenance(void);

#endif /* _lc_utils_h_ */
