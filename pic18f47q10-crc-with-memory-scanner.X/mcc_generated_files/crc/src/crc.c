/**
  CRC Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    crc.c

  @Summary
    This is the generated driver implementation file for the CRC driver.

  @Description
    This source file provides APIs for driver for CRC.
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "../crc.h"



void CRC_Initialize(void)
{
    //CRC Configurations
    //CRCDLEN 15; CRCPLEN 15; 
    CRCCON1 = 0xFF;

    //CRCXORH 16; 
    CRCXORH = 0x10;
    //CRCXORL 33; 
    CRCXORL = 0x21;

    //CRCACCH 255; 
    CRCACCH = 0xFF;
    //CRCACCL 255; 
    CRCACCL = 0xFF;

    //CRCDATH 0; 
    CRCDATH = 0x0;
    //CRCDATL 0; 
    CRCDATL = 0x0;

    //Scanner Configurations
    //
    SCANHADRH = 0xFF;
    //
    SCANHADRL = 0xFF;
    //
    SCANLADRH = 0x0;
    //
    SCANLADRL = 0x0;
    //SCANTSEL LFINTOSC; 
    SCANTRIG = 0x0;

    // Clearing CRC IF flag
    PIR7bits.CRCIF = 0;
    // Disabled CRCI CRC interrupt
    PIE7bits.CRCIE = 0;

    // Clearing Scanner IF flag.
    PIR7bits.SCANIF = 0;
    // Disabled SCANI CRC interrupt
    PIE7bits.SCANIE = 0;

    //CRCEN enabled; CRCGO disabled; CRCACCM data not augmented with 0's; CRCSHIFTM shift left; 
    CRCCON0 = 0x80;
    //SCANEN enabled; SCANGO disabled; SCANINTM not affected; SCANMODE Concurrent mode; 
    SCANCON0 = 0x80;
}

inline void CRC_StartCrc(void)
{
    // Start the serial shifter
    CRCCON0bits.CRCGO = 1;
}

bool CRC_WriteData(uint16_t data)
{
    if(!CRCCON0bits.FULL)
    {
        CRCDATH = (uint8_t)((data >> 8) & 0xFF);
        CRCDATL = (uint8_t)(data & 0xFF);
        return true;
    } 
    else 
    {
        return false;
    }
}

static uint16_t CRC_ReverseValue(uint16_t crc)
{
    uint16_t mask;
    uint16_t reverse;

    mask = 1;
    mask <<= CRCCON1bits.PLEN;
    reverse = 0;

    while(crc)
    {
        if(crc & 0x01)
        {
            reverse |= mask;
        }
        mask >>= 1;
        crc >>= 1;
    }
    return reverse;
}

uint16_t CRC_GetCalculatedResult(bool reverse, uint16_t xorValue)
{
    uint16_t result = 0x00;
    result = (uint16_t)CRCACCL;
    result = result | ((uint16_t)CRCACCH << 8);
    if(reverse)
    {
        result = CRC_ReverseValue(result);
    }
    result ^= xorValue;
    return (result & 0xFFFF);
}

inline bool CRC_IsCrcBusy(void)
{
    return(CRCCON0bits.BUSY);
}

inline void CRC_StartScanner(void)
{
    // Start the serial shifter
    CRCCON0bits.CRCGO = 1;
    // Start the scanner
    SCANCON0bits.SCANGO = 1;
}

inline void CRC_StopScanner(void)
{
    // Stop the serial shifter
    CRCCON0bits.CRCGO = 0;
    // Stop the scanner
    SCANCON0bits.SCANGO = 0;
}

void CRC_SetScannerAddressLimit(uint24_t startAddr, uint24_t endAddr)
{
    SCANHADRU = (uint8_t)((endAddr >> 16) & 0xFF);
    SCANHADRH = (uint8_t)((endAddr >> 8) & 0xFF);
    SCANHADRL = (uint8_t)(endAddr & 0xFF);
    SCANLADRU = (uint8_t)((startAddr >> 16) & 0xFF);
    SCANLADRH = (uint8_t)((startAddr >> 8) & 0xFF);
    SCANLADRL = (uint8_t)(startAddr & 0xFF);
}

inline bool CRC_IsScannerBusy(void)
{
    return(SCANCON0bits.SCANBUSY);
}


