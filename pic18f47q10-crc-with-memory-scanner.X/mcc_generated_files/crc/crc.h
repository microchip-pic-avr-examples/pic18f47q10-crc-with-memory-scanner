/**
  CRC Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    crc.h

  @Summary
    This is the generated header file for the CRC driver.

  @Description
    This header file provides APIs for driver for CRC.
    Generation Information :
        Driver Version    :  1.0.1
    The generated drivers are tested against the following:
        Compiler          :  XC8 v2.20
        MPLAB             :  MPLABX v5.40
*/

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

#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stdbool.h>

/**
  @Summary
    Initializes the CRC.

  @Description
    This function initializes the CRC Registers.
    This function must be called before any other CRC function is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
   
   
  @Example
    <code>
    uint16_t buffer [] = {0x55,0x66,0x77,0x88};
    uint16_t crcVal;
    uint16_t length = sizeof(buffer);
    uint16_t value = 0xff00;

    // Initialize the CRC module
    CRC_Initialize();

    // Start CRC
    CRC_StartCrc();

    CRC_WriteData(value);

    while (CRC_IsCrcBusy());

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
void CRC_Initialize(void);

/**
  @Summary
    Starts the CRC module

  @Description
    This routine sets the CRCGO bit of the CRCCON0 register to begin the shifting
    process.

    This routine must be called after the initialization of the CRC module.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t crcVal;

    // Initialize the CRC module
    CRC_Initialize();

    // Start CRC
    CRC_StartCrc();

    // Write data
    CRC_WriteData(0x55)

    // Check CRC busy?
    while(CRC_IsCrcBusy());

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
inline void CRC_StartCrc(void);

/**
  @Summary
    Writes data into CRCDATL register pair.

  @Description
    This routine writes data into CRCDATHL register pair.

  @Preconditions
    None.

  @Returns
    1 - if CRC data registers are not full
    0 - if CRC data registers are full

  @Param
    data:  crc input data

  @Example
    <code>
    uint16_t crcVal;

    // Initialize the CRC module
    CRC_Initialize();

    // Start CRC
    CRC_StartCrc();

    // write 8-bit data
    bool retVal = CRC_WriteData(0x55);

    // Check CRC busy?
    while(CRC_IsCrcBusy());

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
bool CRC_WriteData(uint16_t data);

/**
  @Summary
   Reads crc check value.

  @Description
    This routine reads and returns the Normal or reverse value.

  @Preconditions
    None.

  @Returns
    None.

  @Param
   reverse: false - Normal value, true - Reverse value
   xorValue: value which xored with CRC.

  @Example
     <code>
    uint16_t crcVal;
    // Initialize the CRC module
    CRC_Initialize();

    // Start CRC
    CRC_StartCrc();

    // write 8-bit data
    CRC_WriteData(0x55)

    // Check CRC busy?
    while(CRC_IsCrcBusy());

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
uint16_t CRC_GetCalculatedResult(bool reverse, uint16_t xorValue);

/**
  @Summary
    Reads the status of BUSY bit of CRCCON0 register.

  @Description
    This routine returns the status of the BUSY bit of CRCCON0 register to check
    CRC calculation is over or not.

  @Preconditions
    None.

  @Returns
    1 - CRC busy.
    0 - CRC not busy.

  @Param
    None.

  @Example
    <code>
    uint16_t crcVal;
    // Initialize the CRC module
    CRC_Initialize();

    // Start CRC
    CRC_StartCrc();

    // write 8-bit data
    CRC_WriteData(0x55)

    // Check CRC busy?
    while(CRC_IsCrcBusy());

    // Read calculated CRC
    crcVal = CRC_GetCalculatedResult();
    </code>
*/
inline bool CRC_IsCrcBusy(void);

/**
  @Summary
    Starts the program memory scan

  @Description
    This routine starts the scanning process

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
     uint16_t crcVal;
    // Initialize the CRC SCAN module
    CRC_Initialize();

    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(0x0000,0x07FF);

    // Start Scanner
    CRC_StartScanner();

    // Scan completed?
    while(CRC_IsCrcBusy() ||  CRC_IsScannerBusy());

    // Stop Scanner
    CRC_StopScanner();

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
inline void CRC_StartScanner(void);

/**
  @Summary
    Stops the program memory scan

  @Description
    This routine Stops the scanning process

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
     uint16_t crcVal;
    // Initialize the CRC SCAN module
    CRC_Initialize();

    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(0x0000,0x07FF);

    // Start Scanner
    CRC_StartScanner();

    // Scan completed?
    while(CRC_IsCrcBusy() ||  CRC_IsScannerBusy());

    // Stop Scanner
    CRC_StopScanner();

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
inline void CRC_StopScanner(void);

/**
  @Summary
    Sets the memory address limit for scanning

  @Description
    This routine loads  address limits into the SCANLADRH/L and SCANHADRH/L register pairs.

  @Preconditions
    None.

  @Returns
    None.

  @Param
   startAddr: Starting address of memory block.
   endAddr:   Ending address of memory block.

  @Example
    <code>
     uint16_t crcVal;
    // Initialize the CRC SCAN module
    CRC_Initialize();

    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(0x0000,0x07FF);

    // Start Scanner
    CRC_StartScanner();

    // Scan completed?
    while(CRC_IsCrcBusy() ||  CRC_IsScannerBusy());

    // Stop Scanner
    CRC_StopScanner();

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
void CRC_SetScannerAddressLimit(uint24_t startAddr, uint24_t endAddr);

/**
  @Summary
    Returns the status of BUSY bit of SCANCON0 register.

  @Description
    This routine returns the status of BUSY bit of SCANCON0 register.

  @Preconditions
    None.

  @Returns
    0 - SCAN not yet started.
    1- SCAN is in progress.

  @Param
    None.

  @Example
    <code>
     uint16_t crcVal;
    // Initialize the CRC SCAN module
    CRC_Initialize();

    // Wait if scanner is in progress.
    While(CRC_IsScannerBusy()!=1);

    // Sets SCAN address limit
    CRC_SetScannerAddressLimit(0x0000,0x07FF);

    // Start Scanner
    CRC_StartScanner();

    // Scan completed?
    while(CRC_IsCrcBusy() ||  CRC_IsScannerBusy());

    // Stop Scanner
    CRC_StopScanner();

    // Read CRC check value
    crcVal = CRC_GetCalculatedResult(false,0x00);
    </code>
*/
inline bool CRC_IsScannerBusy(void);


#endif //CRC_H
