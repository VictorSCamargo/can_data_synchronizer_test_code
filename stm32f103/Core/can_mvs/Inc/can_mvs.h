/**
	******************************************************************************
	* @file    can_mvs.h
	* @author  Victor S. Camargo
	* @brief   CanMvs header file.
	******************************************************************************

	[English]:
	
	Class created for the specific application of syncronize data structs known
	between two STM32 microcontrolers for a project.
	This class is designed to be used with HAL libraries.

	How does it works: 
	- The sending device generates a copy of the struct that you wish to send and
	splits it into packages (since CAN BUS protocol can only send 8 bytes of data per
	shipment. This algorythm uses the first byte as a type of data identifier for the
	destiny device. Also, the first package sent is a header that contains a identifier to
	the target struct.
	- The receiving device unloads the package of data to a space in the memory. When all
	data is received, it will overwrite the target struct.

	To use this class, you should:
	- Configure and enable the HAL's CAN device in the CubeMX software;
	- Configure what is needed in the @file can_mvs_config.h;
	- Add the needed IDs in the @file can_mvs_config.h for the structs you would like to synchronize;
	- Give the adress of the main's @var hcan to the @method init;
	- Associate each ID to the desired struct with the @method connect_struct_to_id;
	- Run the @method receive_data_callback in the main's declaration of HAL_CAN_RxFifo1MsgPendingCallback;
	- Run the @method shipping_application in the main's while loop or how you would like.
	
	Then, use the @method start_shipping_data as necessary to send the struct by the ID.


	[Portugues]:

	Classe criada pensando na aplicacao especifica de enviar structs conhecidas entre
	dois microcontroladores STM32, a fim de mantê-los com os dados sincronizados.
	Classe feita para operar com bibliotecas HAL.

	Como funciona:
	- O dispositivo de envio gera uma cópia da estrutura que você deseja enviar e
	divide em pacotes (já que o protocolo CAN BUS só pode enviar 8 bytes de dados por
	envio. Este algoritmo usa o primeiro byte como um tipo de identificador de dados para o
	dispositivo de destino. Além disso, o primeiro pacote enviado é um cabeçalho que contém um
	identificador para a estrutura de destino.
	- O dispositivo receptor descarrega o pacote de dados para um espaço na memória. Quando todos
	dados são recebidos, os dados vão sobrepor a estrutura de destino.

	Para usar essa classe, faca ambos nas duas placas controladoras:
	- Configurar e habilitar o HAL_CAN pelo CubeMX, com a velocidade e filtros corretamente configurados;
	- Configurar o que for necessario na @file can_mvs_config.h;
	- Adicionar os IDs no @file can_mvs_config.h para as estruturas que deseja sincronizar;
	- Passar o endereco do @var hcan declarado na @file main para o @method init;
	- Associar cada ID a sua struct pelo @method connect_struct_to_id;
	- Rodar o @method receive_data_callback na declaracao do HAL_CAN_RxFifo1MsgPendingCallback;
	- Rodar o @method shipping_application no loop da main ou como preferir;

	Entao, utilize o @method start_shipping_data conforme necessario para mandar a estrutura desejada.
*/

#ifndef __CAN_MVS_H
#define __CAN_MVS_H

#include <stdlib.h>
#include <string.h>

/* open and edit this one */
#include "can_mvs_config.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
	typedef enum
	{
		CAN_MVS_OK = 0,
		CAN_MVS_ERROR,
		CAN_MVS_BUSY,
		CAN_MVS_TIMEOUT,
		CAN_MVS_NULL_POINTER,
		CAN_MVS_INVALID_ID
	} CAN_MVS_status;
	
	typedef enum
	{
		CAN_DATA_ID_STANDARD = 0,
		CAN_DATA_ID_HEADER,
		CAN_DATA_ID_ASK_FOR_DATA,
		CAN_DATA_ID_HAVE_A_FUNCTION_RUN
	} CAN_data_type;
	
	typedef enum
	{
		CAN_FLG_TO_BE_PROCESSED = 0,
		CAN_FLG_TO_BE_REQUESTED,
		CAN_FLG_MAX
	} CAN_flags_type;

	class CanMvs
	{
	public:
		CanMvs();

		void init(CAN_HandleTypeDef*);

		CAN_MVS_status connect_struct_to_id(CAN_MVS_struct_id, uint8_t*, uint8_t);

		CAN_MVS_status start_shipping_data(CAN_MVS_struct_id);

		CAN_MVS_status shipping_application(void);

		void receive_data_callback(void);

		bool is_there_any_data_id_flag_marked(CAN_flags_type);
		bool is_data_id_flag_marked(CAN_flags_type, CAN_MVS_struct_id);
		void mark_data_id_flag(CAN_flags_type, CAN_MVS_struct_id);
		void unmark_data_id_flag(CAN_flags_type, CAN_MVS_struct_id);
		void clear_pending_data_flags(CAN_flags_type);

		bool is_there_any_func_id_flag_marked(CAN_flags_type);
		bool is_func_id_flag_marked(CAN_flags_type, CAN_MVS_functions_id);
		void mark_func_id_flag(CAN_flags_type, CAN_MVS_functions_id);
		void unmark_func_id_flag(CAN_flags_type, CAN_MVS_functions_id);
		void clear_pending_func_flags(CAN_flags_type);

	private:

		CAN_HandleTypeDef *ptr_hcan;

		CAN_TxHeaderTypeDef TxHeader;
		CAN_RxHeaderTypeDef RxHeader;

		uint8_t TxData[8];
		uint8_t RxData[8];
		uint32_t TxMailbox;

		uint32_t flags_data_requisition[CAN_FLG_MAX];
		uint32_t flags_func_requisition[CAN_FLG_MAX];

		uint8_t count_received_bytes;
		uint8_t count_sent_bytes;

		uint8_t max_receivable_bytes;
		uint8_t max_sendable_bytes;

		CAN_MVS_struct_id id_shipping_struct;
		CAN_MVS_struct_id id_receipt_struct;

		uint8_t *ptr_struct_to_send;
		uint8_t *ptr_struct_to_receive;

		uint8_t *ptr_struct_to_send_bytes;
		uint8_t *ptr_struct_to_receive_bytes;

		uint8_t *ptr_structs[ID_STRUCT_MAX];
		uint16_t size_of_structs[ID_STRUCT_MAX];

		void set_package_data_type(CAN_data_type);
		void set_package_struct_id(CAN_MVS_struct_id);

		void prepare_package_to_be_send(CAN_data_type);
		CAN_MVS_status send_package(void);

		CAN_MVS_struct_id identify_received_struct_id(void);
		CAN_MVS_status start_receiving(CAN_MVS_struct_id);
		void process_received_package(void);
		void unload_received_package(void);
		void copy_built_struct_to_destiny(void);

		void callback_set_asked_flags(CAN_data_type);

		bool sent_header;
		bool is_HAL_CAN_busy(void);
		bool was_all_pending_data_sent(void);
		bool was_all_pending_data_received(void);
		bool is_struct_id_valid(CAN_MVS_struct_id);
		CAN_data_type identify_received_data_type(void);
		bool am_i_receiving_this_id(CAN_MVS_struct_id);
	};
	
#ifdef __cplusplus
}
#endif

#endif
