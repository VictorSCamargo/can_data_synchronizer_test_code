#include "funcoes_mvg.h"

/*-------		Dados gerais do Gases configurados no MVG		-------*/
	structGas tipoGas[4] = { 
		/*gas;			vasaoMinAju,	vasaoMaxAju;	fatores;	fatores2;	vasaoMinSelec;	vasaoMaxSelec;*/
		{"Argonio", 	0.0f, 							20.0f, 				936.0f, 			-96740.0f},     
		{"N2",  				0.0f, 							20.0f, 				837.0f, 			-96740.0f},   
		{"O2",  				0.0f, 							20.0f, 				837.0f, 			-96740.0f},			
		{"CO2",  			0.0f, 							20.0f, 				705.0f, 			-96740.0f}  
	};

/*----------------------------*/


/*----------Construtor-------------*/
mvgclass::mvgclass( void){ 
	
	indiceGas = 0; 
	variaveis_mvg.acumulada = 0.0;
	variaveis_mvg.acumuladaTotal = 0.0;
	config_MVG.gasselecionado = indiceGas;
	config_MVG.vazaoMinima[indiceGas] = tipoGas[indiceGas].vazaoMinAju;
	config_MVG.vazaoMaxima[indiceGas] = tipoGas[indiceGas].vazaoMaxAju;
	
	variaveis_mvg.valorLido = 0.0;  
	tempoVazao = 0;	
	vazaoMinimaSalvar = 0;
	vazaoMaximaSalvar = 0;
	flagHabilitaAlarmeMVG = false;
}
/*-------------------------			SET's and GET's			----------------------------------------------*/
void mvgclass::setVazaoMax(float vazMax){config_MVG.vazaoMaxima[indiceGas] = vazMax; }
void mvgclass::setVazaoMin(float vazMin){config_MVG.vazaoMinima[indiceGas] = vazMin; }
void mvgclass::setAcumulo(float acum){variaveis_mvg.acumulada = acum; }
void mvgclass::setAcumTotal(float acumTotal){variaveis_mvg.acumuladaTotal = acumTotal; }
void mvgclass::setDefaultConfigMVG(void)
{
	for(int c = 0; c<4; c++){
	config_MVG.vazaoMaxima[c] = 50; 
	config_MVG.vazaoMinima[c] = 0;
	}		
	config_MVG.gasselecionado = 0;
}


structMVG_config* mvgclass::getConfigMVG(void){return &config_MVG;}
unsigned short mvgclass::getSizoOfConfigMVG(void){return sizeof(config_MVG);}

float mvgclass::getVazaoMax(void){ return config_MVG.vazaoMaxima[indiceGas];}
float mvgclass::getVazaoMin(void){ return config_MVG.vazaoMinima[indiceGas];}
float mvgclass::getAcumulo(void){ return variaveis_mvg.acumulada;}
float mvgclass::getAcumTotal(void){ return variaveis_mvg.acumuladaTotal;}
int mvgclass::getGas(void){	return config_MVG.gasselecionado;}
float mvgclass::getValorLido(void){return variaveis_mvg.valorLido;}


bool mvgclass::getFlagHabilitaAlarmeMVG(void){ return flagHabilitaAlarmeMVG;}
void mvgclass::setFlagHabilitaAlarmeMVG(bool flag){flagHabilitaAlarmeMVG = flag; }


//Zera as variavieis medidas
void mvgclass::Zerar(void){
	variaveis_mvg.acumulada = 0.0;
	variaveis_mvg.acumuladaTotal = 0.0;
	for(int i = 0; i < 4; i++){
		config_MVG.vazaoMinima[i] = tipoGas[i].vazaoMinAju;
		config_MVG.vazaoMaxima[i] = tipoGas[i].vazaoMaxAju;
	}
}
//Troca o gas
void mvgclass::trocaGas(int sinal){
	variaveis_mvg.acumulada = 0.0;
	if(sinal == 1 && indiceGas < 3){
		indiceGas++;
		gasAtivado = indiceGas;
		config_MVG.gasselecionado = indiceGas;
	}else if(sinal == 1 && indiceGas == 3){
		indiceGas = 0;
		gasAtivado = indiceGas;
		config_MVG.gasselecionado = indiceGas;
	}
	if(sinal == -1 && indiceGas > 0){
		indiceGas--;
		gasAtivado = indiceGas;
		config_MVG.gasselecionado = indiceGas;
	}else if(sinal == -1 && indiceGas == 0){
		indiceGas = 3;
		gasAtivado = indiceGas;
		config_MVG.gasselecionado = indiceGas;
	}
}

char* mvgclass::getStrValorLido( void){	
	sprintf(stringRetorno, "%.2f", variaveis_mvg.valorLido);  //variaveis_mvg.valorLido);
	return stringRetorno;
}
char* mvgclass::getStrAcumulo(void){
	sprintf(stringRetorno, "%.2f", variaveis_mvg.acumulada);
	return stringRetorno;
}
char* mvgclass::getStrAcumTotal(void){
	sprintf(stringRetorno, "%.2f", variaveis_mvg.acumuladaTotal);
	return stringRetorno;
}
char* mvgclass::getStrVazaoMax(void){
	sprintf(stringRetorno, "%.2f", config_MVG.vazaoMaxima[indiceGas]);
	return stringRetorno;
}
char* mvgclass::getStrVazaoMin(void){ 
	sprintf(stringRetorno, "%.2f", config_MVG.vazaoMinima[indiceGas]);
	return stringRetorno;
}
char* mvgclass::getStrGas(void){
	if(config_MVG.gasselecionado >= 0 && config_MVG.gasselecionado < 4)
		return tipoGas[config_MVG.gasselecionado].gas;
	else{
		config_MVG.gasselecionado = 0;
		return tipoGas[config_MVG.gasselecionado].gas;
	}
}
/*--------------------------------------------------------------------------------------------------*/

void mvgclass::inc_tempoVazao(void){tempoVazao++;}
void mvgclass::clear_tempoVazao(void){tempoVazao = 0;}

char* mvgclass::get_tipoGasAtivado( void){
	return tipoGas[gasAtivado].gas;
}

bool mvgclass::incVazaoMax(){
	if(config_MVG.vazaoMaxima[indiceGas] < tipoGas[indiceGas].vazaoMaxAju){
		config_MVG.vazaoMaxima[indiceGas] += 0.5f;
		return true;
	}else return false;
}
bool mvgclass::incVazaoMin(){
	if(config_MVG.vazaoMinima[indiceGas] < config_MVG.vazaoMaxima[indiceGas]){
		config_MVG.vazaoMinima[indiceGas] += 0.5f;
		return true;
	}else return false;
}
bool mvgclass::decVazaoMax(){
	if(config_MVG.vazaoMaxima[indiceGas] > config_MVG.vazaoMinima[indiceGas]){
		config_MVG.vazaoMaxima[indiceGas] -= 0.5f;
		return true;
	}else return false;
}
bool mvgclass::decVazaoMin(){
	if(config_MVG.vazaoMinima[indiceGas] > tipoGas[indiceGas].vazaoMinAju){
		config_MVG.vazaoMinima[indiceGas] -= 0.5f;
		return true;
	}else return false;
}


//Funcao para verificar alarme do MVG
//		1 - Vazao de gas acima do limite
//		2 - Vazao de gas abaixo do limite
// 		0 - Vazao de gas dentro do limite

int mvgclass::alarme (void)
	{
		if (variaveis_mvg.valorLido > config_MVG.vazaoMaxima[indiceGas])
			{
				if (flagHabilitaAlarmeMVG)
					//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_SET); //saida digital PD7
				
				return 1;
			}
			
		if (variaveis_mvg.valorLido < config_MVG.vazaoMinima[indiceGas])
			{
				if (flagHabilitaAlarmeMVG)
					//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_SET); //saida digital PD7
				
				return 2;
			}
			
		//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);	
		return 0;
	}
