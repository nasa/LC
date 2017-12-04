PROC $sc_$cpu_lc_withaction
;*******************************************************************************
;  Test Name:  lc_withaction
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this test is to verify that Limit Checker (LC)
;	functions properly when monitoring WatchPoints (WP). All WP evaluate to
;	a mix of true, false, and stale. All ActionPoints (AP) evaluate to a mix
;	of pass, fail, error, and stale so some actions will be taken.
;	During this test the LC is set as follows:
;		LC Active with all AP disabled.  
;		LC Active with all AP active.
;		LC Active with all AP passive
;		LC Active with AP in mix of states (active, passive, disabled,
;		   permanently disabled, unused)
;		LC Passive with all AP active
;		LC Disabled with all AP active
;
;	The following WP true/false transitions and AP pass/fail transitions are
;	tested:
;		WP T/F Transitions Tested
;		WP1	T, T, T, T	WP7	F, T, F, T
;		WP2	T, T, T, F	WP8	F, T, T, F
;		WP3	T, T, F, F	WP9	F, T, T, T
;		WP4	T, F, F, F	WP10	F, F, T, T
;		WP5	T, F, F, T	WP11	F, F, F, T
;		WP6	T, F, T, F	WP12	F, F, F, F
;
;		AP P/F Transitions Tested
;		AP1	P, P, P, P	AP7	F, P, F, P
;		AP2	P, P, P, F	AP8	F, P, P, F
;		AP3	P, P, F, F	AP9	F, P, P, P
;		AP4	P, F, F, F	AP10	F, F, P, P
;		AP5	P, F, F, P	AP11	F, F, F, P
;		AP6	P, F, P, F	AP12	F, F, F, F 
;
;  Requirements Tested
;   LC1003	If LC accepts any command as valid, LC shall execute the
;		command, increment the LC Valid Command Counter and issue an
;		event message
;   LC2003	Upon receipt of a message, LC shall compare the data in the
;		message to the table-defined value using the table-defined
;		comparison value and comparison operator for each data point
;               defined in the Watchpoint Definition Table (WDT) if the LC
;		Application State is one of the following:
;                 a) Active
;                 b) Passive
;   LC2003.1	LC shall support the following comparison values:
;                 a) =
;                 b) !=
;                 c) >
;                 d) >=
;                 e) <
;                 f) <=
;   LC2003.2	If the WDT comparison operator specifies that a Custom Function
;		shall be performed, LC shall apply the custon function to the
;		data contained in the message
;   LC2003.3	If the comparison result for a Watchpoint results in a False,
;		LC shall set the Number of Consecutive True values to zero
;   LC2004	For each Watchpoint, the flight software shall maintain the
;		following statistics in the dump-only Watchpoint Results Table:
;                 a) The result of the last relational comparison (False, True,
;		     Error or Stale)
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;		     False to True result
;                 d) The number of consecutive times the comparison has yielded
;		     a True result
;                 e) The cumulative number of times the comparison has yielded a
;		     True result
;                 f) Most recent FALSE to TRUE transition value
;                 g) Most recent FALSE to TRUE transition timestamp
;                 h) Most recent TRUE to FALSE transition value
;                 i) Most recent TRUE to FALSE transition timestamp
;   LC2005	Upon receipt of a table update indication, LC shall validate
;               the Watchpoint Definition Table for the following:
;                 a) Valid operator
;                 b) Data size
;                 c) Message ID
;   LC3001	Upon receipt of a Sample Request, LC shall process the request
;		specified actionpoints defined in the Actionpoint Definition
;		Table (ADT) if the LC Application State is one of the
;		following:
;                 a) Active
;                 b) Passive
;   LC3001.1	LC shall support the following Reverse Polish Operators:
;                 a) and
;                 b) or
;                 c) xor
;                 d) not
;                 e) equals
;   LC3001.2	If the equation result for an Actionpoint results in a Pass, LC
;		shall set the Number of Consecutive Fail values to zero
;   LC3002	Each table-defined Actionpoint shall be evaluated and the 
;		results stored in the dump-only Actionpoint Results Table if the
;		Actionpoint state is either:
;                 a) Active
;                 b) Passive
;   LC3002.1	If the Actionpoint equation results in a transition from PASS to
;		FAIL, LC shall issue an event message indicating the failure
;   LC3002.1.1	If the PASS to FAIL transition event message has been sent for
;		the table-defined number of times, LC shall apply the 
;		table-defined event message filter.
;   LC3002.2	If the Actionpoint equation results in a transition from FAIL to
;		PASS, LC shall issue an event message indicating that the
;		actionpoint is now within limits
;   LC3002.2.1	If the FAIL to PASS transition event message has been sent for
;		the table-defined number of times, LC shall apply the 
;		table-defined event message filter.
;   LC3002.3	If the equation has yielded a Fail result for the table-defined
;		consecutive number of times limit and the Actionpoint is
;		currently Active, LC shall:
;                 a) generate an event message
;                 b) send a command to start the table-defined RTS
;                 c) Increment the counter indicating Total count of commands
;		     sent to SC task to start an RTS
;   LC3002.3.1	Once an RTS is initiated, LC shall change the current state of
;		the associated Actionpoint to Passive.
;   LC3002.4	If the equation has yielded a Fail result for the defined
;		consecutive number of times and the Actionpoint is currently
;		Passive, LC shall
;                 a) generate an event message indicating that the Actionpoint
;		     Failed but the action was not taken
;                 b) Increment the counter indicating Number of Start RTS
;		     commands NOT sent to SC task because LC Application is
;		     PASSIVE
;   LC3003	If the Actionpoint is Disabled, LC shall skip processing that
;		actionpoint
;   LC3004	If the Actionpoint is Unused, LC shall skip processing that
;		actionpoint
;   LC3005	If the Actionpoint is Permanently Disabled, LC shall skip
;               processing that actionpoint
;   LC3006	For each Actionpoint, the flight software shall maintain the
;		following statistics in the dump-only Actionpoint Results Table:
;                 a) The result of the last Sample(Pass,Fail,Error,Stale)
;                 b) The current state (PermOff,Disabled,Active,Passive,Unused)
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LC3007    Upon receipt of a table update indication, LC shall validate
;               the Actionpoint Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG,INFO,ERROR,CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;   LC4000	Upon receipt of a Set LC Application State To Active Command, 
;		LC shall set the state of the LC Application to Active
;   LC4001	Upon receipt of a Set LC Application State to Passive Command,
;		LC shall set the LC Application State to Passive
;   LC4002	Upon receipt of a Set LC Application State to Disable Command,
;		LC shall set the LC Application State to Disabled
;   LC4003	Upon receipt of a Set Actionpoint to Active Command, LC shall
;		set the state for the command-specified Actionpoint to ACTIVE
;		such that the actionpoint is evaluated and the table-defined
;		actions are taken based on the evaluation
;   LC4004	Upon receipt of a Set All Actionpoints to Active Command, LC
;		shall set the state for all Actionpoints to ACTIVE such that
;		the actionpoints are evaluated and the table-defined actions are
;		taken based on the evaluation
;   LC4005	Upon receipt of a Set Actionpoint to Passive Command, LC shall
;		set the state for the command-specified Actionpoint to PASSIVE
;		such that the actionpoint is evaluated, however, no actions are
;		taken
;   LC4006	Upon receipt of a Set All Actionpoints to Passive Command, LC
;		shall set the state for the all Actionpoints to PASSIVE such
;		that all actionpoints are evaluated, however, no actions are
;		taken
;   LC4007	Upon receipt of a Set Actionpoint to Disabled Command, LC shall
;		set the state for the command-specified Actionpoint to DISABLED
;		such that the actionpoints are not evaluated and no actions are
;		taken
;   LC4009	Upon receipt of a Set Actionpoint to Permanent Disable, LC
;		shall mark the command-specified Actionpoint such that the
;		Actionpoint cannot be Activated
;   LC8000	LC shall generate a housekeeping message containing the
;		following:
;                 a) Valid  Command Counter
;                 b) Command Rejected Counter
;                 c) Number of Start RTS commands NOT sent to SC task because
;		     LC Application is PASSIVE
;                 d) Current LC Application State (LC_ACTIVE, LC_PASSIVE,
;		     LC_DISABLED)...
;                 e) Total count of actionpoints sampled while LC_ACTIVE or
;		     LC_PASSIVE...
;                 f) Total count of packets monitored for watchpoints (cmd and
;		     tlm)
;                 g) Total count of commands sent to SC task to start an RTS
;                 h) Selected data from watchpoint results table
;                 i) Selected data from actionpoint results table
;   LC9000	Upon cFE Power-On LC shall initialize the following
;		Housekeeping data to Zero (or value specified):
;                 a) Valid Command Counter
;                 b) Command Rejected Counter
;                 c) Passive RTS Execution Counter
;                 d) Current LC State to <PLATFORM_DEFINED> Default Power-on
;		     State
;                 e) Actionpoint Sample Count
;                 f) TLM Count
;                 g) RTS Execution Counter
;                 h) Watch Results (bitmapped)
;                 i) Action Results (bitmapped)
;   LC9001	Upon cFE Power-On LC shall initialize the following Watchpoint
;		data to Zero (or value specified) for all Watchpoints:
;                 a) The result of the last watchpoint relational comparison to
;		     STALE
;                 b) The number of times this Watchpoint has been compared
;                 c) The number of times this Watchpoint has crossed from the
;		     False to True result
;                 d) The number of consecutive times the comparison has yielded
;		     a True result
;                 e) The cumulative number of times the comparison has yielded a
;		     True result
;                 f) The value that caused the last False-to-True crossing, and
;		     the crossing time stamp
;                 g) The value that caused the last True-to-False crossing, and
;		     the crossing time stamp
;   LC9002	Upon cFE Power-On LC shall initialize the following Actionpoint
;		data to Zero (or value specified for all Actionpoints:
;                 a) The result of the last Actionpoint Sample to STALE
;                 b) The current state as defined in the ADT
;                 c) The number of times this Actionpoint has crossed from the
;		     Fail to Pass state
;                 d) The number of times this Actionpoint has crossed from the
;		     Pass to Fail state
;                 e) The number of consecutive times the equation result =Failed
;                 f) The cumulative number of times the equation result = Failed
;                 g) The cumulative count of the RTS executions
;                 h) Total number of event messages sent
;   LC9005	Upon any initialization, LC shall validate the Watchpoint
;		Definition Table for the following:
;                 a) valid operator
;                 b) data size
;                 c) Message ID
;   LC9006	Upon any initialization, LC shall validate the Actionpoint
;		Definition Table for the following:
;                 a) valid default state
;                 b) RTS number (in range)
;                 c) Event Type (DEBUG, INFO, ERROR, CRITICAL)
;                 d) Failure Count (in range)
;                 e) Action Equation syntax
;   LC9007	Upon any initialization, LC shall subscribe to the messages
;		defined in the WDT.
;
;  Prerequisite Conditions
;	The cFE is up and running and ready to accept commands. 
;	The LC commands and TLM items exist in the GSE database. 
;	A display page exists for the LC Housekeeping telemetry packet. 
;	LC Test application loaded and running
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date	        Name		Description
;	08/10/12	Walt Moleski	Original Procedure for LCX
;       05/10/17        W. Moleski      Updated to use CPU1 for commanding and
;                                       added a hostCPU variable for the utility
;                                       procs to connect to the proper host.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait        Wait for a specified telemetry point to update to
;                         a specified value. 
;       ut_sendcmd        Send commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_sendrawcmd     Send raw commands to the spacecraft. Verifies command
;                         processed and command error counters.
;       ut_pfindicate     Print the pass fail status of a particular requirement
;                         number.
;       ut_setupevents       Performs setup to verify that a particular event
;                         message was received by ASIST.
;	ut_setrequirements    A directive to set the status of the cFE
;			      requirements array.
;       ftp_file      Procedure to load file containing a table
;       lc_wdt1       Sets up the Watchpoint Definition table files for testing
;       lc_adt1       Sets up the Actionpoint Definition table files for testing
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "to_lab_events.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"
#include "tst_lc_events.h"

%liv (log_procedure) = logging

#define LC_1003       0
#define LC_2003       1
#define LC_20031      2
#define LC_20032      3
#define LC_20033      4
#define LC_2004       5
#define LC_2005       6
#define LC_3001       7
#define LC_30011      8
#define LC_30012      9
#define LC_3002      10
#define LC_30021     11
#define LC_30022     12
#define LC_30023     13
#define LC_300231    14
#define LC_30024     15
#define LC_3003      16
#define LC_3004      17
#define LC_3005      18
#define LC_3006      19
#define LC_3007      20
#define LC_4000      21
#define LC_4001      22
#define LC_4002      23
#define LC_4003      24
#define LC_4004      25
#define LC_4005      26
#define LC_4006      27
#define LC_4007      28
#define LC_4009      29
#define LC_8000      30
#define LC_9000      31      
#define LC_9001      32
#define LC_9002      33

#define WPACKED     (LC_MAX_WATCHPOINTS + 3) / 4
#define APACKED     (LC_MAX_ACTIONPOINTS + 1) / 2

#define CMDFAIL       1
#define CMDSUCCESS    2
#define AP1_EVENTID   1  
#define AP2_EVENTID   2
#define AP3_EVENTID   3
#define AP4_EVENTID   4
#define AP5_EVENTID   5
#define AP6_EVENTID   6
#define AP7_EVENTID   7
#define AP8_EVENTID   8
#define AP9_EVENTID   9
#define AP10_EVENTID  10
#define AP11_EVENTID  9 
#define AP12_EVENTID  10

global ut_req_array_size = 33
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************

local cfe_requirements[0 .. ut_req_array_size] = ["LC_1003","LC_2003", ;;
    "LC_2003.1","LC_2003.2","LC_2003.3","LC_2004","LC_2005","LC_3001", ;;
    "LC_3001.1","LC_3001.2","LC_3002","LC_3002.1","LC_3002.2", ;;
    "LC_3002.3","LC_3002.3.1","LC_3002.4","LC_3003","LC_3004", ;;
    "LC_3005","LC_3006", "LC_3007","LC_4000","LC_4001","LC_4002", ;;
    "LC_4003","LC_4004","LC_4005","LC_4006","LC_4007","LC_4009", ;;
    "LC_8000","LC_9000","LC_9001","LC_9002"] 

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream1
LOCAL index
LOCAL wpindex
LOCAL apindex
LOCAL ADTentries = 12
LOCAL WDTentries = 30
LOCAL CmdStatus 
LOCAL State
Local rdlindex
Local maxwp = LC_MAX_WATCHPOINTS - 1
Local maxap = LC_MAX_ACTIONPOINTS - 1
local LCAppName = LC_APP_NAME
local ramDir = "RAM:0"
local defaultTblDir = "CF:0/apps"
local ARTTblName = LCAppName & ".LC_ART"
local ADTTblName = LCAppName & ".LC_ADT"
local WRTTblName = LCAppName & ".LC_WRT"
local WDTTblName = LCAppName & ".LC_WDT"
local hostCPU = "$CPU"

write ";*********************************************************************"
write ";  Step 1.0:  Initialize the CPU for this test. "
write ";*********************************************************************"
write ";  Step 1.1:  Command a Power-On Reset. "
write ";********************************************************************"
/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60
                                                                                
cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Create the WDT and ADT table loads used for testing and "
write ";  upload them to the proper location. Also, display the LC pages."
write ";********************************************************************"
s $SC_$CPU_lc_wdt1

;; Parse the filename configuration parameters for the default table filenames
local wdtFileName = LC_WDT_FILENAME
local slashLoc = %locate(wdtFileName,"/")

;; loop until all slashes are found for the Destination File Table Name
while (slashLoc <> 0) do
  wdtFileName = %substring(wdtFileName,slashLoc+1,%length(wdtFileName))
  slashLoc = %locate(wdtFileName,"/")
enddo

write "==> Default LC Watchpoint Table filename = '",wdtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_wdt1.tbl", wdtFileName, hostCPU, "P")

s $SC_$CPU_lc_adt1

;; Parse the filename configuration parameters for the default table filenames
local adtFileName = LC_ADT_FILENAME
slashLoc = %locate(adtFileName,"/")

;; loop until all slashes are found for the Actionpoint Definitaion Table Name
while (slashLoc <> 0) do
  adtFileName = %substring(adtFileName,slashLoc+1,%length(adtFileName))
  slashLoc = %locate(adtFileName,"/")
enddo

write "==> Default LC Actionpoint Table filename = '",adtFileName,"'"

s ftp_file(defaultTblDir, "lc_def_adt1.tbl", adtFileName, hostCPU, "P")

;; Display the pages
page $SC_$CPU_LC_HK
page $SC_$CPU_TST_LC_HK
page $SC_$CPU_LC_ADT
page $SC_$CPU_LC_WDT
page $SC_$CPU_LC_ART
page $SC_$CPU_LC_WRT

write ";*********************************************************************"
write ";  Step 1.3:  Start the Limit Checker (LC) Application and "
write ";  add any required subscriptions.  "
write ";********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_INIT_INF_EID, "INFO", 2

s load_start_app (LCAppName,hostCPU, "LC_AppMain")

; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for LC not received."
  endif
else
  write "<!> Failed - LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'8A7'

if ("$CPU" = "CPU2") then
  stream1 = x'9A7'
elseif ("$CPU" = "CPU3") then
  stream1 = x'AA7'
endif

write "Sending command to add subscription for LC housekeeping packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

write ";**********************************************************************"
write ";  Step 1.4:  Start the Limit Checker Test Application (TST_LC) and "
write ";  add any required subscriptions.  "
write ";**********************************************************************"
ut_setupevents "$SC", "$CPU", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_INIT_INF_EID, "INFO", 2
                                                                                
s load_start_app ("TST_LC", hostCPU, "TST_LC_AppMain")
                                                                                
; Wait for app startup events
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_LC Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_LC not received."
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_LC Application start Event Message not received."
endif

;;; Need to set the stream based upon the cpu being used
;;; CPU1 is the default
stream1 = x'933'

if ("$CPU" = "CPU2") then
  stream1 = x'A33'
elseif ("$CPU" = "CPU3") then
  stream1 = x'B33'
endif
                                                                                
write "Sending command to add subscription for TST_LC HK packet."
/$SC_$CPU_TO_ADDPACKET Stream=stream1 Pkt_Size=x'0' Priority=x'0' Reliability=x'1' Buflimit=x'4'
wait 10

wait 5

write ";*********************************************************************"
write ";  Step 1.5: Verify that the LC Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Add the HK message receipt test
local hkPktId

;; Set the SC HK packet ID based upon the cpu being used
;; CPU1 is the default
hkPktId = "p0A7"

if ("$CPU" = "CPU2") then
  hkPktId = "p1A7"
elseif ("$CPU" = "CPU3") then
  hkPktId = "p2A7"
endif

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (8000) - Housekeeping packet is being generated."
  ut_setrequirements LC_8000, "P"
else
  write "<!> Failed (8000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements LC_8000, "F"
endif

;check initialization of housekeeping 
if ($SC_$CPU_LC_CMDPC = 0) AND ($SC_$CPU_LC_CMDEC = 0) AND ;;
   ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_POWER_ON_RESET) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 0) AND ($SC_$CPU_LC_MONMSGCNT = 0) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) THEN
;; 255 because the WPs are all not measured   
  for wpindex = 1 to WPACKED do
    if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 255) then
      break
    endif
  enddo
;first check the 10 APs that are being used
;;255 is because they are disabled and not measured
  for apindex = 1 to 10 do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 255) then
      break                                   
    endif
  enddo
;then check the rest of the APs
;; 51 is because the APs are not used and not measured
  for apindex = 11 to APACKED do
    if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 51) then
      break                                   
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (9000) - WP or AP Housekeeping telemetry NOT initialized properly."
    write "  CMDPC              = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC              = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE         = ", $SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT 
    write "  PASSRTSCNT         = ", $SC_$CPU_LC_PASSRTSCNT

    if (wpindex < WPACKED) then
      write "  WP Packed Results  = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
    endif
    if (apindex < APACKED) then
      write "  AP Packed Results  = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
    endif
    ut_setrequirements LC_9000, "F"
  else
     write "<*> Passed (9000) - Housekeeping telemetry initialized properly."
     ut_setrequirements LC_9000, "P"
  endif  
else
  write "<!> Failed (9000) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE        = ", $SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
  write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT

  if (wpindex < WPACKED) then
    write "  WP Packed Results = ", $SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus
  endif
  if (apindex < APACKED) then
    write "  AP Packed Results = ", $SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus
  endif
  ut_setrequirements LC_9000, "F"
endif

;check initialization of WRT
;;; CPU1 is the default
local wrtID = "0FB9"

if ("$CPU" = "CPU2") then
   wrtID = "0FD7"
elseif ("$CPU" = "CPU3") then
   wrtID = "0FF7"
endif

s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
     ($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
     ($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
       break
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (9001) - Watchpoint Results Table NOT initialized at startup."
  write " WatchResults        = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count    = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True    = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count      = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value        = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value        = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_9001, "F"
else
  write "<*> Passed (9001) - Watchpoint Results Table initialized properly."
  ut_setrequirements LC_9001, "P"  
endif

;check initialization of ART
;;; CPU1 is the default
local artID = "0FB8"

if ("$CPU" = "CPU2") then
   artID= "0FD6"
elseif ("$CPU" = "CPU3") then
   artID = "0FF6"
endif

s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (9002) - Actionpoint Results Table NOT initialized at startup."
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_9002, "F"
else
  write "<*> Passed (9002) - Actionpoint Results Table initialized properly."
  ut_setrequirements LC_9002, "P"  
endif

write ";*********************************************************************"
write ";  Step 1.6: Enable DEBUG Event Messages "
write ";*********************************************************************"
local cmdctr = $SC_$CPU_EVS_CMDPC + 1

;; Enable DEBUG events for the LC and CFE_SB application ONLY
/$SC_$CPU_EVS_EnaAppEVTType Application=LCAppName DEBUG

ut_tlmwait $SC_$CPU_EVS_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Enable Debug events command sent properly."
else
  write "<!> Failed - Enable Debug events command."
endif

write ";*********************************************************************"
write ";  Step 2.0:  Test LC Active, All AP disabled, WP cause a mix of "
write ";  thresholds to be reached"
write ";*********************************************************************"
write ";  Step 2.1:  Send a Set LC Application State to Active Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4000, "P"
else
  write "<!> Failed (1003;4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 2.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("2.2",1)

write ";*********************************************************************"
write ";  Step 2.3 Send Sample Request for all 12 APs.  Nothing should"
write ";  happen since AP are disabled"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

write ";*********************************************************************"
write ";  Step 2.4 Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE = LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 0) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ($SC_$CPU_LC_WPSINUSE = 30) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 0) THEN
   for wpindex = 1 to WPACKED do
      if (wpindex = 1) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
            break
         endif
      elseif  (wpindex = 2) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
            break
         endif
      elseif (wpindex >=3) and (wpindex <=7) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
            break
         endif
      elseif  (wpindex = 8) then
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
            break
         endif
      else
         if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
            break
         endif
      endif 
   enddo

;; for the 10 APs that are being used, should be 0xFF since they are disabled have not been measured
;; rest should be 0x33 since they are not used and not measured
   for apindex = 1 to APACKED do
     if (apindex <= 6) then
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0xff) then
          break                                   
        endif
     else
        if($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
          break                                   
        endif
     endif
   enddo
 
   if (wpindex < WPACKED) OR (apindex < APACKED) then
       write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
       write "  CMDPC              = ", $SC_$CPU_LC_CMDPC 
       write "  CMDEC              = ", $SC_$CPU_LC_CMDEC 
       write "  CURLCSTATE         = ", p@$SC_$CPU_LC_CURLCSTATE
       write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT 
       write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
       write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT 
       write "  Passive RTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT 
       write "  WP in use          = ", $SC_$CPU_LC_WPSINUSE
       write "  Active APs         = ", $SC_$CPU_LC_ACTIVEAPS

       if (wpindex < WPACKED) then
         write "  WP Packed index    = ", wpindex
         write "  WP Packed Results  = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
       endif
       if (apindex < APACKED) then
         write "  AP Packed index    = ", apindex
         write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
       endif
       ut_setrequirements LC_8000, "F"
  else
       write "<*> Passed (8000) - Housekeeping telemetry updated properly."
       ut_setrequirements LC_8000, "P"
endif  

else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC                       =", $SC_$CPU_LC_CMDPC 
  write "  CMDEC                       =", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE                  =", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT                 =", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT                   =", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT                      =", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT              =", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use                   =", $SC_$CPU_LC_WPSINUSE
  write "  Active APs                  =", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_8000, "F"
endif


write ";*********************************************************************"
write ";  Step 2.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <>0x19) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x54) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x230) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index > 5) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;; 
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 2.6 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 2.7 Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 3.0:  Test LC Active, All AP passive, WP cause a mix of "
write ";             thresholds to be reached, AP cause a mix of results"
write ";*********************************************************************"
write ";  Step 3.1:  Send the Set State Commands"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4001) - Set LC Application State to Passive command properly sent."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4001, "P"
else
  write "<!> Failed (1003;4001) - Set LC Application State to Passive command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4001, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4001) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4001, "P"
else
  write "<!> Failed (1003;4001) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4001, "F"
endif

;; Set all APs to Passive
State = LC_APSTATE_PASSIVE
ut_sendcmd "$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State"

if (ut_sc_status = UT_SC_Success) then
  write "<*> Passed (1003;4006) - Set ALL AP State to Passive command properly sent."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4006, "P"
else
  write "<!> Failed (1003;4006) - Set ALL AP State to Passive command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4006, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 3.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("3.2",1)

write ";*********************************************************************"
write ";  Step 3.3 Send Sample Request for all 12 APs. Also AP 9 should cause "
write ";  event message to be generated about the RTS not being sent due to" 
write ";  AP being passive"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=8 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSIVE_FAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
     write "<*> Passed (3002.4) - Event message sent Passive AP Failed Max times but no RTS sent."
     ut_setrequirements LC_30024, "P"
else
     write "<*> Failed (3002.4) - Event message not sent Passive AP Failed Max times but no RTS sent."
     ut_setrequirements LC_30024, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 3.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_APSAMPLECNT = 12) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
        break
      endif
    elseif (wpindex >=3) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif 
  enddo

  ;; for the 10 APs that are being used, 
  ;; should be 0xFF since they are disabled have not been measured
  ;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x88) then
        break                                   
      endif
    elseif (apindex > 3) and (apindex <=6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x99) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (3002.4;8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_30024, "P"
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (3002.4;8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC           = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC           = ", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE      = ", $SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT     = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT       = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT          = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT  = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use       = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs      = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_30024, "F"
  ut_setrequirements LC_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 3.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index >= 0) and (index < 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index >= 6) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004;3001) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 3.6 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 
for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index >= 6) and (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 3.7 Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 4.0:  Test LC Active, AP a mix of states, WP cause a mix of "
write ";             thresholds to be reached, AP cause a mix of results"
write ";*********************************************************************"
write ";  Step 4.1:  Send a Set All AP to Command"
write ";*********************************************************************"
State = LC_STATE_ACTIVE
/$SC_$CPU_LC_SETLCSTATE NewLCState=State
wait 5

State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=4 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=5 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=6  NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=9 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=10 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=11 NewAPState=State
wait 2

State = LC_APSTATE_DISABLED
/$SC_$CPU_LC_SETAPSTATE APNumber=1 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=2 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=7 NewAPState=State
wait 2

State = LC_APSTATE_PASSIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=3 NewAPState=State
wait 2
/$SC_$CPU_LC_SETAPSTATE APNumber=8 NewAPState=State
wait 2

write ";*********************************************************************"
write ";  Step 4.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("4.2",1)

write ";*********************************************************************"
write ";  Step 4.3 Send Sample Request for all 12 APs. Also AP 9 should cause "
write ";  event message to be generated about an RTS being sent" 
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=8 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

local expectedEID = LC_BASE_AP_EID + 10

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
     write "<*> Passed (3002.3.1) - RTS Executed Event message sent."
     ut_setrequirements LC_300231, "P"
else
     write "<*> Failed (3002.3.1) -  RTS Executed Event message NOT sent."
     ut_setrequirements LC_300231, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 4.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_APSAMPLECNT = 9) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) AND ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 6) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
        break
      endif
    elseif (wpindex >=3) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif 
  enddo

;; for the 10 APs that are being used, should be 0xFF since they are disabled have not been measured
;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex = 1) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0xC4) then
        break                                   
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x8C) then
        break                                   
      endif
    elseif (apindex = 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0xD5) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x99) then
        break                                   
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (3002.4;8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_30024, "P"
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (3002.4;8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          =", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          =", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE     =", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    =", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      =", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         =", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT =", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      =", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     =", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_30024, "F"
  ut_setrequirements LC_8000, "F"
endif


write ";*********************************************************************"
write ";  Step 4.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index >= 0) and (index < 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <>0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index >= 6) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 4.6 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 
for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 1) or (index = 2) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif  (index = 3) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 4) or (index = 5) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif  (index = 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif  (index = 7) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_DISABLED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif  (index = 8) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif  (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      break
    endif
  elseif (index = 10) or (index = 11) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo
if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP  = ", index
  write "  Current State           = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count      = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count      = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count  = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count          = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count           = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 4.7 Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 5.0:  Test LC Passive, AP all active, WP cause a mix of "
write ";             thresholds to be reached, AP cause a mix of results"
write ";*********************************************************************"
write ";  Step 5.1:  Send a Set All AP to Command"
write ";*********************************************************************"
State = LC_STATE_PASSIVE
/$SC_$CPU_LC_SETLCSTATE NewLCState=State
wait 5

local cmdctr = $SC_$CPU_LC_CMDPC + 1
State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State

ut_tlmwait $SC_$CPU_LC_CMDPC, {cmdctr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;4004) - Set ALL AP State to Active command sent properly."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4004, "P"
else
  write "<!> Failed (1003;4004) - Set ALL AP State to Active command."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4004, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 5.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("5.2",1)

write ";*********************************************************************"
write ";  Step 5.3 Send Sample Request for all 12 APs. Also AP 9 should cause "
write ";  event message to be generated about the RTS not being sent due to" 
write ";  LC being passive"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=8 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", {LCAppName}, TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_PASSIVE_FAIL_DBG_EID, "DEBUG", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
     write "<*> Passed (3002.4) - Event message sent Passive AP Failed Max times but no RTS sent."
     ut_setrequirements LC_30024, "P"
else
     write "<*> Failed (3002.4) - Event message not sent Passive AP Failed Max times but no  RTS sent."
     ut_setrequirements LC_30024, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5


write ";*********************************************************************"
write ";  Step 5.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_APSAMPLECNT = 12) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ($SC_$CPU_LC_PASSRTSCNT = 1) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 11) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_PASSIVE) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
        break
      endif
    elseif (wpindex >=3) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif 
  enddo

;; for the 10 APs that are being used, should be 0xFF since they are disabled have not been measured
;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif ((apindex > 3) and (apindex <=4)) or (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo

  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (3002.4;8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_30024, "P"
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (3002.4;8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE     = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_30024, "F"
  ut_setrequirements LC_8000, "F"
endif


write ";*********************************************************************"
write ";  Step 5.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index >= 0) and (index < 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <>0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index >= 6) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo


if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2003.4;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 5.6 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) or (index >= 10) and (index < 12) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 5.7 Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 6.0:  Test LC Disabled, AP all active, WP would cause a mix of "
write ";             thresholds to be reached, AP would cause a mix of results"
write ";*********************************************************************"
write ";  Step 6.1:  Send a Set All AP to Command"
write ";*********************************************************************"
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_DISABLED
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_DISABLED) then
  write "<*> Passed (1003;4002) - Set LC Application State to Disabled command properly sent."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4002, "P"
else
  write "<!> Failed (1003;4002) - Set LC Application State to Disabled command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4002, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4002) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4002, "P"
else
  write "<!> Failed (1003;4002) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4002, "F"
endif


State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State
 wait 5

write ";*********************************************************************"
write ";  Step 6.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("6.2",1)

write ";*********************************************************************"
write ";  Step 6.3 Send Sample Request for all 12 APs.  Nothing should happen"
write ";  since LC is disabled"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 6.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_APSAMPLECNT = 0) AND ($SC_$CPU_LC_MONMSGCNT = 0) AND ;;
   ($SC_$CPU_LC_RTSCNT = 0) AND ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 12) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_DISABLED) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
        break
      endif
    elseif (wpindex >=3) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif 
  enddo

  ;; for the 10 APs that are being used, 
  ;; should be 0xFF since they are disabled have not been measured
  ;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif (apindex > 3) and (apindex <= 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (3002.4;8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_30024, "P"
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (3002.4;8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE     = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_30024, "F"
  ut_setrequirements LC_8000, "F"
endif


write ";*********************************************************************"
write ";  Step 6.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index >= 0) and (index < 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <>0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index >= 6) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.2;2003.3;2003.4;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 6.6 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index >= 6) and (index < ADTentries) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 6.7 Reset all statistics"
write ";*********************************************************************"

/$SC_$CPU_LC_RESETCTRS
wait 5
/$SC_$CPU_LC_RESETWPSTATS WPNumber=0xFFFF
wait 5
/$SC_$CPU_LC_RESETAPSTATS APNumber=0xFFFF
wait 5

write ";*********************************************************************"
write ";  Step 7.0:  Test LC Active, All AP Active, 4 data run to test all "
write ";  transitions. WP cause a mix of thresholds to be reached, AP cause a"
write ";  mix of results"
write ";*********************************************************************"
write ";  Step 7.1:  Reload tables and send the Set LC Application State to "
write ";  Active Command"
write ";*********************************************************************"

start load_table("lc_def_wdt1.tbl", hostCPU)
wait 20

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_WDTVAL_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=WDTTblName"
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (2005) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_2005, "P"
else
  write "<!> Failed (2005) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_WDTVAL_INF_EID, "."
  ut_setrequirements LC_2005, "F"
endif
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=WDTTblName"
wait 10

start load_table("lc_def_adt1.tbl", hostCPU)
wait 20

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_ADTVAL_INF_EID, "INFO", 1
ut_sendcmd "$SC_$CPU_TBL_VALIDATE INACTIVE VTABLENAME=ADTTblName"
wait 20

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (3007) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_3007, "P"
else
  write "<!> Failed (3007) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_ADTVAL_INF_EID, "."
  ut_setrequirements LC_3007, "F"
endif
ut_sendcmd "$SC_$CPU_TBL_ACTIVATE ATABLENAME=ADTTblName"
wait 10

ut_setupevents "$SC", "$CPU", {LCAppName}, LC_LCSTATE_INF_EID, "INFO", 1
State = LC_STATE_ACTIVE
ut_sendcmd "$SC_$CPU_LC_SETLCSTATE NewLCState=State"
 
if (ut_sc_status = UT_SC_Success) and ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) then
  write "<*> Passed (1003;4000) - Set LC Application State to Active command properly sent."
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4000, "P"
else
  write "<!> Failed (1003;4000) - Set LC Application State to Active command not sent properly (", ut_sc_status, ")."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4000, "F"
endif

if ($SC_$CPU_find_event[1].num_found_messages = 1) THEN
  write "<*> Passed (1003;4000) - Event message ",$SC_$CPU_find_event[1].eventid, " received"
  ut_setrequirements LC_1003, "P"
  ut_setrequirements LC_4000, "P"
else
  write "<!> Failed (1003;4000) - Event message ",$SC_$CPU_evs_eventid," received. Expected Event message ",LC_LCSTATE_INF_EID, "."
  ut_setrequirements LC_1003, "F"
  ut_setrequirements LC_4000, "F"
endif

State = LC_APSTATE_ACTIVE
/$SC_$CPU_LC_SETAPSTATE APNumber=0xFFFF NewAPState=State
 
wait 5

write ";*********************************************************************"
write ";  Step 7.2 Send packets for all WP defined in WDT, data run 1"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("7.2",1)

write ";*********************************************************************"
write ";  Step 7.3 Send Sample Request for all 12 APs."
write ";  Will get a mix of results"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=8 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;; expectedEID is defined in Step 4.3
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.3) - Received RTS event message"
    ut_setrequirements LC_30023, "P"
  else
    write "<!> Failed (3002.3) - Did not receive RTS event message."
    ut_setrequirements LC_30023, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.3) - Did not receive RTS event message."
  ut_setrequirements LC_30023, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 7.4 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_PASSRTSCNT = 0) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) AND ;;
   ($SC_$CPU_LC_APSAMPLECNT = 12) AND ($SC_$CPU_LC_MONMSGCNT = 20) AND ;;
   ($SC_$CPU_LC_RTSCNT = 1) AND ($SC_$CPU_LC_WPSINUSE = 30) AND ;;
   ($SC_$CPU_LC_ACTIVEAPS = 11) THEN
  for wpindex = 1 to WPACKED do
    if (wpindex = 1) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x55) then
        break
      endif
    elseif  (wpindex = 2) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0x5) then
        break
      endif
    elseif (wpindex >=3) and (wpindex <=7) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
        break
      endif
    elseif  (wpindex = 8) then
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
        break
      endif
    else
      if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
        break
      endif
    endif 
  enddo

  ;; for the 10 APs that are being used, 
  ;; should be 0xFF since they are disabled have not been measured
  ;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex <= 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif (apindex = 4) or (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break                                   
      endif
    elseif (apindex = 5) then
       if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
         break                                   
       endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  CURLCSTATE     = ", $SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.5 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <>0x19) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x54) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x230) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index > 5) and (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 7.6 Dump ART and check counters"   
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 6) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif ((index >= 6) and (index < 9)) or ;;
	 ((index >= 10) and (index < ADTentries)) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
      write "<*> Failed (3002.3;3002.3.1) -  ART shows APs fail max times not handled properly."
      ut_setrequirements LC_30023, "F"
      ut_setrequirements LC_300231, "F"
      break
    else 
      write "<*> Passed (3002.3;3002.3.1) -  ART shows APs fail max times handled properly."
      ut_setrequirements LC_30023, "P"
      ut_setrequirements LC_300231, "P"
    endif
  else
    if  ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP  = ", index
  write "  Action Results          = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State           = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count      = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count      = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count  = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count          = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count           = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 7.7 Send data run 2"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("7.7",2)

write ";*********************************************************************"
write ";  Step 7.8 Send Sample Request for all 12 APs.  Mix of results"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=2 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=3 EndAP=3 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=4 EndAP=4 UpdateAge=0
ut_tlmwait $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=5 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=6 EndAP=6 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive  Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=7 EndAP=7 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive  Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=8 EndAP=8 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive  Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive  Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=10 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;; expectedEID is defined in Step 4.3
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "INFO", 2

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=11 EndAP=11 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.3) - Received RTS event message"
    ut_setrequirements LC_30023, "P"
  else
    write "<!> Failed (3002.3) - Did not receive RTS event message."
    ut_setrequirements LC_30023, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.3) - Did not receive RTS event message."
  ut_setrequirements LC_30023, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 7.9 Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_APSAMPLECNT = 24) AND ($SC_$CPU_LC_MONMSGCNT = 40) AND ;;
   ($SC_$CPU_LC_RTSCNT = 2) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 10) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) THEN
  CmdStatus = CMDSUCCESS
  if ($SC_$CPU_LC_WRRESULTS[1].WPByteEntry.AllStatus <> 0x15) then
    CmdStatus = CMDFAIL
    wpindex = 1
  endif
  if (CmdStatus = CMDSUCCESS) then
    if ($SC_$CPU_LC_WRRESULTS[2].WPByteEntry.AllStatus <> 0x50) then
      CmdStatus = CMDFAIL
      wpindex = 2
    endif
  endif
  if (CmdStatus = CMDSUCCESS) then
    if ($SC_$CPU_LC_WRRESULTS[3].WPByteEntry.AllStatus <> 0x01) then
      CmdStatus = CMDFAIL
      wpindex = 3
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    for wpindex = 4 to WPACKED do
      if (wpindex <=7) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
          break
        endif
      elseif  (wpindex = 8) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
          break
        endif
      else
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
          break
        endif
      endif 
    enddo
  endif

  ;; for the 10 APs that are being used, 
  ;; should be 0x44 since they are active and pass
  ;; rest should be 0x33 since they are not used and not measured
  for apindex = 1 to APACKED do
    if (apindex =1) or (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x54) then  
        break                                   
      endif
    elseif (apindex =3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x55) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x94) then
        break                                   
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT        = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS
    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE     = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.10 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x19) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0054) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x45) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xff54) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0230) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x130) then
      break
    endif 
  elseif (index = 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0012456f) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0012546f) then
      break
    endif
  elseif (index = 7) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x23451200) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x23451300) then
      break
    endif
  elseif (index = 8) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x542) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x546) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xf0ab1543) then
      break
    endif
  elseif (index = 10) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x3f9ddcc6) then
      break
    endif
  elseif (index > 10) and  (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 7.11 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 3) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
        ($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
        ($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index >= 3)  and (index < 6)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index >= 6)  and (index < 9)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
        endif
  elseif (index = 9) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif
  elseif (index = 10) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 11) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          write "<*> Failed (3002.3;3002.3.1) -  ART shows APs fail max times not handled properly."
          ut_setrequirements LC_30023, "F"
          ut_setrequirements LC_300231, "F"
     else 
          write "<*> Passed (3002.3;3002.3.1) -  ART shows APs fail max times handled properly."
          ut_setrequirements LC_30023, "P"
          ut_setrequirements LC_300231, "P"
     endif
  else
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
       break
     endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP  = ", index
  write "  Action Results          = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State           = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count      = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count      = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count  = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count          = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count           = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif

write ";*********************************************************************"
write ";  Step 7.12 Send packets for all WP defined in WDT data run 3"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("7.12",3)

write ";*********************************************************************"
write ";  Step 7.13 Send Sample Request for all 12 APs.  Mixed Results"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=1 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=2 EndAP=2 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

expectedEID = LC_BASE_AP_EID + 4
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "CRITICAL", 2

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=3 EndAP=3 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.3) - Received RTS event message"
    ut_setrequirements LC_30023, "P"
  else
    write "<!> Failed (3002.3) - Did not receive RTS event message."
    ut_setrequirements LC_30023, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.3) - Did not receive RTS event message."
  ut_setrequirements LC_30023, "F"
endif


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=4 EndAP=4 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=5 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive  Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=6 EndAP=6 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=7 EndAP=8 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=9 EndAP=9 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif

expectedEID = LC_BASE_AP_EID + 9
ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, {expectedEID}, "ERROR", 2

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=10 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.3) - Received RTS event message"
    ut_setrequirements LC_30023, "P"
  else
    write "<!> Failed (3002.3) - Did not receive RTS event message."
    ut_setrequirements LC_30023, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.3) - Did not receive RTS event message."
  ut_setrequirements LC_30023, "F"
endif

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=11 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 7.14 Check housekeeping counters"
write ";*********************************************************************"
if ($SC_$CPU_LC_APSAMPLECNT = 36) AND ($SC_$CPU_LC_MONMSGCNT = 60) AND ;;
   ($SC_$CPU_LC_RTSCNT = 4) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 8) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) THEN
  CmdStatus = CMDSUCCESS

  if ($SC_$CPU_LC_WRRESULTS[1].WPByteEntry.AllStatus <> 0x05) then
    CmdStatus = CMDFAIL
    wpindex = 1
  endif

  if (CmdStatus = CMDSUCCESS) then

    if ($SC_$CPU_LC_WRRESULTS[2].WPByteEntry.AllStatus <> 0x44) then
      CmdStatus = CMDFAIL
      wpindex = 2
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    if ($SC_$CPU_LC_WRRESULTS[3].WPByteEntry.AllStatus <> 0x05) then
      CmdStatus = CMDFAIL
      wpindex = 3
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    for wpindex = 4 to WPACKED do
      if (wpindex <=7) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
          break
        endif
      elseif  (wpindex = 8) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
          break
        endif
      else
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
          break
        endif
      endif 
    enddo
  endif

  for apindex = 1 to APACKED do
    if (apindex = 1) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x44) then
        break                                   
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
        break                                   
      endif
    elseif (apindex = 3) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x45) then
        break                                   
      endif
    elseif (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x45) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x84) then
        break
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x99) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC              = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC              = ", $SC_$CPU_LC_CMDEC 
    write "  PASSRTSCNT         = ", $SC_$CPU_LC_PASSRTSCNT
    write "  CURLCSTATE         = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT        = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT          = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT             = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use          = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs         = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index    = ", wpindex
      write "  WP Packed Results  = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif
    if (apindex < APACKED) then
      write "  AP Packed index    = ", apindex
      write "  AP Packed Results  = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE     = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS
  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_8000, "F"
endif

write ";*********************************************************************"
write ";  Step 7.15 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x19) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x1345) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0054) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0045) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xff54) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0133) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0130) then
      break
    endif 
  elseif (index = 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0012456f) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0012546f) then
      break
    endif
  elseif (index = 7) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x23451200) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x23451300) then
      break
    endif
  elseif (index = 8) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x00000542) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x546) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x00ab1543) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xf0ab1543) then
      break
    endif
  elseif (index = 10) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x3f9ddcc6) then
      break
    endif
  elseif (index > 10) and  (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 7.16 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
  if (index < 2) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 2)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          write "<*> Failed (3002.3;3002.3.1) -  ART shows APs fail max times not handled properly."
          ut_setrequirements LC_30023, "F"
          ut_setrequirements LC_300231, "F"
          break
     else 
          write "<*> Passed (3002.3;3002.3.1) -  ART shows APs fail max times handled properly."
          ut_setrequirements LC_30023, "P"
          ut_setrequirements LC_300231, "P"
     endif
  elseif (index= 3)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif  
  elseif (index = 4)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 5)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 6)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 7)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 8)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  elseif (index = 9) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif
  elseif (index = 10) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          write "<*> Failed (3002.3;3002.3.1) -  ART shows APs fail max times not handled properly."
          ut_setrequirements LC_30023, "F"
          ut_setrequirements LC_300231, "F"
          break
     else 
          write "<*> Passed (3002.3;3002.3.1) -  ART shows APs fail max times handled properly."
          ut_setrequirements LC_30023, "P"
          ut_setrequirements LC_300231, "P"
     endif
  elseif (index = 11) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif
  else
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
  endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3006) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3006, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3006) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3006, "P"
endif


write ";*********************************************************************"
write ";  Step 7.17 Send packets for all WP defined in WDT, data run 4"
write ";*********************************************************************"

s $sc_$cpu_lc_sendpackets("7.17",4)

write ";*********************************************************************"
write ";  Step 7.18 Send Sample Request for all 12 APs.  Mixed results"
write ";*********************************************************************"

/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=0 EndAP=0 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=1 EndAP=1 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=2 EndAP=3 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=4 EndAP=4 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif


ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=5 EndAP=5 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=6 EndAP=6 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_PASSTOFAIL_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=7 EndAP=7 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.1) - Received Pass to Fail transition event"
    ut_setrequirements LC_30021, "P"
  else
    write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
    ut_setrequirements LC_30021, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.1) - Did not receive Pass to Fail transition event."
  ut_setrequirements LC_30021, "F"
endif


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=8 EndAP=9 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

ut_setupevents "$SC", "$CPU", "TST_LC", TST_LC_SEND_SAMPLE_INF_EID, "INFO", 1
ut_setupevents "$SC", "$CPU", {LCAppName}, LC_AP_FAILTOPASS_INF_EID, "INFO", 2
/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=10 EndAP=10 UpdateAge=0
ut_tlmwait  $SC_$CPU_find_event[2].num_found_messages, 1
if (UT_TW_Status = UT_Success) THEN
  if ($SC_$CPU_find_event[1].num_found_messages = 1) then
    write "<*> Passed (3002.2) - Received Fail to Pass transition event"
    ut_setrequirements LC_30022, "P"
  else
    write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
    ut_setrequirements LC_30022, "F"
    write "Event Message count = ",$SC_$CPU_find_event[1].num_found_messages
  endif
else
  write "<!> Failed (3002.2) - Did not receive Fail to Pass transition event."
  ut_setrequirements LC_30022, "F"
endif


/$SC_$CPU_TST_LC_SENDSAMPLE StartAP=11 EndAP=11 UpdateAge=0
ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

wait 5

write ";*********************************************************************"
write ";  Step 7.19 Check housekeeping counters"
write ";*********************************************************************"

if ($SC_$CPU_LC_APSAMPLECNT = 48) AND ($SC_$CPU_LC_MONMSGCNT = 80) AND ;;
   ($SC_$CPU_LC_RTSCNT = 4) AND ;;
   ($SC_$CPU_LC_WPSINUSE = 30) AND ($SC_$CPU_LC_ACTIVEAPS = 8) AND ;;
   ($SC_$CPU_LC_CURLCSTATE=LC_STATE_ACTIVE) THEN
  CmdStatus = CMDSUCCESS

  if ($SC_$CPU_LC_WRRESULTS[1].WPByteEntry.AllStatus <> 0x01) then
    CmdStatus = CMDFAIL
    wpindex = 1
  endif

  if (CmdStatus = CMDSUCCESS) then
    if ($SC_$CPU_LC_WRRESULTS[2].WPByteEntry.AllStatus <> 0x11) then
      CmdStatus = CMDFAIL
      wpindex = 2
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    if ($SC_$CPU_LC_WRRESULTS[3].WPByteEntry.AllStatus <> 0x15) then
      CmdStatus = CMDFAIL
      wpindex = 3
    endif
  endif

  if (CmdStatus = CMDSUCCESS) then
    for wpindex = 4 to WPACKED do
      if (wpindex <=7) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0) then
          break
        endif
      elseif  (wpindex = 8) then
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xf0) then
          break
        endif
      else
        if ($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus <> 0xff) then
          break
        endif
      endif 
    enddo
  endif

  for apindex = 1 to APACKED do
    if (apindex = 1) or (apindex = 3) or (apindex = 4) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x54) then
        break                                   
      endif
    elseif (apindex = 2) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x95) then
        break                                   
      endif
    elseif (apindex = 5) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x84) then
        break                                   
      endif
    elseif (apindex = 6) then
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x98) then
        break                                   
      endif
    else
      if ($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus <> 0x33) then
        break                                   
      endif
    endif
  enddo
 
  if (wpindex < WPACKED) OR (apindex < APACKED) then
    write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
    write "  CMDPC             = ", $SC_$CPU_LC_CMDPC 
    write "  CMDEC             = ", $SC_$CPU_LC_CMDEC 
    write "  CURLCSTATE        = ", p@$SC_$CPU_LC_CURLCSTATE
    write "  APSAMPLECNT       = ", $SC_$CPU_LC_APSAMPLECNT 
    write "  MONMSGCNT         = ", $SC_$CPU_LC_MONMSGCNT
    write "  RTSCNT            = ", $SC_$CPU_LC_RTSCNT 
    write "  Passive RTSCNT    = ", $SC_$CPU_LC_PASSRTSCNT 
    write "  WP in use         = ", $SC_$CPU_LC_WPSINUSE
    write "  Active APs        = ", $SC_$CPU_LC_ACTIVEAPS

    if (wpindex < WPACKED) then
      write "  WP Packed index   = ", wpindex
      write "  WP Packed Results = ", %hex($SC_$CPU_LC_WRRESULTS[wpindex].WPByteEntry.AllStatus,2)
    endif

    if (apindex < APACKED) then
      write "  AP Packed index   = ", apindex
      write "  AP Packed Results = ", %hex($SC_$CPU_LC_ARRESULTS[apindex].APByteEntry.AllStatus,2)
    endif
    ut_setrequirements LC_8000, "F"
  else
    write "<*> Passed (8000) - Housekeeping telemetry updated properly."
    ut_setrequirements LC_8000, "P"
  endif  
else
  write "<!> Failed (8000) - Housekeeping telemetry NOT correct after processing WPs."
  write "  CMDPC          = ", $SC_$CPU_LC_CMDPC 
  write "  CMDEC          = ", $SC_$CPU_LC_CMDEC 
  write "  PASSRTSCNT     = ", $SC_$CPU_LC_PASSRTSCNT
  write "  CURLCSTATE     = ", p@$SC_$CPU_LC_CURLCSTATE
  write "  APSAMPLECNT    = ", $SC_$CPU_LC_APSAMPLECNT 
  write "  MONMSGCNT      = ", $SC_$CPU_LC_MONMSGCNT
  write "  RTSCNT         = ", $SC_$CPU_LC_RTSCNT 
  write "  Passive RTSCNT = ", $SC_$CPU_LC_PASSRTSCNT 
  write "  WP in use      = ", $SC_$CPU_LC_WPSINUSE
  write "  Active APs     = ", $SC_$CPU_LC_ACTIVEAPS

  write "  Failed due to housekeeping counters so Packed Results were not checked"
  ut_setrequirements LC_8000, "F"
endif


write ";*********************************************************************"
write ";  Step 7.20 Dump WRT and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, WRTTblName, "A", "$cpu_dumpwrt", hostCPU, wrtID)
wait 5

for index = 0 to LC_MAX_WATCHPOINTS-1 do
  if (index = 0) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x19) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  elseif (index = 1) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x45) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x50) then
      break
    endif
  elseif (index = 2) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x1346) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x1345) then
      break
    endif
  elseif (index = 3) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0054) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0045) then
      break
    endif
  elseif (index = 4) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0xff60) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xff54) then
      break
    endif
  elseif (index = 5) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0133) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0130) then
      break
    endif 
  elseif (index = 6) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x0012456f) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x0012546f) then
      break
    endif
  elseif (index = 7) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x23451200) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x23451300) then
      break
    endif
  elseif (index = 8) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 3) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x00000542) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x546) then
      break
    endif
  elseif (index = 9) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 2) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0x00ab1543) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0xf0ab1543) then
      break
    endif
  elseif (index = 10) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_TRUE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 1) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> $SC_$CPU_LC_WDT[index].ComparisonValue.UnSigned32) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0x3f9ddcc6) then
      break
    endif
  elseif (index > 10) and  (index < WDTentries) then
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_FALSE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 4) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) then
;;	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  else
    if  ($SC_$CPU_LC_WRT[index].WatchResults <> LC_WATCH_STALE) or ;;
	($SC_$CPU_LC_WRT[index].EvaluationCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FalsetoTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].ConsectiveTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].CumulativeTrueCount <> 0) or ;;
	($SC_$CPU_LC_WRT[index].FtoTValue <> 0) or ;;
	($SC_$CPU_LC_WRT[index].TtoFValue <> 0) then
      break
    endif
  endif
enddo

if (index < LC_MAX_WATCHPOINTS-1) then
  write "<!> Failed (2003;2003.1;2003.2;2003.3;2004) - WRT show WPs not evaluated properly."
  write "<!> Error occurred at WP = ", index
  write " WatchResults            = ", p@$SC_$CPU_LC_WRT[index].WatchResults
  write " Evaluation Count        = ", $SC_$CPU_LC_WRT[index].EvaluationCount
  write " False to True Count     = ", $SC_$CPU_LC_WRT[index].FalsetoTrueCount
  write " Consecutive True        = ", $SC_$CPU_LC_WRT[index].ConsectiveTrueCount
  write " Cum True Count          = ", $SC_$CPU_LC_WRT[index].CumulativeTrueCount
  write " F to T Value            = ", $SC_$CPU_LC_WRT[index].FtoTValue
  write " T to F Value            = ", $SC_$CPU_LC_WRT[index].TtoFValue
  ut_setrequirements LC_2003, "F"
  ut_setrequirements LC_20031, "F"
  ut_setrequirements LC_20032, "F"
  ut_setrequirements LC_20033, "F"
  ut_setrequirements LC_2004, "F"
else
  write "<*> Passed (2003;2003.1;2003.2;2003.3;2004) -  WRT shows WPs evaluated properly."
  ut_setrequirements LC_2003, "P"
  ut_setrequirements LC_20031, "P"
  ut_setrequirements LC_20032, "P"
  ut_setrequirements LC_20033, "P"
  ut_setrequirements LC_2004, "P"
endif

write ";*********************************************************************"
write ";  Step 7.21 Dump ART and check counters"
write ";*********************************************************************"
s get_tbl_to_cvt(ramDir, ARTTblName, "A", "$cpu_dumpart", hostCPU, artID)
wait 5 

for index = 0 to LC_MAX_ACTIONPOINTS-1 do
   if (index = 0) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 1) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 2)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 3)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;; 
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif  
   elseif (index = 4)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 5)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 6)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   elseif (index = 7)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
       break
     endif
   elseif (index = 8)  then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_ACTIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
       break
     endif
   elseif (index = 9) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 2) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
       break
     endif
   elseif (index = 10) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_PASS) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 1) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 3) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
       break
     endif
   elseif (index = 11) then
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_FAIL) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_APSTATE_PASSIVE) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 4) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 4) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 1) then
          break
     endif
   else
     if ($SC_$CPU_LC_ART[index].ActionResult <> LC_ACTION_STALE) or ;;
	($SC_$CPU_LC_ART[index].CurrentState <> LC_ACTION_NOT_USED) or ;;
	($SC_$CPU_LC_ART[index].FailToPassCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].PassToFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].ConsecutiveFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeFailCount <> 0) or ;;
	($SC_$CPU_LC_ART[index].CumulativeRTSExecCount <> 0) then
          break
     endif
   endif
enddo

if (index < LC_MAX_ACTIONPOINTS-1) then
  write "<!> Failed (3001;3001.1;3001.2;3002;3003;3004;3005;3006;4003;4005;4007;4009) - ART show APs not evaluated properly."
  write "<!> Error occurred at AP = ", index
  write "  Action Results         = ", p@$SC_$CPU_LC_ART[index].ActionResult
  write "  Current State          = ", p@$SC_$CPU_LC_ART[index].CurrentState
  write "  Fail to Pass Count     = ", $SC_$CPU_LC_ART[index].FailToPassCount
  write "  Pass to Fail Count     = ", $SC_$CPU_LC_ART[index].PassToFailCount
  write "  Consecutive Fail Count = ", $SC_$CPU_LC_ART[index].ConsecutiveFailCount
  write "  Cum Fail Count         = ", $SC_$CPU_LC_ART[index].CumulativeFailCount
  write "  Cum RTS Count          = ", $SC_$CPU_LC_ART[index].CumulativeRTSExecCount
  ut_setrequirements LC_3001, "F"
  ut_setrequirements LC_30011, "F"
  ut_setrequirements LC_30012, "F"
  ut_setrequirements LC_3002, "F"
  ut_setrequirements LC_3003, "F"
  ut_setrequirements LC_3004, "F"
  ut_setrequirements LC_3005, "F"
  ut_setrequirements LC_3006, "F"
  ut_setrequirements LC_4003, "F"
  ut_setrequirements LC_4005, "F"
  ut_setrequirements LC_4007, "F"
  ut_setrequirements LC_4009, "F"
else
  write "<*> Passed (3001;3001.1;3001.2;3002;3003;3004;3005;3006;4003;4005;4007;4009) -  ART shows APs evaluated properly."
  ut_setrequirements LC_3001, "P"
  ut_setrequirements LC_30011, "P"
  ut_setrequirements LC_30012, "P"
  ut_setrequirements LC_3002, "P"
  ut_setrequirements LC_3003, "P"
  ut_setrequirements LC_3004, "P"
  ut_setrequirements LC_3005, "P"
  ut_setrequirements LC_3006, "P"
  ut_setrequirements LC_4003, "P"
  ut_setrequirements LC_4005, "P"
  ut_setrequirements LC_4007, "P"
  ut_setrequirements LC_4009, "P"
endif

write ";*********************************************************************"
write ";  Step 8.0:  Clean-up"
write ";*********************************************************************"

/$SC_$CPU_ES_POWERONRESET
wait 10

close_data_center
wait 60
                                                                                
cfe_startup {hostCPU}
wait 5

write "**** Requirements Status Reporting"
                                                                                
write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"
                                                                                
FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO
                                                                                
drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure $SC_$CPU_lc_withaction                                "
write ";*********************************************************************"
ENDPROC
