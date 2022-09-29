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

/* ADAPTADO PARA PLACA STM32F103C8T6 */
	 
/* include the HAL for your board model */
#include "stm32f1xx_hal.h"

/* to debug with only one STM32 board */
//#define CAN_MVS_LOOPBACK_MODE

#define MASTER_BOARD
//#define SLAVE_BOARD
	 
/* to use different CAN pins */
//#define PINOS_CAN_A11_A12

#if defined(MASTER_BOARD) + defined(SLAVE_BOARD) == 1
	#error "Select between MASTER OR SLAVE."
#endif

/* this should be the ID that the other device's CAN filter was configured to receive*/
#ifdef MASTER_BOARD
	#define CAN_MVS_SENDER_ID (0x103<<5)
#else
	#define CAN_MVS_SENDER_ID (0x407<<5)
#endif

/* the device will receive messages that have this ID */
#ifdef MASTER_BOARD
	#define CAN_MVS_FILTER_ID (0x407<<5)
#else
	#define CAN_MVS_FILTER_ID (0x103<<5)
#endif

#ifdef CAN_MVS_LOOPBACK_MODE
	#define HAL_CAN_FILTER_ID CAN_MVS_SENDER_ID //should be the same in the loopback mode
#else
	#define HAL_CAN_FILTER_ID CAN_MVS_FILTER_ID
#endif

/**
  * Add the IDs for your structs in this enum.
  * - first ID should be equal to zero.
  * - do not remove ID_STRUCT_MAX.
  */
typedef enum
{
	/*	ATENCAO com a ordem das variaveis! quanto menor, maior a prioridade de envio.	*/
	ID_STRUCT_EXEMPLO_1 = 0,
	ID_STRUCT_EXEMPLO_2,
	ID_STRUCT_MAX
} CAN_MVS_struct_id;

typedef enum
{
	ID_FNC_COMECAR_AQUISICAO = 0,
	ID_FNC_FINALIZAR_AQUISICAO,
	ID_FNC_SETAR_NOVA_HORA_RTC,
	ID_FNC_MAX
} CAN_MVS_functions_id;

#ifdef __cplusplus
}
#endif

#endif
