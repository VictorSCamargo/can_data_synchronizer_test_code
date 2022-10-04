#ifndef __CONTROL_IHM_H
#define __CONTROL_IHM_H

#include "can_mvs.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum{
		BUTTON_NONE = 0,
		BUTTON_1,
		BUTTON_MAX
	} buttons;
	
	typedef enum {
		CAN_TEST_SEND_DATA = 0,
		CAN_TEST_REQUEST_DATA,
		CAN_TEST_EXEC_FUNCTION,
		CAN_TEST_REQUEST_FUNC_EXEC,
		CAN_TEST_MAX
	} can_test_type;
	
	typedef struct
	{
		uint8_t v1;
		float v2;
	} struct_type_1;

	typedef struct
	{
		float v1;
		int v2;
	} struct_type_2;
	
	class ControlIhm
	{
		private:
			CanMvs can;
		
			struct_type_1 data_1;
			struct_type_2 data_2;
		
			uint8_t test_option;

			uint8_t delay_button_read;
			buttons pressed_button;

			void can_data_sync_program(void);
			void process_read_button(void);

			CAN_MVS_data_id get_next_data_id_to_be_sent(void);
			CAN_MVS_functions_id get_next_func_id_to_be_exec(void);
		
		public:
			ControlIhm();

			void init(CAN_HandleTypeDef*);
			void main_program(void);
		
			void can_receive_data_callback(void);

			void gpio_button_callback(buttons);
			void timer_2_callback(void);
	};

#ifdef __cplusplus
}
#endif

#endif
