/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:    st_inputs_analogical.h
 * Author:  JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 4 de octubre de 2017, 11:00
 * 
 * Log:
 *     Control of analogical inputs
 *     
 *     This module reads two analog inputs, stores its status current.
 *      
 *     The analog inputs serve the two ACS712. 
 *     Another module will take care of that data.
 * 
 * =====================================================================
 */


#ifndef ST_INPUTS_ANALOGICAL_H
#define	ST_INPUTS_ANALOGICAL_H

    // Eventos de la FSM
    enum  Inputs_Analog_Event_Codes {                     
        evINI_INPUTS_ANALOG = 0,
        evSampleTime_INPUTs_ANALOG,
        evRead_SensorA_INPUTs_ANALOG,
        evRead_SensorB_INPUTs_ANALOG,
    };

    // Nombre de la FSM
    StateMachine		inputs_analog_StateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Inputs_analog[], stSampleTime_Inputs_analog[], stRead_SensorA_Inputs_analog[], stRead_SensorB_Inputs_analog[];

    // Estructura y variables para entradas externas
    struct Inputs_Analog  inputs_analog_pcl[] = {   // Define la extructura de entradas externas y establece valor inicial
        { in_ana_ra , &PORTA , 0x01 , 0x0000 , 0x00, 0x0000, 0x0000 },          
        { in_ana_rb , &PORTA , 0x02 , 0x0000 , 0x00, 0x0000, 0x0000 },        
    };

    // Declarando funciones
    void Ini_InputsAnalogStateMachine(void);        // Inicializa InputsAnalogicalStateMachine 
    void Time_InputsAnalogStateMachine(void);       // Chequea si es tiempo de muestreo
    void SensorA_InputsAnalogStateMachine(void);    // Actualiza valor sensor A
    void SensorB_InputsAnalogStateMachine(void);    // Actualiza valor sensor B

#endif	/* ST_INPUTS_ANALOGICAL_H */
