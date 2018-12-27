#include <xc.h>

/**/#include "def_hardware.h"

#include "def_global_var.h"
#include "state_machine.h"
#include "st_timers.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Timers[] = { 
	evINI_TIMERS       ,Ini_TimersStateMachine        ,stUpgrade_Timers ,
};

TStateList	stUpgrade_Timers[] = {
	evUPGRADE_TIMERS   ,Upgrade_TimersStateMachine    ,stUpgrade_Timers ,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_TimersStateMachine(void)   // Inicializacion de la FSM TimersStateMachine
{
    timers[0x02].pre_scale = ON_LINE;       // Carga al pre scale del timer el tiempo On_line
 	timers[0x03].pre_scale = TOUCH_SWITCH;  // Carga al pre scale del timer el tiempo Timer Touch_Factory_Default
   
    stm_msg2statemachine(evUPGRADE_TIMERS, &timersStateMachine);   // Pasa al siguiente estado
    return;
}

void Upgrade_TimersStateMachine(void)   // Actualiza todos los temporizadores  
{
    unsigned char aux_for;
	
    if(flag_timer_interrup){    // Consulta si se produjo una interrupucion  
		flag_timer_interrup = 0x00; 						// Reset variable, espera la proxima interrupcion

		for(aux_for = 0; aux_for < NUM_TIMERS ; aux_for++){ // Actualiza contadores en todos los timers activos  
				if(timers[aux_for].regressive_counter)		// Si es distinto de 0x00 el timer esta activo y debe decrementar 
					timers[aux_for].regressive_counter--;
		}
	}
	else{
		for(aux_for = 0; aux_for < NUM_TIMERS ; aux_for++){     // Actualizacion de todos los timers 

			if(timers[aux_for].status_timer & TIMER_RESET){                         // Consulta si se solicito un Reset 
                                                                                        
				timers[aux_for].status_timer &= ~TIMER_RESET;                       // Pone a 0 el bit de RESET 
				timers[aux_for].status_timer &= ~TIMER_TRIGGER;                           // Pone a 0 el bit de TIMER_TRIGGER

				if(timers[aux_for].status_timer & TIMER_LEVEL)					    // Consulta TIMER_LEVEL para poner su valor en el bit TIMER_OUT   					
					timers[aux_for].status_timer |= TIMER_OUT;                      // Pone a 1 el bit de TIMER_OUT
				else
					timers[aux_for].status_timer &= ~TIMER_OUT;                     // Pone a 0 el bit de TIMER_OUT

				timers[aux_for].regressive_counter = 0x00000000;                    // Reset contador para indicar que el timer esta parado
			}
			else{
				if(timers[aux_for].status_timer & TIMER_TRIGGER){					       	// Consulta si se solicito un TIMER_TRIGGER 

					timers[aux_for].status_timer &= ~TIMER_TRIGGER;                       // Pone a 0 el bit de TIMER_TRIGGER
					
					if(timers[aux_for].status_timer & TIMER_LEVEL)                    // Consulta TIMER_LEVEL antes de iniciar el timer fijando valor del bit TIMER_OUT 					       					
						timers[aux_for].status_timer &= ~TIMER_OUT;                 // 0 es el valor que va a tomar el timer durante la temporizacion. Opuesto a su estado final.
					else
						timers[aux_for].status_timer |= TIMER_OUT;                  // 1 es el valor que va a tomar el timer durante la temporizacion. Opuesto a su estado final.

					timers[aux_for].regressive_counter = timers[aux_for].pre_scale; // Carga el pre_scale en el contador regresivo, se activa el timer    
				}
				else{
					if(!timers[aux_for].regressive_counter){                        // Consulta si el contador regresivo llego a 0. En caso afirmativo final temporizacion.     					
                                                                        
					if(timers[aux_for].status_timer & TIMER_LEVEL)					// Consulta TIMER_LEVEL para poner su valor en el bit TIMER_OUT       					
							timers[aux_for].status_timer |= TIMER_OUT;              // Pone a 1 el bit de TIMER_OUT
						else
							timers[aux_for].status_timer &= ~TIMER_OUT;             // Pone a 0 el bit de TIMER_OUT
					}
				}		
			}		
		}	
	}	
    
    stm_msg2statemachine(evUPGRADE_TIMERS, &timersStateMachine);        // Pasa al mismo estado
    return;
}
