/**
 * @file command.cpp
 *
 * @brief Command interpreter
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
#include "CpmIncl.h"
#include "command.h"
#include "ip.h"
#include "io.h"
#include "config.h"
#ifndef WIN32
#include "i2c.h"
#include "logging.h"
#include "filesystem.h"
#endif // !WIN32


void printHelp(void);
void echo(void);

/**
 * @brief Process a command
 * 
 * @param[in] pCmd Command to process.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Command_process(const char_t *pCmd)
{
    bool_t success = TRUE;

    char_t cmd[256];
    uint32_t idx;
    bool_t keepParsing = TRUE;
    uint8_t strappingPosition;
    bool_t strappingPositionValid;
                                                                        
    // Trim command, up to the NULL or any line terminator character.
    for (idx = 0; idx < sizeof(cmd) && keepParsing; idx++)
    {
        switch(pCmd[idx])
        {
        case 0:
        case 10:
        case 11:
        case 12:
        case 13:
            cmd[idx] = 0;
            keepParsing = FALSE;
            break;
        default:
            cmd[idx] = pCmd[idx];
            break;
        }
    }

    // Parse command
    if (strcmp(cmd, "help") == 0)
    {
        printHelp();
    }
    else if (strcmp(cmd, "echo") == 0)
    {
        echo();
    }
    else if (strcmp(cmd, "readDiscretes") == 0)
    {
        if (!IO_readDiscreteINs())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "setDiscretesHI") == 0)
    {
        if (!IO_setDiscreteOUTs(TRUE))
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "setDiscretesLO") == 0)
    {
        if (!IO_setDiscreteOUTs(FALSE))
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readSerials") == 0)
    {
        if (!IO_readSerials())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "writeSerials") == 0)
    {
        if (!IO_writeSerials())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readA429s") == 0)
    {
        if (!IO_readA429s())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "writeA429s") == 0)
    {
        if (!IO_writeA429s())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readAnalog") == 0)
    {
        if (!IO_readAnalog())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readCANBus") == 0)
    {
        if (!IO_readCANBus())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "writeCANBus") == 0)
    {
        if (!IO_writeCANBus())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readPAP") == 0)
    {
        if (!IO_readPAP())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "readGPIO") == 0)
    {
        if (!IO_readGPIO())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "getPDI") == 0)
    {
#ifndef WIN32
        if (!Config_printDemoPDIContent())
        {
            IP_sendMessage("Command FAILED\n");
        }
#else
        IP_sendMessage("Command not supported on SIMphony\n");
#endif // !WIN32
    }
    else if (strcmp(cmd, "getStrappingPosition") == 0)
    {
        if (Config_readStrappingPosition(&strappingPosition,
                                         &strappingPositionValid))
        {
            if (strappingPositionValid)
            {
                IP_sendMessage("Strapping position = %02d\n", strappingPosition);
            }
            else
            {
                IP_sendMessage("Strapping position is FAILED (invalid)\n");
            }
        }
        else
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strcmp(cmd, "getTime") == 0)
    {
        if (!Config_printTime())
        {
            IP_sendMessage("Command FAILED\n");
        }
    }
    else if (strncmp(cmd, "setTime ", 8) == 0)
    {
#ifndef WIN32
        if (!I2C_setTime(&cmd[8]))
        {
            IP_sendMessage("Command FAILED\n");
        }
#else
        IP_sendMessage("Command not supported on SIMphony\n");
#endif // !WIN32
    }
    else if (strncmp(cmd, "logEvent ", 8) == 0)
    {
#ifndef WIN32
        if (!Logging_logEvent(&cmd[8]))
        {
            IP_sendMessage("Command FAILED\n");
        }
#else
        IP_sendMessage("Command not supported on SIMphony\n");
#endif // !WIN32
    }
    else if (strcmp(cmd, "printEvents") == 0)
    {
#ifndef WIN32
        if (!Filesystem_printLogFile("sa0:/Vol0/events.log"))
        {
            IP_sendMessage("Command FAILED\n");
        }
#else
        IP_sendMessage("Command not supported on SIMphony\n");
#endif // !WIN32
    }
    else if (strcmp(cmd, "printFaults") == 0)
    {
#ifndef WIN32
        if (!Filesystem_printLogFile("sa0:/Vol0/faults.log"))
        {
            IP_sendMessage("Command FAILED\n");
        }
#else
        IP_sendMessage("Command not supported on SIMphony\n");
#endif // !WIN32
    }
    else
    {
        IP_sendMessage("Invalid command: %s\n", cmd);
    }

    // Send end-of-message 
    IP_sendMessage("<EOM>");
    
    return success;
}

void printHelp(void)
{
    IP_sendMessage("Syntax:\n");
    IP_sendMessage("  echo\n");
    IP_sendMessage("      Send text \"echo\" on the TX UDP port.\n");
    IP_sendMessage("  readDiscretes\n");
    IP_sendMessage("      Read all discrete inputs\n");
    IP_sendMessage("  setDiscretesHI\n");
    IP_sendMessage("      Set all discrete outputs HIGH\n");
    IP_sendMessage("  setDiscretesLO\n");
    IP_sendMessage("      Set all discrete outputs LOW\n");
    IP_sendMessage("  readSerials\n");
    IP_sendMessage("      Read data from all RX serial ports\n");
    IP_sendMessage("  writeSerials\n");
    IP_sendMessage("      Write data on all TX serial ports\n");
    IP_sendMessage("  readA429s\n");
    IP_sendMessage("      Read labels from all RX ARINC-429 ports\n");
    IP_sendMessage("  writeA429s\n");
    IP_sendMessage("      Write dummy labels data on all TX ARINC-429 ports\n");
    IP_sendMessage("  getPDI\n");
    IP_sendMessage("      Get the content of the demo PDI\n");
    IP_sendMessage("  getStrappingPosition\n");
    IP_sendMessage("      Get the strapping position\n");
    IP_sendMessage("  getTime\n");
    IP_sendMessage("      Read the RTC (real-time clock)\n");
    IP_sendMessage("  setTime yyyy/mm/dd (D) hh:mm:ss\n");
    IP_sendMessage("      Set the RTC (real-time clock).\n");
    IP_sendMessage("      Parameter D is the day of the week (1 = Sunday, 7 = Saturday).\n");
    IP_sendMessage("  logEvent text\n");
    IP_sendMessage("      Log an event containing the specified text.\n");
    IP_sendMessage("  printEvents\n");
    IP_sendMessage("      Print the content of the event log file.\n");
    IP_sendMessage("  printFaults\n");
    IP_sendMessage("      Print the content of the fault log file.\n");
    IP_sendMessage("  readAnalog\n");
    IP_sendMessage("      Read Analog Inputs \n");
    IP_sendMessage("  readCANBus\n");
    IP_sendMessage("      Read CAN bus (CAN_CH1).\n");
    IP_sendMessage("  writeCANBus\n");
    IP_sendMessage("      Write CAN bus(CAN_CH1).\n");
    IP_sendMessage("  readPAP\n");
    IP_sendMessage("      Read PAP (CAN Bus: CAN_CH1).\n");
    IP_sendMessage("  readGPIO\n");
    IP_sendMessage("      Read GPIO (WOW).\n");
    IP_sendMessage("=============================================\n");
}

void echo(void)
{
    IP_sendMessage("echo\n");
}
 
