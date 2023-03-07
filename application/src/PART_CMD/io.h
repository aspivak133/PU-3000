/**
 * @file io.h
 *
 * @brief I/O functionality demo header file
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
#include "CpmIncl.h"
 
// See io.cpp for details 
bool_t IO_initialize(void);

// See io.cpp for details 
bool_t IO_readDiscreteINs(void);

// See io.cpp for details 
bool_t IO_setDiscreteOUTs(bool_t high);

// See io.cpp for details 
bool_t IO_readSerials(void);

// See io.cpp for details 
bool_t IO_writeSerials(void);

// See io.cpp for details 
bool_t IO_readA429s(void);

// See io.cpp for details 
bool_t IO_writeA429s(void);

// See  io.cpp for details 
bool_t IO_readAnalog(void);

// See  io.cpp for details
bool_t IO_readDigital(void);

// See  io.cpp for details
bool_t IO_readCANBus(void);

// See  io.cpp for details
bool_t IO_writeCANBus(void);

// See  io.cpp for details
bool_t IO_readPAP(void);

// See  io.cpp for details
bool_t IO_readGPIO(void);

// See io.cpp for details
const char_t* IO_getReturnCodeText(RETURN_CODE_TYPE rc);

// See io.cpp for details
const char_t* IO_getPortStatusText(uint8_t portStatus);
 
