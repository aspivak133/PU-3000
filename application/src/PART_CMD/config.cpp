/**
 * @file config.cpp
 *
 * @brief Configuration and Platform Parameters functionality demo
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
#include "MessageFormat_types.h"
#include "ConfigurationLibrary.h"
#include "PlatformParameter_types.h"
#include "ConfigurationLibraryInitialize.h"
#include "PART_CMD.h"
#include "config.h"
#include "ip.h"

static ConfigurationLibrary m_platformParamConfigLib;

/**
 * @brief Wait for the platform to be ready.
 *
 * @details Application should wait for this event
 *          before attempting to use platform services.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
static bool_t waitForPlatformReady(void)
{
    bool_t success;
    int32_t retCode;
    const void *pParamTable = NULL;
    const sPtfParam32Bits_t *pHealthStateParam = NULL;
    uint32_t paramSize = 0;

    // Obtain the parameter table desciptor
    retCode = m_platformParamConfigLib.GetDescriptorByIdName(PTF_PARAM_TABLE_DESC_TYPE,
                                                             PTF_PARAM_TABLE_ID_NAME,
                                                             &pParamTable);
    
    // Obtain the Platform Health State parameter value
    if (retCode == CPM_RC_SUCCESS)
    {
        retCode = m_platformParamConfigLib.GetParameterValue(pParamTable,
                                                             PTF_PARAM_PLATFORM_HEALTH_STATE_NAME,
                                                             (const void **)&pHealthStateParam,
                                                             &paramSize);
    }

    // Wait until the parameter status is published, and its value is other than
    // 0 (platform initializing).
    if (retCode == CPM_RC_SUCCESS)
    {
        while (pHealthStateParam->status  == FS_NO_COMPUTED_DATA
               || pHealthStateParam->data == 0)
        {
            // Keep waiting, health state not published or still initializing
        }
    }

    if (retCode == CPM_RC_SUCCESS)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }

    return success;
}

/**
 * @brief Demonstrate how to use the configuration service.
 *
 * @details This function demonstrates how to:
 *          - Initialize the configuration service client.
 *          - Initialize configuration libraries associated with PDIs,
 *            in this case the Platform Parameter record.
 *          - Wait for the Platform to finish initializing, using the Platform Parameters.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Config_initialize(void)
{
    bool_t success;
    int32_t retCode;

    // Initialize the configuration service client, with the generated configuration
    // shared memory.
    if (InitializeConfigurationServiceClient(MEM_CMD_CONFIG_DATA) == CPM_RC_SUCCESS)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }
    
    // Initialize the Platform Parameter configuration library.
    if (success)
    {
        do
        {
            retCode = m_platformParamConfigLib.Initialize(PTF_PARAM_REC_ID_NAME);
        }
        while (retCode == CPM_RC_NOT_READY);        
        
        if (retCode != CPM_RC_SUCCESS)
        {
            success = FALSE;
        }
    }
    
    // Wait for the platform to be ready before proceeding
    // with other service initializations.
    if (success)
    {
        success = waitForPlatformReady();
    }

    return success;
}

/**
 * @brief Demonstrate how to use the configuration service.
 *
 * @details This function demonstrates how to:
 *          - Initialize configuration libraries associated with a PDI, in this
 *            case, a foreign record.
 *          - Obtain the content of a PDI.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Config_printDemoPDIContent(void)
{
    bool_t success;
    int32_t retCode;
    ConfigurationLibrary configLib;
    uint32_t demoDataLength;
    const char_t *pDemoData;    

    // Initialize a configuration library associated with the
    // demo partition's PDI
    do
    {
        // "CMD_PDI" is the PDI's Foreign Configuration Record Name,
        // as per the NVMAT.
        retCode = configLib.Initialize("CMD_PDI");
    }
    while (retCode == CPM_RC_NOT_READY);        
    
    if (retCode == CPM_RC_SUCCESS)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }

    // Obtain the content of the demo partition's PDI.
    // It should be string "This is the demo application PDI's content."
    if (success)
    {
        retCode = configLib.GetRecordRawData((const void**)&pDemoData, &demoDataLength);
        if (retCode == CPM_RC_SUCCESS)
        {
            IP_sendMessage("Demo PDI content: %s\n", pDemoData);
        }
        else
        {
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Demonstrate how to obtain the strapping position using the
 *        platform parameters.
 *
 * @param[out] pPosition Strapping position, set to 0 if pValid is set
 *                       to FALSE.
 * @param[out] pValid Strapping position validity.
 *
 * @retval TRUE if the operation succeeded (whether valid or not).
 * @retval FALSE if the operation failed.
 */
bool_t Config_readStrappingPosition(uint8_t *pPosition, bool_t *pValid)
{
    bool_t success;
    int32_t retCode;
    const void *pParamTable = NULL;
    const sPtfParam8Bits_t *pStrappingPosParam = NULL;
    uint32_t paramSize = 0;

    *pPosition = 0;
    *pValid = FALSE;

    // Obtain the parameter table desciptor. Note that the Configuration Library
    // is already initialized (see Config_initialize())
    retCode = m_platformParamConfigLib.GetDescriptorByIdName(PTF_PARAM_TABLE_DESC_TYPE,
                                                             PTF_PARAM_TABLE_ID_NAME,
                                                             &pParamTable);

    // Obtain the Strapping Position parameter value
    if (retCode == CPM_RC_SUCCESS)
    {
        retCode = m_platformParamConfigLib.GetParameterValue(pParamTable,
                                                             PTF_PARAM_STRAPPING_POSITION_NAME,
                                                             (const void **)&pStrappingPosParam,
                                                             &paramSize);
    }

    // Wait until the parameter status is published.
    if (retCode == CPM_RC_SUCCESS)
    {
        while (pStrappingPosParam->status  == FS_NO_COMPUTED_DATA)
        {
            // Keep waiting, strapping position not computed.
        }

        if (pStrappingPosParam->status == FS_NORMAL)
        {
            *pPosition = pStrappingPosParam->data;
            *pValid = TRUE;
        }
        else
        {
            // Strapping position is invalid
        }
    }

    if (retCode == CPM_RC_SUCCESS)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }

    return success;
}

/**
 * @brief Demonstrate how to obtain the real-time clock value using the
 *        platform parameters.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Config_printTime(void)
{
    bool_t success;
    int32_t retCode;
    const void *pParamTable = NULL;
    const sPtfParamDateTime_t *pRTCParam = NULL;
    uint32_t paramSize = 0;

    // Obtain the parameter table desciptor. Note that the Configuration Library
    // is already initialized (see Config_initialize())
    retCode = m_platformParamConfigLib.GetDescriptorByIdName(PTF_PARAM_TABLE_DESC_TYPE,
                                                             PTF_PARAM_TABLE_ID_NAME,
                                                             &pParamTable);

    // Obtain the Strapping Position parameter value
    if (retCode == CPM_RC_SUCCESS)
    {
        retCode = m_platformParamConfigLib.GetParameterValue(pParamTable,
                                                             PTF_PARAM_LRU_TIME_NAME,
                                                             (const void **)&pRTCParam,
                                                             &paramSize);
    }

    // Wait until the parameter status is published.
    if (retCode == CPM_RC_SUCCESS)
    {
        if (pRTCParam->status == FS_NO_COMPUTED_DATA)
        {
            IP_sendMessage("RTC is not available (no computed data)\n");
        }
        else if (pRTCParam->status == FS_NORMAL)
        {
            IP_sendMessage("RTC = %04d/%02d/%02d (%d) %02d:%02d:%02d\n",
                           pRTCParam->data.year,
                           pRTCParam->data.month,
                           pRTCParam->data.date,
                           pRTCParam->data.day,
                           pRTCParam->data.hours,
                           pRTCParam->data.minutes,
                           pRTCParam->data.seconds);
        }
        else
        {
            IP_sendMessage("RTC is FAILED\n");
        }
    }

    if (retCode == CPM_RC_SUCCESS)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }

    return success;
}
