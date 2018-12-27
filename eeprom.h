/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   eeprom.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 16 de octubre de 2018, 11:40
 * 
 * Log:
 *     Define data in Eeprom. 
 *     Factory default and initialization data.  
 *  
 * =====================================================================
 */


#ifndef EEPROM_H
#define	EEPROM_H
    
    // Posicion Identificacion placa
    #define BOARD_DATA_INI   0x00   // Inicio mensaje Identificacion de la placa
    #define BOARD_DATA_END   0x2F   // Fin mensaje Identificacion de la placa

    // Posicion de Factory Default 
    #define FACTORYD_PD_ID       0x30   // PD-ID
    #define FACTORYD_BR_SPBRG    0x31   // Baud Rate SPBRG
    #define FACTORYD_BR_SPBRGH   0x32   // Baud Rate SPBRGH

    // Posicion de User Default
    #define USERD_PD_ID       0x40  // PD-ID
    #define USERD_BR_SPBRG    0x41  // Baud Rate SPBRG
    #define USERD_BR_SPBRGH   0x42  // Baud Rate SPBRGH

    // Posicion de contadores
    #define POS_INI   0x50
    #define POS_POR   0x00  // Power-on Reset
    #define POS_BOR   0x01  // Brown-out Reset
    #define POS_TO    0x02  // Watchdog Time-out
    #define POS_PD    0x03  // Power-Down Detection
    #define POS_RI    0x04  // RESET Instruction

    // Posicion Informacion adicional A
    #define INF_ADDA_INI   0xD0  
    #define INF_ADDA_END   0xDF  

    // Posicion Informacion adicional B
    #define INF_ADDB_INI   0xE0  
    #define INF_ADDB_END   0xEF  

    // Declarando funciones
    unsigned char read_eeprom(unsigned char direction);
    void write_eeprom(unsigned char direction, unsigned char data);

#endif	/* EEPROM_H */
