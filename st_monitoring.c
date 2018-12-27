#include <xc.h>
#include "def_global_var.h"
#include "state_machine.h"
#include "eeprom.h"
#include "osdp.h"
#include "st_monitoring.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Monitoring[] = { 
	evINI_MONITORING    ,Ini_MonitoringStateMachine     ,stTest_Monitoring   ,
};

TStateList	stTest_Monitoring[] = {
	evTEST_MONITORING   ,Test_MonitoringStateMachine    ,stTest_Monitoring ,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_MonitoringStateMachine(void)       // Inicializacion de la FSM monitoringStateMachine
{
    flag_monitoring = 0;                                // Inicializa flag
    
    // Sistema Activo. Señalizacion.
    outputs_signaling_pcl[7].status_out |= 0x1F;        // Al inicializar el sistema out_led_3 esta encendido fijo, luego comienza a destellar
                                                        // indicando que el sistema esta activo. 

    // Sistema No configurado.
                                // Luego de cada Power-on o Reset la placa necesitara recibir su configuracion por parte del CP(Control Panel),
                                // dstella out_led_2 indicando que el sistema no esta configurado.  
                                // En este estado solamente se espera recibir el comando OSDP_CONFIG
    outputs_signaling_pcl[5].status_out |= 0x1F;        // Destella out_led_1
    flag_system_configuration = 0;                      // Con este valor indica que el sistema no esta configurado pues al pasar por este estado de esta FSM
                                                        // se viene de un Power-on o Reset
    
    stm_msg2statemachine(evTEST_MONITORING, &monitoringStateMachine);   // Pasa al siguiente estado
    return;
}

void Test_MonitoringStateMachine(void)       // Monitorea Comandos - Touch Switch - Señalizacion PD-ID - On Line status
{
    switch(flag_monitoring) {   // Analiza variable flag_monitoring por si hubo algún comando que necesite RESET
        case 0x00:          // Sin novedades de eventos 
            break;

        case 0x01:          //  Comando que lo ha solicitado: OSDP_COMSET. Configura con nuevos parametros indicados en el comando. 
            if(aux_flag_RXoTX==0x00){           // Espera que finalice la comunicacion del mensaje OSDP informando que el comando fue aceptado antes 
                                                // de realizar los cambios. aux_flag_RXoTX es una copia de flag_RXoTX cuando es igual a 0 finalizo la TX
                
                write_eeprom(USERD_PD_ID, aux_pd_id);               // Actualiza PD-ID
                write_eeprom(USERD_BR_SPBRG, aux_baut_rate);        // Actualiza Baud Rate SPBRG 
                write_eeprom(USERD_BR_SPBRGH, aux_baut_rate_h);     // Actualiza Baud Rate SPBRGH

                RESET();                                            // Reset de Placa
            }
            break;
            
        case 0x02:          // Comando que lo ha solicitado: OSDP_FACTORY. Configura parametros indicados en Factory Default.
            if(aux_flag_RXoTX==0x00){   // Espera que finalice la comunicacion del mensaje OSDP informando que el comando fue aceptado antes 
                                        // de realizar los cambios. aux_flag_RXoTX es una copia de flag_RXoTX cuando es igual a 0 finalizo la TX
                
                write_eeprom(USERD_PD_ID, read_eeprom(FACTORYD_PD_ID));             // Actualiza PD-ID
                write_eeprom(USERD_BR_SPBRG, read_eeprom(FACTORYD_BR_SPBRG));       // Actualiza Baud Rate SPBRG 
                write_eeprom(USERD_BR_SPBRGH, read_eeprom(FACTORYD_BR_SPBRGH));     // Actualiza Baud Rate SPBRGH
                
                RESET();                                                            // Reset de Placa
            }
            break;

		case 0x03:          //  Comando que lo ha solicitado: OSDP_RESET(Comando propietario Skylock). Generador de reset.
            if(aux_flag_RXoTX==0x00){   // Espera que finalice la comunicacion del mensaje OSDP informando que el comando fue aceptado antes 
                                        // de realizar los cambios. aux_flag_RXoTX es una copia de flag_RXoTX cuando es igual a 0 finalizo la TX

                RESET();                // Reset de Placa
            }
            break;

        default:                        // Comandos no reconocido
            break;
    }

    // Modo configuracion PD_ID. Señalización con out_led_1. Indica el tipo de valor que contiene PD_ID
    if(PD_ID == OSDP_PD_Factory_Default)                       // Consulta por el tipo de valor que tiene PD_ID
        outputs_signaling_pcl[4].status_out |= 0x1A;           // Es un OSDP_PD_Factory_Default, destella out_led_0
    else
        outputs_signaling_pcl[4].status_out &= 0xF9;           // El valor de PD_ID varia entre 0x00 y 0x7D, el out_led_0 queda encendido fijo
                                                               // valor configurado por el usuario 
    
    // Modo estado On_Line. Señalización con out_led_5. Es utilizado para indicar si el sistema esta en modo On_Line.
                                                        // Se relaciona con st_timer.h y st_mpnotoring.h
	if((timers[0x02].status_timer & TIMER_OUT) && (flag_system_configuration == 0x01))  // Chequea si la salida del timer es 1 y si la placa fue configurada por el PD
                                                                                        // despues del ultimo reset, entonces produce el reset del sistema debido que se
                                                                                        // sobre paso el tiempo maximo de No comunicacion entre el PD(Panel de control)
                                                                                        // con esta placa. Definido en ON_LINE (ver def_global.h)
		flag_monitoring = 0x03;
															
	if(timers[0x02].status_timer & TIMER_OUT)               // Chequea el estado de la salida del timer y define el estado del out_led_5       
		outputs_signaling_pcl[9].status_out &= ~TEMPORIZE;  // Apaga out_led_5 indicando que el sistema esta Off-Line		
	else													
		outputs_signaling_pcl[9].status_out |= TEMPORIZE;   // Destella out_led_5 indicando que el sistema esta On-Line		

    // Modo Touch_Factory_Default. Es utilizado para configurar manualmente el estado Factory Default. Esto se logra al presionar el touch switch durante un tiempo mayor 
                                // al definido en TOUCH_SWITCH. El touch switch se podria utilizar para propositos generales siendo presionado con tiempos menores.
                                    
    if( !(inputs_pcl[0x07].status_in & 0x01) ){             // Consulta si esta presionado el touch switch
		if(timers[0x03].status_timer & TIMER_OUT){          // En caso de estar presionado el touch switch chequea si se activo la salida del timer 
			flag_monitoring = 0x02;                         // Produce el Factory Default
		}
	}
	else
		timers[0x03].status_timer |= TIMER_TRIGGER;         // Al no estar presionado el Touch switch genera Trigger en el timer  
    
    stm_msg2statemachine(evTEST_MONITORING, &monitoringStateMachine);   // Permanecera siempre en este estado
    return;
}
