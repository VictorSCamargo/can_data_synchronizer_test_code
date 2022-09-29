#include "funcoes_mtc.h"

mtcclass::mtcclass( void){ 
	variaveis_MTC.mediaV = 0.0;
	variaveis_MTC.mediaI = 0.0;
	variaveis_MTC.vEficaz = 0.0;
	variaveis_MTC.iEficaz = 0.0;	

	config_MTC.correnteMaxima = 600;
	config_MTC.correnteMinima = 0;
	config_MTC.tensaoMaxima = 50;
	config_MTC.tensaoMinima = 0;
	flagHabilitaAlarmeMTC = false;
}

void mtcclass::setEficazI(float cE){variaveis_MTC.iEficaz = cE;}
void mtcclass::setMediaI(float cM){variaveis_MTC.mediaI = cM;}
void mtcclass::setEficazV(float vE){variaveis_MTC.vEficaz = vE;}
void mtcclass::setMediaV(float vM){variaveis_MTC.mediaV = vM;}
void mtcclass::setDefaultConfigMTC(void)
{
	config_MTC.tensaoMaxima = 100; 
	config_MTC.tensaoMinima = -100;
	config_MTC.correnteMaxima = 1000;
	config_MTC.correnteMinima = -1000;
}

structMTC* mtcclass::getVariaveisMTC(void){return &variaveis_MTC;}
int mtcclass::getSizoOfVariaveisMTC(void){return sizeof(variaveis_MTC);}

float mtcclass::getEficazI(void){return variaveis_MTC.iEficaz;}
float mtcclass::getMediaI(void){return variaveis_MTC.mediaI;}
float mtcclass::getEficazV(void){return variaveis_MTC.vEficaz;}
float mtcclass::getMediaV(void){return variaveis_MTC.mediaV;}

float mtcclass::getCorrenteMax(void){return config_MTC.correnteMaxima;}
float mtcclass::getCorrenteMin(void){return config_MTC.correnteMinima;}
float mtcclass::getTensaoMax(void){return config_MTC.tensaoMaxima;}
float mtcclass::getTensaoMin(void){return config_MTC.tensaoMinima;}

bool mtcclass::getFlagHabilitaAlarmeMTC(void){ return flagHabilitaAlarmeMTC;}
void mtcclass::setFlagHabilitaAlarmeMTC(bool flag){flagHabilitaAlarmeMTC = flag; }

structMTC_config* mtcclass::getConfigMTC(void){return &config_MTC;}
unsigned short mtcclass::getSizoOfConfigMTC(void){return sizeof(config_MTC);}


char* mtcclass::getStrEficazI(void){
	sprintf(stringRetorno, "%.0f ", variaveis_MTC.iEficaz);
	return stringRetorno;
}
char* mtcclass::getStrMediaI(void){	
	sprintf(stringRetorno, "%.0f ", variaveis_MTC.mediaI);
	return stringRetorno;
}
char* mtcclass::getStrEficazV(void){	
	sprintf(stringRetorno, "%.1f", variaveis_MTC.vEficaz);
	return stringRetorno;
}
char* mtcclass::getStrMediaV(void){	
	sprintf(stringRetorno, "%.1f", variaveis_MTC.mediaV);
	return stringRetorno;
}
char* mtcclass::getStrCorrenteMax(void){
	sprintf(stringRetorno, "%.1f", config_MTC.correnteMaxima);
	return stringRetorno;
}
char* mtcclass::getStrCorrenteMin(void){
	sprintf(stringRetorno, "%.1f", config_MTC.correnteMinima);
	return stringRetorno;
}
char* mtcclass::getStrTensaoMax(void){
	sprintf(stringRetorno, "%.1f", config_MTC.tensaoMaxima);
	return stringRetorno;
}
char* mtcclass::getStrTensaoMin(void){
	sprintf(stringRetorno, "%.1f", config_MTC.tensaoMinima);
	return stringRetorno;
}


//Mudar limites de tensao e corrente para alarme
bool mtcclass::incTensaoMax(){
	if(config_MTC.tensaoMaxima < 100){
		config_MTC.tensaoMaxima += 0.5f;
		return true;
	}else return false;
}
bool mtcclass::incTensaoMin(){
	if(config_MTC.tensaoMinima < config_MTC.tensaoMaxima){
		config_MTC.tensaoMinima += 0.5f;
		return true;
	}else return false;
}
bool mtcclass::decTensaoMax(){
	if(config_MTC.tensaoMaxima > config_MTC.tensaoMinima){
		config_MTC.tensaoMaxima -= 0.5f;
		return true;
	}else return false;
}
bool mtcclass::decTensaoMin(){
	if(config_MTC.tensaoMinima > -100){
		config_MTC.tensaoMinima -= 0.5f;
		return true;
	}else return false;
}

bool mtcclass::incCorrenteMax(){
	if(config_MTC.correnteMaxima < 1200){
		config_MTC.correnteMaxima += 1.0f;
		return true;
	}else return false;
}
bool mtcclass::incCorrenteMin(){
	if(config_MTC.correnteMinima < config_MTC.correnteMaxima){
		config_MTC.correnteMinima += 1.0f;
		return true;
	}else return false;
}
bool mtcclass::decCorrenteMax(){
	if(config_MTC.correnteMaxima > config_MTC.correnteMinima){
		config_MTC.correnteMaxima -= 1.0f;
		return true;
	}else return false;
}
bool mtcclass::decCorrenteMin(){
	if(config_MTC.correnteMinima > -1200){
		config_MTC.correnteMinima -= 1.0f;
		return true;
	}else return false;
}

//Funcao para verificar alarme do MTC_V
//		1 - Tensao acima do limite
//		2 - Tensao abaixo do limite
// 		0 - Tensao dentro do limite

int mtcclass::alarme_T (void)
	{
		if (variaveis_MTC.mediaV > config_MTC.tensaoMaxima)
			{
			
				if (flagHabilitaAlarmeMTC)
					//HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET); //saida digital PD7

			
				return 1;
			}
			
		if (variaveis_MTC.mediaV < config_MTC.tensaoMinima)
			{	
				if (flagHabilitaAlarmeMTC)
					//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_SET); //saida digital PD7

				return 2;
			}
		
		//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
		return 0;
	}

//Funcao para verificar alarme do MTC_C
//		1 - Corrente acima do limite
//		2 - Corrente abaixo do limite
// 		0 - Corrente dentro do limite
int mtcclass::alarme_I (void)
	{
		if (variaveis_MTC.mediaI > config_MTC.correnteMaxima)
			{			
				if (flagHabilitaAlarmeMTC)
					//HAL_GPIO_WritePin (GPIOD,GPIO_PIN_7, GPIO_PIN_SET);//saida digital PD7
			
				return 1;
			}
			
			if (variaveis_MTC.mediaI < config_MTC.correnteMinima)
				{				
					if (flagHabilitaAlarmeMTC)
						//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_SET);//saida digital PD7
				
					return 2;
				}
				

		//HAL_GPIO_WritePin (GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);		
		return 0;
	}
