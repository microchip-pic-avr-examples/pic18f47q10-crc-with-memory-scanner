/*
Copyright (c) [2012-2020] Microchip Technology Inc.  

    All rights reserved.

    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
    
    To the fullest extend allowed by law, Microchip and its licensors 
    liability will not exceed the amount of fees, if any, that you paid 
    directly to Microchip to use this software. 
    
    THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any 
    third party software accompanying this software is subject to the terms 
    and conditions of the third party's license agreement.  To the extent 
    required by third party licenses covering such third party software, 
    the terms of such license will apply in lieu of the terms provided in 
    this notice or applicable license.  To the extent the terms of such 
    third party licenses prohibit any of the restrictions described here, 
    such restrictions will not apply to such third party software.
*/
#include "mcc_generated_files/system/system.h"

//#define ERASE_EEPROM_CRC              //Uncomment to erase stored CRC

#define START_ADDRESS       ((uint32_t)0x0000) // flash start add
#define END_ADDRESS         ((uint32_t)0x7FFE) // end flash add. total Memory = 32Kb
#define CRC_LOW_BYTE_ADD    (0x0000)           // EEPROM add for storing CRC low byte
#define CRC_HIGH_BYTE_ADD   (0x0001)           // EEPROM add for storing CRC high byte
#define EEPROM_RESET_VALUE  (0xFFFF)           // initial value in EEPROM
#define CRC_SEED_VALUE      (0xFFFF)           // seed value for CRC calculation

#define LED_OFF() LED_SetHigh() //LED will be turned off when the port pin is set high
#define LED_ON() LED_SetLow() //LED will be turned On when the port pin is set low


uint16_t CalculateFlashCRC(void);
void TMR0_UserInterruptHandler(void);

/* Used for computation of CRC of flash. Updated in TMR0 ISR*/
volatile uint8_t periodicCRCflag = 1; 

/*
    Main application
*/

int main(void)
{
    volatile uint16_t CRCStored = 0x0000;
    volatile uint16_t CRCComputed = 0xFFFF;
    
    
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    // Set User Interrupt Handler
    Timer0_OverflowCallbackRegister(TMR0_UserInterruptHandler);
    
#ifdef ERASE_EEPROM_CRC
    
    DATAEE_WriteByte(CRC_LOW_BYTE_ADD, (uint8_t)(EEPROM_RESET_VALUE));
    DATAEE_WriteByte(CRC_HIGH_BYTE_ADD, (uint8_t)(EEPROM_RESET_VALUE >> 8));
    
    printf("\n\rEEPROM location for CRC storage is erased. Reprogram the device with new firmware.\n");
    
    while(1);
    
#else
    
    //Read EEPROM for CRC data
    CRCStored = DATAEE_ReadByte(CRC_HIGH_BYTE_ADD);
    CRCStored = (CRCStored << 8) & 0xFF00;
    CRCStored |= DATAEE_ReadByte(CRC_LOW_BYTE_ADD);
    
    
    //check if is earlier calculated CRC stored in EEPROM or not
    if(CRCStored == EEPROM_RESET_VALUE)
    {
        printf("\n\rCalculating CRC for the first time.\n");
        
        //Compute program memory CRC
        CRCComputed = CalculateFlashCRC();        
        printf("\n\rComputed CRC is 0x%4x.\n",(CRCComputed));
        
        //Store calculated CRC at EEPROM
        DATAEE_WriteByte(CRC_LOW_BYTE_ADD, (uint8_t)(CRCComputed));
        DATAEE_WriteByte(CRC_HIGH_BYTE_ADD, (uint8_t)(CRCComputed >> 8));
    }
  
    while (1)
    {
        // check for flag
        if (periodicCRCflag)
        {
            //clear the flag & reset to initial values
            periodicCRCflag = 0;
            
            CRCStored = 0x0000;
            CRCComputed = 0xFFFF;
            
            //read stored CRC from EEPROM
            CRCStored = DATAEE_ReadByte(CRC_HIGH_BYTE_ADD);
            CRCStored = (CRCStored << 8) & 0xFF00;
            CRCStored |= DATAEE_ReadByte(CRC_LOW_BYTE_ADD);
            printf ("\n\rStored CRC is 0x%4x.", (CRCStored));            
       
            //Compute program memory CRC
            CRCComputed = CalculateFlashCRC ();
            printf ("\n\rPeriodically computed CRC is 0x%4x.", (CRCComputed));

            //compare computed & stored CRC values
            if (CRCComputed != CRCStored)
            {
                //Turn on the LED to indicate CRC mismatch and halt the CPU
                printf ("\n\rCRC error! Computed CRC doesn't match with stored CRC.\n");
                LED_ON();
                while(1);
            }
            else
            {
                printf ("\n\rCRC OK! Computed CRC matches with stored CRC.\n");
                LED_OFF(); 
            }
        }
    }
#endif    
}

/**
  @Description
    This routine reads a data word from given Program Flash Memory 
    address and calculate the CRC

  @Param
    None

  @Returns
    16 bit CRC value of the flash program memory

  @Example
    <code>
    uint16_t CRC;
    CRC = CalculateFlashCRC();
    </code>
*/
uint16_t CalculateFlashCRC(void)
{
    uint16_t CRCVal;
    
    //Load CRC ACC register with seed value
    CRCACCH = (uint8_t)(CRC_SEED_VALUE >> 8);
    CRCACCL = (uint8_t)(CRC_SEED_VALUE);
            
    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(START_ADDRESS, END_ADDRESS);
    // Start Scanner
    CRC_StartScanner();

    // Scan completed?
    while(CRC_IsCrcBusy() || CRC_IsScannerBusy()); 
   
    // Read CRC check value
    CRCVal = CRC_GetCalculatedResult(false,0x00);
    
    return (CRCVal);
}

/**
  @Description
    This is a custom ISR handler for TMR0 ISR.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    TMR0_SetInterruptHandler(TMR0_UserInterruptHandler);
    </code>
*/
void TMR0_UserInterruptHandler(void)
{
    // add your TMR0 interrupt custom code
    // or set custom function using TMR0_SetInterruptHandler()
    
     periodicCRCflag = 1;   //set the flag
}