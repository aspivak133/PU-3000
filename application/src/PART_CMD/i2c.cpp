/**
 * @file i2c.cpp
 *
 * @brief I2C functionality demo
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
#include <stdio.h>
#include "MessageFormat_types.h"
#include "I2CMessage.h"
#include "I2CPlatformDevices.h"
#include "I2CDeviceCommands.h"
#include "PART_CMD.h"
#include "ip.h"
#include "io.h"

static QUEUING_PORT_ID_TYPE  m_I2CTxPort = 0;
static QUEUING_PORT_ID_TYPE  m_I2CRxPort = 0;

/**
 * @brief Demonstrate how to initialize the I2C service
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t I2C_initialize(void)
{
    bool_t success = TRUE;
    RETURN_CODE_TYPE rc;

    // Open the TX port
    CREATE_QUEUING_PORT(CMD_I2C_TX_APEX_NAME,
                        325,
                        30,
                        SOURCE,
                        FIFO,
                        &m_I2CTxPort,
                        &rc);
    if (rc != NO_ERROR)
    {
        success = FALSE;
    }                             

    // Open the RX port
    CREATE_QUEUING_PORT(CMD_I2C_RX_APEX_NAME,
                        325,
                        30,
                        DESTINATION,
                        FIFO,
                        &m_I2CRxPort,
                        &rc);
    if (rc != NO_ERROR)
    {
        success = FALSE;
    }             

    return success;                
}

/**
 * @brief Demonstrate how to set the RTC (real-time clock) using
 *        the I2C service.
 *
 * @warning In this example, the I2C request is sent on the TX port,
 *          but the RX port is not monitored for I2C replies.
 *          Operational software should always monitor the RX port in
 *          order to determine if the request was successful, or not.
 *
 * @param[in] pTime Time, of the form yyyy/mm/dd (D) hh:mm:ss
 *                  where D is the day of the week (1 = Sun, 7 = Sat).
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t I2C_setTime(char_t *pTime)
{
    bool_t success;
    uint8_t buffer[325];
    sI2CMessageHeader_t *pHdr;
    sI2CFieldAddressPayloadDataHeader_t *pFieldHdr;
    uint8_t *pPayload;

    RETURN_CODE_TYPE rc;
    int32_t len;
    int32_t hours;
    int32_t minutes;
    int32_t seconds;
    int32_t day;
    int32_t date;
    int32_t month;
    int32_t year;

    // Parse the time string
    len = sscanf(pTime, "%04d/%02d/%02d (%1d) %02d:%02d:%02d",
                 &year, &month, &date,
                 &day,
                 &hours, &minutes, &seconds);

    if (len == 7)
    {
        success = TRUE;
    }
    else
    {
        success = FALSE;
    }

    // Build the message
    if (success)
    {
        memset(&buffer[0], 0, sizeof(buffer));

        pHdr = (sI2CMessageHeader_t*)&buffer[0];
        pFieldHdr = (sI2CFieldAddressPayloadDataHeader_t*)(pHdr + 1);
        pPayload = (uint8_t*)(pFieldHdr + 1);        

        pHdr->avionicBusHeader.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pHdr->avionicBusHeader.avionicBusIOHeader.ioType = IOTYPE_I2C;        
        pHdr->avionicBusHeader.avionicBusIOHeader.messageType = MESSAGE_DATA;
        pHdr->payloadHeader.messageType = POST_COMMAND_I2C_MESSAGE;
        pHdr->payloadHeader.address = kI2cCpmRtc;
        pHdr->payloadHeader.addressingType = FIELD_I2C_ADDRESSING;
        pFieldHdr->nbOfField = 7;
        
        // Field years
        strcpy((char_t*)pPayload, I2C_RTC_YEAR);    // field name
        pPayload += (strlen(I2C_RTC_YEAR) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)(year % 2000);         // Data
        pPayload++;
        
        // Field months
        strcpy((char_t*)pPayload, I2C_RTC_MONTH);   // field name
        pPayload += (strlen(I2C_RTC_MONTH) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)month;                 // Data
        pPayload++;

        // Field date
        strcpy((char_t*)pPayload, I2C_RTC_DATE);    // field name
        pPayload += (strlen(I2C_RTC_DATE) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)date;                  // Data
        pPayload++;

        // Field day
        strcpy((char_t*)pPayload, I2C_RTC_DAY);     // field name
        pPayload += (strlen(I2C_RTC_DAY) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)day;                   // Data
        pPayload++;

        // Field hours
        strcpy((char_t*)pPayload, I2C_RTC_HOURS);   // field name
        pPayload += (strlen(I2C_RTC_HOURS) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)hours;                 // Data
        pPayload++;

        // Field minutes
        strcpy((char_t*)pPayload, I2C_RTC_MINUTES); // field name
        pPayload += (strlen(I2C_RTC_MINUTES) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)minutes;               // Data
        pPayload++;

        // Field seconds
        strcpy((char_t*)pPayload, I2C_RTC_SECONDS); // field name
        pPayload += (strlen(I2C_RTC_SECONDS) + 1);
        *((uint32_t*)pPayload) = 1;                 // 1 byte of data
        pPayload += sizeof(uint32_t);
        *pPayload = (uint8_t)seconds;               // Data
        pPayload++;
        
        pHdr->avionicBusHeader.avionicBusIOHeader.messageLength = pPayload - ((uint8_t*)&pHdr->payloadHeader);

        // Send the message
        SEND_QUEUING_MESSAGE(m_I2CTxPort,
                             (MESSAGE_ADDR_TYPE)&buffer[0],
                             (MESSAGE_SIZE_TYPE)(pPayload - &buffer[0]),
                             0,
                             &rc);

        IP_sendMessage("setTime: %s\n", IO_getReturnCodeText(rc));
        if (rc != NO_ERROR)
        {            
            success = FALSE;
        }
    }

    return success;
}
