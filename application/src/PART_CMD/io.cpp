/**
 * @file io.cpp
 *
 * @brief I/O functionality demo
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

#include <stdio.h>
#include "MessageFormat_types.h"
#include "PART_CMD.h"
#include "io.h"
#include "ip.h"

#define NUM_DISC_IN    (8)
#define NUM_DISC_OUT   (6)
#define NUM_A429       (4)
#define NUM_SERIAL     (3)
#define NUM_ANALOG_IN  (4)
#define NUM_AIN        (1)
#define NUM_CANBUS     (1)
#define NUM_PAP        (2)
#define NUM_GPIO       (1)

static SAMPLING_PORT_ID_TYPE m_discreteINPorts[NUM_DISC_IN] = {0};
static SAMPLING_PORT_ID_TYPE m_discreteOUTPorts[NUM_DISC_OUT] = {0};
static QUEUING_PORT_ID_TYPE  m_A429TxPorts[NUM_A429] = {0};
static QUEUING_PORT_ID_TYPE  m_A429RxPorts[NUM_A429] = {0};
static QUEUING_PORT_ID_TYPE  m_serialTxPorts[NUM_SERIAL] = {0};
static QUEUING_PORT_ID_TYPE  m_serialRxPorts[NUM_SERIAL] = {0};
static QUEUING_PORT_ID_TYPE  m_canBusRxPorts[NUM_CANBUS] = {0};
static QUEUING_PORT_ID_TYPE  m_canBusTxPorts[NUM_CANBUS] = {0};
static QUEUING_PORT_ID_TYPE  m_papPorts[NUM_PAP] = {0};
static SAMPLING_PORT_ID_TYPE m_analogInputs[NUM_ANALOG_IN] = {0};
static SAMPLING_PORT_ID_TYPE m_GPIOPorts[NUM_GPIO] = {0};

static char_t m_analogInputsNames[NUM_ANALOG_IN][30] = {
    "AIN1",
    "AIN15",
    "AIN35",
    "AIN41"
};

static char_t m_canBusRxPortsNames[NUM_CANBUS][30] = {
    "CAN_BUS1_RX1"
};

static char_t m_canBusTxPortsNames[NUM_CANBUS][30] = {
    "CAN_BUS1_TX"
};

static char_t m_papPortsNames[NUM_PAP][30] = {
    "PAP_1",
    "PAP_2"
};

static char_t m_GPIOPortsNames[NUM_GPIO][30] = {
    "GPIO_WOW"
};

static char_t m_discreteINPortNames[NUM_DISC_IN][30] = {
    "DISC_IN1",
    "DISC_IN6",
    "DISC_IN11",
    "DISC_IN19",
    "DISC_IN33",
    "DISC_IN25",
    "DISC_IN31",
    "DISC_IN41"
};

static char_t m_discreteOUTPortNames[NUM_DISC_OUT][30] = {
    "DISC_OUT1",
    "DISC_OUT4",
    "DISC_OUT14",
    "DISC_OUT11",
    "DISC_OUT7",
    "DISC_OUT9"
};

static char_t m_A429TxPortNames[NUM_A429][30] = {
    "A429_OUT_01",
    "A429_OUT_03",
    "A429_OUT_05",
    "A429_OUT_09"
};

static char_t m_A429RxPortNames[NUM_A429][30] = {
    "A429_IN_01",
    "A429_IN_05",
    "A429_IN_09",
    "A429_IN_13"
};

static char_t m_serialTxPortNames[NUM_SERIAL][30] = {
    "SERIAL_OUT_01",
    "SERIAL_OUT_02",
    "SERIAL_OUT_03"
};

static char_t m_serialRxPortNames[NUM_SERIAL][30] = {
    "SERIAL_IN_01",
    "SERIAL_IN_02",
    "SERIAL_IN_03"
};



/**
 * @brief Demonstrate how to initialize the I/O service
 *
 * @details This function opens the following I/O ports:
 *          - 8 x Discrete inputs (1 per bank):
 *          - 6 x Discrete outputs (1 per bank):
 *          - 4 x ARINC-429 TX:
 *          - 4 x ARINC-429 RX:
 *          - 3 x Serial TX:
 *          - 3 x Serial RX:
 *          - 4 x Analog inputs:
 *          - 1 x CAN Bus read:
 *          - 1 x CAN Bus write:
 *          - 2 x PAP:
 *          - 1 x GPIO:
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_initialize(void)
{
    bool_t success = TRUE;
    RETURN_CODE_TYPE rc;
    uint32_t idx;

    // Analog inputs
    for (idx = 0; idx < NUM_ANALOG_IN; idx++)
    {
        CREATE_SAMPLING_PORT(m_analogInputsNames[idx],
                             40,
                             DESTINATION,
                             50000000ll,
                             &m_analogInputs[idx],
                             &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // Discrete inputs
    for (idx = 0; idx < NUM_DISC_IN; idx++)
    {
        CREATE_SAMPLING_PORT(m_discreteINPortNames[idx],
                             36,
                             DESTINATION,
                             50000000ll,
                             &m_discreteINPorts[idx],
                             &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // Discrete outputs
    for (idx = 0; idx < NUM_DISC_OUT; idx++)
    {
        CREATE_SAMPLING_PORT(m_discreteOUTPortNames[idx],
                             36,
                             SOURCE,
                             50000000ll,
                             &m_discreteOUTPorts[idx],
                             &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // GPIO
    for (idx = 0; idx < NUM_GPIO; idx++)
    {
        CREATE_SAMPLING_PORT(m_GPIOPortsNames[idx],
                             36,
                             DESTINATION,
                             50000000ll,
                             &m_GPIOPorts[idx],
                             &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // ARINC-429 RX
    for (idx = 0; idx < NUM_A429; idx++)
    {
        CREATE_QUEUING_PORT(m_A429RxPortNames[idx],
                            76,
                            1,
                            DESTINATION,
                            FIFO,
                            &m_A429RxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // ARINC-429 TX
    for (idx = 0; idx < NUM_A429; idx++)
    {
        CREATE_QUEUING_PORT(m_A429TxPortNames[idx],
                            76,
                            1,
                            SOURCE,
                            FIFO,
                            &m_A429TxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // Serial RX
    for (idx = 0; idx < NUM_SERIAL; idx++)
    {
        CREATE_QUEUING_PORT(m_serialRxPortNames[idx],
                            2084,
                            1,
                            DESTINATION,
                            FIFO,
                            &m_serialRxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // Serial TX
    for (idx = 0; idx < NUM_SERIAL; idx++)
    {
        CREATE_QUEUING_PORT(m_serialTxPortNames[idx],
                            2084,
                            1,
                            SOURCE,
                            FIFO,
                            &m_serialTxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }

    // CAN bus
    for (idx = 0; idx < NUM_CANBUS; idx++)
    {
        CREATE_QUEUING_PORT(m_canBusRxPortsNames[idx],
                            2084,
                            1,
                            DESTINATION,
                            FIFO,
                            &m_canBusRxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
            success = FALSE;
        }                             
    }
    for (idx = 0; idx < NUM_CANBUS; idx++)
    {
        CREATE_QUEUING_PORT(m_canBusTxPortsNames[idx],
                            2084,
                            1,
                            SOURCE,
                            FIFO,
                            &m_canBusTxPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
           success = FALSE;
        }                             
    }

    // PAP
    for (idx = 0; idx < NUM_PAP; idx++)
    {
        CREATE_QUEUING_PORT(m_papPortsNames[idx],
                            2084,
                            1,
                            DESTINATION,
                            FIFO,
                            &m_papPorts[idx],
                            &rc);
        if (rc != NO_ERROR)
        {
           success = FALSE;
        }                             
    }

    return success;
}

/**
 * @brief Obtain the description for a port status.
 *
 * @param[in] portStatus Port status;
 *
 * @return Port status description.
 */
const char_t* IO_getPortStatusText(uint8_t portStatus)
{
    switch(portStatus)
    {
    case PORT_STATUS_NORMAL:
        return "NORMAL";
    case PORT_STATUS_FAILURE:
        return "FAILURE";
    case PORT_STATUS_DEGRADED_DATA_OVERFLOW:
        return "DEGRADED OVERFLOW";
    case PORT_STATUS_DEGRADED_DATA_LOST:
        return "DEGRADED_DATA_LOST";
    case PORT_STATUS_DEGRADED_DATA_OUT_OF_RANGE:
        return "DEGRATED DATA OUT OF RANGE";
    case PORT_STATUS_DEGRADED_INTEGRITY:
        return "DEGRADED INTEGRITY";
    case PORT_STATUS_DEGRADED_CONNECTION_INTEGRITY:
        return "DEGRADED_CONNECTION_INTEGRITY";
    case PORT_STATUS_INIT:
        return "INIT";
    default:
        return "<unexpected>";
    }
}

/**
 * @brief Obtain the description for a ARINC-653 return code
 *
 * @param[in] rc Return code
 *
 * @return Return code description.
 */
const char_t* IO_getReturnCodeText(RETURN_CODE_TYPE rc)
{
    switch(rc)
    {
    case NO_ERROR:
        return "NO_ERROR";
    case NO_ACTION:
        return "NO_ACTION";
    case NOT_AVAILABLE:
        return "NOT_AVAILABLE";
    case INVALID_PARAM:
        return "INVALID_PARAM";
    case INVALID_CONFIG:
        return "INVALID_CONFIG";
    case INVALID_MODE:
        return "INVALID_MODE";
    case TIMED_OUT:
        return "TIMED_OUT";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Read all discrete inputs.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readDiscreteINs(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    VALIDITY_TYPE validity;
    sDiscreteDataMsg_t msg;

    for (idx = 0; idx < NUM_DISC_IN; idx++)
    {
        // Format the message header
        memset(&msg, 0, sizeof(msg));        
        msg.header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        msg.header.avionicBusIOHeader.ioType = IOTYPE_DISCRETE;

        READ_SAMPLING_MESSAGE(m_discreteINPorts[idx],
                              (MESSAGE_ADDR_TYPE)&msg,
                              &size,
                              &validity,
                              &rc);
        if (rc == NO_ERROR)
        {            
            IP_sendMessage("%s: %s, port status = %s value = %s\n", m_discreteINPortNames[idx],
                           IO_getReturnCodeText(rc), IO_getPortStatusText(msg.payload.status),
                           msg.payload.st == 0 ? "Low" : "High");
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_discreteINPortNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Set all discrete outputs HIGH or LOW.
 *
 * @param[in] high If TRUE, set the discreted HIGH, else
 *                 set them LOW.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_setDiscreteOUTs(bool_t high)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    sDiscreteDataMsg_t msg;

    for (idx = 0; idx < NUM_DISC_OUT; idx++)
    {
        // Format the message header
        memset(&msg, 0, sizeof(msg));        
        msg.header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        msg.header.avionicBusIOHeader.ioType = IOTYPE_DISCRETE;
        msg.header.avionicBusIOHeader.messageLength = sizeof(sDiscretePayloadBitField_t);
        msg.header.avionicBusIOHeader.messageType = MESSAGE_DATA;
        msg.payload.st = (high ? 1 : 0);

        WRITE_SAMPLING_MESSAGE(m_discreteOUTPorts[idx],
                               (MESSAGE_ADDR_TYPE)&msg,
                               (MESSAGE_SIZE_TYPE)sizeof(msg),
                               &rc);

        IP_sendMessage("%s: %s, port status = %s\n", m_discreteOUTPortNames[idx],
                       IO_getReturnCodeText(rc), IO_getPortStatusText(msg.payload.status));
        if (rc != NO_ERROR)
        {            
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read all serial ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readSerials(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    char_t buffer[2084];
    sSerialDataMsg_t *pMsg;
    char_t *pPayload;

    pMsg = (sSerialDataMsg_t*)&buffer[0];
    pPayload = (char_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_SERIAL; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_SERIAL;

        RECEIVE_QUEUING_MESSAGE(m_serialRxPorts[idx],
                                0,
                                (MESSAGE_ADDR_TYPE)&buffer[0],
                                &size,
                                &rc);

        if (rc == NO_ERROR || rc == INVALID_CONFIG)
        {    
            pPayload[pMsg->payload.nbOfData] = '\0';
            IP_sendMessage("%s: %s, port status = %s value = %s\n", m_serialRxPortNames[idx],
                           IO_getReturnCodeText(rc), IO_getPortStatusText(pMsg->payload.status),
                           pPayload);
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_serialRxPortNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Write on all serial ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_writeSerials(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    char_t buffer[2084];
    sSerialDataMsg_t *pMsg;
    char_t *pPayload;

    pMsg = (sSerialDataMsg_t*)&buffer[0];
    pPayload = (char_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_SERIAL; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_SERIAL;
        pMsg->header.avionicBusIOHeader.messageType = MESSAGE_DATA;
        pMsg->header.avionicBusIOHeader.messageLength = 23 + sizeof(sSerialPayloadHeader_t);
        pMsg->payload.nbOfData = 23;
        memcpy(pPayload, "Message to Serial TX n\n", 23);
        pPayload[21] = '1' + (char_t)idx;
        
        SEND_QUEUING_MESSAGE(m_serialTxPorts[idx],
                             (MESSAGE_ADDR_TYPE)&buffer[0],
                             sizeof(*pMsg) + pMsg->payload.nbOfData,
                             0,
                             &rc);

        IP_sendMessage("%s: %s, port status = %s\n", m_serialTxPortNames[idx],
                       IO_getReturnCodeText(rc), IO_getPortStatusText(pMsg->payload.status));
        if (rc != NO_ERROR)
        {            
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read all ARINC-429 ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readA429s(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    uint8_t buffer[76];
    sArinc429DataMsg_t *pMsg;
    uint32_t *pPayload;
    uint32_t payloadIdx;

    pMsg = (sArinc429DataMsg_t*)&buffer[0];
    pPayload = (uint32_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_A429; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_A429;

        RECEIVE_QUEUING_MESSAGE(m_A429RxPorts[idx],
                                0,
                                (MESSAGE_ADDR_TYPE)&buffer[0],
                                &size,
                                &rc);

        if (rc == NO_ERROR || rc == INVALID_CONFIG)
        {            
            IP_sendMessage("%s: %s, port status = %s nbLabels = %d labels:\n", m_A429RxPortNames[idx],
                           IO_getReturnCodeText(rc), IO_getPortStatusText(pMsg->payload.status),
                           pMsg->payload.nbOfData);
            for (payloadIdx = 0; payloadIdx < pMsg->payload.nbOfData; payloadIdx++)
            {
                IP_sendMessage("%08X\n", *pPayload);
                pPayload++;
            }
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_A429RxPortNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Write a dummy label on all ARINC-429 ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_writeA429s(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    uint8_t buffer[76];
    sArinc429DataMsg_t *pMsg;
    uint32_t *pPayload;

    pMsg = (sArinc429DataMsg_t*)&buffer[0];
    pPayload = (uint32_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_A429; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_A429;
        pMsg->header.avionicBusIOHeader.messageType = MESSAGE_DATA;
        pMsg->header.avionicBusIOHeader.messageLength = sizeof(uint32_t) + sizeof(sArinc429PayloadHeader_t);
        pMsg->payload.nbOfData = 1;
        pPayload[0] = 0xAABBCCDDu; // Dummy label
        
        SEND_QUEUING_MESSAGE(m_A429TxPorts[idx],
                             (MESSAGE_ADDR_TYPE)&buffer[0],
                             sizeof(*pMsg) + (sizeof(uint32_t) * pMsg->payload.nbOfData),
                             0,
                             &rc);
        IP_sendMessage("%s: %s, port status = %s\n", m_A429TxPortNames[idx],
                       IO_getReturnCodeText(rc), IO_getPortStatusText(pMsg->payload.status));
        if (rc != NO_ERROR)
        {            
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read Analog Input ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readAnalog(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    VALIDITY_TYPE validity;
    sAnalogInputDataMsg_t msg;

    for (idx = 0; idx < NUM_ANALOG_IN; idx++)
    {
        // Format the message header
        memset(&msg, 0, sizeof(msg));
        msg.header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        msg.header.avionicBusIOHeader.ioType = IOTYPE_ANALOG;

        READ_SAMPLING_MESSAGE(m_analogInputs[idx],
                               (MESSAGE_ADDR_TYPE)&msg,
                                &size,
                                &validity,
                                &rc);

        if (rc == NO_ERROR)
        {            
            IP_sendMessage("%s: %s, port status = %s value = %f\n", m_analogInputsNames[idx],
                           IO_getReturnCodeText(rc), IO_getPortStatusText(msg.payload.status),
                           msg.payload.data);
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_analogInputsNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read GPIO WOW.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readGPIO(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    VALIDITY_TYPE validity;
    sDiscreteDataMsg_t msg;

    for (idx = 0; idx < NUM_GPIO; idx++)
    {
        // Format the message header
        msg.header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        msg.header.avionicBusIOHeader.ioType = IOTYPE_DISCRETE;

        READ_SAMPLING_MESSAGE(m_GPIOPorts[idx],
                               (MESSAGE_ADDR_TYPE)&msg,
                                &size,
                                &validity,
                                &rc);

        if (rc == NO_ERROR)
        {            
            IP_sendMessage("%s: %s, port status = %s value = %d\n", m_GPIOPortsNames[idx],
                           IO_getReturnCodeText(rc), IO_getPortStatusText(msg.payload.status),
                           msg.payload.st);
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_GPIOPortsNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read CAN Bus RX ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readCANBus(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    char_t buffer[2084] = {0};
    sCanBusDataMsg_t *pMsg;
    sCanBusMessage_t *pPayload;

    pMsg = (sCanBusDataMsg_t*)&buffer[0];
    pPayload = (sCanBusMessage_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_CANBUS; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_CANBUS;

        RECEIVE_QUEUING_MESSAGE(m_canBusRxPorts[idx],
                                0,
                                (MESSAGE_ADDR_TYPE)&buffer[0],
                                &size,
                                &rc);

        if (rc == NO_ERROR || rc == INVALID_CONFIG)
        {
            IP_sendMessage("%s: %s, port status = %s nbOfData = %d, canId = %d, data[0] = 0x%x, data[5] = 0x%x\n", 
                           m_canBusRxPortsNames[idx],
                           IO_getReturnCodeText(rc), 
                           IO_getPortStatusText(pMsg->payload.status),
                           pMsg->payload.nbOfData, 
                           pPayload->canId, 
                           pPayload->data[0],
                           pPayload->data[5]
                           );
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_canBusRxPortsNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}

/**
 * @brief Read CAN Bus RX ports.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_writeCANBus(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    char_t buffer[2084] = {0};
    sCanBusDataMsg_t *pMsg;
    sCanBusMessage_t *pPayload;

    pMsg = (sCanBusDataMsg_t*)&buffer[0];
    pPayload = (sCanBusMessage_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_CANBUS; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));        
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_CANBUS;
        pMsg->header.avionicBusIOHeader.messageType = MESSAGE_DATA;
        pMsg->payload.nbOfData = 1;
        pMsg->header.avionicBusIOHeader.messageLength = sizeof(sCanBusPayloadHeader_t) + (sizeof(sCanBusMessage_t) * pMsg->payload.nbOfData);

        pPayload->canId = 100;
        pPayload->dlc = 8;
        pPayload->data[0] = 101;
        pPayload->data[1] = 102;
        pPayload->data[2] = 103;
        pPayload->data[3] = 104;
        pPayload->data[4] = 105;
        pPayload->data[5] = 106;
        pPayload->data[6] = 107;
        pPayload->data[7] = 108;
        
        SEND_QUEUING_MESSAGE(m_canBusTxPorts[idx],
                             (MESSAGE_ADDR_TYPE)&buffer[0],
                             sizeof(*pMsg) + (sizeof(sCanBusMessage_t) * (pMsg->payload.nbOfData)),
                             0,
                             &rc);

        IP_sendMessage("%s: %s, port status = %s\n", m_canBusTxPortsNames[idx],
                       IO_getReturnCodeText(rc), IO_getPortStatusText(pMsg->payload.status));
        if (rc != NO_ERROR)
        {            
            success = FALSE;
        }
    }

    return success;
}


/**
 * @brief Read PAR (CAN Bus RX) port.
 *
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IO_readPAP(void)
{
    bool_t success = TRUE;
    uint32_t idx;
    RETURN_CODE_TYPE rc;
    MESSAGE_SIZE_TYPE size;
    char_t buffer[2084] = {0};
    sCanBusDataMsg_t *pMsg;
    sCanBusMessage_t *pPayload;

    pMsg = (sCanBusDataMsg_t*)&buffer[0];
    pPayload = (sCanBusMessage_t*)(pMsg + 1);

    for (idx = 0; idx < NUM_PAP; idx++)
    {
        // Format the message header
        memset(pMsg, 0, sizeof(*pMsg));
        memset(&buffer, 0, sizeof(buffer));
        pMsg->header.commonClassHeader.classCode = CLASS_CODE_AVIONICS_BUS_IO_HEADER;
        pMsg->header.avionicBusIOHeader.ioType = IOTYPE_CANBUS;

        RECEIVE_QUEUING_MESSAGE(m_papPorts[idx],
                                0,
                                (MESSAGE_ADDR_TYPE)&buffer[0],
                                &size,
                                &rc);

        if (rc == NO_ERROR || rc == INVALID_CONFIG)
        {
            IP_sendMessage("%s: %s, port status = %s nbOfData = %d, canId = %d, data[0] = 0x%x, data[5] = 0x%x\n", 
                           m_papPortsNames[idx],
                           IO_getReturnCodeText(rc), 
                           IO_getPortStatusText(pMsg->payload.status),
                           pMsg->payload.nbOfData, 
                           pPayload->canId, 
                           pPayload->data[0],
                           pPayload->data[5]
                           );
        }
        else
        {
            IP_sendMessage("%s: %s\n", m_papPortsNames[idx],
                           IO_getReturnCodeText(rc));
            success = FALSE;
        }
    }

    return success;
}


