#include "ihm_mvs.h"
#include "control.h"

/*	DEFINES	*/
#define T_MAX_CONFIG_SALVAR 3	//quantidade de telas
#define T_MAX_CONFIG_WIFI 3	//quantidade de telas
#define HABILITA_OPCAO_10KHZ 0 // 1 == habilita / 0 == desabilita

#define NUM_OPCOES_SALVAR_EEPROM 7

#ifndef DOIS_SENSORES_T
	#define NUMERO_DE_VARIAVEIS_MP 12
#else
	#define NUMERO_DE_VARIAVEIS_MP 13
#endif
#define NVMP NUMERO_DE_VARIAVEIS_MP

//	Instancias	//

	mvaclass mva;
	mvgclass mvg;
	mtcclass mtc;
	sd_usb_class sd_usb;
	Wifi wifi;
	rtcclass rtc;
	
#ifdef DOIS_SENSORES_T
	mtcclass mtc2;
#endif


//	Variaveis diversas //

//char ssid[16];
//char web_ip[16];


const char* nome_opcoes_MP[] = {
	"Vaz.Gas", "Con.Gas",
	"v.Aram.", "Con.Ara", "Pro.Ins", "Pro.Glo", "Pro. AA",
	"U. Med.", "U. Efi.", "I. Med.", "I. Efi.", 
	"t. Arc.",
	"V2.Med."
};  // [0,1] - MVG; [2,3,4,5,6] e [11] - MVA; [7,8.9.10] e [12] - MTC

const char* unidades_opcoes_MP[] = {
	"l/min", "l", 
	"m/min", "m", "kg/h", "kg", "kg",
	"V", "V", "A", "A", 
	"",
	"V"
};

const char* ihm_mvs::valores_opcoes_MP(int  indice)
{
		char* valor;
	
		switch(indice){
			case 0:
				valor = mvg.getStrValorLido();
				break;
			case 1:
				valor = mvg.getStrAcumulo();
				break;
			case 2:
				valor = mva.getStrVelocMedia();
				break;
			case 3:
				valor = mva.getStrConsumo();
				break;
			case 4:
				valor = mva.getStrProdInstantanea();
				break;
			case 5:
				valor = mva.getStrProdGlobal();
				break;
			case 6:
				valor = mva.getStrProdArcoAberto();
				break;
			case 7:
				valor = mtc.getStrMediaV();
				break;
			case 8:
				valor = mtc.getStrEficazV();
				break;
			case 9:
				valor = mtc.getStrMediaI();
				break;
			case 10:
				valor = mtc.getStrEficazI();
				break;
			case 11:
				valor = mva.getStrtotalSegundosArcoAberto();
				break;
			#ifdef DOIS_SENSORES_T
			case 12:
				valor = mtc2.getStrMediaV();
				break;
			#endif
			default:
				break;
		}

		return valor;
}

//	Fim de Variaveis diversas //


ihm_mvs::ihm_mvs( void){
	
	sincronizar_dados = false;

	tela_MP = t_mp;
	tela_MVA = t_vel;
	tela_MVG = t_vazInst;
	tela_MTC = t_tensao;
	tela_MVA_config = t_tempoArcoAberto;
	tela_MVG_config = t_tipoGas;
	tela_MTC_config = t_correnteMax;
	programa_atual = select_MP;
	mudaOpcoes = 0;
	
	ajustar_rtc = 0;
	flag_seta_mp = 0;
	telaswifi = 0;
	flag_config_salvar = 0;
	data_hora = 0;
	
	seta_1 = t_mp;
	seta_2 = 0;		
	seta_anterior_2 = 0;
	seta_3 = 0;		
	seta_anterior_3 = 0;

	flagHabilitaTelasConfig = false;
	flagHabilitaAlterarConfig = false;

	aquisitar_dados = false;
	flagHabilitaSalvarDados_USB = false;
	flagDelayAlerta = false;

	flags_IHM.salvar_eeprom = 0;
	flags_IHM.iniciar_aquisicao = false;
	flags_IHM.salvar_hora_no_rtc = false;

	setDefaultConfigIHM();
}

void ihm_mvs::inicializacao(void)
{

}

void ihm_mvs::imprime_opcao_especial(opcoes_impressao opcao)
{
	limpar_tela();

	switch(opcao)
	{
		case IMPRIMIR_NENHUM_PROGRAMA_SELECIONADO:
			imprimir(3, 1, "Nenhum programa");
			imprimir(5, 2, "selecionado");
			break;
		case IMPRIMIR_ALERTA_SEM_SD_USB:
			imprimir(8, 1, "ERRO");
			imprimir(4, 2, "SEM SD OU USB");
			break;
		case IMPRIMIR_SALVAMENTO_INICIO:
			imprimir(5, 1, "Salvamento");
			imprimir(6, 2, "Iniciado");
			break;
		case IMPRIMIR_SALVAMENTO_FIM:
			imprimir(5, 1, "Salvamento");
			imprimir(7, 2, "Parado");
			break;
		default:
			break;
	}
}

void ihm_mvs::setDefaultConfigIHM(void)
{
	//Opcões default do MP
	opcao_linha_MP[0] = 0;	//Vaz.gas
	opcao_linha_MP[1] = 2;	//Veloc.
	opcao_linha_MP[2] = 7;	//V. Med.	
	opcao_linha_MP[3] = 9;	//I. Med.

	config_IHM.flagSalvarTempo = false;
	config_IHM.flagSalvarMVG = false;
	config_IHM.flagSalvamentoDinamico = false;
	config_IHM.flag_habilita_buzzer = true;
}

void ihm_mvs::set_flag_salvar_eeprom(int opcao)
{
	if((opcao >= 0) && (opcao < NUM_OPCOES_SALVAR_EEPROM))
	{
		flags_IHM.salvar_eeprom |= 1<<opcao;
	}
}

void ihm_mvs::set_flag_salvar_eeprom_all(void)
{
	flags_IHM.salvar_eeprom = 255; //(2^8)-1
}

void ihm_mvs::limpar_flag_salvar_eeprom(int opcao)
{
	if((opcao >= 0) && (opcao < NUM_OPCOES_SALVAR_EEPROM))
	{
		flags_IHM.salvar_eeprom &= ~(1<<opcao);
	}
}

uint8_t ihm_mvs::get_flag_salvar_eeprom(void){return flags_IHM.salvar_eeprom;}

void ihm_mvs::setFlagHabilitaSalvarDados_USB(bool flag){	flagHabilitaSalvarDados_USB = flag; }
void ihm_mvs::setFlagDelayAlerta(bool flag){flagDelayAlerta = flag;}
void ihm_mvs::set_flag_iniciar_aquisicao(bool x){flags_IHM.iniciar_aquisicao = x;}

bool ihm_mvs::getFlagHabilitaSalvarDadosMVG(void){return config_IHM.flagSalvarMVG; }
bool ihm_mvs::getFlagHabilitaSalvarDados_USB(void){return flagHabilitaSalvarDados_USB;}
bool ihm_mvs::getFlagDelayAlerta(void){return flagDelayAlerta;}
bool ihm_mvs::getFlagSalvamentoDinamico(void){return config_IHM.flagSalvamentoDinamico;}
bool ihm_mvs::getFlagHabilitaBuzzer(void){return config_IHM.flag_habilita_buzzer;}
bool ihm_mvs::get_flag_iniciar_aquisicao(void){return flags_IHM.iniciar_aquisicao;}

int8_t* ihm_mvs::get_opcao_linha_MP(void){return &opcao_linha_MP[0];}
uint8_t ihm_mvs::get_size_of_opcao_linha_MP(void){return sizeof(opcao_linha_MP);}

structIHM_config* ihm_mvs::get_config_IHM(void){return &config_IHM;}
uint8_t ihm_mvs::get_size_of_config_IHM(void){return sizeof(config_IHM);}

structIHM_flags* ihm_mvs::get_flags_IHM(void){return &flags_IHM;}
uint8_t ihm_mvs::get_size_of_flags_IHM(void){return sizeof(flags_IHM);}

int ihm_mvs::get_num_opcao_1_MP(void){return opcao_linha_MP[0];}
int ihm_mvs::get_num_opcao_2_MP(void){return opcao_linha_MP[1];}
int ihm_mvs::get_num_opcao_3_MP(void){return opcao_linha_MP[2];}
int ihm_mvs::get_num_opcao_4_MP(void){return opcao_linha_MP[3];}

const char* ihm_mvs::get_dados_opcao_1_MP(void){return valores_opcoes_MP(opcao_linha_MP[0]);}
const char* ihm_mvs::get_dados_opcao_2_MP(void){return valores_opcoes_MP(opcao_linha_MP[1]);}
const char* ihm_mvs::get_dados_opcao_3_MP(void){return valores_opcoes_MP(opcao_linha_MP[2]);}
const char* ihm_mvs::get_dados_opcao_4_MP(void){return valores_opcoes_MP(opcao_linha_MP[3]);}

const char* ihm_mvs::get_nome_opcao_1_MP(void){return nome_opcoes_MP[opcao_linha_MP[0]];}
const char* ihm_mvs::get_nome_opcao_2_MP(void){return nome_opcoes_MP[opcao_linha_MP[1]];}
const char* ihm_mvs::get_nome_opcao_3_MP(void){return nome_opcoes_MP[opcao_linha_MP[2]];}
const char* ihm_mvs::get_nome_opcao_4_MP(void){return nome_opcoes_MP[opcao_linha_MP[3]];}

const char* ihm_mvs::get_unidade_opcao_1_MP(void){return unidades_opcoes_MP[opcao_linha_MP[0]];}
const char* ihm_mvs::get_unidade_opcao_2_MP(void){return unidades_opcoes_MP[opcao_linha_MP[1]];}
const char* ihm_mvs::get_unidade_opcao_3_MP(void){return unidades_opcoes_MP[opcao_linha_MP[2]];}
const char* ihm_mvs::get_unidade_opcao_4_MP(void){return unidades_opcoes_MP[opcao_linha_MP[3]];}

tela_selecionada ihm_mvs::getTelaMVS( void){ return programa_atual;}

void ihm_mvs::limpar_tela( void){
	TM_HD44780_Clear();
}

void ihm_mvs::imprimir(uint8_t x, uint8_t y, const char* str){
	TM_HD44780_Puts(x, y, str);
}


void ihm_mvs::processa_botao(Botbot botao_selecionado){
	
	switch(programa_atual)
	{
		case  select_MP:
			BOTOES_MP(botao_selecionado);
			break;
		case  select_MVG:
			BOTOES_MVG(botao_selecionado);
			break; 
		case select_MTC:
			BOTOES_MTC(botao_selecionado);
			break;
		case select_MVA:
			BOTOES_MVA(botao_selecionado);
			break;
		default:
			break;
	}

	atualiza_tela();
}
 
void ihm_mvs::atualiza_tela(void){
	
	limpar_tela();
	
	switch (programa_atual)
	{
		case select_MP:
			atualiza_tela_MP();										
			break;
			
		case select_MVG:
			atualiza_tela_MVG();				
			break;
				
		case select_MTC:		
			atualiza_tela_MTC();
			break;
			
		case select_MVA:
			atualiza_tela_MVA();				
			break;
			
		default:
			break;
	}
}


void ihm_mvs::atualiza_tela_MP(void){	// MVS- Tela inicial
	
	char text[20];	// Texto da opcao do menu
	
//		//Verifica se dados salvos na EEPROM nao estao corrompidos
//	for(int i = 0; i < 4; i++){
//		if(opcao_linha_MP[i] < 0 | opcao_linha_MP[i] > 10)
//			opcao_linha_MP[i] = 0;		//Vaz.gas
//}

	// TELA MENU PRINCIPAL ----------------------------------------------------------------------------
		if (tela_MP == t_mp){

			if(seta_1 == t_mp){
				
				if(flag_seta_mp == 1)
				{
					imprimir(0, seta_2, ">");	// Seta seleção programa MP

					if(seta_2 != seta_anterior_2)
					{
						imprimir(0, seta_anterior_2, " ");	// Apaga posição anterior da seta
					}
				}
				
				// Preenche as quatro linhas do menu principal de acordo com as opcoes selecionadas
				for(int linha = 0; linha < 4; linha++){

					uint8_t opcao_selecionada = opcao_linha_MP[linha];

					// Nome da variavel, Valor da variavel, unidade da variavel	
					snprintf(
						text, 
						20,
						"%s: %s %s",
						nome_opcoes_MP[opcao_selecionada],
						valores_opcoes_MP(opcao_selecionada),
						unidades_opcoes_MP[opcao_selecionada]
					);

					imprimir(1, linha, text);

					//Escreve sinal de alerta (#) na linha correspondente ao programa com alarme ativo 
					if (flagDelayAlerta == true) 
					{
						if (mvg.getFlagHabilitaAlarmeMVG())
						{
							if ( (opcao_selecionada == 0) || (opcao_selecionada == 1) )
								if( mvg.alarme() != 0 )
								{
									imprimir(9, linha, "#");
								}
						}
						
						//if (mva.getFlagHabilitaAlarmeMVA())
							//if ( (opcao_linha_MP[linha] >= 2) && (opcao_linha_MP[linha] <= 6) )

						if (mtc.getFlagHabilitaAlarmeMTC())
						{
							if ( (opcao_selecionada == 7) || (opcao_selecionada == 8) )
							{
								if (mtc.alarme_T() != 0) 
								{
									imprimir(9, linha, "#");
								}
							}
							
							if ((opcao_selecionada == 9) | (opcao_selecionada == 10))
							{
								if (mtc.alarme_I() != 0)
								{
									imprimir(9, linha, "#");
								}
							}
						}
						flagDelayAlerta = false;
					}
					else
					{
						flagDelayAlerta = true;
					}
				}

			}else
			
			if(seta_1 == t_inicia_salvar){

				imprimir(1, 0, "Aquisicao de dados");
				
				imprimir(0, 2, "Modo:");
				if(getFlagSalvamentoDinamico() == true){
					imprimir(6, 2, "Dinamico");
				}else{
					imprimir(6, 2, "Monitoramento");
				}

				imprimir(16, 3, "n: ");
				imprimir(18, 3, sd_usb.getNumArquivo());

				if(aquisitar_dados == false){

					if(sd_usb.get_status_SD_USB() == SD_SELECIONADO) imprimir(0, 3, "MEM: SD");
					if(sd_usb.get_status_SD_USB() == USB_SELECIONADO) imprimir(0, 3, "MEM: USB");
					if(sd_usb.get_status_SD_USB() == NENHUM_SELECIONADO) imprimir(0, 3, "MEM: NENHUMA");
				}else{
					imprimir(0, 1, "Aquisitando...");
					imprimir(0, 3, "Tempo: ");
					imprimir(7, 3, mva.getStrClock());
				}

			}else
			
			if(seta_1 == t_config_MP)
			{
				imprimir(2, 1, "Configurar opcoes");
				imprimir(3, 2, "menu principal");
			}else
			
			if(seta_1 == t_config_salvar)
			{
				imprimir(5, 1, "Configurar");
				imprimir(1, 2, "salvamento de dados");
//					if(sd_usb.get_status_SD_USB() == 0x00) {imprimir(9, 2, "SD"); imprimir(0, 3, sd_usb.getSDfreeSize());}
//					if(sd_usb.get_status_SD_USB() == 0x01){ imprimir(8, 2, "USB"); imprimir(0, 3, sd_usb.getUSBfreeSize());}
			}else
			
			if(seta_1 == t_config_alarmes)
			{
				imprimir(5, 1, "Configurar");
				imprimir(6, 2, "alarmes");
			}else
			
			if(seta_1 == t_config_tempo)
			{
				if (!rtc.is_connected()){
						imprimir(2, 3, "erro rtc externo");	// Testa a comunicacao com o rtc
				}
				
				if(!flags_IHM.salvar_hora_no_rtc)
				{
					imprimir(5, 1, rtc.get_str_data(true, RTC_ARRAY_DISPLAY));	// Mostra em tela a string
					imprimir(6, 2, rtc.get_str_hora(true, RTC_ARRAY_DISPLAY));	// Mostra em tela a string
				}
			}else
			
			if(seta_1 == t_config_wifi){
				imprimir(8, 1, "Wifi");
			}
			
		}else
		
		if (tela_MP == t_config_MP /*&& seta_2 < 5*/){
			imprimir(0, seta_anterior_2, " ");	// Apaga posição anterior da seta
			imprimir(0, seta_2, ">");	// Seta seleção programa
			
			if(mudaOpcoes == 1)
				imprimir(19, seta_2, "*");
			else
				imprimir(19, seta_2, " ");

			imprimir(1, 0, "Opcao 1:");
			imprimir(10, 0, nome_opcoes_MP[opcao_linha_MP[0]]);
			imprimir(1, 1, "Opcao 2:");
			imprimir(10, 1, nome_opcoes_MP[opcao_linha_MP[1]]);
			imprimir(1, 2, "Opcao 3:");
			imprimir(10, 2, nome_opcoes_MP[opcao_linha_MP[2]]);
			imprimir(1, 3, "Opcao 4:");
			imprimir(10, 3, nome_opcoes_MP[opcao_linha_MP[3]]);
		}else
		
		if (tela_MP == t_config_salvar){
			
			if(seta_2 == 1){
				
				limpar_tela();
				if( flag_config_salvar == 1){
					imprimir(0, seta_3, ">");			// Seta seleção programa
					imprimir(0, seta_anterior_3, " ");	// Apaga posição anterior da seta
				}
				
				//imprimir(1, 2, "Dados TC+VA");
				imprimir(0, 0, "Modo de aquisicao:");
				imprimir(2, 1, "Dinamico");
				imprimir(2, 2, "Monitoramento");
				
				if(getFlagSalvamentoDinamico() == true){
					imprimir(1, 1, "*");
				}else{
					imprimir(1, 2, "*");
				}

				imprimir(1, 3, "Freq:");

				if(getFlagSalvamentoDinamico() == false){
					imprimir(12, 3, "[fixo]");
				}

				switch(mva.getDivFreq())
				{
					case freq_20Hz:
						imprimir(7, 3, "20Hz");
						break;
					case freq_100Hz:
						imprimir(7, 3, "100Hz");
						break;
					case freq_500Hz:
						imprimir(7, 3, "500Hz");
						break;
					case freq_1KHz:
						imprimir(7, 3, "1KHz");
						break;
					case freq_2KHz:
						imprimir(7, 3, "2KHz");
						break;
					case freq_5KHz:
						imprimir(7, 3, "5KHz");
						break;
					case freq_10KHz:
						imprimir(7, 3, "10KHz");
						break;
				}


			}else
			
			if(seta_2 == 2){
				imprimir(3, 1, "Zerar contagem");
				imprimir(2, 2, "e apagar arquivos");
			}else
			
			if(seta_2 == 3){
				imprimir(2, 1, "Transferir dados");
				imprimir(3, 2, "para USB FLASH");
				
				if (sd_usb.get_status_SD_USB() == USB_SELECIONADO) {
						imprimir(14, 3, "USB OK");
				}
				else{																													
						//imprimir(0, 3, "USB Desconectado");
				}
			}
		}else
		
		if (tela_MP == t_config_alarmes)
			{		
				switch (seta_2)
				{
					case 4:
						imprimir(3, 1, "Configurar VA");
						break;
					
					case 5:
						imprimir(3, 1, "Configurar VG");
						break;
					
					case 6:
						imprimir(3, 1, "Configurar TC");
						break;
					
					case 7:
						imprimir(0, 1, "Habilita Rele&Buzzer");
						if(config_IHM.flag_habilita_buzzer == true){imprimir(9, 2, "ON");}
						else{imprimir(9, 2, "OFF");}
						break;
					
					default:
						if (seta_2 >= 1 && seta_2 <= 3) 
							{
								imprimir(0, seta_2, ">");	// Seta seleção programa
								imprimir(0, seta_anterior_2, " ");	// Apaga posição anterior da seta
								
								imprimir(6, 0, "Alarmes:");

								imprimir(1, 1, "Alarme VG");
								imprimir(1, 2, "Alarme VA");
								imprimir(1, 3, "Alarme TC");
								
								if(mvg.getFlagHabilitaAlarmeMVG()) imprimir(14, 1, "ON");
								else	imprimir(14, 1, "OFF");
								if(mva.getFlagHabilitaAlarmeMVA()) imprimir(14, 2, "ON");
								else	imprimir(14, 2, "OFF");
								if(mtc.getFlagHabilitaAlarmeMTC()) imprimir(14, 3, "ON");
								else	imprimir(14, 3, "OFF");
							} 
							else
							{
								imprimir(2,2,"erro");
							}
						break;
				}
		 }else

		if(tela_MP == t_alarmes){
			if(seta_2 == 0){
				imprimir(4, 0, "Alarmes VG");
				alarme(0);
			}else
			if(seta_2 == 1){
				imprimir(3, 0, "Alarmes TC C"); //corrente
				alarme(1);
			}else
			if(seta_2 == 2){
				imprimir(3, 0, "Alarmes TC T"); //tensao
				alarme(2);
			}else
			if(seta_2 == 3){
				imprimir(4, 0, "Alarmes VA");
				alarme(3);
			}
		}else	
		
		if ( tela_MP == t_config_tempo )
		{
			imprimir(5, 1, rtc.get_str_data(false, RTC_ARRAY_DISPLAY));		
			imprimir(6, 2, rtc.get_str_hora(false, RTC_ARRAY_DISPLAY));
			
			switch(ajustar_rtc)
			{
				case 1:
					imprimir(4, 0, "Ajuste o dia");
					break;
				case 2:
					imprimir(4, 0, "Ajuste o mes");
					break;
				case 3:
					imprimir(4, 0, "Ajuste o ano");
					break;
				case 4:
					imprimir(4, 0, "Ajuste a hora");
					break;
				case 5:
					imprimir(4, 0, "Ajuste o minuto");
					break;
				case 6:
					imprimir(4, 0, "Ajuste o segundo");
					break;
				default:
					break;
			}
		}else
		
		if( tela_MP == t_config_wifi ) {
			if(telaswifi == 0){
				if( wifi.sendmsg() ){ //modo de operacao config
					imprimir(0, 0, "Wifi conectado");
					imprimir(0, 1, "Acesse:");
					imprimir(0, 2, "mvs-page.");
					imprimir(0, 3, "mybluemix.net/ui");
				} else {
					imprimir(0, 0, "Wifi desconectado");
					imprimir(0, 1, "Entra para mais info");
					imprimir(0, 2, "Wifi: MVS_CONFIG");
					imprimir(0, 3, "Senha: mvs123456");
				}
				HAL_Delay(500);	
			} else { 
				imprimir(0,2,wifi.message_viewer());
				imprimir(1, 0, "Wifi:");
				imprimir(12, 0, "Reset");
				imprimir(1, 1, "Config");
				if( wifi.status() )
					imprimir(7, 0, "On");
				else
					imprimir(7, 0, "Off");
			}
			if( telaswifi == 1 )
				imprimir(0, 0, ">");				
			if( telaswifi == 2 )
				imprimir(0, 1, ">");
			if( telaswifi == 3 )
				imprimir(11, 0, ">");
		}
	
} //atualiza_tela_MP

void ihm_mvs::atualiza_tela_MVA( void){

			if(flagHabilitaTelasConfig == false){
				switch(tela_MVA){
					case t_vel:
						imprimir(1, 1, "Velocidade (m/min)");	// Mostra em tela a string
						imprimir(8, 2, mva.getStrVeloc());		// Mostra em tela a string
						break;
					case t_prodInst:
						imprimir(1, 1, "Prod. Inst. (kg/h)");	// Mostra em tela a string
						imprimir(8, 2, mva.getStrProdInstantanea());  				
						break;
					case t_consArame:
						imprimir(2, 1, "Consumo Arame (m)");	
						imprimir(8, 2, mva.getStrConsumo()); 
						break;
					case t_prodGlobal:
						imprimir(0, 1, "Prod. A. Aberto (kg)");	
						imprimir(8, 2, mva.getStrProdArcoAberto()); 
						break;
					case t_config_MVA:
						imprimir(3, 1, "Configurar VA");	
						break;
					default:
						break;
				}
		}else{
			switch(tela_MVA_config){
					case t_tempoArcoAberto:
						imprimir(2, 1, "Tempo Arco Aberto");
						imprimir(5, 2, mva.getStrtotalSegundosArcoAberto());		
						break;
					case t_material:
						imprimir(6, 1, "Material");
						imprimir(6, 2, mva.getMaterial());
						break;
					case t_bitola:
						imprimir(7, 1, "Bitola");
						imprimir(7, 2, mva.getStrBitola());
						imprimir(11, 2, "mm");
						break;
					case t_trocaEncoder:
						imprimir(6, 1, "Encoder");
						imprimir(7, 2, mva.getEncoder());
						break;
					case t_zerarVariaveisMVA:
						imprimir(3, 1, "Zerar Variaveis");
						break;
					case t_salvarDadosConfigMVA:
						imprimir(4, 1, "Salvar Dados");
						imprimir(4, 2, "Configurados");
						break;
//					case t_apagarDadosConfigMVA:
//						imprimir(4, 1, "Apagar Dados");
//						imprimir(4, 2, "Configurados");
//						break;
					case t_inverteSentido:
						imprimir(2, 1, "Inverter sentido:");
						if(mva.getSentido() == 1){
							imprimir(7, 2, "false");
						}else{
							imprimir(7, 2, "true ");
						}
						break;
					case t_inverteSentidoAlarme:
						imprimir(1, 1, "Sentido do alarme:");
						if(mva.getSentidoAlarme() == 1){
							imprimir(4, 2, "v. positiva");
						}else{
							imprimir(4, 2, "v. negativa");
						}
						break;
					default:
						break;
				}
			//Mostra * para informar variavel selecionada para alteração
			if(flagHabilitaAlterarConfig == true) imprimir(19, 2, "*");
			else imprimir(19, 2, " ");
		}
}
void ihm_mvs::atualiza_tela_MVG( void){

			if(flagHabilitaTelasConfig == false){
				switch(tela_MVG){
					case t_vazInst:
						imprimir(1, 1, "Vazao Inst. (l/min)");
						imprimir(8, 2, mvg.getStrValorLido());		
						break;
					case t_acumAtual:
						imprimir(2, 1, "Acumulo Atual (l)");
						imprimir(8, 2, mvg.getStrAcumulo());
						break;
					case t_acumTotal:
						imprimir(2, 1, "Acumulo Total (l)");
						imprimir(8, 2, mvg.getStrAcumTotal());
						break;
					case t_config_MVG:
						imprimir(3, 1, "Configurar VG");
						break;
					default:
						break;
				}
		}else{
 				switch(tela_MVG_config){
					case t_tipoGas:
						imprimir(2, 1, "Gas Selecionado");
						imprimir(5, 2, mvg.getStrGas());		
						break;
					case t_vazaoMax:
						imprimir(4, 1, "Vazao Maxima");
						imprimir(5, 2, mvg.getStrVazaoMax());
						break;
					case t_vazaoMin:
						imprimir(4, 1, "Vazao Minima");
						imprimir(5, 2, mvg.getStrVazaoMin());
						break;
					case t_salvarDadosConfigMVG:
						imprimir(4, 1, "Salvar Dados");
						imprimir(4, 2, "Configurados");
						break;
//					case t_apagarDadosConfigMVG:
//						imprimir(4, 1, "Apagar Dados");
//						imprimir(4, 2, "Configurados");
//						break;
					case t_zerarVariaveisMVG:
						imprimir(3, 1, "Zerar Variaveis");
						break;
					default:
						break;
				}
				//Mostra * para informar variavel selecionada para alteração
				if(flagHabilitaAlterarConfig == true) imprimir(19, 2, "*");
				else imprimir(19, 2, " ");
		}
}
void ihm_mvs::atualiza_tela_MTC( void){

		if(flagHabilitaTelasConfig == false){
			switch(tela_MTC){
				case t_tensao:
					imprimir(2, 0, "Tensao Media (V)");
					imprimir(8, 1, mtc.getStrMediaV());
					imprimir(2, 2, "Tensao Eficaz (V)");
					imprimir(8, 3, mtc.getStrEficazV());
					break;
				case t_corrente:
					imprimir(1, 0, "Corrente Media (A)");
					imprimir(8, 1, mtc.getStrMediaI());
					imprimir(1, 2, "Corrente Eficaz (A)");
					imprimir(8, 3, mtc.getStrEficazI());
					break;
				case t_config_MTC:
					imprimir(3, 1, "Configurar TC");
//					imprimir(0, 2, "ADC C: ");
//					imprimir(6, 2, mtc.getStrPinoentradaC());
//					imprimir(0, 3, "ADC T: ");
//					imprimir(6, 3, mtc.getStrPinoentradaT());
					
					break;
				default:
					break;
			}
		}else{
			switch(tela_MTC_config){
				case t_correnteMax:
					imprimir(1, 1, "Corrente maxima (A)");
					imprimir(6, 2, mtc.getStrCorrenteMax());		
					break;
				case t_correnteMin:
					imprimir(1, 1, "Corrente minima (A)");
					imprimir(6, 2, mtc.getStrCorrenteMin());
					break;
				case t_tensaoMax:
					imprimir(2, 1, "Tensao maxima (V)");
					imprimir(6, 2, mtc.getStrTensaoMax());
					break;
				case t_tensaoMin:
					imprimir(2, 1, "Tensao minima (V)");
					imprimir(6, 2, mtc.getStrTensaoMin());
					break;
				case t_salvarDadosConfigMVG:
					imprimir(4, 1, "Salvar Dados");
					imprimir(4, 2, "Configurados");
					break;
				default:
					break;
			}
			//Mostra * para informar variavel selecionada para alteração
			if(flagHabilitaAlterarConfig == true) imprimir(19, 2, "*");
			else imprimir(19, 2, " ");
		}

}

void ihm_mvs::BOTOES_MP(Botbot botao_selecionado){

	// AÇÕES DOS BOTÕES POR TELA //
	switch(tela_MP)
	{
		
	// TELA MENU PRINCIPAL -------------
		case t_mp:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
						if(flag_seta_mp == 1){
							
							seta_anterior_2 = seta_2;
							seta_2++;
							
							if(seta_2 > 3){
								seta_2 = 0;
								seta_anterior_2 = 3;
							}
							
						} else {
													
							seta_1++;
							
							if (seta_1 >= t_max_MP ){	//maximo opções
								seta_1 = t_mp;
							}
						}
						break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
						if(flag_seta_mp == 1){
							
							seta_anterior_2 = seta_2;
							seta_2--;
							
							if(seta_2 < 0){
								seta_2 = 3;
								seta_anterior_2 = 0;
							}
					
						} else {
							
							seta_1--;
							
							if (seta_1 < t_mp){
								seta_1 = t_max_MP - 1;
							}
						}
						break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					if(seta_1 == t_mp){
						
						if(flag_seta_mp == 0){
						
							flag_seta_mp = 1;
							seta_2 = 0;
						
						}else{
						
							/*			MVG			*/
							if (opcao_linha_MP[seta_2] >= 0 && opcao_linha_MP[seta_2] <= 1){
								programa_atual = select_MVG;
							}
							/*			MVA			*/
							if (opcao_linha_MP[seta_2] >= 2 && opcao_linha_MP[seta_2] <= 6){
								programa_atual = select_MVA;
							}
							/*			MTC			*/
							if (opcao_linha_MP[seta_2] >= 7 && opcao_linha_MP[seta_2] <= 10){
								programa_atual = select_MTC;
							}
						}
					}else

					if (seta_1 == t_config_MP){
						if (aquisitar_dados == false){
							seta_2 = 0;
							tela_MP = t_config_MP;
						}
					}else

					if(seta_1 == t_config_salvar){
						if (aquisitar_dados == false){
							seta_2 = 1;
							tela_MP = t_config_salvar;
						}
					}else
					
					if(seta_1 == t_config_alarmes){
						seta_2 = 1;
						tela_MP = t_config_alarmes;
					}else
					
					if(seta_1 == t_config_tempo){
						ajustar_rtc = 1;
						data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_DIA);
						tela_MP = t_config_tempo;
					}else
					
					if(seta_1 == t_config_wifi){
						tela_MP = t_config_wifi;
						telaswifi = 0;
					}
					break;
				}
				
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					if(flag_seta_mp == 0){
						seta_1 = t_mp;
					}
					
					if(flag_seta_mp == 1){
						flag_seta_mp = 0;
					}
					break;
				}
				
			}
			break;
		}
	
	// TELA CONFIG MP -----------------
		case t_config_MP:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
					if(mudaOpcoes == 0){
						
						seta_anterior_2 = seta_2;
						seta_2++;
						
						if(seta_2 > 3){
							seta_2 = 0;
						}
						
					}else{
						altera_opcao_MP(1);	//Altera para a opção seguinte
					}
					break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
					if(mudaOpcoes == 0){
					
						seta_anterior_2 = seta_2;
						seta_2--;
						
						if (seta_2 < 0){
							seta_2 = 3;
						}
						
					}else{	
						altera_opcao_MP(-1);	//Altera para a opção anterior	
					}
					break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					if(mudaOpcoes == 0 /*&& seta_2 < 5*/){
						mudaOpcoes = 1;
					}else{
						mudaOpcoes = 0;
					}
					break;
				}
				
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					if(mudaOpcoes == 0){
						
						TM_HD44780_Clear();
						tela_MP = t_mp;
						
						set_flag_salvar_eeprom(opcao_eeprom_mp);

//						if(eeprom.salvar_estrutura(EEPROM_MP_CONFIG_LOCATION, (unsigned char*) &opcao_linha_MP, sizeof(opcao_linha_MP)) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						
					}else{
					
						mudaOpcoes = 0;
						
					}
					break;
				}
			}
			break;
		}
		
	// TELA CONFIG SALVAR -----------------
		case t_config_salvar:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
					if(flag_config_salvar == 1){
						seta_anterior_3 = seta_3;
						seta_3++;
					}else{
						seta_anterior_2 = seta_2;
						seta_2++;
					}
					
					setas_t_config_salvar();

					break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
					if(flag_config_salvar == 1){
						seta_anterior_3 = seta_3;
						seta_3--;
					}else{
						seta_anterior_2 = seta_2;
						seta_2--;
					}				
					
					setas_t_config_salvar();
					break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					if(seta_2 == 1){

						if(flag_config_salvar == 0){

							flag_config_salvar = 1;
							seta_3 = 1;

						}else{ //muda o valor dependendo de onde a seta estiver

							if(seta_3 == 1){

									config_IHM.flagSalvamentoDinamico = true;
									mva.setDivFreq(freq_500Hz);
								
							}else
							
							if(seta_3 == 2){
								
								config_IHM.flagSalvamentoDinamico = false;
								mva.setDivFreq(freq_20Hz);
								
							}else if(seta_3 == 3) {
								
								if(config_IHM.flagSalvamentoDinamico == true){

									uint16_t freq;

									switch(mva.getDivFreq())
									{
										case freq_100Hz:
											freq = freq_500Hz;
											break;
										case freq_500Hz:
											freq = freq_1KHz;
											break;
										case freq_1KHz:
											freq = freq_2KHz;
											break;
										case freq_2KHz:
											freq = freq_5KHz;
											break;
										case freq_5KHz:
											if( HABILITA_OPCAO_10KHZ == 1){
												freq = freq_10KHz;
											}else{
												freq = freq_500Hz;
											}										
											break;
										case freq_10KHz:
											freq = freq_100Hz;
											break;
										default:
											freq = freq_500Hz;
											break;
									}

									mtc.setDivFreq(freq);
									mva.setDivFreq(freq);
								}
							}
						}
					}else
					
					if(seta_2 == 2){
					
						if(sd_usb.deletarAquivos() == FR_OK){
							//eeprom.salvar_estrutura(EEPROM_SD_CONFIG_LOCATION, (unsigned char*) sd_usb.getConfigSD(), sd_usb.getSizoOfConfigSD());
							set_flag_salvar_eeprom(opcao_eeprom_sd);
							limpar_tela();
							imprimir(1, 1, "Arquivos excluidos");
							HAL_Delay(1000);
						}else {
							limpar_tela();
							imprimir(7, 1, "ERROR");
							HAL_Delay(1000);
						}
						
					}else
					
					if(seta_2 == 3){
					
						limpar_tela();
						
						if(sd_usb.get_status_SD_USB() == USB_SELECIONADO){
							
							NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
							imprimir(3, 1, "Transferindo...");	
							
							if(sd_usb.transfer_sd_to_usb() != true){
								
								limpar_tela();
								imprimir(7, 1, "Erro na");
								imprimir(4, 2, "transferencia");
								
							}else{
								
								limpar_tela();
								imprimir(6, 1, "Sucesso");
								
							}
							
							HAL_Delay(1000);
							NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
							
						}else{
							
							imprimir(8, 1, "Erro");
							imprimir(8, 2, "USB");
							HAL_Delay(1000);
							
						}
					}
					break;
				}
				
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					if(flag_config_salvar == 1 && seta_2 == 1){
						
						flag_config_salvar = 0;
						seta_2 = 1;
						
					} else {
						
						tela_MP = t_mp;
						//eeprom.salvar_estrutura(EEPROM_IHM_CONFIG_LOCATION, (unsigned char*) &config_IHM, sizeof(config_IHM));
						set_flag_salvar_eeprom(opcao_eeprom_ihm);
					}
					break;
				}
			}
			break;
		}
		
	// TELA CONFIG ALARMES -----------------
		case t_config_alarmes:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
					seta_anterior_2 = seta_2;
					seta_2++;						
					
					if (seta_2 > 7){				
						seta_2 = 1;
					}
					break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
					seta_anterior_2 = seta_2;		
					seta_2--;						
					if (seta_2 <= 0){			
						seta_2 = 7;
					}
					break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					switch(seta_2){
					
						case 1:
							if(mvg.getFlagHabilitaAlarmeMVG() == false){ // on/off alarme do MVG
								mvg.setFlagHabilitaAlarmeMVG(true);
							}else{mvg.setFlagHabilitaAlarmeMVG(false);}
							break;

						case 2:
							if(mva.getFlagHabilitaAlarmeMVA() == false){ // on/off alarme do MVA
								mva.setFlagHabilitaAlarmeMVA(true);
							}else{mva.setFlagHabilitaAlarmeMVA(false);}
							break;

						case 3:
							if(mtc.getFlagHabilitaAlarmeMTC() == false){ // on/off alarme do MTC
								mtc.setFlagHabilitaAlarmeMTC(true);
							}else{mtc.setFlagHabilitaAlarmeMTC(false);}
							break;

						case 4:
							flagHabilitaTelasConfig = true;
							programa_atual = select_MVA;
							break;

						case 5:
							flagHabilitaTelasConfig = true;
							programa_atual = select_MVG;
							break;

						case 6:
							flagHabilitaTelasConfig = true;
							programa_atual = select_MTC;
							break;

						case 7:
							config_IHM.flag_habilita_buzzer = !config_IHM.flag_habilita_buzzer;
							break;
						}
					break;
				}
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					tela_MP = t_mp;
//					config_IHM.flagAlarmeMVG = mvg.getFlagHabilitaAlarmeMVG();
//					config_IHM.flagAlarmeMVA = mva.getFlagHabilitaAlarmeMVA();
//					config_IHM.flagAlarmeMTC = mtc.getFlagHabilitaAlarmeMTC();
					//eeprom.salvar_estrutura(EEPROM_IHM_CONFIG_LOCATION, (unsigned char*) &config_IHM, sizeof(config_IHM));
					set_flag_salvar_eeprom(opcao_eeprom_mva);
					set_flag_salvar_eeprom(opcao_eeprom_mvg);
					set_flag_salvar_eeprom(opcao_eeprom_mtc);
					set_flag_salvar_eeprom(opcao_eeprom_ihm);
					break;
				}
			}
			break;
		}
		
	// TELA ALARMES -----------------
		case t_alarmes:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
						seta_anterior_2 = seta_2;
						seta_2++;
						if (seta_2 >=  4){						
							seta_2 = 0;
						}
						break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
						seta_anterior_2 = seta_2;	
						seta_2--;					
						if (seta_2 < 0)				
							seta_2 = 3;
						break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					break;
				}
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					tela_MP = t_mp;
					break;
				}
			}
			break;
		}
		
	// TELA CONFIG TEMPO -----------------
		case t_config_tempo:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
						data_hora++;
						uint8_t temp = data_hora << 4;
						if ( temp >= 0xA0 ) {
							data_hora += 6;
						}
						ajuste_config_data_hora();
						break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
						data_hora--; 
						uint8_t temp = data_hora << 4;
						if ( temp >= 0xA0 ) {
							data_hora -= 6;
						}
						ajuste_config_data_hora();
						break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					ajustar_rtc++;
					if (ajustar_rtc > 6)
					{
						flags_IHM.salvar_hora_no_rtc = true;
						ajustar_rtc = 0;
						tela_MP = t_mp;
					}
					switch(ajustar_rtc){
						case 1:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_DIA);
						break;
						
						case 2:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_MES);
						break;
						
						case 3:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_ANO);
						break;
						
						case 4:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_HORAS);
						break;
						
						case 5:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_MINUTOS);
						break;
						
						case 6:
							data_hora = rtc.get_dado(RTC_ARRAY_DISPLAY, RTC_DATA_SEGUNDOS);
						break;
						
						default:
							//data_hora = 0;
						break;
					}
					break;
				}
				
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					tela_MP = t_mp;      
					ajustar_rtc = 0;
					break;
				}
			}
			break;
		}
		
	// TELA CONFIG WIFI -----------------
		case t_config_wifi:
		{
			switch(botao_selecionado)
			{
				case tSentidoHorario: // [ENCODER -->] //
				{
						if(telaswifi < T_MAX_CONFIG_WIFI)
							telaswifi++;
						else
							telaswifi = 1;
						break;
				}
				
				case tSentidoAntiHorario: // [ENCODER <--] //
				{
						if(telaswifi > 1)
							telaswifi--;
						else
							telaswifi = T_MAX_CONFIG_WIFI;
						break;
				}
				
				case tBotaoEntrar: // [BOTAO ENTRAR] //
				{
					uint8_t tx[50] = {0};						
					switch (telaswifi)
          {
          	case 1:
							wifi.change_status();
          		break;
						
          	case 3:
							sprintf((char*)tx, "reset\r\n");	// Escreve na string o texto
							HAL_UART_Transmit(&huart3, tx, strlen((char*)tx), 100);						
							imprimir(12, 0, "OK    ");
							HAL_Delay(200);	
          		break;
						
						case 2:
							sprintf((char*)tx, "config\r\n");	// Escreve na string o texto
							HAL_UART_Transmit(&huart3, tx, strlen((char*)tx), 100);
							imprimir(1, 1, "OK                ");
							HAL_Delay(200);
							break;
						
          	default:
							telaswifi = 1;
          		break;
          }
					
//				//Entra no modo configurar wifi
//				if(tela_MP == t_config_wifi){
//					telaswifi = 1;
//				}		
					
					break;
				}
				case tBotaoVoltar: // [BOTAO VOLTAR] //
				{
					if( telaswifi == 0 )
						tela_MP = t_mp;
					if( telaswifi != 0)
						telaswifi = 0;
					break;
				}
			}
			break;
		}
		
	}
}

void ihm_mvs::BOTOES_MVG(Botbot botao_selecionado){

	switch(botao_selecionado){
		
			case tBotaoVoltar:
					if(flagHabilitaAlterarConfig == true) 
						flagHabilitaAlterarConfig = false;
					else 
					if(flagHabilitaTelasConfig == true && tela_MP != t_config_alarmes)
						flagHabilitaTelasConfig = false;
					else
					{	
//						if(eeprom.salvar_estrutura(EEPROM_MVG_CONFIG_LOCATION, (unsigned char*) mvg.getConfigMVG(), mvg.getSizoOfConfigMVG()) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						set_flag_salvar_eeprom(opcao_eeprom_mvg);
						flagHabilitaTelasConfig = false;
						programa_atual = select_MP;
					}
				break;			
					
			case tBotaoSalvar:
				//if (flagHabilitaTelasConfig == true)
				break;
			
			case tBotaoEntrar:
				if(tela_MVG_config == t_tipoGas && flagHabilitaTelasConfig == true){
					flagHabilitaAlterarConfig = true;
				}
				
				if(tela_MVG_config == t_zerarVariaveisMVG && flagHabilitaTelasConfig == true){
					mvg.Zerar(); 
					limpar_tela(); 
					imprimir(9, 1, "OK"); 
					HAL_Delay(1500);
				}
				
				if(tela_MVG == t_config_MVG){
					flagHabilitaTelasConfig = true;
				} //tela de configurações do MVG
				
				if(tela_MVG_config == t_vazaoMax && flagHabilitaTelasConfig == true){
					flagHabilitaAlterarConfig = true;
				}
				
				//*
				if(tela_MVG_config == t_salvarDadosConfigMVG && flagHabilitaTelasConfig == true){
//					if(eeprom.salvar_estrutura(EEPROM_MVG_CONFIG_LOCATION, (unsigned char*) mvg.getConfigMVG(), mvg.getSizoOfConfigMVG()) == 0){
//						limpar_tela();
//						imprimir(5, 1, "Salvo com");
//						imprimir(6, 2, "sucesso");
//						HAL_Delay(1000);
//					}else{
//						limpar_tela();
//						imprimir(5, 1, "Erro ao");
//						imprimir(6, 2, "salvar");
//						HAL_Delay(1000);
//					}
					set_flag_salvar_eeprom(opcao_eeprom_mvg);
				}
				//*/
				
				if(tela_MVG_config == t_vazaoMin && flagHabilitaTelasConfig == true){
					flagHabilitaAlterarConfig = true;
				}
				
				break;
				
			case tSentidoHorario:
				setas_t_config_variaveis(+1, &tela_MVG, &tela_MVG_config, t_max_MVG, t_max_MVG_config);
				break;
				
			case tSentidoAntiHorario:
				setas_t_config_variaveis(-1, &tela_MVG, &tela_MVG_config, t_max_MVG, t_max_MVG_config);
				break;
				
			default:
				break;
		}
}

void ihm_mvs::BOTOES_MTC(Botbot botao_selecionado){ 	  // leitura dos botoes do MTC

		switch(botao_selecionado){
			case tBotaoVoltar:
					if(flagHabilitaAlterarConfig == true) 
						flagHabilitaAlterarConfig = false;
					else 
					if(flagHabilitaTelasConfig == true && tela_MP != t_config_alarmes)
						flagHabilitaTelasConfig = false;
					else
					{
//						if(eeprom.salvar_estrutura(EEPROM_MTC_CONFIG_LOCATION, (unsigned char*) mtc.getConfigMTC(), mtc.getSizoOfConfigMTC()) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						set_flag_salvar_eeprom(opcao_eeprom_mtc);
						flagHabilitaTelasConfig = false;
						programa_atual = select_MP;
					}	
				break;			
					
			case tBotaoSalvar:
				//if (flagHabilitaTelasConfig == true)
				break;
				
			case tBotaoEntrar:
//				if(tela_MTC == t_config_MTC){} //tela de configurações do MTC
				
				if(flagHabilitaTelasConfig == true){
					
					if(tela_MTC_config == t_correnteMax || tela_MTC_config == t_correnteMin || tela_MTC_config == t_tensaoMax || tela_MTC_config == t_tensaoMin){
						flagHabilitaAlterarConfig = true;
					}else
					
					if(tela_MTC_config == t_salvarDadosConfigMTC){
//						if(eeprom.salvar_estrutura(EEPROM_MTC_CONFIG_LOCATION, (unsigned char*) mtc.getConfigMTC(), mtc.getSizoOfConfigMTC()) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						set_flag_salvar_eeprom(opcao_eeprom_mtc);
					}
				}else
				
				if(tela_MTC == t_config_MTC){
					flagHabilitaTelasConfig = true;
				} //tela de configurações do MVG
				
				break;	
				
			case tSentidoHorario:
				setas_t_config_variaveis(+1, &tela_MTC, &tela_MTC_config, t_max_MTC, t_max_MTC_config);
				break;
				
			case tSentidoAntiHorario:
				setas_t_config_variaveis(-1, &tela_MTC, &tela_MTC_config, t_max_MTC, t_max_MTC_config);
				break;
				
			default:
				break;
		}
}

void ihm_mvs::BOTOES_MVA(Botbot botao_selecionado){ 	// Leitura dos botoes do MVA		

		switch(botao_selecionado){
			
			case tBotaoVoltar:
				
					if(flagHabilitaAlterarConfig == true){
						flagHabilitaAlterarConfig = false;
					}else
					if(flagHabilitaTelasConfig == true && tela_MP != t_config_alarmes){
						flagHabilitaTelasConfig = false;
					}else
					{
//						if(eeprom.salvar_estrutura(EEPROM_MVA_CONFIG_LOCATION, (unsigned char*) mva.getConfigMVA(), mva.getSizoOfConfigMVA()) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						set_flag_salvar_eeprom(opcao_eeprom_mva);
						flagHabilitaTelasConfig = false;
						programa_atual = select_MP;
					}	
				break;
					
			case tBotaoSalvar:
				//if (flagHabilitaTelasConfig == true)
				break;
				
			case tBotaoEntrar:
			{
				if(flagHabilitaTelasConfig == true){
					
					if(tela_MVA_config == t_material || tela_MVA_config == t_bitola || tela_MVA_config == t_trocaEncoder){
						flagHabilitaAlterarConfig = true;
					}else
					
					if(tela_MVA_config == t_zerarVariaveisMVA){
						mva.Zerar();
						limpar_tela(); imprimir(9, 1, "OK"); HAL_Delay(1500);
						__HAL_TIM_SET_COUNTER(&htim8, 32000);
					}else
					
					if(tela_MVA_config == t_salvarDadosConfigMVA){
//						if(eeprom.salvar_estrutura(EEPROM_MVA_CONFIG_LOCATION, (unsigned char*) mva.getConfigMVA(), mva.getSizoOfConfigMVA()) == 0){
//							limpar_tela();
//							imprimir(5, 1, "Salvo com");
//							imprimir(6, 2, "sucesso");
//							HAL_Delay(1000);
//						}else{
//							limpar_tela();
//							imprimir(5, 1, "Erro ao");
//							imprimir(6, 2, "salvar");
//							HAL_Delay(1000);
//						}
						set_flag_salvar_eeprom(opcao_eeprom_mva);
					}else
					
					if(tela_MVA_config == t_inverteSentido){
						mva.trocaSentido();
					}
					if(tela_MVA_config == t_inverteSentidoAlarme){
						mva.trocaSentidoAlarme();
					}
					
				}else
				
				if(tela_MVA == t_config_MVA){
					flagHabilitaTelasConfig = true;
				} //tela de configurações do MVA
					
				break;	
			}
			
			case tSentidoHorario:
			{
				setas_t_config_variaveis(+1, &tela_MVA, &tela_MVA_config, t_max_MVA, t_max_MVA_config);
				break;
			}
				
			case tSentidoAntiHorario:
			{
				setas_t_config_variaveis(-1, &tela_MVA, &tela_MVA_config, t_max_MVA, t_max_MVA_config);
				break;
			}
			
			default:
				break;
		}
}


void ihm_mvs::altera_opcao_MP(int sinal){          										// Função que altera opcao do menu inicial
		
	for(int i = 0; i < 4; i++)
	{
		if(seta_2 == i){					//Seta na primeira linha - 0
			opcao_linha_MP[i]+=sinal;	//1 ou -1
			//Zera se os valores ultrapassarem o valor maximo
			if(opcao_linha_MP[i] >= NVMP)opcao_linha_MP[i] = 0;
			//Seta valor maximo caso valores fiquem negativos
			if(opcao_linha_MP[i] < 0) opcao_linha_MP[i] = NVMP - 1;
			
			break;
		}
	}
}


void ihm_mvs::alarme(int prog){
		switch(prog){
			case 0:
				if(mvg.getFlagHabilitaAlarmeMVG()){
					if(mvg.alarme() == 1){
						imprimir(4, 2, "Vazao de gas");
						imprimir(2, 3, "acima do limite");
					}
					if(mvg.alarme() == 2){
						imprimir(4, 2, "Vazao de gas");
						imprimir(2, 3, "abaixo do limite");
					}
				}
				break;
			case 1:		
				if(mtc.getFlagHabilitaAlarmeMTC()){
						if(mtc.alarme_I() == 1){
							imprimir(6, 2, "Corrente");
							imprimir(2, 3, "acima do limite");
						}
						if(mtc.alarme_I() == 2){
							imprimir(6, 2, "Corrente");
							imprimir(2, 3, "abaixo do limite");
						}
				}
			break;
			case 2:
				if(mtc.getFlagHabilitaAlarmeMTC()){
						if(mtc.alarme_T() == 1){
							imprimir(7, 2, "Tensao");
							imprimir(2, 3, "acima do limite");
						}
						if(mtc.alarme_T() == 2){	
							imprimir(7, 2, "Tensao");
							imprimir(2, 3, "abaixo do limite");
						}
				}
				break;
			case 3:
				break;
			default:
				break;
	}
}

void ihm_mvs::setas_t_config_salvar(void){
	
	if(flag_config_salvar == 1){ //seta 1 a 3 dedicadas para as opcoes
						
			if (seta_3 > 3){
				seta_3 = 1;
			}
			else if (seta_3 < 1){
				seta_3 = 3;
			}
			
	} else { //se não estiver no modo de salvar
			
			if(seta_2 < 1){
				seta_2 = T_MAX_CONFIG_SALVAR;
			}
			else if (seta_2 > T_MAX_CONFIG_SALVAR){
				seta_2 = 1;
			}
			
	}
}

void ihm_mvs::setas_t_config_variaveis(const int x, int *tela, int *tela_config, const int t_max, const int t_max_config){
	
	if(flagHabilitaTelasConfig == false && flagHabilitaAlterarConfig == false){
		
		*tela += x;
		if(*tela < 0){
			*tela = t_max - 1;
		}else{
			*tela %= t_max; //Divisão e atribui o resto para tela_MVA - mantem valor sempre entre zero e t_max_MVA
		}
		
	}else if(flagHabilitaTelasConfig == true && flagHabilitaAlterarConfig == false){
		
		*tela_config += x;
		if(*tela_config < 0){
			*tela_config = t_max_config - 1;
		}else{
			*tela_config %= t_max_config;
		}
		
	}else if(flagHabilitaAlterarConfig == true){
		
		switch (getTelaMVS()){
		
			case select_MVG:								// MVG
				if(x == 1){
					if(tela_MVG_config == t_tipoGas){mvg.trocaGas(x);}
					if(tela_MVG_config == t_vazaoMax){mvg.incVazaoMax();}
					if(tela_MVG_config == t_vazaoMin){mvg.incVazaoMin();}
				}else if(x == -1){
					if(tela_MVG_config == t_tipoGas){mvg.trocaGas(x);}
					if(tela_MVG_config == t_vazaoMax){mvg.decVazaoMax();}
					if(tela_MVG_config == t_vazaoMin){mvg.decVazaoMin();}
				}
				break;
					
			case select_MTC:								// MTC
				if(x == 1){
						if(tela_MTC_config == t_correnteMax){mtc.incCorrenteMax();} else
						if(tela_MTC_config == t_correnteMin){mtc.incCorrenteMin();} else
						if(tela_MTC_config == t_tensaoMax){mtc.incTensaoMax();} else
						if(tela_MTC_config == t_tensaoMin){mtc.incTensaoMin();}
				}else if(x == -1){
						if(tela_MTC_config == t_correnteMax){mtc.decCorrenteMax();} else
						if(tela_MTC_config == t_correnteMin){mtc.decCorrenteMin();} else
						if(tela_MTC_config == t_tensaoMax){mtc.decTensaoMax();} else
						if(tela_MTC_config == t_tensaoMin){mtc.decTensaoMin();}
				}
				break;
				
   		case select_MVA:								// MVA
				if(tela_MVA_config == t_material){mva.trocaMaterial(x);}
				if(tela_MVA_config == t_bitola){mva.trocaBitola(x);}
				if(tela_MVA_config == t_trocaEncoder){mva.trocaEncoder(x);}
				break;
				
			default:
				break;

		}
	}
}

void ihm_mvs::ajuste_config_data_hora(void)
{
	switch(ajustar_rtc)
	{
		case 1:
			if ( data_hora < 0x01)
				data_hora = 0x31;
			if ( data_hora > 0x31)
				data_hora = 0x01;
			
			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_DIA);
			break;
		
		case 2:
			if ( data_hora < 0x01) 
				data_hora = 0x12;
			if ( data_hora > 0x12) 
				data_hora = 0x01;
		
			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_MES);
			break;
		
		case 3:		
			if ( (data_hora>>4) >= 0xE) 
				data_hora =0x99;
			if ( data_hora >= 0xA0) 
				data_hora =0;					
		
			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_ANO);
			break;
		
		case 4:			
			if ( (data_hora>>4) >= 0xA) 
				data_hora =0x23;
			if ( data_hora >= 0x24) 
				data_hora =0;
			
			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_HORAS);
			break;
		
		case 5:
			if ( (data_hora>>4) >= 0xA) 
				data_hora =0x59;
			if ( data_hora >= 0x60) 
				data_hora =0;

			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_MINUTOS);
			break;
		
		case 6:
			if ( (data_hora>>4) >= 0xA) 
				data_hora =0x59;
			if ( data_hora >= 0x60) 
				data_hora =0;

			rtc.set_dado( data_hora, RTC_ARRAY_DISPLAY, RTC_DATA_SEGUNDOS);
			break;
		
		default:
			ajustar_rtc=1;
			break;
	}
}
