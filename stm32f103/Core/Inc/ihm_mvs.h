#ifndef __IHM_MVS_H__
	#define __IHM_MVS_H__
	
		// C header
	#ifdef __cplusplus  // Insert start of extern C construct
	extern "C" {
	#endif
		#include "tm_stm32_hd44780.h" //mostrar no LCD
		#include "usb_host.h"
		#include <string.h> //snprintf
	#ifdef __cplusplus  // Insert end of extern C construct.
	}                   // The C header file can now be
	#endif              // included in either C or C++ code.

	#ifdef DOIS_SENSORES_T
	 extern mtcclass mtc2;	//
	#endif
	
		/*-------Telas dos programas-------*/
				
			typedef enum{
				t_mp = 0,				//Tela Menu Principal - enum incia em 3 pois t_mp tem 4 opcoes
				t_inicia_salvar,
				t_config_MP,		//Tela Configuração Menu Principal
				t_config_salvar,
				t_config_alarmes,
				//t_hora_data,
				t_config_tempo,
				t_config_wifi,
				t_max_MP,
				t_alarmes
			}telas_MP;  			//Telas Menu Principal
			
			typedef enum{
				t_vel = 0,
				t_prodInst,
				t_consArame,
				t_prodGlobal,
				t_config_MVA,
				t_max_MVA
			}telas_MVA;				//Telas MVA
			
			typedef enum{
				t_tempoArcoAberto = 0,
				t_salvarDadosConfigMVA,
				//t_apagarDadosConfigMVA,
				t_material,
				t_bitola,
				t_inverteSentido,
				t_inverteSentidoAlarme,
				t_trocaEncoder,
				t_zerarVariaveisMVA,
				t_max_MVA_config
			}telas_MVA_config;
			
			typedef enum{
				t_vazInst = 0,
				t_acumAtual,
				t_acumTotal,
				t_config_MVG,
				t_max_MVG
			}telas_MVG;				//Telas MVG
			
			typedef enum{
				t_tipoGas = 0,
				t_vazaoMax,
				t_vazaoMin,
				t_zerarVariaveisMVG,
				t_salvarDadosConfigMVG,
				//t_apagarDadosConfigMVG,
				t_max_MVG_config
			}telas_MVG_config;
			
			typedef enum{
				t_tensao = 0,
				t_corrente,
				t_config_MTC,
				t_max_MTC
			}telas_MTC;				//Telas MTC
			
			typedef enum{
				t_correnteMax = 0,
				t_correnteMin,
				t_tensaoMax,
				t_tensaoMin,
				t_salvarDadosConfigMTC,
				t_max_MTC_config
			}telas_MTC_config;
			
			typedef enum{
				select_MP = 0,
				select_MVG,
				select_MTC,
				select_MVA,
			}tela_selecionada;
			
	/*-------Botões IHM--------*/		
			
			typedef enum {
				tBotaoVoltar = 0,
				tBotaoSalvar,
				tBotaoEntrar,
				tSentidoHorario,
				tSentidoAntiHorario,
				tNenhum
			}Botbot;
			
	/*---Taxas de aquisição---*/
			
			enum FREQ_AQUISICAO{
				freq_off = 0,
				freq_10Hz = 1000,
				freq_20Hz = 500,
				freq_50Hz = 200,
				freq_100Hz = 100,
				freq_500Hz = 20,
				freq_1KHz = 10,
				freq_2KHz = 5,
				freq_5KHz = 2,
				freq_10KHz = 1
			};
			
	/*---Impressoes especiais---*/
			
			typedef enum{
				IMPRIMIR_NORMAL = 0,
				IMPRIMIR_NENHUM_PROGRAMA_SELECIONADO,
				IMPRIMIR_ALERTA_SEM_SD_USB,
				IMPRIMIR_SALVAMENTO_INICIO,
				IMPRIMIR_SALVAMENTO_FIM
			}opcoes_impressao;
			
	/*------Config IHM-----Flags de controle -  alarme e amarzenamento de dados*/

			enum partes_eeprom{
				opcao_eeprom_mva,
				opcao_eeprom_mvg,
				opcao_eeprom_mtc,
				opcao_eeprom_mp,
				opcao_eeprom_sd,
				opcao_eeprom_ihm,
				opcao_eeprom_wifi
			};
			
			typedef struct {
				bool flagSalvarMVG;
				bool flagSalvarTempo;
				bool flagSalvamentoDinamico;
				bool flag_habilita_buzzer;
			} structIHM_config;

			typedef struct {
				uint8_t salvar_eeprom;
				bool iniciar_aquisicao;
				bool salvar_hora_no_rtc;
			} structIHM_flags;


	class ihm_mvs
	{
		private:
			telas_MP tela_MP;

			int ajustar_rtc;
			bool flag_seta_mp;
			int telaswifi;
			bool flag_config_salvar;
			uint8_t data_hora;
		
			void BOTOES_MP(Botbot);
			void BOTOES_MVG(Botbot);
			void BOTOES_MTC(Botbot);
			void BOTOES_MVA(Botbot);

		  structIHM_config config_IHM;

			int tela_MVA;
			int tela_MVG;
			int tela_MTC;
			int tela_MVA_config;
			int tela_MVG_config;
			int tela_MTC_config;

			tela_selecionada programa_atual;
			int mudaOpcoes;
			int retornaWifi;

			int8_t seta_1;
			int8_t seta_2;
			int8_t seta_anterior_2;
			int8_t seta_3;
			int8_t seta_anterior_3;

			int8_t opcao_linha_MP[4];
			const char* valores_opcoes_MP(int);

			bool flagHabilitaTelasConfig;				//Flag para habilitar telas de configurações MVG, MVA e futuramente MTC
			bool flagHabilitaAlterarConfig;			//Controle para alterar configuracoes MVA, MVG e MTC
			bool flagHabilitaSalvarDados_USB;
			bool flagDelayAlerta;

			void setas_t_config_salvar(void);
			void setas_t_config_variaveis(const int, int*, int*, const int, const int);
			
			void ajuste_config_data_hora(void);

		public:
			
			ihm_mvs();
		
			structIHM_flags flags_IHM;
		
			bool sincronizar_dados;

			void inicializacao(void);

			bool aquisitar_dados;

			void set_flag_iniciar_aquisicao(bool);
			void set_flag_salvar_eeprom(int);
			void set_flag_salvar_eeprom_all(void);

			bool get_flag_iniciar_aquisicao(void);
			uint8_t get_flag_salvar_eeprom(void);
			void limpar_flag_salvar_eeprom(int);

			void setFlagHabilitaDisplay(bool);
			void setFlagHabilitaSalvarDados_USB(bool);
			void setFlagDelayAlerta(bool);
			void setDefaultConfigIHM(void);

			int getFlagHabilitaSalvarDadosMVA(void);
			bool getFlagHabilitaSalvarDadosMVG(void);
			int getFlagHabilitaSalvarDadosMTC(void);
			bool getFlagHabilitaSalvarDados_USB(void);
			bool getFlagDelayAlerta(void);
			bool getFlagSalvamentoDinamico(void);
			bool getFlagHabilitaBuzzer(void);
			
			int8_t* get_opcao_linha_MP(void);
			uint8_t get_size_of_opcao_linha_MP(void);
			
			structIHM_flags* get_flags_IHM(void);
			uint8_t get_size_of_flags_IHM(void);
			
			structIHM_config* get_config_IHM(void);
			uint8_t get_size_of_config_IHM(void);

			int get_num_opcao_1_MP(void);
			int get_num_opcao_2_MP(void);
			int get_num_opcao_3_MP(void);
			int get_num_opcao_4_MP(void);
			
			const char* get_dados_opcao_1_MP(void);
			const char* get_dados_opcao_2_MP(void);
			const char* get_dados_opcao_3_MP(void);
			const char* get_dados_opcao_4_MP(void);
			
			const char* get_nome_opcao_1_MP(void);
			const char* get_nome_opcao_2_MP(void);
			const char* get_nome_opcao_3_MP(void);
			const char* get_nome_opcao_4_MP(void);
			
			const char* get_unidade_opcao_1_MP(void);
			const char* get_unidade_opcao_2_MP(void);
			const char* get_unidade_opcao_3_MP(void);
			const char* get_unidade_opcao_4_MP(void);

			const char* getDadosOpcao_MP(int);

			tela_selecionada getTelaMVS( void);

			//atualiza telas
			void atualiza_tela_MP( void);
			void atualiza_tela_MVA( void);
			void atualiza_tela_MVG( void);
			void atualiza_tela_MTC( void);

			void limpar_tela( void);
			void imprimir(uint8_t, uint8_t, const char*);
			void mostra_linha_selecionada( void);
			
			void carregar_dados(void);
			
			void alarme(int);
			
			void atualiza_tela(void);
			void imprime_opcao_especial(opcoes_impressao);

			void processa_botao(Botbot);

			void altera_opcao_MP(int);			
		
	};
	
#endif //__IHM_MVS_H__
