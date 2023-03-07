/**
 * @file ip.h
 *
 * @brief IP communication functionality demo header file
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
 
// See ip.cpp for details 
bool_t IP_execute(void);

// See ip.cpp for details 
bool_t IP_sendMessage(const char_t *pFormat, ...);
 
