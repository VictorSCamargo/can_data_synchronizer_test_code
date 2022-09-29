#include "control_ihm.h"

typedef struct
{
	uint8_t v1;
	float v2;
} struct_tipo_1;

typedef struct
{
	float v1;
	int v2;
} struct_tipo_2;

enum can_test_type{
	CAN_TEST_SEND_DATA = 0,
	CAN_TEST_EXEC_FUNCTION,
	CAN_TEST_REQUEST_DATA,
	CAN_TEST_REQUEST_FUNC_EXEC,
	CAN_TEST_MAX
};

struct_tipo_1 struct_exemplo_1;
struct_tipo_2 struct_exemplo_2;

/*	Variaveis diversas	*/

int8_t opcao_can = 0;

/*	Construtor	*/

ControlIhm::ControlIhm()
{
	delayLeituraBotoesIhm = 0;
	delayAtualizaTela = 0;
	botaoSelecionado = tNenhum;
}

void ControlIhm::iniciar(void)
{
	can.init(&hcan);
	
	#ifdef MASTER_BOARD
	struct_exemplo_1.v1 = 7;
	struct_exemplo_1.v2 = 15.2;
	struct_exemplo_2.v1 = 0;
	struct_exemplo_2.v2 = 0;
	#else
	struct_exemplo_1.v1 = 13;
	struct_exemplo_1.v2 = 2.25;
	struct_exemplo_2.v1 = 55.2;
	struct_exemplo_2.v2 = -2;
	#endif
	
	can.connect_struct_to_id(ID_STRUCT_EXEMPLO_1, (uint8_t*)&struct_exemplo_1, sizeof(struct_exemplo_1));
	can.connect_struct_to_id(ID_STRUCT_EXEMPLO_2, (uint8_t*)&struct_exemplo_2, sizeof(struct_exemplo_2));
	
}

void ControlIhm::programa_principal(void)
{
	leituraBotoesIhm();
	sincronizar_dados_entre_placas();
}

void ControlIhm::sincronizar_dados_entre_placas(void)
{
	can.shipping_application();

	if(can.is_there_any_data_id_flag_marked(CAN_FLG_TO_BE_PROCESSED))
	{
		/* Dentro fica a logica de prioridade de qual dado sera enviado */
		CAN_MVS_struct_id a_ser_enviada = pega_id_marcado_de_data();
		
		if(can.start_shipping_data(a_ser_enviada) == CAN_MVS_OK)
		{
			can.unmark_data_id_flag(CAN_FLG_TO_BE_PROCESSED, a_ser_enviada);
		}
	}

	while(can.is_there_any_func_id_flag_marked(CAN_FLG_TO_BE_PROCESSED))
	{
		CAN_MVS_functions_id func_id = pega_id_marcado_de_func();
		
		switch(func_id)
		{
			case ID_FNC_COMECAR_AQUISICAO:
				break;

			case ID_FNC_FINALIZAR_AQUISICAO:
				break;

			case ID_FNC_SETAR_NOVA_HORA_RTC:
				break;

			default:
				break;
		}
		can.unmark_func_id_flag(CAN_FLG_TO_BE_PROCESSED, func_id);
	}	
}

CAN_MVS_struct_id ControlIhm::pega_id_marcado_de_data(void)
{
	/* Decidi enviar por ordem de prioridade */
	for(uint8_t id = 0; id < ID_STRUCT_MAX; id++)
	{
		if(can.is_data_id_flag_marked(CAN_FLG_TO_BE_PROCESSED, (CAN_MVS_struct_id)id))
		{
			return (CAN_MVS_struct_id)id;
		}
	}
	return ID_STRUCT_MAX;
}

CAN_MVS_functions_id ControlIhm::pega_id_marcado_de_func(void)
{
	/* Decidi enviar por ordem de prioridade */
	for(uint8_t id = 0; id < ID_FNC_MAX; id++)
	{
		if(can.is_func_id_flag_marked(CAN_FLG_TO_BE_PROCESSED, (CAN_MVS_functions_id)id))
		{
			return (CAN_MVS_functions_id)id;
		}
	}
	return ID_FNC_MAX;
}

/*	Setters	*/

void ControlIhm::setBotaoSelecionado(Botbot botao){
	if (delayLeituraBotoesIhm == 0){
		botaoSelecionado = botao;
	}
}
void ControlIhm::setDelayLeituraBotoesIhm(uint8_t d){
	delayLeituraBotoesIhm = d;
}
void ControlIhm::setDelayAtualizaTela(uint8_t d){
	delayAtualizaTela = d;
}

/*	Getters	*/

uint8_t ControlIhm::getDelayLeituraBotoesIhm(void){
	return delayLeituraBotoesIhm;
}
uint8_t ControlIhm::getDelayAtualizaTela(void){
	return delayAtualizaTela;
}

/*	Outros	*/

void ControlIhm::leituraBotoesIhm(void){

	// Entra se algum botão for apertado
	if (botaoSelecionado != tNenhum){
		
		if(botaoSelecionado == tBotaoEntrar)
		{
			
			#ifdef MASTER_BOARD
			switch(opcao_can)
			{
				case 0:
					can.mark_data_id_flag(CAN_FLG_TO_BE_PROCESSED, ID_STRUCT_EXEMPLO_1);
					break;
				case 1:
					can.mark_data_id_flag(CAN_FLG_TO_BE_REQUESTED, ID_STRUCT_EXEMPLO_2);
					break;
				default:
					opcao_can = -1;
			}
			if(++opcao_can == 2)
			{
				opcao_can = 0;
			}
			#endif
		}
		
		botaoSelecionado = tNenhum;
		delayLeituraBotoesIhm = 1;
	}
}

void ControlIhm::atualizaTela(void){
	
	if (delayAtualizaTela == 0){

		delayAtualizaTela = 5;
	}
}
