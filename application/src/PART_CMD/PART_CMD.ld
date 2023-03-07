/**
 * @file PART_CMD.ld
 *
 * @brief Linker file for the PART_CMD.
 *
 */
/*=============================================================================
 *
 * THIS FILE CONTAINS CMC ELECTRONICS PROPRIETARY INFORMATION AND SHALL
 * NOT BE COPIED OR DISTRIBUTED TO THIRD PARTIES WITHOUT THE PRIOR WRITTEN
 * PERMISSION OF CMC ELECTRONICS INC.
 *
 *=============================================================================
 *
 */

OPTION ("-map")
OPTION ("-Mn")


-sec
{
// text/readonly segment
	.picbase	0x10000 	:
	.text				:
	.syscall 			:
	.intercall			:
	.interfunc			:
	.secinfo 			:
	.rodata 			:
	.ROM.data 		ROM(.data)		:
	.ROM.sdabase 	ROM(.sdabase)	:
	.ROM.sdata 		ROM(.sdata)		:
	.sdata2 			:
	.fixaddr 			:
	.fixtype 			:
// .textchecksum MUST be the last section of the text/readonly segment
	.textchecksum			:

// data/readwrite segment
	.pidbase	align(0x4000)	:
	.data 				:
	.sdabase 			:
	.sdata 				:
// .datachecksum MUST be the last section of the data portion of the 
// data/read/write segment (i.e., it must come before the BSS sections).
	.datachecksum			:
	.sbss 				:
	.bss 				:

	.heap	align(16) pad(0x1000000) 	:
}
