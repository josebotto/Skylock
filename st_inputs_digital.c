#include <xc.h>
#include "def_hardware.h"
#include "def_global_var.h"
#include "state_machine.h"
#include "st_inputs_digital.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Inputs[] = { 
	evINI_INPUTS         ,Ini_InputsStateMachine       ,stRead_Inputs,
};

TStateList	stRead_Inputs[] = {
	evRead_INPUTS        ,Read_InputsStateMachine 	   ,stUpdate_Inputs,
};

TStateList	stUpdate_Inputs[] = {
    evUpdate_INPUTS      ,Update_InputsStateMachine    ,stRead_Inputs,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_InputsStateMachine(void)          // Inicializa InputsStateMachine
{
    char aux_for;                  
    char aux_data;
    char *aux_dir_memory;

    for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++) {     // Recorre la inputs_pcl[] y guarda el estado  
                                                             // actual de la entrada en los bits 0 y 1 de cada status_in
        aux_dir_memory = inputs_pcl[aux_for].id_port;        // Guarda direccion del Port
        aux_data = *aux_dir_memory;                          // Guarda contenido del Port en aux_data      
        aux_data &= inputs_pcl[aux_for].num_pin;             // Prepara para consultar el estado de la entrada

        if(aux_data)                                         // Definido el valor de la entrada lo guarda en status_in el mismo valor en los bits 0 y 1 
            inputs_pcl[aux_for].status_in |= 0x03;           // Guarda 1
        else
            inputs_pcl[aux_for].status_in &= 0xFC;           // Guarda 0
    }

    stm_msg2statemachine(evRead_INPUTS, &inputsStateMachine);  // Pasa al siguiente estado
    return;
}

void Read_InputsStateMachine(void)      // Lecturas de las entradas digitales
{
    char aux_for;
    char aux_data;
    char *aux_dir_memory;
 
    for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++) {   // Recorre la inputs_pcl[] y guarda el estado
                                                           // actual de la entrada en el bits 0 
        aux_dir_memory = inputs_pcl[aux_for].id_port;          // Guarda direccion del Port
        aux_data = *aux_dir_memory;                            // Guarda contenido del Port en aux_data
        aux_data &= inputs_pcl[aux_for].num_pin;               // Prepara para consultar el estado de la entrada

        if(aux_data)                                           // Definido el valor de la entrada lo guarda en status_in bit 0
            inputs_pcl[aux_for].status_in |= 0x01;             // Si la entrada esta en 1 logico
        else
            inputs_pcl[aux_for].status_in &= 0xFE;             // Si la entrada esta en 0 logico

    }

    stm_msg2statemachine(evUpdate_INPUTS, &inputsStateMachine); // Pasa al siguiente estado
    return;
}

void Update_InputsStateMachine(void)       // Update a la tabla previo analisis de sus entradas
{
    char aux_for;
    char aux_data;
     
    for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++) {        // Recorre la inputs_pcl[] para su actualizacion

        aux_data = inputs_pcl[aux_for].status_in;               // Guarda el contenido de status_in para su analisis 
        aux_data &= 0x03;                                       // Se queda con los dos primeros bits(LSB)  

        if(inputs_pcl[aux_for].debounced == 0xFF){              // Chequea si el contador debounced de cada entrada esta activo  0xFF inactivo
            if((aux_data != 0x00)&(aux_data != 0x03))           // Chequea si el estado anterior de la entrada es igual al estado actual
                inputs_pcl[aux_for].debounced = DEBOUNCED_TIME; // Como son diferentes inicializa el contador debounced de esa entrada
        }
        else{
            if(inputs_pcl[aux_for].debounced == 0x00){                  // Chequea si el contador debounced llego a cero 
                if((aux_data != 0x00)&(aux_data != 0x03)){              // Si el estado anterior y el actual son los mismos sera considerado como ruido
                    aux_data = ~aux_data;                               // No es un ruido. cambia de posicion los bits 0 y 1                        
                    aux_data &= 0x03;                                   // Solo considera los bits 0 y 1
                    inputs_pcl[aux_for].status_in &= 0xFC;              // Pone en 0 los Bits 0 y 1
                    inputs_pcl[aux_for].status_in |= aux_data;          // Actualiza los bits 0 y 1 con la info de aux_data
                    aux_data = inputs_pcl[aux_for].status_in;           // Guarda la info en la variable auxiliar para realizar nuevas comparaciones
                    aux_data &= REPORT_CHANGE;                          // Prepara para consultar si el cambio se debe reportar
                    if(aux_data)                                        // Consulta
                        inputs_pcl[aux_for].status_in |= REPORT_PD;     // El cambio sera reportado al poner en 1 el bit de status_in 
                    else
                        inputs_pcl[aux_for].status_in &= ~REPORT_PD;    // El cambio no sera reportado al poner en 0 el bit de status_in
                }
                inputs_pcl[aux_for].debounced = 0xFF;                   // El contador se pasa a estado inactivo
            } 
            else{
                if(timers[0x00].status_timer & TIMER_OUT)               // Como el contador debounced no llego a cero chequea la salida del timer
                                                                        // para poderlo decrementar
                    --inputs_pcl[aux_for].debounced;                 
            }    
        }
    }

    if(timers[0x00].status_timer & TIMER_OUT)       // Consulta la salida del timer, si es 1 debera realizar un trigger para generar un nuevo ciclo.
        timers[0x00].status_timer |= TIMER_TRIGGER; // Produce el redisparo del timer para comenzar un nuevo conteo de 1 mseg.
    
    stm_msg2statemachine(evRead_INPUTS, &inputsStateMachine);         // Pasa al siguiente estado
    return;
}
