#ifndef _LC_APP_TEST_UTILS_H_
#define _LC_APP_TEST_UTILS_H_

#include "lc_app.h"
#include "utstubs.h"

/*
 * Allow UT access to the global "LC_AppData" object.
 */
extern LC_AppData_t  LC_AppData;
extern LC_OperData_t LC_OperData;

/* Global table variables for table pointers contained in LC_OperData */
LC_WDTEntry_t WDTable[LC_MAX_WATCHPOINTS];
LC_ADTEntry_t ADTable[LC_MAX_ACTIONPOINTS];
LC_WRTEntry_t WRTable[LC_MAX_WATCHPOINTS];
LC_ARTEntry_t ARTable[LC_MAX_ACTIONPOINTS];

LC_MessageList_t HashTable[LC_HASH_TABLE_ENTRIES];

LC_WatchPtList_t WatchPtList[LC_HASH_TABLE_ENTRIES][LC_MAX_WATCHPOINTS];

/*
 * Global context structures
 */
typedef struct
{
    uint16      EventID;
    uint16      EventType;
    const char *Spec;
} __attribute__((packed)) CFE_EVS_SendEvent_context_t;

typedef struct
{
    const char *Spec;
} __attribute__((packed)) CFE_ES_WriteToSysLog_context_t;

/*
 * Macro to call a function and check its int32 return code
 */
#define UT_TEST_FUNCTION_RC(func, exp)                                                                \
    {                                                                                                 \
        int32 rcexp = exp;                                                                            \
        int32 rcact = func;                                                                           \
        UtAssert_True(rcact == rcexp, "%s (%ld) == %s (%ld)", #func, (long)rcact, #exp, (long)rcexp); \
    }

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), LC_UT_Setup, LC_UT_TearDown, #test)

int32 UT_Utils_stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);

/*
 * Setup function prior to every test
 */
void LC_Test_Setup(void);

/*
 * Teardown function after every test
 */
void LC_Test_TearDown(void);

#endif
