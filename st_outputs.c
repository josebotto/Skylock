#include <xc.h>
#include "def_hardware.h"
#include "def_global_var.h"
#include "state_machine.h"
#include "st_outputs.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Outputs[] = { 
	evINI_OUTPUTS         ,Ini_OutputsStateMachine      ,stCounters_Outputs,
};

TStateList	stCounters_Outputs[] = { 
	evCounters_OUTPUTS    ,Counters_OutputsStateMachine ,stUpdate_Outputs,
};

TStateList	stUpdate_Outputs[] = {
	evUpdate_OUTPUTS      ,Update_OutputsStateMachine  ,stCounters_Outputs,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_OutputsStateMachine(void)      // Inicializa outputsStateMachine
{
    char aux_for;
    char aux_data;
    char *aux_dir_memory;
 
    for(aux_for = 0; aux_for < NUM_OUT_AND_LEDS_PCL ; aux_for++) {          // Recorre toda la tabla actualizando las salidas

        aux_data = outputs_signaling_pcl[aux_for].status_out;               // Consulta el bit del estado actual 
        aux_data &= ACTUAL_STATE;
        aux_dir_memory = outputs_signaling_pcl[aux_for].id_port;            // Consulta la direccion del port

        if(aux_data)                                                        // Dependiendo del valor del estado actual lo carga en el port/pin
            *aux_dir_memory |= outputs_signaling_pcl[aux_for].num_pin;
        else
            *aux_dir_memory &= ~outputs_signaling_pcl[aux_for].num_pin;
    }

    stm_msg2statemachine(evCounters_OUTPUTS, &outputsStateMachine);         // Avanza hacia el proximo estado
    return;
}

void Counters_OutputsStateMachine(void)     // Actualiza los contadores que estan activos 
{
    char aux_for;
    
    if(timers[0x01].status_timer & TIMER_OUT){   // Consulta el estado del flag
        
        timers[0x01].status_timer |= TIMER_TRIGGER;                         // Produce el redisparo del timer para comenzar un nuevo 
                                                                            // conteo de 100 mseg

        for(aux_for = 0; aux_for < NUM_OUT_AND_LEDS_PCL ; aux_for++) {      // Actualiza todos los counters de la tabla
            if(outputs_signaling_pcl[aux_for].counter_time_on_off != 0x00)
                --outputs_signaling_pcl[aux_for].counter_time_on_off;

            if(outputs_signaling_pcl[aux_for].counter_cycle_time != 0x00)
                --outputs_signaling_pcl[aux_for].counter_cycle_time;
            }
    } 
    
    stm_msg2statemachine(evUpdate_OUTPUTS, &outputsStateMachine);               // Avanza hacia el proximo estado
    return;
}

void Update_OutputsStateMachine(void)       // Update segun tabla a las salidas previo analisis 
{
    char aux_for;
    char aux_data;
    char *aux_dir_memory;

    for(aux_for = 0; aux_for < NUM_OUT_AND_LEDS_PCL ; aux_for++) {              // Actualizacion de todas las salidas  

        aux_data = outputs_signaling_pcl[aux_for].status_out;
        aux_data &= TEMPORIZE;                                                  // Prepara consulta por salida temporizada o permanente
        aux_dir_memory = outputs_signaling_pcl[aux_for].id_port;                // Carga direccion del port 

        if(aux_data){                                                           // Consulta  
            aux_data = outputs_signaling_pcl[aux_for].status_out;               // Es temporizada, ahora prepara una nueva consulta por si es la primera vez
            aux_data &= INI_CYCLE_OSC;                                          // que pasa por aqui en este nuevo ciclo

            if(!aux_data){                                                      // Si es la primera vez avanza dentro del if
                outputs_signaling_pcl[aux_for].status_out |= INI_CYCLE_OSC;     // Pone en 1 el bit INI_CYCLE_OSC del status_out para indicar que paso

                outputs_signaling_pcl[aux_for].counter_cycle_time = outputs_signaling_pcl[aux_for].osc_cycle_time; // Carga counter

                aux_data = outputs_signaling_pcl[aux_for].status_out;           // Consulta cual sera el estado que debera tener la salida al comenzar a oscilar
                aux_data &= OSC_INI_STATE;                                      // y posteriormente la actualiza 
                if(aux_data){
                    outputs_signaling_pcl[aux_for].counter_time_on_off = outputs_signaling_pcl[aux_for].osc_time_on;    // En caso que la salida comience en 1
                    *aux_dir_memory |= outputs_signaling_pcl[aux_for].num_pin;                                          // Actualiza salida
                    outputs_signaling_pcl[aux_for].status_out |= 0x01;                                                  // Actualiza status.out
                } 
                else{
                    outputs_signaling_pcl[aux_for].counter_time_on_off = outputs_signaling_pcl[aux_for].osc_time_off;   // En caso que la salida comience en 0
                    *aux_dir_memory &= ~outputs_signaling_pcl[aux_for].num_pin;                                         // Actualiza salida
                    outputs_signaling_pcl[aux_for].status_out &= 0xFE;                                                  // Actualiza status.out  
                } 
            } 

            aux_data = outputs_signaling_pcl[aux_for].status_out;               // Prepara consulta si la salida tendra una oscilacion repetitiva
            aux_data &= OSC_CYCLE_REPETITIVE;
            if(aux_data)
                Generate_Signal(aux_for);                                       // Se establece una oscilacion repetitiva
            else{
                if(outputs_signaling_pcl[aux_for].counter_cycle_time == 0x00){  // Se establece una oscilacion no repetitiva. Se consulta al counter
                    aux_data = ~INI_CYCLE_OSC;                                    
                    outputs_signaling_pcl[aux_for].status_out &= aux_data;      // Pone a cero el bit definido por INI_CYCLE_OSC. Disponibilidad prox oscilacion si la hay 

                    aux_data = ~TEMPORIZE;                                      // Pone a cero el bit indicado por TEMPORIZE, indicando una salida permanente 
                    outputs_signaling_pcl[aux_for].status_out &= aux_data;

                    aux_data = outputs_signaling_pcl[aux_for].status_out;       // Consulta el estado permanente que debera tener la salida   
                    aux_data &= PERMANENT_STATE;
                    if(aux_data){                                                  
                        *aux_dir_memory |= outputs_signaling_pcl[aux_for].num_pin; // Salida debe ser 1 
                        outputs_signaling_pcl[aux_for].status_out |= 0x01;         // Actualiza la tabla
                    } 
                    else{
                        *aux_dir_memory &= ~outputs_signaling_pcl[aux_for].num_pin;  // Salida debe ser 0
                        outputs_signaling_pcl[aux_for].status_out &= 0xFE;           // Actualiza la tabla
                    } 
                } 
                else
                    Generate_Signal(aux_for);                                   // La salida debe seguir oscilando dado que no se alcanzo el tiempo
            } 
        } 
        else{
            aux_data = ~INI_CYCLE_OSC;                                          // Pone a cero el bit definido por INI_CYCLE_OSC. Se hace por seguridad en el caso
            outputs_signaling_pcl[aux_for].status_out &= aux_data;              // si hubo un salto del estado temporizado al permanete 

            aux_data = outputs_signaling_pcl[aux_for].status_out;               // Consulta cual es el valor en forma permanente que debe tener la salida
            aux_data &= PERMANENT_STATE;

            if(aux_data)                                                        // Actualiza el valor 
                *aux_dir_memory |= outputs_signaling_pcl[aux_for].num_pin;      // En el caso de ser 1 
            else
                *aux_dir_memory &= ~outputs_signaling_pcl[aux_for].num_pin;     // En el caso de ser 0
        } 
    }

    stm_msg2statemachine(evCounters_OUTPUTS, &outputsStateMachine);             // Avanza hacia el proximo estado 
    return;
}

void Generate_Signal(char aux_for)      // Se la utiliza para generar la señal de salida digital 
{
    char aux_data;
    char *aux_dir_memory;

    if(outputs_signaling_pcl[aux_for].counter_time_on_off == 0x00){             // Consulta si se cumplio el semiperiodo de la señal
        aux_dir_memory = outputs_signaling_pcl[aux_for].id_port;                // Se cumplio el tiempo. Carga la direccion del port. 
        aux_data = outputs_signaling_pcl[aux_for].status_out;                   // Consulta cual es el valor actual de la salida 
        aux_data &= ACTUAL_STATE;

        if(aux_data){                                                           // Si la salida esta en 1 continua dentro del if 
            outputs_signaling_pcl[aux_for].counter_time_on_off = outputs_signaling_pcl[aux_for].osc_time_off;  // Actualiza counter con valor off
            outputs_signaling_pcl[aux_for].status_out &= 0xFE;                                                 // Actualiza status_out con valor de bit a 0
            *aux_dir_memory &= ~outputs_signaling_pcl[aux_for].num_pin;                                        // Actualiza salida con valor de bit a 0
        }
        else{                                                                      // La salida esta en 0 continua dentro del else
            outputs_signaling_pcl[aux_for].counter_time_on_off = outputs_signaling_pcl[aux_for].osc_time_on;    // Actualiza counter con valor on
            outputs_signaling_pcl[aux_for].status_out |= 0x01;                                                  // Actualiza status_out con valor de bit a 1
            *aux_dir_memory |= outputs_signaling_pcl[aux_for].num_pin;                                          // Actualiza salida con valor de bit a 1
        } 
    }

    return;
}
