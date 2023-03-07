/**
 * @file main.cpp
 *
 * @brief Main file for the IAP-7000 Platform sample application (PART_CMD)
 *
 **/
/*=============================================================================
 *
 * THIS FILE CONTAINS CMC ELECTRONICS PROPRIETARY INFORMATION AND SHALL
 * NOT BE COPIED OR DISTRIBUTED TO THIRD PARTIES WITHOUT THE PRIOR WRITTEN
 * PERMISSION OF CMC ELECTRONICS INC.
 *
 *=============================================================================
 *
 */

#include <apex_api.h>
#include "CpmIncl.h"
#include "PART_CMD.h"
#include "config.h"
#include "ip.h"
#include "io.h"
#ifndef WIN32
#include "i2c.h"
#include "logging.h"
#include "filesystem.h"
#endif // !WIN32

/** @brief Entry point of process PROC_TEST */
void procTestEntryPoint(void);

/**
 * @brief ARINC-653 process error handler
 */
void A653ErrorHandler(void)
{
    ERROR_STATUS_TYPE errorStatus;
    RETURN_CODE_TYPE ret;

    GET_ERROR_STATUS (&(errorStatus), &(ret));

    if(ret != NO_ACTION)
    {
        switch (errorStatus.ERROR_CODE)
        {
            case DEADLINE_MISSED:
                // Ignore
                break;
            case APPLICATION_ERROR:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
            case NUMERIC_ERROR:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
            case ILLEGAL_REQUEST:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
            case STACK_OVERFLOW:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
            case MEMORY_VIOLATION:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
            case POWER_FAIL:
            case HARDWARE_FAULT:
            default:
                SET_PARTITION_MODE(IDLE, &ret);
                break;
        }
    }
}

/**
 * @brief Main function.
 *
 * @details This function is the initial task's entry point.
 */
#ifdef WIN32
void main(void)
#else  // WIN32
int main(void)
#endif // WIN32
{
    bool_t success;
    PROCESS_ATTRIBUTE_TYPE procAttrib;
    PROCESS_ID_TYPE procId;
    RETURN_CODE_TYPE rc;
    
    success = TRUE;

    // Register the ARINC-653 process error handler
    CREATE_ERROR_HANDLER((SYSTEM_ADDRESS_TYPE)A653ErrorHandler, (STACK_SIZE_TYPE)0x200, &rc);
    if (rc != NO_ERROR)
    {
        success = FALSE;
    }   
    
    // Demonstrate how to use the configuration service,
    // including Platform Parameters.
    if (!Config_initialize())
    {
        success = FALSE;
    }

    // Demonstrate how to initialize the I/Os
    if (!IO_initialize())
    {
        success = FALSE;
    }
    
#ifndef WIN32
    // Demonstrate how to initialize the I2C interface
    if (!I2C_initialize())
    {
        success = FALSE;
    }

    // Demonstrate how to initialize the logging service
    if (!Logging_initialize())
    {
        success = FALSE;
    }

    // Demonstrate how to initialize the file system service
    if (!Filesystem_initialize())
    {
        success = FALSE;
    }
#endif // !WIN32

    // Create and start the PROC_TEST process.
    procAttrib.PERIOD = 50000000ll;
    procAttrib.TIME_CAPACITY = INFINITE_TIME_VALUE;
    procAttrib.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)procTestEntryPoint;
    procAttrib.STACK_SIZE = 0x10000;
    procAttrib.BASE_PRIORITY = 150;
    procAttrib.DEADLINE = SOFT;
    strcpy(procAttrib.NAME, PROC_TEST_APEX_NAME);
    
    CREATE_PROCESS(&procAttrib, &procId, &rc);
    if (rc == NO_ERROR)
    {
        START(procId, &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }
    }
    else
    {
        success = FALSE;
    }
    
    // Switch to NORMAL mode, after which the PROC_TEST process
    // will start executing.
    if (success)
    {
        SET_PARTITION_MODE(NORMAL, &rc);
    }
    else
    {
        SET_PARTITION_MODE(IDLE, &rc);
    }
    if (rc != NO_ERROR)
    {
        success = FALSE;
    }
    
#ifdef WIN32
    return;
#else  // WIN32
    return Success;
#endif // WIN32
}

void procTestEntryPoint(void)
{
#ifdef WIN32
    static bool_t firstTime = TRUE;
#endif // WIN32

    bool_t success = TRUE;
    RETURN_CODE_TYPE rc;

    while (success)
    {        
        // Execute IP processing
        if (!IP_execute())
        {
            success = FALSE;
        }

        // Wait for the next period.
        PERIODIC_WAIT(&rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }
    }    
}
