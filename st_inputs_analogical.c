#include <xc.h>
#include "def_hardware.h"
#include "def_global_var.h"
#include "state_machine.h"
#include "st_inputs_analogical.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Inputs_analog[] = { 
	evINI_INPUTS_ANALOG          ,Ini_InputsAnalogStateMachine     ,stSampleTime_Inputs_analog   ,
};

TStateList	stSampleTime_Inputs_analog[] = {
	evSampleTime_INPUTs_ANALOG   ,Time_InputsAnalogStateMachine    ,stRead_SensorA_Inputs_analog ,
};

TStateList	stRead_SensorA_Inputs_analog[] = {
    evRead_SensorA_INPUTs_ANALOG ,SensorA_InputsAnalogStateMachine ,stRead_SensorB_Inputs_analog ,
};

TStateList	stRead_SensorB_Inputs_analog[] = {
    evRead_SensorB_INPUTs_ANALOG ,SensorB_InputsAnalogStateMachine ,stSampleTime_Inputs_analog   ,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_InputsAnalogStateMachine(void)           // inicializa InputsAnalogStateMachine
{
    // Inicializa conversor A/D
    ADCON0 = 0x00;       // Selecciona por default AN0, Modulo conversor esta desactivado   
    ADCON1 = 0x0F;       // Define como Vref a VCC y GND. Define a AIN0 y AN1 las entradas utilizadas
    ADCON2 = 0xA6;       // Define justificacion derecha, T adquisicion 8 TAD y Tiempo de coversion 1.6 useg
    
    
    // Interpretacion Lectura Analogica PIC     Resolución= Vref/((2^^10)-1) = Vref/1023=5V/1023 = 4,9mV
    //                                          Tension medida es ((ADRESH+ADRESL)* Resolución)
    
    // Analisis dato ACS712ELCTR-05B-T  Sensibilidad 0.185V/A segun fabricante
    //                                  Para corregirla a cada circuito integrado en particular si es necesario
    //                                  se deberia realizar una medicion externa
    //                                  con voltimetro, amperimetro y dos cargas conocida
    //                                  Primero  con una I baja  se obtendra V1 e I1  
    //                                  Segundo  con una I alta  se obtendra V2 e I2  
    //                                  Sensibilidad_Calculada = (V2-V1)/(I2-I1)
    //    
    //                                  I= (Vin - 2.5V)/Sensibilidad              segun fabricante
    //                                  I= (Vin - 2.5V)/Sensibilidad_Calculada    si se la quiere corregir
    
    
    stm_msg2statemachine(evSampleTime_INPUTs_ANALOG, &inputs_analog_StateMachine); // Pasa al siguiente estado
    return;
}

void Time_InputsAnalogStateMachine(void)          // Chequea si es tiempo de muestreo
{
    int a;
    a=0;
    stm_msg2statemachine(evRead_SensorA_INPUTs_ANALOG, &inputs_analog_StateMachine); // Pasa al siguiente estado
    return;
}

void SensorA_InputsAnalogStateMachine(void)       // Actualiza valor sensor A
{
    int a;
    a=0;
    stm_msg2statemachine(evRead_SensorB_INPUTs_ANALOG, &inputs_analog_StateMachine); // Pasa al siguiente estado
    return;
}

void SensorB_InputsAnalogStateMachine(void)       // Actualiza valor sensor B
{
    int a;
    a=0;
    stm_msg2statemachine(evSampleTime_INPUTs_ANALOG, &inputs_analog_StateMachine); // Pasa al siguiente estado
    return;
}
