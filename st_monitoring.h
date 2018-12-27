/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:    st_monitoring.h
 * Author:  JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 17 de octubre de 2018, 16:58
 * 
 * Log:
 *     Initializes values of PD-ID and speed in existing Baud Rate in 
 *     eeprom for your first communication.
 *     Then monitor if any directive appears for configuration change 
 *     or reset
 * 
 * =====================================================================
 */


#ifndef ST_MONITORING_H
#define	ST_MONITORING_H

    // Eventos de la FSM
    enum  Monitoring_Event_Codes {                     
                            evINI_MONITORING = 0,
                            evTEST_MONITORING,
    };

    // Nombre de la FSM
    StateMachine		monitoringStateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Monitoring[], stTest_Monitoring[];

    // Variables y constantes


    // Declarando funciones
    void Ini_MonitoringStateMachine(void);      // Inicializacion de la FSM monitoringStateMachine
    void Test_MonitoringStateMachine(void);     // Monitorea y define  

#endif	/* ST_MONITORING_H */
