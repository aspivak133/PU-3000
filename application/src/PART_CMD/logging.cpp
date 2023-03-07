/**
 * @file logging.cpp
 *
 * @brief Event Log functionality demo
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
#include "EventLogLoggerClient.h"
#include "logging.h"
#include "ip.h"

/* @brief Logger instance */
static EventLogLoggerClient *m_pLogger = NULL;

/**
 * @brief Demonstrate how to initialize the logger client
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Logging_initialize(void)
{    
    bool_t success;
    int32_t retCode;

    m_pLogger = EventLogLoggerClient::GetInstance();

    if (m_pLogger == NULL)
    {
        success = FALSE;
    }
    else
    {
        success = TRUE;
    }

    if (success)
    {
        retCode = m_pLogger->Initialize(3,  // System ID, see Platform_EVLOG_Config.cxml
                                        7); // Subsystem ID, see Platform_EVLOG_Config.cxml
        if (retCode != CPM_RC_SUCCESS)
        {
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Demonstrate how to log an event.
 *
 * @param[in] pText Text to log.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Logging_logEvent(const char_t *pText)
{
    bool_t success = TRUE;
    int32_t retCode;
    
    retCode = m_pLogger->LogEvent(3,  // System ID
                                  7,  // Subsystem ID
                                  17, // Event code
                                  EVENTLOG_CRITICALITY_INFORMATION,
                                  pText);

    if (retCode != CPM_RC_SUCCESS)
    {
        success = FALSE;
    }

    return success;
}
