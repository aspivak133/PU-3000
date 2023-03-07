/**
 * @file config.h
 *
 * @brief Configuration and Platform Parameters functionality demo header file
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
 
// See config.cpp for details
bool_t Config_initialize(void);

// see config.cpp for details
bool_t Config_printDemoPDIContent(void);

// see config.cpp for details
bool_t Config_readStrappingPosition(uint8_t *pPosition, bool_t *pValid);

// see config.cpp for details
bool_t Config_printTime(void);
