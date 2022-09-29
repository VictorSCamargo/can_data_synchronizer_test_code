#ifndef __CONTROL_IHM_H
#define __CONTROL_IHM_H

#include "ihm_mvs.h"
#include "can_mvs.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern CAN_HandleTypeDef hcan;
	
		class ControlIhm
		{
			private:
				
				uint8_t delayLeituraBotoesIhm;
				uint8_t delayAtualizaTela;
				Botbot botaoSelecionado;

				void BOTOES_MP(void);
				void BOTOES_MVG(void);
				void BOTOES_MTC(void);
				void BOTOES_MVA(void);
			
				void altera_opcao_MP(int);
				void setas_t_config_salvar(void);
				void setas_t_config_variaveis(const int, int*, int*, const int, const int);
			
			public:
				ControlIhm();
				CanMvs can;
			
				void iniciar(void);
				void programa_principal(void);
			
				void sincronizar_dados_entre_placas(void);
				CAN_MVS_struct_id pega_id_marcado_de_data(void);
				CAN_MVS_functions_id pega_id_marcado_de_func(void);
				
				void setBotaoSelecionado(Botbot);
				void setDelayLeituraBotoesIhm(uint8_t);
				void setDelayAtualizaTela(uint8_t);

				uint8_t getDelayLeituraBotoesIhm(void);
				uint8_t getDelayAtualizaTela(void);
			
				void leituraBotoesIhm(void);
				void atualizaTela(void);
			
				void alarme(int);
				
			
		};
	
	
	
#ifdef __cplusplus
}
#endif		
		
			 		 
	 


#endif
