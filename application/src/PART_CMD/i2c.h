/**
 * @file i2c.h
 *
 * @brief I2C functionality demo header file
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
 
// See io.cpp for details 
bool_t I2C_initialize(void);

// See io.cpp for details 
bool_t I2C_setTime(char_t *pTime);
