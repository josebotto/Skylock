#include <xc.h>
#include <string.h>
#include "def_global_var.h"
#include "eeprom.h"
#include "osdp.h"
#include "osdp_functions.h"


//******************************************************************************
// Declaring Functions
//******************************************************************************
void Osdp_Functions_Comset(void)	// Se efectua la actualizacion del PD-ID y la velocidad de comunicacion a demanda
{                                    
	#define BAUD_9600      0x00002580    
	#define BAUD_19200     0x00004B00   
	#define BAUD_38400     0x00009600   
	#define BAUD_57600     0x0000E100   
	#define BAUD_115200    0x0001C200   

	int calc_aux;               								// Utilizado para calculo auxiliar
	int num_bytes;               								// Utilizado para calculo auxiliar
	int size_data_frame;										// Tamaño que debe tener la trama osdp para este comando
			
	size_data_frame = 0x0000000D;    						    // Es el tamaño que debe tener la trama osdp para este comando

	num_bytes = (int)msg_osdp[4];								// En calc_aux almacena la cantidad total de bytes del mensaje recibido
	num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

	if(num_bytes == size_data_frame){                   		// Chequea que la cantidad total de bytes recibidos sea igual a la definida para este comando                 
		if( msg_osdp[7] >= 0x00 && msg_osdp[7] <= 0x7D ){  		// Chequea que el PD-ID recibido se encuentre dentro de los valores permitidos                 

            // Precarga de datos a enviar
			flag_monitoring = 0x01;     // Este valor define que es un COMSET. Se analizara en st_monitoring
			msg_osdp[3] = 0x06;       	// Es la cantidad de elementos (comando + atributos)
            msg_osdp[4] = 0x00;         						
			msg_osdp[6] = OSDP_COM;   	// Comando OSDP_COM   (0x54)
                                        // Se envia el mismo contenido recibido en msg_osdp[7] al msg_osdp[11]

            // Análisis de la velocidad de transmisión recibida
			calc_aux = (int)msg_osdp[11];						// En calc_aux almacena la velocidad en Baud recibida
			calc_aux = (calc_aux<<8)+(int)msg_osdp[10];
			calc_aux = (calc_aux<<8)+(int)msg_osdp[9];
			calc_aux = (calc_aux<<8)+(int)msg_osdp[8];

			switch(calc_aux) {                  // Chequea velocidad recibida para grabar sobre eeprom
                                                // Define velocidad de comunicacion
                                                // Formula utilizada para el calculo Baudio=Fosc/[4*(n+1)] donde n es SPBRGH-SPBRG

				case BAUD_9600:                 // Carga en forma temporal. Se actualizara definitivamente en st_monitoring
                                                // garantiza el envio del mensaje antes de actualizar la placa                    

                    aux_pd_id = msg_osdp[7];    // Carga PD ID
                    aux_baut_rate = 0xFF;       // Para una velocidad de 9600 baudios SPBRGH=0x03 SPBRG=0xFF
                    aux_baut_rate_h = 0x03; 
					return;       	         	// Se envie mensaje al CP.  
					break;
                    
				case BAUD_19200:  				// Carga en forma temporal. Se actualizara definitivamente en st_monitoring 
                                                // garantiza el envio del mensaje antes de actualizar la placa                    

                    aux_pd_id = msg_osdp[7];    // Carga PD ID
                    aux_baut_rate = 0xFF;       // Para una velocidad de 19200 baudios SPBRGH=0x01 SPBRG=0xFF
                    aux_baut_rate_h = 0x01; 
					return;       	         	// Se envie mensaje al CP.  
					break;
                    
				case BAUD_38400:   				// Carga en forma temporal. Se actualizara definitivamente en st_monitoring. 
                                                // garantiza el envio del mensaje antes de actualizar la placa                    
                    
                    aux_pd_id = msg_osdp[7];    // Carga PD ID
                    aux_baut_rate = 0xFF;       // Para una velocidad de 38400 baudios SPBRGH=0x00 SPBRG=0xFF
                    aux_baut_rate_h = 0x00; 
					return;       	         	// Se envie mensaje al CP.  
					break;

				case BAUD_57600:    			// Carga en forma temporal. Se actualizara definitivamente en st_monitoring   
                                                // garantiza el envio del mensaje antes de actualizar la placa                    

                    aux_pd_id = msg_osdp[7];    // Carga PD ID
                    aux_baut_rate = 0xAA;       // Para una velocidad de 57600 baudios SPBRGH=0x00 SPBRG=0xAA
                    aux_baut_rate_h = 0x00; 
					return;       	         	// Se envie mensaje al CP.  
					break;

				case BAUD_115200:      			// Carga en forma temporal. Se actualizara definitivamente en st_monitoring  
                                                // garantiza el envio del mensaje antes de actualizar la placa                    

                    aux_pd_id = msg_osdp[7];    // Carga PD ID
                    aux_baut_rate = 0x54;       // Para una velocidad de 115200 baudios SPBRGH=0x00 SPBRG=0x54
                    aux_baut_rate_h = 0x00; 
					return;       	         	// Se envie mensaje al CP.  
					break;

				default:                        // Valor no reconocido
					break;
			}
		}
	}
    
    // Cargando datos a enviar
	flag_monitoring = 0x00;                     // Este valor indica que no habra modificaciones. Se analizara en la st_monitoring
	msg_osdp[3] = 0x02;         				// Es la cantidad de elementos (comando + atributos), 
	msg_osdp[4] = 0x00;         						
	msg_osdp[6] = OSDP_NAK;  					// Comando OSDP_NAK   0x41
	msg_osdp[7] = 0x01;      					// Indica Message check character(s) error (bad cks/crc/mac[4]  

	return;                              		// Responde al CP
}		

void Osdp_Functions_Istat(void)       // Recorre inputs_pcl e informa el estado de cada una de las entradas y si reporta ante cambios
{                                    
    char aux_for;
    char aux_data;
    char aux_data1;
    char pos_data;              // Indice de posicion.

    pos_data = 0x06;            // Inicializa el indice de posicion para carga de datos 

    msg_osdp[3] = 0x01;         // Es la cantidad de elementos (comando + atributos), 
	msg_osdp[4] = 0x00;         // es inicializado en 0x01 para considerar en este momento solo el comando
								
	msg_osdp[6] = OSDP_ISTATR;  // Guarda comando de respuesta. En este caso son las entradas digitales.

    for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++){     // Recorre la tabla inputs_pcl[] 
		aux_data = inputs_pcl[aux_for].status_in;           // Carga status_in
		aux_data = (aux_data<<7);                           // Desplaza el bit 0 a la posicion 7. Este bit representa el estado de la entrada actual
		aux_data &= 0x80;                                   // Se asegura que los bits 0 al 6 son 0

		aux_data1 = inputs_pcl[aux_for].status_in;          // Carga status_in
		aux_data1 = (aux_data1<<4);                         // Desplaza el bit 2 a la posicion 6. Este bit representa si la entrada reporta ante una variacion
		aux_data1 &= 0x40;                                  // Se asegura que los bits 0 al 5 y el 7 son 0

		aux_data += aux_data1;                              // Adiciona quedando en los bits de mayor peso el estado de la entrada y si reporta variacion

		aux_data |= inputs_pcl[aux_for].input_name;         // Carga la identificacion de la entrada y la adiciona al estado de la misma
		pos_data++ ;                                        // Incremente indice de posicion
        msg_osdp[3]++ ;                                     // Incrementa cantidad de elementos (comando + atributos) 
	    msg_osdp[pos_data] = aux_data;                      // Guarda el dato en el string que se enviara como respuesta
    }
	
	return;                                 				// responde al CP
}

void Osdp_Functions_Establish(void)     // Permite habilitar o deshabilitar la accion reportar eventos de los dispositivos. 
{                                    
    unsigned char aux_for;
	unsigned int num_bytes;     // Utilizado para calculo auxiliar

	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
	num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

    if(num_bytes == 0x000A){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos para el comando.
                                    // El comando esta compuesto por tres bytes: Comando + Atributo + dispositivo. Hace un total de 10 bytes la trama OSDP
        
        if((flag_ini_input_dig < msg_osdp[8]) && (msg_osdp[8] < flag_end_input_dig) ){   // Consulta si la entrada esta dentro del listado de input digitales 
            for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++){                          // Recorre la tabla inputs_pcl[] 
                if(inputs_pcl[aux_for].input_name == msg_osdp[8]){                       // Busca la entrada dentro de la tabla                                             
                    if(msg_osdp[7] == 0x00){                                             // Una vez encontrada consulta si habilita o deshabilita
                        inputs_pcl[aux_for].status_in &= 0xF3;                           // Deshabilita modo reportar reset bits REPORT_CHANGE y REPORT_PD
                    }
                    else{
                        inputs_pcl[aux_for].status_in |= 0x04;                           // Habilita modo reportar set bit REPORT_CHANGE
                        inputs_pcl[aux_for].status_in &= 0xF7;                           // y reset bit REPORT_PD  
                    }                 

                    msg_osdp[3] = 0x01;                 // Es la cantidad de elementos (comando) 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_ESTABLISHR;      // Comando OSDP_ESTABLISHR   0xD5
                    return;                             // responde al CP
                }                 
            }                 
        } 
        
        if((flag_ini_input_ana < msg_osdp[8]) && (msg_osdp[8] < flag_end_input_ana) ){   // Consulta si la entrada esta dentro del listado de inputs analogicas
            for(aux_for = 0; aux_for < NUM_INPUTS_ANALOG ; aux_for++){                   // Recorre la tabla inputs_analog_pcl[]
                if(inputs_analog_pcl[aux_for].input_analog_name == msg_osdp[8]){         // Busca la entrada dentro de la tabla                                             
                    if(msg_osdp[7] == 0x00){                                             // Una vez encontrada consulta si habilita o no modo reportar
                        inputs_analog_pcl[aux_for].status_analog_in &= 0xFC;             // Deshabilita modo reportar reset bits REPORT_ALARM y ALARM 
                    }
                    else{
                        inputs_analog_pcl[aux_for].status_analog_in |= 0x01;             // Habilita modo reportar set bit REPORT_ALARM
                        inputs_analog_pcl[aux_for].status_analog_in &= 0xFD;             // y reset bit ALARM
                    }                 

                    msg_osdp[3] = 0x01;                 // Es la cantidad de elementos (comando) 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_ESTABLISHR;      // Comando OSDP_ESTABLISHR   0xD5
                    return;                             // responde al CP
                }                 
            }                 
        }                 

        if((flag_ini_reader < msg_osdp[8]) && (msg_osdp[8] < flag_end_reader) ){   //  Consulta si la entrada esta dentro del listado de readers
            for(aux_for = 0; aux_for < NUM_READERS ; aux_for++){                   // Recorre la tabla de lectoras
                if(readers_pcl[aux_for].reader_name == msg_osdp[8]){               // Busca la entrada dentro de la tabla                                             
                    if(msg_osdp[7] == 0x00){                                       // Una vez encontrada consulta si habilita o deshabilita
                        readers_pcl[aux_for].status_in &= 0xF3;                    // Deshabilita modo reportar reset bits REPORT_READER_CHANGE y REPORT_READER_PD
                    }
                    else{
                        readers_pcl[aux_for].status_in |= 0x04;                    // Habilita modo reportar set bit REPORT_READER_CHANGE
                        readers_pcl[aux_for].status_in &= 0xF7;                    // y reset bit REPORT_READER_PD  
                    }                 

                    msg_osdp[3] = 0x01;                 // Es la cantidad de elementos (comando) 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_ESTABLISHR;      // Comando OSDP_ESTABLISHR   0xD5
                    return;                             // responde al CP
                }                 
            }                 
        }           

    }                                  

    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
    msg_osdp[4] = 0x00;         			
    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
    msg_osdp[7] = 0x01;             // Indica Message check character(s) error (bad cks/crc/mac[4]  

    return;                         // responde al CP
}

void Osdp_Functions_Ostat(void)       // Recorre outputs_signaling_pcl e informa el estado de cada una de las salidas
{                                    
    char aux_for;
    char aux_data;
    char pos_data;              // Indice de posicion.

    pos_data = 0x06;            // Inicializa el indice de posicion para carga de datos 

    msg_osdp[3] = 0x01;         // Es la cantidad de elementos (comando + atributos), 
	msg_osdp[4] = 0x00;         // es inicializado en 0x01 para considerar en este momento solo el comando
								
	msg_osdp[6] = OSDP_OSTATR;  // Guarda comando de respuesta. En este caso son las salidas digitales.

    for(aux_for = 0; aux_for < NUM_OUT_AND_LEDS_PCL ; aux_for++){		// Recorre la tabla outputs_signaling_pcl[] 

		aux_data = outputs_signaling_pcl[aux_for].status_out;           // Carga status_out
		aux_data = (aux_data<<7);                                       // Desplaza el bit 0 a la posicion 7. Este bit representa el estado de la salida actual
		aux_data &= 0x80;                                               // Se asegura que los bits 0 al 6 son 0
		aux_data |= outputs_signaling_pcl[aux_for].output_name;         // Carga la identificacion de la salida y la adiciona al estado de la misma
	
        pos_data++ ;                                          			// Incremente indice de posicion

        msg_osdp[3]++ ;                                       			// Incrementa cantidad de elementos (comando + atributos) 
	    msg_osdp[pos_data] = aux_data;
    }

	return;                                                             // responde al CP
}

void Osdp_Functions_Out(void)	// Chequea informacion recibida sobre salidas, clasifica y ejecuta
{                                    
    char aux_for;
    char aux_for_b;
    char aux_data;
    char pos_data;										// Indice de posicion.
    int num_bytes;                                      // Cantidad de bytes totales recibidos
    int calc_aux;               						// Utilizado para calculo auxiliar
    int calc_res;               						// Utilizado para calculo del residuo
	
    pos_data = 0x06;            						// Inicializa el indice de posicion para carga de datos. 

	num_bytes = (int)msg_osdp[4];						// En calc_aux almacena la cantidad total de bytes del mensaje recibido
	num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

    msg_osdp[3] = 0x01;         						// Es la cantidad de elementos (comando + atributos), 
	msg_osdp[4] = 0x00;         						// es inicializado en 0x01 para considerar en este momento solo el comando

    // Verificar si la cantidad de bytes pertenecientes a los datos del comando son multiplos de 4. Ver comando en Manual de SIA OSDP para mas detalles
	calc_aux = num_bytes - 0x0008;                      // Resta 8 que son los bytes de encabezado y checksum
	calc_res = num_bytes % 0x0004;                      // Calcula si es multiplo de 4 mediante el residuo

    if( !(calc_aux > 0x0000 && calc_res == 0x0000) ){   // Chequea que la cantidad de bytes de datos recibida sea mayor a cero y multiplo de 4                 
        msg_osdp[3]++ ;									// Incrementa cantidad de elementos (comando + atributos) 
		msg_osdp[6] = OSDP_NAK;  						// Comando OSDP_NAK   0x41
		msg_osdp[7] = 0x01;      						// Indica Message check character(s) error (bad cks/crc/mac[4]  
		return;                                     	// responde al CP
    }

	calc_aux = calc_aux / 0x0004;                       // Calcula cuantos multiplos de 4 hay sabiendo que es posible hacerlo por el proceso anterior 
	
	// Verificar si la o las salidas indicadas estan dentro de la salidas reconocidas
	aux_data = pos_data + 0x01;							// Primera posicion de datos en la trama msg_osdpd agrupada cada cuatro bytes

    for(aux_for = 0; aux_for < calc_aux ; aux_for++){	// Recorre los datos de la trama msg_osdp 

	    if((msg_osdp[aux_data] <= flag_ini_out) || (msg_osdp[aux_data] >= flag_end_out)){   //Verifica que la salida se encuentre entre las reconocidas

			msg_osdp[3]++ ;								// Incrementa cantidad de elementos (comando + atributos) 
			msg_osdp[6] = OSDP_NAK;  					// Comando OSDP_NAK   0x41
			msg_osdp[7] = 0x01;      					// Indica Message check character(s) error (bad cks/crc/mac[4]  
			return;                              		// responde al CP
		}

		aux_data += 0x04;							
	}
								
	// Configurar nuevos valores en las salidas indicadas
	aux_data = pos_data + 0x01;							// Primera posicion de datos en la trama msg_osdpd agrupada cada cuatro bytes

    for(aux_for = 0; aux_for < calc_aux ; aux_for++){	// Recorre los datos de la trama msg_osdp 

        if(!(msg_osdp[aux_data] == out_led_0 || msg_osdp[aux_data] == out_led_1 || msg_osdp[aux_data] == out_led_3 || msg_osdp[aux_data] == out_led_5)){
                                                        // Chequea que la salida a modificar no pertenezca a los led que se utilizan 
                                                        // para informacion visual Led_0, Led_1, Led_3 y Led_5  
            
			for(aux_for_b = 0; aux_for_b < NUM_OUT_AND_LEDS_PCL ; aux_for_b++){		

				if(msg_osdp[aux_data] == outputs_signaling_pcl[aux_for_b].output_name){ 

					switch(msg_osdp[aux_data + 0x01]){      
						case 0x00:                      // No modifica la salida             
							break;

						case 0x01:                      // Establece estado permanente en off, anule la operación temporizada (si existe)
							outputs_signaling_pcl[aux_for_b].status_out &= ~TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out &= ~PERMANENT_STATE;
							break;

						case 0x02:                      // Establece estado permanente en on, anule la operación temporizada (si existe)              
							outputs_signaling_pcl[aux_for_b].status_out &= ~TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out |= PERMANENT_STATE;
							break;

						case 0x03:                      // Establece estado permanente en off, permite que se complete la operación temporizada          
							outputs_signaling_pcl[aux_for_b].status_out &= ~PERMANENT_STATE;
							break;

						case 0x04:                      //  Establece estado permanente en on, permite que se complete la operación temporizada          
							outputs_signaling_pcl[aux_for_b].status_out |= PERMANENT_STATE;
							break;

						case 0x05:                      // Establece estado temporal en on, reanudar estado permanente en el tiempo de espera      
							outputs_signaling_pcl[aux_for_b].status_out |= TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out |= OSC_INI_STATE;
							outputs_signaling_pcl[aux_for_b].status_out &= ~OSC_CYCLE_REPETITIVE;

							outputs_signaling_pcl[aux_for_b].osc_time_on = (int)msg_osdp[aux_data + 0x03];
							outputs_signaling_pcl[aux_for_b].osc_time_on = (outputs_signaling_pcl[aux_for_b].osc_time_on<<8) + (int)msg_osdp[aux_data + 0x02];
							outputs_signaling_pcl[aux_for_b].osc_time_off = 0x0000;
                            outputs_signaling_pcl[aux_for_b].osc_cycle_time = outputs_signaling_pcl[aux_for_b].osc_time_on + outputs_signaling_pcl[aux_for_b].osc_time_off;
                                                                            // La st_outputs permite generar varios periodos oscilatorios dentro de una ventana de tiempo
                                                                            // sin llegar a hacer una oscilacion permanente. Se definio hacer esto debido a las limitaciones
                                                                            // del comando OSDP       
							break;
							
						case 0x06:                      // Establece estado temporal en off, reanudar estado permanente en el tiempo de espera  
							outputs_signaling_pcl[aux_for_b].status_out |= TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out &= ~OSC_INI_STATE;
							outputs_signaling_pcl[aux_for_b].status_out &= ~OSC_CYCLE_REPETITIVE;

							outputs_signaling_pcl[aux_for_b].osc_time_on = 0x0000;
							outputs_signaling_pcl[aux_for_b].osc_time_off = (int)msg_osdp[aux_data + 0x03];
							outputs_signaling_pcl[aux_for_b].osc_time_off = (outputs_signaling_pcl[aux_for_b].osc_time_off<<8) + (int)msg_osdp[aux_data + 0x02];
                            outputs_signaling_pcl[aux_for_b].osc_cycle_time = outputs_signaling_pcl[aux_for_b].osc_time_on + outputs_signaling_pcl[aux_for_b].osc_time_off;
                                                                            // La st_outputs permite generar varios periodos oscilatorios dentro de una ventana de tiempo
                                                                            // sin llegar a hacer una oscilacion permanente. Se definio hacer esto debido a las limitaciones
                                                                            // del comando OSDP       
							break;
						
						case 0x07:                      // Establece una oscilacion permanente arrancando en on generando una onda cuadrada  
							outputs_signaling_pcl[aux_for_b].status_out |= TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out |= OSC_INI_STATE;
							outputs_signaling_pcl[aux_for_b].status_out |= OSC_CYCLE_REPETITIVE;

							outputs_signaling_pcl[aux_for_b].osc_time_on = (int)msg_osdp[aux_data + 0x03];
							outputs_signaling_pcl[aux_for_b].osc_time_on = (outputs_signaling_pcl[aux_for_b].osc_time_on<<8) + (int)msg_osdp[aux_data + 0x02];
							outputs_signaling_pcl[aux_for_b].osc_time_off = outputs_signaling_pcl[aux_for_b].osc_time_on;
							break;

						case 0x08:                      // Establece una oscilacion permanente arrancando en off generando una onda cuadrada  
							outputs_signaling_pcl[aux_for_b].status_out |= TEMPORIZE;
							outputs_signaling_pcl[aux_for_b].status_out &= ~OSC_INI_STATE;
							outputs_signaling_pcl[aux_for_b].status_out |= OSC_CYCLE_REPETITIVE;

							outputs_signaling_pcl[aux_for_b].osc_time_off = (int)msg_osdp[aux_data + 0x03];
							outputs_signaling_pcl[aux_for_b].osc_time_off = (outputs_signaling_pcl[aux_for_b].osc_time_off<<8) + (int)msg_osdp[aux_data + 0x02];
							outputs_signaling_pcl[aux_for_b].osc_time_on = outputs_signaling_pcl[aux_for_b].osc_time_off;
							break;

						default:     
							break;
					}
				}
			}
		}

		aux_data += 0x04;							
	}

	msg_osdp[6] = OSDP_ACK;     // Responde Comando
	return;                     // Responde al CP
}

char Osdp_Functions_Poll(void)       // Chequea existencia de eventos pendientes a ser enviado
{                                    
    char aux_for;
    char aux_data;
    char aux_index;
    char pos_data;              // Indice de posicion.

    pos_data = 0x06;            // Inicializa el indice de posicion para carga de datos. 
    msg_osdp[3] = 0x01;         // Es la cantidad de elementos (comando + atributos) 
   	msg_osdp[4] = 0x00;         // Es inicializado en 0x01 para considerar en este momento solo el comando    

    // Busqueda estado de Entradas Digitales a reportar
    msg_osdp[6] = OSDP_ISTATR;  // Guarda comando a analizar. En este caso son las entradas digitales.

    for(aux_for = 0; aux_for < NUM_IN_PCL ; aux_for++){           // Recorre la tabla inputs_pcl[] 

        aux_data = inputs_pcl[aux_for].status_in & REPORT_PD;     // Busca entradas a reportar 
        if(aux_data){                                             
            inputs_pcl[aux_for].status_in &= ~REPORT_PD;          // Limpia flag de reportar evento
            pos_data++ ;                                          // Incremente indice de posicion
            msg_osdp[3]++ ;                                       // Incrementa cantidad de elementos (comando + atributos) 
            
            aux_data = inputs_pcl[aux_for].status_in & ACTUAL_VALUE;          // Consulta el valor actual para reportarlo  
            if(aux_data) 
                msg_osdp[pos_data] = inputs_pcl[aux_for].input_name | 0x80;   // Envia el nombre de la entrada cuyo digito de mayor peso es el valor actual 
            else 
                msg_osdp[pos_data] = inputs_pcl[aux_for].input_name & 0x7F;   // Envia el nombre de la entrada cuyo digito de mayor peso es el valor actual
                                                                              // Esto se hizo para poder enviar la informacion de nombre y estado en un byte 
        }    
    }

    if(msg_osdp[3] != 0x01)     // Si se cumple indica que hay inputs digital que reportar, caso contrario sigue buscando que reportar  
        return(1);      

    // Busqueda registraciones de lectoras a reportar  
    msg_osdp[6] = OSDP_FMT;  // Guarda comando a analizar. En este caso son las registraciones
    pos_data = 0x09;         // Inicializa el indice de posicion para carga de datos. 
    
    for(aux_index = 0; aux_index < NUM_READERS ; aux_index++){                // Recorre la tabla para las dos entradas de lectoras 

        aux_data = readers_pcl[aux_index].status_in & REPORT_READER_PD;       // Busca si hay registraciones a reportar 
        if(aux_data){                                                         // Si se cumple hay registraciones                                  
            readers_pcl[aux_index].status_in &= ~REPORT_READER_PD;            // Limpia flag de reportar evento

            for(aux_for = 0; aux_for < READER_BUF ; aux_for++){
                pos_data++ ;                                                  // Incremente indice de posicion
                msg_osdp[3]++ ;                                               // Incrementa cantidad de elementos (comando + atributos) 
                msg_osdp[pos_data] = readers_pcl[aux_index].value[aux_for];   // Comienza a pasar los datos al buffer OSDP

                if(msg_osdp[pos_data] == 0xFF){                               // Consulta si es el ultimo 

                    msg_osdp[3] = msg_osdp[3] +0x03 ;                         // Adiciona tres posiciones.(Reader Number-Read Direction-Digit Count)                
                    msg_osdp[3]-- ;                                           // Actualiza evitando contar el ultimo leido que fue 0xFF                                       

                    msg_osdp[7] = aux_index;                                  // Guarda cual es la lectora. (Reader Number)       
                    msg_osdp[8] = 0x00;                                       // Indica el modo de lectura. (Read Direction. Direction of data in array 0 = forward read 1 = reverse read)  
                    msg_osdp[9] = aux_for -1;                                 // Actualiza evitando contar el ultimo leido 0xFF. (Digit Count. Number of digits, including START, END, CKSUM)

                    aux_for = READER_BUF;                                     // Para finalizar el for
                    aux_index = 0x02;                                         // Para finalizar el for
                }
            }
        }           
    }

    if(msg_osdp[3] != 0x01)     // Si se cumple indica que hay registraciones que reportar, caso contrario termina la busqueda  
        return(1);      

    return(0);      
}

void Osdp_Functions_Watch(void)     // Permite visualizar e inicializar contadores de errones, brinda informacion del sistema. 
{
    unsigned char aux_data;
    unsigned char aux_index;
    unsigned int num_bytes;                         // Cantidad de bytes totales recibidos

	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
	num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

    if(num_bytes == 0x0009){            // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos para el comando.

        msg_osdp[3] = 0x01;         	// Es la cantidad de elementos (comando + atributos), 
        msg_osdp[4] = 0x00;         	// es inicializado en 0x01 para considerar en este momento solo el comando    

        switch(msg_osdp[7]){      
            case 0x00:                  // Envia informacion sobre el sistema             
                aux_data = 0x00;                                                                // Inicializa para ser utilizado como contador
                for(aux_index = BOARD_DATA_INI; aux_index <= BOARD_DATA_END ; aux_index++){     // Copia de la eeprom el mensaje a msg_osdp para ser enviado
                    msg_osdp[aux_data+0x08] = read_eeprom(aux_index);                           // a partir de la posicion 0x08 del msg_osdp                     
                    aux_data++;                                                                 // Cuenta la cantidad de char que se estan copiando a msg_osdp
                }
                msg_osdp[3] += (aux_data+1) ;   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;  	// OSDP_WATCHR   (0xD7). Respuesta a comando OSDP_WATCH
                msg_osdp[7] = 0x00;             // Se envia el atributo recibido
                return;                         // responde al CP
                break;

            case 0x01:                      // Envia informacion de los contadores de error(cnt_power.h) en el siguiente orden: 
                                            //                 POR, BOR, TO, PD, RI . Cada variable es representada por un char
                msg_osdp[3] += (0x01 + 0x05);                   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;                      // Respuesta a comando OSDP_WATCH(comando)
                msg_osdp[7] = 0x01;                             // Se envia el atributo recibido
                msg_osdp[8] = read_eeprom(POS_INI+POS_POR);     // Se envia POR(mensaje)
                msg_osdp[9] = read_eeprom(POS_INI+POS_BOR);     // Se envia BOR(mensaje)
                msg_osdp[10] = read_eeprom(POS_INI+POS_TO);     // Se envia TO(mensaje)
                msg_osdp[11] = read_eeprom(POS_INI+POS_PD);     // Se envia PD(mensaje)
                msg_osdp[12] = read_eeprom(POS_INI+POS_RI);     // Se envia RI(mensaje)
                return;                                         // responde al CP
                break;

            case 0x02:                      // Inicializa contadores de error en st_osdp.h
                count_err_chks_osdp = 0x00; 
                count_min_byte_osdp = 0x00; 
                count_max_byte_osdp = 0x00;
                count_max_buf_osdp = 0x00;

            case 0x03:                      // Envia informacion de los contadores de error(st_osdp.h) en el siguiente orden: count_err_chks_osdp, 
                                            //                                        count_min_byte_osdp, count_max_byte_osdp, count_max_buf_osdp
                msg_osdp[3] += (0x01 + 0x08);                   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;                      // Respuesta a comando OSDP_WATCH(comando)
                msg_osdp[7] = 0x03;                             // Se envia este atributo indicando valores actuales
                msg_osdp[8] = (char)count_err_chks_osdp;        // Mensaje valor LSB
                msg_osdp[9] = (char)(count_err_chks_osdp>>8);   // Mensaje valor MSB
                msg_osdp[10] = (char)count_min_byte_osdp;       // Mensaje valor LSB
                msg_osdp[11] = (char)(count_min_byte_osdp>>8);  // Mensaje valor MSB
                msg_osdp[12] = (char)count_max_byte_osdp;       // Mensaje valor LSB
                msg_osdp[13] = (char)(count_max_byte_osdp>>8);  // Mensaje valor MSB
                msg_osdp[14] = (char)count_max_buf_osdp;        // Mensaje valor LSB
                msg_osdp[15] = (char)(count_max_buf_osdp>>8);   // Mensaje valor MSB
                return;                                // responde al CP
                break;

            case 0x04:                      // Inicializa contadores de error en st_rs485             
                count_FERR = 0x00;
                count_OERR = 0x00;

            case 0x05:                      // Envia informacion de los contadores de error(st_rs485) en el siguiente orden: 
                                            //                                                       count_FERR, count_OERR
                msg_osdp[3] += (0x01 + 0x04);   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;      // Respuesta a comando OSDP_WATCH(comando)
                msg_osdp[7] = 0x05;             // Se envia este atributo indicando valores actuales
                msg_osdp[8] = (char)count_FERR;         // Mensaje valor LSB
                msg_osdp[9] = (char)(count_FERR>>8);    // Mensaje valor MSB
                msg_osdp[10] = (char)count_OERR;        // Mensaje valor LSB
                msg_osdp[11] = (char)(count_OERR>>8);   // Mensaje valor MSB
                return;                                 // responde al CP
                break;

            case 0x43:                  // Envia Informacion adicional A             
                aux_data = 0x00;                                                           // Inicializa para ser utilizado como contador
                for(aux_index = INF_ADDA_INI; aux_index <= INF_ADDA_END ; aux_index++){    // Copia el mensaje a msg_osdp para ser enviado
                    msg_osdp[aux_data+0x08] = read_eeprom(aux_index);                      // a partir de la posicion 0x08 del msg_osdp                     
                    aux_data++;                                                            // Cuenta la cantidad de char que se estan copiando a msg_osdp
                }
                msg_osdp[3] += (aux_data+1) ;   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;  	// OSDP_WATCHR   (0xD7). Respuesta a comando OSDP_WATCH
                msg_osdp[7] = 0x43;             // Se envia el atributo recibido
                return;                         // responde al CP
                break;

            case 0x44:                  // Envia Informacion adicional B             
                aux_data = 0x00;                                                           // Inicializa para ser utilizado como contador
                for(aux_index = INF_ADDB_INI; aux_index <= INF_ADDB_END ; aux_index++){    // Copia el mensaje a msg_osdp para ser enviado
                    msg_osdp[aux_data+0x08] = read_eeprom(aux_index);                      // a partir de la posicion 0x08 del msg_osdp                     
                    aux_data++;                                                            // Cuenta la cantidad de char que se estan copiando a msg_osdp
                }
                msg_osdp[3] += (aux_data+1) ;   // Es la cantidad de elementos (comando + atributo + mensaje), 
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_WATCHR;  	// OSDP_WATCHR   (0xD7). Respuesta a comando OSDP_WATCH
                msg_osdp[7] = 0x44;             // Se envia el atributo recibido
                return;                         // responde al CP
                break;
                
            default:
                break;
        }
    }                                  

    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
    msg_osdp[4] = 0x00;         			
    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
    msg_osdp[7] = 0x01;             // Indica Message check character(s) error (bad cks/crc/mac[4]  

    return;                         // responde al CP
}
