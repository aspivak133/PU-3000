/**
 * @file filesystem.cpp
 *
 * @brief File system functionality demo
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
#include <apex_file_system.h>
#include <filesystem/pjfs_mount.h>
#include "EventLog_types.h"
#include "ip.h"
#include "io.h"
#include "filesystem.h"

/**
 * @brief Demonstrate how to initialize the file system
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Filesystem_initialize(void)
{    
    bool_t success;
    Error error;

    error = pjfs_initlib();
    if (error == Success)
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
 * @brief Demonstrate how to read from the file system.
 *
 * @details This function reads a log file and prints its
 *          content.
 *
 * @param[in] pPath Absolute path of the file.
 *
 * @retval TRUE if the operation succeeded.
 * @retval FALSE if the operation failed.
 */
bool_t Filesystem_printLogFile(char_t *pPath)
{
    bool_t success;
    FILE_ID_TYPE handle;
    RETURN_CODE_TYPE rc;
    FILE_ERRNO_TYPE errno;
    bool_t fileOpen;
    sEventLogRecord_t readBuffer[20];
    MESSAGE_SIZE_TYPE readLen;
    uint32_t numEventsRead;
    uint32_t eventIdx;
    sEventLogRecord_t *pRecord;
    bool_t hasMore;

    // Open the file
    OPEN_FILE(pPath, READ, &handle, &rc, &errno);
    if (rc == NO_ERROR)
    {
        fileOpen = TRUE;
        success = TRUE;
    }
    else if (rc == INVALID_PARAM && errno == ENOENT)
    {
        // File does not exist
        fileOpen = FALSE;
        success = TRUE;
        IP_sendMessage("File %s does not exist\n", pPath);
    }
    else
    {
        fileOpen = FALSE;
        success = FALSE;
        IP_sendMessage("Cannot open file %s: %s errno = %d\n",
                       pPath,
                       IO_getReturnCodeText(rc),
                       errno);
    }

    // Read the file's content, in chunks of 20 log records
    hasMore = fileOpen;
    while (success && hasMore)
    {
        READ_FILE(handle,
                  (MESSAGE_ADDR_TYPE)&readBuffer[0],
                  (MESSAGE_SIZE_TYPE)sizeof(readBuffer),
                  &readLen,
                  &rc,
                  &errno);
        if (rc == NO_ERROR)
        {
            if (readLen < sizeof(sEventLogRecord_t))
            {
                // No more complete record to read
                hasMore = FALSE;
            }
            else
            {
                numEventsRead = readLen / sizeof(sEventLogRecord_t);

                // Print the events
                for (eventIdx = 0; eventIdx < numEventsRead; eventIdx++)
                {
                    pRecord = &readBuffer[eventIdx];
                    IP_sendMessage("B%08x R%08x %04u/%02u/%02u %02u:%02u:%02u S%04x SS%04x E%08x \"%s\" C%08x T%016llx\n",
                                   pRecord->blockId,
                                   pRecord->recordId,
                                   pRecord->time.year,
                                   pRecord->time.month,
                                   pRecord->time.date,
                                   pRecord->time.hours,
                                   pRecord->time.minutes,
                                   pRecord->time.seconds,
                                   pRecord->systemId,
                                   pRecord->subSystemId,
                                   pRecord->eventCode,
                                   pRecord->eventText,
                                   pRecord->eventCriticality,
                                   pRecord->ticks);
                }

                if (numEventsRead * sizeof(sEventLogRecord_t) != readLen)
                {
                    // The last record was truncated, meaning the file is
                    // being written to. Consider there is no more data.
                    hasMore = FALSE;
                }
            }
        }
        else
        {
            success = FALSE;
            IP_sendMessage("Error reading file %s: %s errno = %d\n",
                           pPath,
                           IO_getReturnCodeText(rc),
                           errno);
        }
    }

    // Close the file
    if (fileOpen)
    {
        CLOSE_FILE(handle, &rc, &errno);
        if (rc != NO_ERROR)
        {
            IP_sendMessage("Cannot close file %s: %s errno = %d\n",
                           pPath,
                           IO_getReturnCodeText(rc),
                           errno);
            success = FALSE;
        }
    }

    return success;
}
