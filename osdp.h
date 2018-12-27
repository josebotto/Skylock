/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   osdp.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 15 de septiembre de 2017, 11:54
 * 
 * Log:
 *     OSDP Protocol / CRC calculation
 * 
 * =====================================================================
 */


#ifndef OSDP_H
#define	OSDP_H

    // OSDP defined constants
    #define C_SOM                     (0x53)
    #define C_OSDP_MARK               (0xFF)    // used in OSDP TLS to poke CP
    #define OSDP_DEST_CP              (0x80)    // ID para Panel de Control (CP)
    #define OSDP_PD_Broadcast         (0x7F) 
    #define OSDP_PD_Factory_Default   (0x7E)    // Representa al ID 126

    #define OSDP_KEY_SCBK_D   (0)
    #define OSDP_SEC_SCS_11   (0x11)
    #define OSDP_SEC_SCS_12   (0x12)
    #define OSDP_SEC_SCS_13   (0x13)

    // OSDP commands
    // 0x6c - 0x 6f
    // 0x70 - 0x79
    // 0x80
    // 0xA0 - 0xA1
    // replies 0x40 41 45 46 48-4B 50-58 76 78 90 79 b1
    #define OSDP_POLL        (0x60)
    #define OSDP_ID          (0x61)
    #define OSDP_CAP         (0x62)
    #define OSDP_DIAG        (0x63)
    #define OSDP_LSTAT       (0x64)
    #define OSDP_ISTAT       (0x65)
    #define OSDP_OSTAT       (0x66)
    #define OSDP_RSTAT       (0x67)
    #define OSDP_OUT         (0x68)
    #define OSDP_LED         (0x69)
    #define OSDP_BUZ         (0x6A)
    #define OSDP_TEXT        (0x6B)
    #define OSDP_TDSET       (0x6D)
    #define OSDP_COMSET      (0x6E)
    #define OSDP_DATA        (0x6F)
    #define OSDP_PROMPT      (0x71)
    #define OSDP_BIOREAD     (0x73)
    #define OSDP_KEYSET      (0x75)
    #define OSDP_CHLNG       (0x76)
    #define OSDP_MFG         (0x80)
    #define OSDP_FACTORY     (0xD0)     // Comando Skylock. Set Factory Default. Setea con parametros de fabrica y reinicia el sistema
    #define OSDP_RESET       (0xD2)     // Comando Skylock. Reset System. Genera reset en la placa
    #define OSDP_ESTABLISH   (0xD4)     // Comando Skylock. Set Values. Permite set o reset valores 
    #define OSDP_WATCH       (0xD6)     // Comando Skylock. Status Errors Command. Permite visualizar contadores, inputs analogicas y sensores
    #define OSDP_CONFIG      (0xD9)     // Comando Skylock. Configuracion del Sistema. Informa a la placa que sera configurada

    #define OSDP_ACK           (0x40)
    #define OSDP_NAK           (0x41)
    #define OSDP_PDID          (0x45)
    #define OSDP_PDCAP         (0x46)
    #define OSDP_LSTATR        (0x48)
    #define OSDP_ISTATR        (0x49)
    #define OSDP_OSTATR        (0x4A)
    #define OSDP_RSTATR        (0x4B)
    #define OSDP_RAW           (0x50)
    #define OSDP_FMT           (0x51)
    #define OSDP_KEYPAD        (0x53)
    #define OSDP_COM           (0x54)
    #define OSDP_CCRYPT        (0x76)
    #define OSDP_SCRYPT        (0x77)
    #define OSDP_BUSY          (0x79)      
    #define OSDP_MFGREP        (0x90)
    #define OSDP_FACTORYR      (0xD1)   // Comando Skylock. Respuesta a comando OSDP_FACTORY
    #define OSDP_RESETR        (0xD3)   // Comando Skylock. Respuesta a comando OSDP_RESET
    #define OSDP_ESTABLISHR    (0xD5)   // Comando Skylock. Respuesta a comando OSDP_ESTABLISH
    #define OSDP_WATCHR        (0xD7)   // Comando Skylock. Respuesta a comando OSDP_WATCH
    #define OSDP_CHANGE_PDID   (0xD8)   // Comando Skylock. Respuesta cuando el PD-ID es un Factory Default. Es decir indicar cambiar PD-ID al usuario
    #define OSDP_CONFIGR       (0xDA)   // Comando Skylock. Respuesta de reconocimiento al comando OSDP_CONFIG   
    #define OSDP_CONF_SYSTEM   (0xDB)   // Comando Skylock. Respuesta cuando el sistema no fue configurado despues de un Power-on o Reset. Solicita configuracion.   

    // Declarando funciones
    unsigned short int fCrcBlk  (unsigned char *pData, unsigned short int nLength);

#endif	/* OSDP_H */
    