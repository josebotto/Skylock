/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   st_timers.h
 * Author:  JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 13 de noviembre de 2018, 11:16
 * 
 * Log:
 *     Esta FSM contiene la tabla de temporizadores del sistema. 
 *     Su fuente de sincronizacion es flag_timer_interrup que se 
 *     encuentra en interrup.h 
 * 
 * =====================================================================
 */


#ifndef ST_TIMERS_H
#define	ST_TIMERS_H

    // Eventos de la FSM
    enum  Timers_Event_Codes {                     
                            evINI_TIMERS = 0,
                            evUPGRADE_TIMERS,
    };

    // Nombre de la FSM
    StateMachine		timersStateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Timers[], stUpgrade_Timers[];

    // Estructura y variables para entradas externas
    struct Table_Timers  timers[] = {                           // Configura cada timer en Timers[]. Definida en def_global.h
    //    status_timer   pre_scale    regressive_counter 
        { 0x84         , 0x00000001 , 0x00000000         },		// Timer utilizado en st_inputs_digital.h / actualiza cada 10mseg
        { 0x84         , 0x0000000A , 0x00000000         },     // Timer utilizado en st_outputs.h / actualiza cada 100mseg
        { 0x84         , 0x0000012C , 0x00000000         },		// Timer utilizado en st_monitoring.h / Time Online actualiza cada 3 seg
        { 0x84         , 0x00000BB8 , 0x00000000         },		// Timer utilizado en st_monitoring.h / Timer Touch_Factory_Default actualiza cada 30 seg
    };

    // Declarando funciones
    void Ini_TimersStateMachine(void);      // Inicializacion de la FSM TimersStateMachine
    void Upgrade_TimersStateMachine(void);  // Actualiza todos los temporizadores  

#endif	/* ST_TIMERS_H */
