/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   osdp_functions.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 14 de agosto de 2018, 10:40
 * 
 * Log:
 *     Functions for OSDP commands. 
 *    
 * =====================================================================
 */


#ifndef OSDP_FUNCTIONS_H
#define	OSDP_FUNCTIONS_H

    // Declarando funciones
    void Osdp_Functions_Comset(void);       // Se efectua la actualizacion del PD-ID y la velocidad de comunicacion a demanda
    void Osdp_Functions_Istat(void);        // Recorre inputs_pcl e informa el estado de cada una de las entradas
    void Osdp_Functions_Establish(void);    // Permite habilitar o deshabilitar el reporte de eventos por parte de los dispositivos. 
    void Osdp_Functions_Ostat(void);        // Recorre outputs_signaling_pcl e informa el estado de cada una de las salidas
    void Osdp_Functions_Out(void);          // Chequea informacion recibida sobre salidas, clasifica y ejecuta
    char Osdp_Functions_Poll(void);         // Chequea existencia de eventos pendientes a ser enviado
    void Osdp_Functions_Watch(void);        // Permite visualizar e inicializar contadores de errones, brinda informacion del sistema. 
    
#endif	/* OSDP_FUNCTIONS_H */
