/**
 * @file ip.cpp
 *
 * @brief IP communication functionality demo
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
#ifndef WIN32
#include <ipflite.h>
#else
#include <WinSock2.h>
#endif // !WIN32
#include <stdio.h>
#include "CpmIncl.h"
#include "PART_CMD.h"
#include "ip.h"
#include "command.h"

#ifndef WIN32
#define REMOTE_IP_ADDR (0xC0A803C9)
#else
#define REMOTE_IP_ADDR (0x7F000001)
#endif // !WIN32
#define CMD_RX_UDP_PORT (21500)
#define CMD_TX_UDP_PORT (21501)
#define SOCKET_BUFFER_SIZE (4096)

static int32_t m_socketTxFd = -1;
static int32_t m_socketRxFd = -1;
static sockaddr_in remoteCmdTxUDPAddress = {0};
static sockaddr_in remoteCmdRxUDPAddress = {0};

/**
 * @brief Demonstrate how to initialize the IP service and receive data.
 *
 * @details This function demonstrates how to open a socket and read from it.
 *          Sockets are bound to a process, hence the necessity
 *          of opening the socket in the process that uses it.
 *
 * See function IP_sendMessage() for the TX portion.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IP_execute(void)
{    
    static bool firstTime = TRUE;
    bool_t success;
    char_t cmdRxBuffer[SOCKET_BUFFER_SIZE + 1];
    int32_t rxLen;
    int32_t socketBufferSize  = SOCKET_BUFFER_SIZE;
    sockaddr_in from;
    int32_t fromLen;
#ifndef WIN32
    uint32_t nonBlocking = 1;
#else
	unsigned long nonBlocking = 1;
#endif // !WIN32

    
    if (firstTime)
    {
#ifndef WIN32
        // Initialize IPFLITE library
        if (ipflite_client_initialize((Connection)LNK_CONN_IPSTACK_CMD_0) == Success)
        {
            success = TRUE;        
        }
        else
        {
            success = FALSE;
        }
#else
        success = TRUE;
#endif // !WIN32

        // Create and configure the sockets
        if (success)
        {
            m_socketTxFd = socket(AF_INET, SOCK_DGRAM, 0);
            if (m_socketTxFd < 0)
            {
                success = FALSE;
            }
        }

        if (success)
        {
#ifndef WIN32
            if (setsockopt(m_socketTxFd, SOL_SOCKET, SO_SNDBUF, &socketBufferSize, sizeof(int32_t)) != 0)
#else
            if (socket_setsockopt(m_socketTxFd, SOL_SOCKET, SO_SNDBUF, &socketBufferSize, sizeof(int32_t)) != 0)
#endif //!WIN32
            {
                success = FALSE;
            }
        }

        if (success)
        {
            m_socketRxFd = socket(AF_INET, SOCK_DGRAM, 0);
            if (m_socketRxFd < 0)
            {
                success = FALSE;
            }
        }

        if (success)
        {
#ifndef WIN32
            if (setsockopt(m_socketRxFd, SOL_SOCKET, SO_RCVBUF, &socketBufferSize, sizeof(int32_t)) != 0)
#else
            if (socket_setsockopt(m_socketRxFd, SOL_SOCKET, SO_RCVBUF, &socketBufferSize, sizeof(int32_t)) != 0)
#endif // !WIN32
            {
                success = FALSE;
            }
        }

        if (success)
        {
#ifndef WIN32
            if (ipflite_ioctl(m_socketRxFd, FIONBIO, &nonBlocking, sizeof(nonBlocking)) != 0)
#else
            if (ioctlsocket(m_socketRxFd, FIONBIO, &nonBlocking) != 0)  
#endif // !WIN32
            {
                success = FALSE;
            }
        }

        // Configure the UDP addresses
        memset(&remoteCmdTxUDPAddress, 0, sizeof(remoteCmdTxUDPAddress));
        remoteCmdTxUDPAddress.sin_family = AF_INET;
        remoteCmdTxUDPAddress.sin_port = htons(CMD_TX_UDP_PORT);
        remoteCmdTxUDPAddress.sin_addr.s_addr = htonl(REMOTE_IP_ADDR);

        memset(&remoteCmdRxUDPAddress, 0, sizeof(remoteCmdRxUDPAddress));
        remoteCmdRxUDPAddress.sin_family = AF_INET;
        remoteCmdRxUDPAddress.sin_port = htons(CMD_RX_UDP_PORT);
        remoteCmdRxUDPAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        // Bind the RX socket
        if (success)
        {
            if (bind(m_socketRxFd, (const struct sockaddr *)&remoteCmdRxUDPAddress, sizeof(remoteCmdRxUDPAddress)) != 0)
            {
                success = FALSE;
            } 
        }

        firstTime = FALSE;
    }
    else
    {
        success = TRUE;
    }
     
    // Receive command from the RX socket, and pass them to the command
    // interpreter.
    if (success)
    {
        fromLen = sizeof(from);
        rxLen = recvfrom(m_socketRxFd, 
                         &cmdRxBuffer[0], 
                         sizeof(cmdRxBuffer) - 1, 
                         0,
                         (struct sockaddr*)&from,
                         &fromLen);
        if (rxLen > 0 && rxLen <= (sizeof(cmdRxBuffer) - 1))
        {
            cmdRxBuffer[rxLen] = '\0';                
            Command_process(&cmdRxBuffer[0]);
        }
    }
    
    return success;
}

/**
 * @brief Demonstrate how to transmit using the IP service.
 *        This function has the same form as printf() does.
 *        However, the resulting string may not exceed 4096 characters, and
 *        there is not protection for overlow.
 *
 * @details This function demonstrates how to:
 *          - Transmit on a TX socket
 *
 * @param[in] pMsg message to transmit.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t IP_sendMessage(const char_t *pFormat, ...)
{
    static char_t message[SOCKET_BUFFER_SIZE];

    bool_t success = TRUE;
    int32_t len;
    va_list args;

    (void)va_start(args, pFormat);
    len = vsprintf(&message[0], pFormat, args);
    (void)va_end(args);
    
    if (len >= SOCKET_BUFFER_SIZE)
    {
        len = (SOCKET_BUFFER_SIZE - 1);
    }
    else if (len > 0)
    {
        message[len] = '\0';

        if (m_socketTxFd >= 0)
        {
            (void)sendto(m_socketTxFd, 
                         message, 
                         len, 
                         0, 
                         (const struct sockaddr *)&remoteCmdTxUDPAddress, 
                         sizeof(remoteCmdTxUDPAddress));
        }
    }

    return success;
}
