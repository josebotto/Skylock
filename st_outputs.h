/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   st_outputs.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 14 de septiembre de 2017, 15:39
 * 
 * Log:
 *     Control of Leds and Rele outputs
 * 
 *     This module controls leds, relays, reader power control
 *     and Alarm.
 *     The data is stored in outputs_signaling_pcl [], configured
 *     initially and may be modified by another module.
 *     The function of st_outputs is to keep all exits updated
 *     with the information found in outputs_signaling_pcl []
 * 
 * =====================================================================
 */


#ifndef ST_OUTPUTS_H
#define	ST_OUTPUTS_H

    // Eventos de la FSM
    enum  Outputs_Event_Codes {                     
        evINI_OUTPUTS = 0,
        evCounters_OUTPUTS,
        evUpdate_OUTPUTS,
    };

    // Nombre de la FSM
    StateMachine		outputsStateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Outputs[], stCounters_Outputs[], stUpdate_Outputs[];

    struct Outputs outputs_signaling_pcl[] = {            // Almacena estado inicial de salidas y Leds
    //   output_name     id_port    num_pin  status_out       osc_time_off     counter_time_on_off
    //                                               osc_time_on       osc_cycle_time   counter_cycle_time
        { out_led_a1   ,  &PORTD  ,  0x40  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_led_a2   ,  &PORTD  ,  0x80  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_led_b1   ,  &PORTC  ,  0x01  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_led_b2   ,  &PORTC  ,  0x02  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },        
        { out_led_0    ,  &PORTD  ,  0x01  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 }, // Destella cuando es un OSDP_PD_Factory_Default. Ver st_monitoring.c 
        { out_led_1    ,  &PORTD  ,  0x02  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 }, // Destella cuando el Sistema no esta configurado. Ver st_monitoring.c   
        { out_led_2    ,  &PORTD  ,  0x04  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 }, 
        { out_led_3    ,  &PORTD  ,  0x08  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 }, // Estado del led: Encendido:Inicializando Destellando:Sistema activo. Ver st_monitoring.c
        { out_led_4    ,  &PORTD  ,  0x10  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 },
        { out_led_5    ,  &PORTD  ,  0x20  ,  0x15 ,  0x0005  ,  0x0005  ,  0x0000  ,  0x0000  ,  0x0000 }, // Estado del led: Destella:On_line Apagado:Off_Line. Ver st_monitoring.c      
        { out_rele_a   ,  &PORTE  ,  0x01  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_rele_b   ,  &PORTE  ,  0x02  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_rele_c   ,  &PORTE  ,  0x04  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_act_lec  ,  &PORTC  ,  0x04  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
        { out_alarm    ,  &PORTB  ,  0x20  ,  0x08 ,  0x001E  ,  0x0001  ,  0x001E  ,  0x0000  ,  0x0000 },
    };

    // Declatando funciones
    void Ini_OutputsStateMachine(void);         // Inicializa outputsStateMachine 
    void Counters_OutputsStateMachine(void);    // Actualiza los contadores que estan activos 
    void Update_OutputsStateMachine(void);      // Update segun tabla a las salidas previo analisis 
    void Generate_Signal(char aux_for);         // Se la utiliza para generar la señal de salida digital

#endif	/* ST_OUTPUTS_H */
