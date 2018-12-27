#include <xc.h>
#include "eeprom.h"
#include "cnt_power.h"


void cnt_power_reset(void)      // Chequea cual fue el motivo de la inicializacion y lo adiciona en la eeprom 
{
    if (!POR){                  // Contador Power-on Reset
        write_eeprom(POS_INI+POS_POR, 0x01+read_eeprom(POS_INI+POS_POR));
    }
    else{
        if (!BOR){              // Contador Brown-out Reset
            write_eeprom(POS_INI+POS_BOR, 0x01+read_eeprom(POS_INI+POS_BOR));
        }
        else{
            if (!TO){           // Contador Watchdog Time-out
                write_eeprom(POS_INI+POS_TO, 0x01+read_eeprom(POS_INI+POS_TO));
            }
            else{
                if (!PD){       // Contador Power-Down Detection
                    write_eeprom(POS_INI+POS_PD, 0x01+read_eeprom(POS_INI+POS_PD));
                }
                else{           // Contador RESET Instruction (RI))
                    write_eeprom(POS_INI+POS_RI, 0x01+read_eeprom(POS_INI+POS_RI));
                }
            }
        }  
    }      

    POR = 1;
    BOR = 1;
    RI = 1;
}
