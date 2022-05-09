PROC scx_cpu1_lc_wdt1
;*******************************************************************************
;  Test Name:  lc_wdt1
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate WatchPoint Definition Table
;	(WDT) 1 BVT.
;       Note that the message ids used are borrowed from the other CFS
;	applications (MM, FM, MD, and SCH).    
;
;  WDT1:  Used by GenCmds, Monitoring, NoAction, WithAction, Initialization
;	  procedures. 20 unique MsgIds, 30 WPs, 1 entry calls a custom function,
;	  Entries include all data types and all operands
;
;  Requirements Tested
;       None
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.	
;       The LC commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet
;       LC Test application loaded and running;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		 Name		Description
;      08/08/12         W. Moleski	Initial release for LCX
;      05/10/17         W. Moleski      Updated to use CPU1 for commanding and
;                                       added a hostCPU variable for the utility
;                                       procs to connect to the proper host.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			 Description
;       create_tbl_file_from_cvt Procedure that creates a load file from
;                                the specified arguments and cvt
;                                
;
;  Expected Test Results and Analysis
;
;**********************************************************************
local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"

write ";*********************************************************************"
write ";  define local variables "
write ";*********************************************************************"

LOCAL entry
LOCAL apid
LOCAL Message1
LOCAL Message2
LOCAL Message3
LOCAL Message4
LOCAL Message5
LOCAL Message6
LOCAL Message7
LOCAL Message8
LOCAL Message9
LOCAL Message10
LOCAL Message11
LOCAL Message12
LOCAL Message13
LOCAL Message14
LOCAL Message15
LOCAL Message16
LOCAL Message17
LOCAL Message18
LOCAL Message19
LOCAL Message20
local hostCPU = "CPU3"

;; CPU1 is the default
apid = 0xfb7
;; Use CPU2 Message IDs
Message1 = 0x987
Message2 = 0x988
Message3 = 0x989
Message4 = 0x98a
Message5 = 0x98b
Message6 = 0x98c
Message7 = 0x98d
Message8 = 0x98e
Message9 = 0x98f
Message10 = 0x990
Message11 = 0x991
Message12 = 0x992
Message13 = 0x993
Message14 = 0x994
Message15 = 0x995
Message16 = 0x996
Message17 = 0x997
Message18 = 0x998
Message19 = 0x999
Message20 = 0x99a

write ";*********************************************************************"
write ";  Step 1.0:  Define Watch Point Definition Table 1. "
write ";*********************************************************************"
; WP 0
entry = 0
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_BYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LT
SCX_CPU1_LC_WDT[entry].MessageID = Message1
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000020
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed8 = 0x20
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 1
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType =LC_DATA_UBYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LE
SCX_CPU1_LC_WDT[entry].MessageID = Message2
SCX_CPU1_LC_WDT[entry].WPOffset = 19
;SCX_CPU1_LC_WDT[entry].WPOffset = 15
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000045
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned8 = 0x45
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 2
entry = entry +1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message3
SCX_CPU1_LC_WDT[entry].WPOffset = 23
;SCX_CPU1_LC_WDT[entry].WPOffset = 19
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00001345
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed16 = 0x1345
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 3
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_EQ
SCX_CPU1_LC_WDT[entry].MessageID = Message4
SCX_CPU1_LC_WDT[entry].WPOffset = 18
;SCX_CPU1_LC_WDT[entry].WPOffset = 14
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000054
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed16 = 0x54
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0 
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 4
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GE
SCX_CPU1_LC_WDT[entry].MessageID = Message5
SCX_CPU1_LC_WDT[entry].WPOffset = 20
;SCX_CPU1_LC_WDT[entry].WPOffset = 16
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x0000FF56
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xFF56
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 5
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GT
SCX_CPU1_LC_WDT[entry].MessageID = Message6
SCX_CPU1_LC_WDT[entry].WPOffset = 30
;SCX_CPU1_LC_WDT[entry].WPOffset = 26
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000130
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x0130
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 6
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_DWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message7
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x0012546F
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0x12345678
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 7
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_DWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LT
SCX_CPU1_LC_WDT[entry].MessageID = Message8
SCX_CPU1_LC_WDT[entry].WPOffset = 23
;SCX_CPU1_LC_WDT[entry].WPOffset = 19
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x23451236
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 8
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UDWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LE
SCX_CPU1_LC_WDT[entry].MessageID = Message9
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000543
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 9
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UDWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message10
SCX_CPU1_LC_WDT[entry].WPOffset = 20
;SCX_CPU1_LC_WDT[entry].WPOffset = 16
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0xF0AB1543
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 10
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_FLOAT_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_EQ
SCX_CPU1_LC_WDT[entry].MessageID = Message11
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Float32 = 1.2345
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0 
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 11
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_FLOAT_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GE
SCX_CPU1_LC_WDT[entry].MessageID = Message12
SCX_CPU1_LC_WDT[entry].WPOffset = 20
;SCX_CPU1_LC_WDT[entry].WPOffset = 16
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Float32 = 321.34
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0 
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 12
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_FLOAT_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_CUSTOM
SCX_CPU1_LC_WDT[entry].MessageID = Message13
SCX_CPU1_LC_WDT[entry].WPOffset = 26
;SCX_CPU1_LC_WDT[entry].WPOffset = 22
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Float32 = 65.987654321
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 13
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_FLOAT_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LT
SCX_CPU1_LC_WDT[entry].MessageID = Message14
SCX_CPU1_LC_WDT[entry].WPOffset = 38
;SCX_CPU1_LC_WDT[entry].WPOffset = 34
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Float32 = 3.456
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 14
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UDWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LE
SCX_CPU1_LC_WDT[entry].MessageID = Message15
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0x000000FF
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0X00000023
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 15
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UDWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message16
SCX_CPU1_LC_WDT[entry].WPOffset = 20
;SCX_CPU1_LC_WDT[entry].WPOffset = 16
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFF000000
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x12000000
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 16
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_DWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_EQ
SCX_CPU1_LC_WDT[entry].MessageID = Message17
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFF0000FF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0xFF011456
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 17
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_DWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GE
SCX_CPU1_LC_WDT[entry].MessageID = Message18
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0x000000FF
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00F3ABCD
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 18
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GT
SCX_CPU1_LC_WDT[entry].MessageID = Message19
SCX_CPU1_LC_WDT[entry].WPOffset = 22
;SCX_CPU1_LC_WDT[entry].WPOffset = 18
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000012
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x0012
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 19
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LT
SCX_CPU1_LC_WDT[entry].MessageID = Message20
SCX_CPU1_LC_WDT[entry].WPOffset = 18
;SCX_CPU1_LC_WDT[entry].WPOffset = 14
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0xFFFF1345
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x1345
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 20
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LE
SCX_CPU1_LC_WDT[entry].MessageID = Message1
SCX_CPU1_LC_WDT[entry].WPOffset = 18
;SCX_CPU1_LC_WDT[entry].WPOffset = 14
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x0000F123
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xF123
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 21
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message2
SCX_CPU1_LC_WDT[entry].WPOffset = 16
;SCX_CPU1_LC_WDT[entry].WPOffset = 12
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000543
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed16 = 0x0543
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 22
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UBYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_EQ
SCX_CPU1_LC_WDT[entry].MessageID = Message3
SCX_CPU1_LC_WDT[entry].WPOffset = 26
;SCX_CPU1_LC_WDT[entry].WPOffset = 22
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x000000F3
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xF3
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 23
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_BYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GE
SCX_CPU1_LC_WDT[entry].MessageID = Message4
SCX_CPU1_LC_WDT[entry].WPOffset = 17
;SCX_CPU1_LC_WDT[entry].WPOffset = 13
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000031
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed8 = 0x31
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0  
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 24
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_BYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GT
SCX_CPU1_LC_WDT[entry].MessageID = Message5
SCX_CPU1_LC_WDT[entry].WPOffset = 22
;SCX_CPU1_LC_WDT[entry].WPOffset = 18
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x000000AB
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xAB
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 25
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UBYTE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LT
SCX_CPU1_LC_WDT[entry].MessageID = Message6
SCX_CPU1_LC_WDT[entry].WPOffset = 21
;SCX_CPU1_LC_WDT[entry].WPOffset = 17
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x000000C5
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xC5
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 26
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_LE
SCX_CPU1_LC_WDT[entry].MessageID = Message7
SCX_CPU1_LC_WDT[entry].WPOffset = 27
;SCX_CPU1_LC_WDT[entry].WPOffset = 23
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x0000B123
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xB123
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 27
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_WORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_NE
SCX_CPU1_LC_WDT[entry].MessageID = Message8
SCX_CPU1_LC_WDT[entry].WPOffset = 30
;SCX_CPU1_LC_WDT[entry].WPOffset = 26
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed32 = 0x000009AC
SCX_CPU1_LC_WDT[entry].ComparisonValue.Signed16 = 0x09AC
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 28
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_EQ
SCX_CPU1_LC_WDT[entry].MessageID = Message9
SCX_CPU1_LC_WDT[entry].WPOffset = 20
;SCX_CPU1_LC_WDT[entry].WPOffset = 16
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x0000FABC
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xFABC
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

; WP 29
entry = entry + 1
SCX_CPU1_LC_WDT[entry].DataType = LC_DATA_UWORD_LE
SCX_CPU1_LC_WDT[entry].OperatorID = LC_OPER_GE
SCX_CPU1_LC_WDT[entry].MessageID = Message10
SCX_CPU1_LC_WDT[entry].WPOffset = 17
;SCX_CPU1_LC_WDT[entry].WPOffset = 13
SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000678
SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x0678
SCX_CPU1_LC_WDT[entry].CustFctArgument = 0
SCX_CPU1_LC_WDT[entry].StaleAge = 0

;; zero out the rest of the table
for entry=30 to LC_MAX_WATCHPOINTS-1 do
  SCX_CPU1_LC_WDT[entry].DataType = LC_WATCH_NOT_USED
  SCX_CPU1_LC_WDT[entry].OperatorID = LC_NO_OPER
  SCX_CPU1_LC_WDT[entry].MessageID = 0
  SCX_CPU1_LC_WDT[entry].WPOffset = 0
  SCX_CPU1_LC_WDT[entry].Bitmask = 0xFFFFFFFF
  SCX_CPU1_LC_WDT[entry].ComparisonValue.UnSigned32 = 0
  SCX_CPU1_LC_WDT[entry].CustFctArgument = 0 
  SCX_CPU1_LC_WDT[entry].StaleAge = 0
enddo 
;; Restore procedure logging
%liv (log_procedure) = logging

local wpIndex = LC_MAX_WATCHPOINTS - 1
local startMnemonic = "SCX_CPU1_LC_WDT[0]"
local endMnemonic = "SCX_CPU1_LC_WDT[" & wpIndex & "]"
local tableName = LC_APP_NAME & ".LC_WDT"
 
s create_tbl_file_from_cvt(hostCPU,apid,"WDTTable1","lc_def_wdt1.tbl",tableName,startMnemonic,endMnemonic)

write ";*********************************************************************"
write ";  End procedure SCX_CPU1_lc_wdt1                                     "
write ";*********************************************************************"
ENDPROC
