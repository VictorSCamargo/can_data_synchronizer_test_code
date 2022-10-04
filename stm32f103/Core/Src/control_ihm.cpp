#include "control_ihm.h"

#define DELAY_BUTTON_READ_MAX 3

ControlIhm::ControlIhm()
{
	delay_button_read = 0;
	test_option = 0;
	pressed_button = BUTTON_NONE;
}

void ControlIhm::init(CAN_HandleTypeDef* hcan)
{
	can.init(hcan);
	
	//example values
	#ifdef MVS_CAN_BOARD_1
	data_1.v1 = 2;
	data_1.v2 = 1.5;
	data_2.v1 = 0;
	data_2.v2 = 0;
	#else
	data_1.v1 = 0;
	data_1.v2 = 0;
	data_2.v1 = 30.4;
	data_2.v2 = -5;
	#endif
	
	CAN_status status = CAN_STATUS_OK;
	
	status = can.connect_struct_to_id(ID_DATA_EX_1, (uint8_t*)&data_1, sizeof(data_1));
	status = can.connect_struct_to_id(ID_DATA_EX_2, (uint8_t*)&data_2, sizeof(data_2));
	
	if(status != CAN_STATUS_OK)
	{
		while(true){}
	}
}

void ControlIhm::main_program(void)
{
	process_read_button();
	can_data_sync_program();
}

void ControlIhm::can_receive_data_callback(void)
{
	can.receive_data_callback();
}

void ControlIhm::can_data_sync_program(void)
{
	can.shipping_application();

	if(can.is_there_any_data_id_flag_marked(CAN_MARK_TO_BE_PROCESSED))
	{
		CAN_MVS_data_id data_id_to_be_sent = get_next_data_id_to_be_sent();
		
		if(can.start_shipping_data(data_id_to_be_sent) == CAN_STATUS_OK)
		{
			can.unmark_data_id_flag(CAN_MARK_TO_BE_PROCESSED, data_id_to_be_sent);
		}
	}

	while(can.is_there_any_func_id_flag_marked(CAN_MARK_TO_BE_PROCESSED))
	{
		CAN_MVS_functions_id func_id_to_be_exec = get_next_func_id_to_be_exec();
		
		switch(func_id_to_be_exec)
		{
			case ID_FNC_ADD_1_TO_DATA_1:
				data_1.v1++;
				data_1.v2++;
				break;

			case ID_FNC_ADD_1_TO_DATA_2:
				data_2.v1++;
				data_2.v2++;
				break;

			default:
				break;
		}
		can.unmark_func_id_flag(CAN_MARK_TO_BE_PROCESSED, func_id_to_be_exec);
	}	
}

CAN_MVS_data_id ControlIhm::get_next_data_id_to_be_sent(void)
{
	/* The priority of this algorythm is by ID position */
	for(uint8_t id = 0; id < ID_DATA_MAX; id++)
	{
		if(can.is_data_id_flag_marked(CAN_MARK_TO_BE_PROCESSED, (CAN_MVS_data_id)id))
		{
			return (CAN_MVS_data_id)id;
		}
	}
	return ID_DATA_MAX;
}

CAN_MVS_functions_id ControlIhm::get_next_func_id_to_be_exec(void)
{
	/* The priority of this algorythm is by ID position */
	for(uint8_t id = 0; id < ID_FNC_MAX; id++)
	{
		if(can.is_func_id_flag_marked(CAN_MARK_TO_BE_PROCESSED, (CAN_MVS_functions_id)id))
		{
			return (CAN_MVS_functions_id)id;
		}
	}
	return ID_FNC_MAX;
}

void ControlIhm::process_read_button(void){

	if (pressed_button != BUTTON_NONE)
	{
		
		if(pressed_button == BUTTON_1)
		{
			#ifndef CAN_MVS_LOOPBACK_MODE

			switch(test_option)
			{
				case CAN_TEST_SEND_DATA:
					can.mark_data_id_flag(CAN_MARK_TO_BE_PROCESSED, ID_DATA_EX_1);
					break;

				case CAN_TEST_REQUEST_DATA:
					can.mark_data_id_flag(CAN_MARK_TO_BE_REQUESTED, ID_DATA_EX_2);
					break;
				
				case CAN_TEST_EXEC_FUNCTION:
					can.mark_func_id_flag(CAN_MARK_TO_BE_PROCESSED, ID_FNC_ADD_1_TO_DATA_1);
					break;
				
				case CAN_TEST_REQUEST_FUNC_EXEC:
					can.mark_func_id_flag(CAN_MARK_TO_BE_REQUESTED, ID_FNC_ADD_1_TO_DATA_2);
					break;

				default:
					break;
			}

			if(++test_option == CAN_TEST_MAX)
			{
				test_option = 0;
			}
			
			#else
			
				can.start_shipping_data(ID_DATA_MAX);
			
			#endif
		}
		
		pressed_button = BUTTON_NONE;
		delay_button_read = 5;
	}
}

void ControlIhm::gpio_button_callback(buttons botao)
{
	if (delay_button_read == 0)
	{
		pressed_button = botao;
		delay_button_read = DELAY_BUTTON_READ_MAX;
	}
}

void ControlIhm::timer_2_callback(void)
{
	if (delay_button_read > 0)
	{
		delay_button_read--;
	}
}
