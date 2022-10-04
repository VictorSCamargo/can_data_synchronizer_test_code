/**
  ******************************************************************************
  * @file    can_mvs_config.h
  * @brief   CanMvs configuration file.
  ******************************************************************************
  */

#ifndef __CAN_MVS_CONFIG_H
#define __CAN_MVS_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
/*----------------------------------------------*/
	 
/* include the HAL for your board model */
#include "stm32f1xx_hal.h"

/* to debug with only one STM32 board */
//#define CAN_MVS_LOOPBACK_MODE
	 
/* CUSTOM USER CODE BEGIN */

#define MVS_CAN_BOARD_1
//#define MVS_CAN_BOARD_2

/* to use different CAN pins in STM32F103C8T6 */
//#define PINOS_CAN_A11_A12

#if defined(MVS_CAN_BOARD_1) + defined(MVS_CAN_BOARD_2) != 1
	#error "Select between board 1 OR 2."
#endif

#ifdef MVS_CAN_BOARD_1
	#define CUSTOM_SENDER_ID (0x103)
	#define CUSTOM_FILTER_ID (0x407)
#else
	#define CUSTOM_SENDER_ID (0x407)
	#define CUSTOM_FILTER_ID (0x103)
#endif

/* CUSTOM USER CODE END */

/* this should be the ID that the other device's CAN filter was configured to receive*/
#define CAN_MVS_SENDER_ID CUSTOM_SENDER_ID

/* the device will receive messages that have this ID */
#define CAN_MVS_FILTER_ID CUSTOM_FILTER_ID

#ifdef CAN_MVS_LOOPBACK_MODE
	#define HAL_CAN_FILTER_ID (CAN_MVS_SENDER_ID<<5) //should be the same in the loopback mode
#else
	#define HAL_CAN_FILTER_ID (CAN_MVS_FILTER_ID<<5)
#endif


/**
  * Add the IDs for your structs in this enum.
  * - first ID should be equal to zero.
  * - do not remove ID_DATA_MAX / ID_FNC_MAX.
  */

/* Up to 32 IDs + ID_DATA_MAX */
typedef enum
{
	ID_DATA_EX_1 = 0,
	ID_DATA_EX_2,
	ID_DATA_MAX
} CAN_MVS_data_id;

/* Up to 32 IDs + ID_FNC_MAX */
typedef enum
{
	ID_FNC_ADD_1_TO_DATA_1 = 0,
	ID_FNC_ADD_1_TO_DATA_2,
	ID_FNC_MAX
} CAN_MVS_functions_id;

/*----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
