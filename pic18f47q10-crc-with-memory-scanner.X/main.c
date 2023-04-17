 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

//#define ERASE_EEPROM_CRC              //Uncomment to erase stored CRC

#define START_ADDRESS       ((uint32_t)0x0000) // flash start add
#define END_ADDRESS         ((uint32_t)0x7FFE) // end flash add. total Memory = 32Kb
#define CRC_LOW_BYTE_ADD    (0x310000)           // EEPROM add for storing CRC low byte
#define CRC_HIGH_BYTE_ADD   (0x310001)           // EEPROM add for storing CRC high byte
#define EEPROM_RESET_VALUE  (0xFFFF)           // initial value in EEPROM
#define CRC_SEED_VALUE      (0xFFFF)           // seed value for CRC calculation

#define LED_OFF() LED_SetHigh() //LED will be turned off when the port pin is set high
#define LED_ON() LED_SetLow() //LED will be turned On when the port pin is set low

uint16_t CalculateFlashCRC(void);
void TMR0_UserInterruptHandler(void);
void DATAEE_WriteByte(eeprom_address_t , eeprom_data_t );

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

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    // Set User Interrupt Handler
    Timer0_OverflowCallbackRegister(TMR0_UserInterruptHandler);
     
#ifdef ERASE_EEPROM_CRC
    

    DATAEE_WriteByte(CRC_LOW_BYTE_ADD, (uint8_t)(EEPROM_RESET_VALUE));
    DATAEE_WriteByte(CRC_HIGH_BYTE_ADD, (uint8_t)(EEPROM_RESET_VALUE >> 8));
   
    printf("\n\rEEPROM location for CRC storage is erased. Reprogram the device with new firmware.\n");
            
    while(1);
    
#else    

    //Read EEPROM for CRC data
    CRCStored = EEPROM_Read(CRC_HIGH_BYTE_ADD);
    CRCStored = (CRCStored << 8) & 0xFF00;
    CRCStored |= EEPROM_Read(CRC_LOW_BYTE_ADD);
    
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
    
    while(1)
    {
       // check for flag
        if (periodicCRCflag)
        {
            //clear the flag & reset to initial values
            periodicCRCflag = 0;
            
            CRCStored = 0x0000;
            CRCComputed = 0xFFFF;
            
            //read stored CRC from EEPROM
            CRCStored = EEPROM_Read(CRC_HIGH_BYTE_ADD);
            CRCStored = (CRCStored << 8) & 0xFF00;
            CRCStored |= EEPROM_Read(CRC_LOW_BYTE_ADD);
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
    uint16_t CRCVal = 0;
    bool busyStatus = 0; 
    
    //Load CRC ACC register with seed value
    CRCACCH = (uint8_t)(CRC_SEED_VALUE >> 8);
    CRCACCL = (uint8_t)(CRC_SEED_VALUE);
            
    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(START_ADDRESS, END_ADDRESS);
    // Start Scanner
    CRC_StartScanner();
  
    do
    {
        busyStatus = CRC_IsCrcBusy() || CRC_IsScannerBusy();   
    }
    while(busyStatus);

    // Read CRC check value
    CRCVal = CRC_GetCalculatedResult(false,0x00);
    
    return (CRCVal);
}
/**
  @Description
    This routine writes a data byte on a given EEPROM Memory address

  @Param
    EEPROM Address and data

  @Returns
    None

  @Example
    <code>
    #define HIGH_BYTE_ADD   (0x310001)
    eeprom_data_t  data;
    DATAEE_WriteByte(HIGH_BYTE_ADD,data);
    </code>
*/
void DATAEE_WriteByte(eeprom_address_t address, eeprom_data_t data)
{
    NVM_UnlockKeySet(UNLOCK_KEY_WORD_BYTE_WRITE);
    EEPROM_Write(address,data);
    NVM_UnlockKeyClear();
    while(NVM_IsBusy());
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