PROC $sc_$cpu_lc_wdt2a
;*******************************************************************************
;  Test Name:  lc_wdt2a
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the WatchPoint Definition
;	Table (WDT).
;       Note that the message ids used are borrowed from the other CFS 
;	applications (MM, FM, MD, and SCH).    
;
;  WDT2a:  Used by TableTesting procedure.  
;         19 unique MsgIds, 31 WPs, Entries include 
;         all data types and all operands
;
;  Requirements Tested
;       None
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands.	
;       The LC commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet
;       LC Test application loaded and running;
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;      08/10/12         W. Moleski      Initial release for LCX
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			 Description
;       create_tbl_file_from_cvt Procedure that creates a load file from
;                                the specified arguments and cvt
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
local hostCPU = "$CPU"

;; CPU1 is the default
apid = 0xfb7
;; User CPU2 Message IDs
Message1 = 0x989
Message2 = 0x988
Message3 = 0x987
Message4 = 0x98a
Message5 = 0x98b
Message6 = 0x98c
Message7 = 0x98d
Message8 = 0x98e
Message9 = 0x98f
Message10 = 0x998
Message11 = 0x991
Message12 = 0x992
Message13 = 0x993
Message14 = 0x994
Message15 = 0x995
Message16 = 0x996
Message17 = 0x997
Message18 = 0x999
Message19 = 0x99a

write ";*********************************************************************"
write ";  Step 1.0:  Define Watch Point Definition Table 2. "
write ";*********************************************************************"
; Entry 1
entry = 0
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_BYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message1
$SC_$CPU_LC_WDT[entry].WPOffset = 10
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x0000003F
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed8 = 0x3F
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 2
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message1
$SC_$CPU_LC_WDT[entry].WPOffset = 19
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00001234
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed16 = 0x1234
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 3
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UBYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message2
$SC_$CPU_LC_WDT[entry].WPOffset = 15
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000045
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0x45
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 4
entry = entry +1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_BYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LT
$SC_$CPU_LC_WDT[entry].MessageID = Message3
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000020
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed8 = 0x20
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 5
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message4
$SC_$CPU_LC_WDT[entry].WPOffset = 14
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000054
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed16 = 0x0054
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0 
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 6
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GE
$SC_$CPU_LC_WDT[entry].MessageID = Message5
$SC_$CPU_LC_WDT[entry].WPOffset = 16
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x0000FF56
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xFF56
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 7
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_FLOAT_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message6
$SC_$CPU_LC_WDT[entry].WPOffset = 26
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000130
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 8
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_DWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GT
$SC_$CPU_LC_WDT[entry].MessageID = Message7
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x0012546F
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0x12345678
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 9
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_DWORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LT
$SC_$CPU_LC_WDT[entry].MessageID = Message8
$SC_$CPU_LC_WDT[entry].WPOffset = 19
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x23451236
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 10
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UDWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message9
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000543
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 11
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UBYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message10
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0xC3
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xC3
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 12
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message11
$SC_$CPU_LC_WDT[entry].WPOffset = 18
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000123
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed16 = 0x0123
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0 
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 13
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_FLOAT_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message11
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Float32 = 1.2345
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0 
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 14
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_FLOAT_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GE
$SC_$CPU_LC_WDT[entry].MessageID = Message12
$SC_$CPU_LC_WDT[entry].WPOffset = 16
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Float32 = 321.342
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 15
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_FLOAT_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message13
$SC_$CPU_LC_WDT[entry].WPOffset = 22
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Float32 = 65.9876543
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 16
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_FLOAT_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GT
$SC_$CPU_LC_WDT[entry].MessageID = Message14
$SC_$CPU_LC_WDT[entry].WPOffset = 34
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Float32 = 3.456
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 17
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UDWORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message15
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0x00FF0000
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00000023
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0


; Entry 18
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UDWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message16
$SC_$CPU_LC_WDT[entry].WPOffset = 16
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFF000000
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x12000000
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0


; Entry 19
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_DWORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message17
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFF0000FF
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0xFF000056
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 20
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UWORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GT
$SC_$CPU_LC_WDT[entry].MessageID = Message18
$SC_$CPU_LC_WDT[entry].WPOffset = 18
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000012
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x0012
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 21
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LT
$SC_$CPU_LC_WDT[entry].MessageID = Message19
$SC_$CPU_LC_WDT[entry].WPOffset = 14
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x00001345
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0x1345
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 22
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message3
$SC_$CPU_LC_WDT[entry].WPOffset = 14
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x0000F123
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xF123
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 23
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message2
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000543
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed16 = 0x0543
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 24
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UBYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message1
$SC_$CPU_LC_WDT[entry].WPOffset = 3
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x000000F3
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xF3
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 25
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_BYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GE
$SC_$CPU_LC_WDT[entry].MessageID = Message4
$SC_$CPU_LC_WDT[entry].WPOffset = 13
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x00000031
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed8 = 0x31
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0  
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 26
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_BYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_GT
$SC_$CPU_LC_WDT[entry].MessageID = Message5
$SC_$CPU_LC_WDT[entry].WPOffset = 18
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x000000AB
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xAB
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 27
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UBYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LT
$SC_$CPU_LC_WDT[entry].MessageID = Message6
$SC_$CPU_LC_WDT[entry].WPOffset = 17
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x000000C5
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0xC5
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 28
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message7
$SC_$CPU_LC_WDT[entry].WPOffset = 23
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x0000B123
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xB123
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 29
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_WORD_LE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_NE
$SC_$CPU_LC_WDT[entry].MessageID = Message8
$SC_$CPU_LC_WDT[entry].WPOffset = 26
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed32 = 0x000009AC
$SC_$CPU_LC_WDT[entry].ComparisonValue.Signed16 = 0x09AC
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 30
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UWORD_BE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_EQ
$SC_$CPU_LC_WDT[entry].MessageID = Message9
$SC_$CPU_LC_WDT[entry].WPOffset = 16
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x0000FABC
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned16 = 0xFABC
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

; Entry 31
entry = entry + 1
$SC_$CPU_LC_WDT[entry].DataType = LC_DATA_UBYTE
$SC_$CPU_LC_WDT[entry].OperatorID = LC_OPER_LE
$SC_$CPU_LC_WDT[entry].MessageID = Message13
$SC_$CPU_LC_WDT[entry].WPOffset = 12
$SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
;;$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0x0000001
$SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned8 = 0x01
$SC_$CPU_LC_WDT[entry].CustFctArgument = 0
$SC_$CPU_LC_WDT[entry].StaleAge = 0

;zero out the rest of the table
for entry=31 to LC_MAX_WATCHPOINTS-1 do
  $SC_$CPU_LC_WDT[entry].DataType = LC_WATCH_NOT_USED
  $SC_$CPU_LC_WDT[entry].OperatorID = LC_NO_OPER
  $SC_$CPU_LC_WDT[entry].MessageID = 0
  $SC_$CPU_LC_WDT[entry].WPOffset = 0
  $SC_$CPU_LC_WDT[entry].Bitmask = 0xFFFFFFFF
  $SC_$CPU_LC_WDT[entry].ComparisonValue.UnSigned32 = 0
  $SC_$CPU_LC_WDT[entry].CustFctArgument = 0 
  $SC_$CPU_LC_WDT[entry].StaleAge = 0
enddo 
;; Restore procedure logging
%liv (log_procedure) = logging

local wpIndex = LC_MAX_WATCHPOINTS - 1
local startMnemonic = "$SC_$CPU_LC_WDT[0]"
local endMnemonic = "$SC_$CPU_LC_WDT[" & wpIndex & "]"
local tableName = LC_APP_NAME & ".LC_WDT"

s create_tbl_file_from_cvt(hostCPU,apid,"WDTTable2a","lc_def_wdt2a.tbl",tableName,startMnemonic,endMnemonic)

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_lc_wdt2a                                     "
write ";*********************************************************************"
ENDPROC
