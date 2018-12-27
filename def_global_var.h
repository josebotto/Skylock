/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   def_global_var.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 25 de agosto de 2017, 09:30
 * 
 * Log:
 *     Global variables and constants.  
 *  
 * =====================================================================
 */


#ifndef DEF_GLOBAL_VAR_H
#define	DEF_GLOBAL_VAR_H

    // Definiciones útiles
    #define ON      1
    #define OFF     0
    #define IN      1
    #define OUT     0
    #define TRUE    1
    #define FALSE   0
    #define NULL    (0)


    // ******* Tomado de software Placa ML02
    // Entrada-Salida de datos
    #define	L1_CK_MSK     0x01       // LEC_A  W0-CLK
    #define	L2_CK_MSK     0x04       // LEC_B  W0-CLK
    #define	L1_DATA_MSK   0x02       // LEC_A  W1-DATA
    #define	L2_DATA_MSK   0x08       // LEC_B  W1-DATA
    #define	RDRS_MSK      (L1_DATA_MSK | L2_DATA_MSK | L1_CK_MSK | L2_CK_MSK )

    typedef unsigned char byte;
    typedef unsigned int  word;
    typedef unsigned long dword;
    // ******* Tomado de software Placa ML02

    
    // Constantes globales    
    extern enum name_inputs_and_outputs_pcl {
                              // Utilizado para enumeras entradas y salidas
                              // de la placa. 
        
        flag_ini_input_dig = 0x00,  // Utilizado para indicar comienzo identificacion de entradas digitales
        inaa,                       // Entrada lectora A In A
        inab,                       // Entrada lectora A In B
        inba,                       // Entrada lectora B In A
        inbb,                       // Entrada lectora B In B
        i_aux1,                     // Entrada auxiliar 1 
        i_aux2,                     // Entrada auxiliar 2
        tamp,                       // Entrada Tamper
        swconf,                     // Entrada Switch Configuracion
        flag_end_input_dig,         // Utilizado para indicar finalizacion identificacion de entradas digitales
                
        flag_ini_input_ana = 0x10,  // Utilizado para indicar comienzo identificacion de entradas analogicas
        in_ana_ra,                  // Entrada Analogica sobre Rele A
        in_ana_rb,                  // Entrada Analogica sobre Rele B
        flag_end_input_ana,         // Utilizado para indicar finalizacion identificacion de entradas analogicas
                
        flag_ini_reader = 0x20,     // Utilizado para indicar comienzo identificacion de readers
        reader_a,                   // Lectora A
        reader_b,                   // Lectora B
        flag_end_reader,            // Utilizado para indicar finalizacion identificacion de readers
                
        flag_ini_out = 0x30,        // Utilizado para indicar comienzo identificacion de salidas
        out_led_a1,                 // Led lectora A Verde
        out_led_a2,                 // Led lectora A Rojo
        out_led_b1,                 // Led lectora B Verde
        out_led_b2,                 // Led lectora B Rojo
        out_led_0,                  // Led de señalizacion(Blanco). Indica valor PD-ID. Destellando: 7E (Factory Default)-Fijo: Otro valor dentro de los permitidos  
        out_led_1,                  // Led de señalizacion(Blanco). Destella indicando sistema activo
        out_led_2,                  // Led de señalizacion
        out_led_3,                  // Led de señalizacion
        out_led_4,                  // Led de señalizacion
        out_led_5,                  // Led de señalizacion(Blue). Indica conexion con PCC (Apagado: No conexion / Destellando: ON-Line)
        out_rele_a,                 // Rele lectora A 
        out_rele_b,                 // Rele lectora B
        out_rele_c,                 // Rele C
        out_act_lec,                // Habilita Tension de Alimentacion a ambas lectoras
        out_alarm,                  // Salida de Alarma 12Vcc
        flag_end_out,               // Utilizado para indicar finalizacion identificacion de salidas
    };
    
    
    // Estructura y variables utilizadas por st_inputs_analogical.h
    #define NUM_INPUTS_ANALOG   0x02       // Indica cantidad de entradas analogicas que posee la placa PCL
    #define SAMPLE_TIME         0x32       // Tiempo a decrementar. Se utiliza para determinar el periodo de muestreo(500 mseg)
    #define SAMPLES_NUM         0x32       // Cantidad de muestras a tomar por sensor. (50 muestras)
    
    typedef struct Inputs_Analog{
        char   input_analog_name;               // Identificacion de la entrada
        unsigned int   id_analog_port;          // Direccion del Port
        char  num_analog_pin;                   // Mascara para el nro de pin

        unsigned int   actual_analog_value;     // Ultimo valor tomado en la entrada

        char   status_analog_in;
            #define REPORT_ALARM_CHANGE    0x01     // Indica reportar alarmas 0:No Reporta 1:Reporta
            #define REPORT_ALARM_PD        0x02     // Indica si se produjo una alarma 0:No se produjo 1:Se produjo
            #define REPORT_ALARM_MINIMUN   0x04     // Indica alarma por valor minimo
            #define REPORT_ALARM_MAXIMUN   0x08     // Indica alarma por valor maximo

        unsigned short int alarm_minimum_value;     // Valor minino aceptado como normal
        unsigned short int alarm_maximum_value;     // Valor maximo aceptado como normal
    };
    struct Inputs_Analog  inputs_analog_pcl[];      // Define la extructura de entradas externas y establece valor inicial
    
    
    // Estructura y variables utilizadas por st_inputs_digital.h y osdp_functions.h
    #define NUM_IN_PCL       0x08       // Indica la cantidad de entradas digitales que posee la placa PCL
    #define DEBOUNCED_TIME   0x14       // Tiempo a decrementar. Valor debe multiplicarse por 0.010 seg

    typedef struct Inputs{
        char  input_name;                   // Identificacion de la entrada
        unsigned int   id_port;             // Direccion del Port
        char  num_pin;                      // Mascara para el nro de pin

        char  status_in;
            #define ACTUAL_VALUE     0x01    // Valor actual de la entrada  
            #define PREVIOUS_VALUE   0x02    // Indica el valor previo a la nueva lectura
            #define REPORT_CHANGE    0x04    // Indica si debe reportar ante un cambio 0:No reporta 1:Reporta 
            #define REPORT_PD        0x08    // Indica que el valor actual cambio 0:No 1:Si

        char  debounced;                     // Tiempo a decrementar antes de considerar valido un cambio en la entrada
                                             // con 0xFF se indica que esta inactivo.
    };
    struct Inputs  inputs_pcl[];             // Define la extructura de entradas externas

    
    // Estructura y variables utilizadas por st_outputs.h y osdp_functions.h
    #define NUM_OUT_AND_LEDS_PCL   15      // Indica la cantidad de salidas y leds que posee la placa PCL 

    typedef struct Outputs{
        char  output_name;                 // Identificacion de la salida
        unsigned int   id_port;            // Direccion del Port
        char  num_pin;                     // Mascara para el nro de pin

        char  status_out;                         // Palabra de estado
            #define ACTUAL_STATE           0x01   // Indica el estado actual de la salida
            #define TEMPORIZE              0x02   // Indica si la salida sera temporizada (1) o permanecer en un estado definido permanente(0)
            #define PERMANENT_STATE        0x04   // Indica el valor del estado permanente que debe tener la salida
            #define OSC_INI_STATE          0x08   // Indica el valor que debe tomar la salida cuando comienza a oscilar
            #define OSC_CYCLE_REPETITIVE   0x10   // Indica si la oscilacion es permanente con 1
            #define INI_CYCLE_OSC          0x80   // Uso interno. Se utiliza en la primera pasada al temporizar. Solo configura el inicio de la etapa. 1:Paso 

        int  osc_time_on;                     // Tiempo de duracion en mseg cuando oscila en estado alto. Valor debe multiplicarse por 0.100 seg
        int  osc_time_off;                    // Tiempo de duracion en mseg cuando oscila en estado bajo. Valor debe multiplicarse por 0.100 seg   
        int  osc_cycle_time;                  // Tiempo que debe durar la oscilacion antes de pasar al estado permanente. Valor debe multiplicarse por 0.100 seg

        int  counter_time_on_off;             // Variable auxiliar utilizada como contador de tiempo en osc_time_on y osc_time_off.
        int  counter_cycle_time;              // Variable auxiliar utilizada como contador de tiempo en osc_cycle_time.
    };
    struct Outputs  outputs_signaling_pcl[];  // Define la extructura de entradas externas

    
    // Estructura y variables utilizadas por st_osdp.h y osdp_functions.h
    #define max_buf_osdp     0x0064      // Maxima cantidad de byte de la trama a soportar(100 bytes) 
    char  msg_osdp[max_buf_osdp];        // Buffer donde se almacena la trama para Rx o Tx

    
    // Estructura y variables utilizadas por st_readers.h y osdp_functions.h
    #define NUM_READERS   0x02       // Indica cantidad de readers soportados por la placa PCL

    typedef struct Readers{                  // Almacenamiento intermedio de una lectura
        char  reader_name;                   // Identificacion de la entrada

            #define READER_BUF       0x11    // El tamaño del array se lo dimensiono igual a TARBUF_SZ+1 en este caso (17 posiciones)
        char  value[READER_BUF];             // Valor leido  

        char  status_in;
            #define REPORT_READER_CHANGE    0x04    // Indica si debe reportar ante un cambio 0: No reporta 1: Reporta 
            #define REPORT_READER_PD        0x08    // Indica que el valor actual debe ser reportado 0: No reportar  1: Reportar
    };
    struct Readers  readers_pcl[2];      //Define extructura 

    
    // Estructura y variables utilizadas por st_timers.h
    #define NUM_TIMERS   0x04       // Indica la cantidad de timers en Timers[] 
	#define ON_LINE    0x0000012C  	// Tiempo que se considerara On Line despues de recibir el ultimo comando exitoso. Valor a multiplicar por 0.010 seg. 
                                    // Utilizada en st_timers.h					
	#define TOUCH_SWITCH    0x0BB8  // Tiempo para generar retardo de 30 seg antes de reiniciar la placa en modo Factory Default. 
                                    // Valor a multiplicar por 0.010 seg. Utilizada en st_timers.h

    typedef struct Table_Timers{
		unsigned char	status_timer;		// Control del timer
			#define TIMER_TRIGGER	0x01    // Genera disparo del timer si esta parado o redisparo recargando pre-scale si esta activo
			#define TIMER_RESET		0x02    // Para el timer 			
			#define TIMER_LEVEL		0x04    // Valor que debe tener la salida al finalizar la temporizacion
			#define	TIMER_OUT		0x80	// Indica el valor de salida del timer actual 

		unsigned int pre_scale;				// Valor pre cargado antes de iniciar cuenta regresiva del timer 
		unsigned int regressive_counter;	// Variable auxiliar utilizada para ejecutar la cuenta regresiva
	};
    struct Table_Timers  timers[];  // Define la extructura de Timers[]
    
    
    // Variables globales    
    extern char flag_monitoring = 0;        // Utilizada en osdp_functions.h, st_osdp.h y st_monitoring.h. 
                                            // Indica que paso se debera seguir, los comandos le asignan valores
    
    unsigned char flag_timer_interrup = 0x00;    // Utilizada para temporizar en def_hardware.c y st_timers.c 
        
                // Utilizado en st_osdp.c y st_monitoring.c
    char  PD_ID;                // PD-ID Peripheral device

                // Utilizado en osdp_functions.h y st_monitoring.h
    char aux_pd_id = 0;         // Utilizada para guardar en forma temporal el PD-ID

                // Utilizado en osdp_functions.h y st_monitoring.h
    char aux_baut_rate  = 0;    // Utilizada para guardar en forma temporal el SPBRG (Baud Rate)
    char aux_baut_rate_h = 0;   // Utilizada para guardar en forma temporal el SPBRGH (Baud Rate)

                // Utilizado en st_monitoring.h y st_rs485
    char aux_flag_RXoTX = 0;    // Es una copia de flag_RXoTX
    
                // Utilizado en st_monitoring.h y st_osdp.h
    char flag_system_configuration = 0;    // Utilizada para conocer si el sistema fue configurado despues de un Power-on o Reset
                                           // Finalmente cuando el sistema este configurado pasara a 0x01 permaneciendo en este estado.  
    
                // Utilizado en st_osdp.c y st_functions.c
    unsigned short int count_err_chks_osdp;  // Contador errores de checksum 
    unsigned short int count_min_byte_osdp;  // Contador errores de recepcion tramas menores al tamaño minimo establecido por OSDP
    unsigned short int count_max_byte_osdp;  // Contador errores de recepcion tramas mayores al tamaño maximo establecido por OSDP   
    unsigned short int count_max_buf_osdp;   // Contador errores de recepcion tramas con tamaño mayor al soportado por la placa y menor del establecido por OSDP

                // Utilizado en st_rs485.c y st_functions.c
    unsigned short int count_FERR;   // Contador de Framing Error  
    unsigned short int count_OERR;   // Contador de Overrun Error 
    
#endif	/* DEF_GLOBAL_VAR_H */
