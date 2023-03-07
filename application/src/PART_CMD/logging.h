/**
 * @file logging.h
 *
 * @brief Event Log functionality demo header file
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
 
// See logging.cpp for details 
bool_t Logging_initialize(void);

// See logging.cpp for details 
bool_t Logging_logEvent(const char_t *pText);
 
