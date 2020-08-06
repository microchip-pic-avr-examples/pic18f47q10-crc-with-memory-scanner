[![MCHP](https://cldup.com/U0qhLwBijF.png)](https://www.microchip.com)

# CRC with MEMORY SCANNER

# Introduction

This example demonstrates the use of CRC peripheral in PIC18F47Q10 MCU. CRC module in PIC MCUs is hardware implemented checksum generator which computes 16-bit CRC with programmable polynomial. It is coupled with memory scanner for faster CRC calculations. The memory scanner can automatically provide data to the CRC module This example uses CRC-16-CCITT standard parameters.
This demo calculates the CRC of the program memory and store it in the EEPROM area of the controller after programming the device for the first time. On the subsequent power-ups, the device   computes the flash CRC at startup and checks it against the CRC stored in EEPROM area. In case of mismatch the program execution is indicates CRC error. This CRC checking can be scheduled periodically during device operation to ensure flash integrity.

![](https://i.imgur.com/HrnpMqR.jpg)

Figure 1: Program Flowchart

# Related Documentation
- [PIC18-Q10 Product Family Page](https://www.microchip.com/design-centers/8-bit/pic-mcus/device-selection/pic18f-q10-product-family)
- [PIC18F47Q10 Data Sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/40002043D.pdf)
- [PIC18F47Q10 Code Examples on GitHub](https://github.com/microchip-pic-avr-examples?q=pic18f47q10-cnano)

# Hardware Used
* [PIC18F47Q10 Curiosity nano evaluation board](https://www.microchip.com/Developmenttools/ProductDetails/DM182029)

With full program and debug capabilities, the PIC18F47Q10 Curiosity Nano evaluation kit offers complete support for the new design. With the award-winning MPLAB X integrated development platform and MPLAB Code Configurator (MCC), the kit provides access to the Intelligent analog and Core Independent Peripherals on the PIC18F47Q10. Figure 2 shows PIC18F47Q10 Curiosity Nano board.
![](https://i.imgur.com/IRS8AIx.jpg?1)

Figure 2: PIC18F47Q10 Curiosity Nano board
# Software tools
Microchip’s free MPLAB X IDE, compiler and MPLAB Code Configurator (MCC) graphical code generator are used throughout the application firmware development to provide easy and hassle-free user experience. Following are the tool versions used for this demo application:

* [MPLAB X IDE v5.30](http://www.microchip.com/mplab/mplab-x-ide)
* [XC8 Compiler v2.10](http://www.microchip.com/mplab/compilers)
* [MPLAB Code Configurator (MCC) v3.85.1](https://www.microchip.com/mplab/mplab-code-configurator)
* [Microchip PIC18F-Q Series Device Support 1.3.89](https://packs.download.microchip.com/)

*Note: For running the demo, the installed tool versions should be same or later. This example is not tested with previous versions.*
# Demo realization
1. **Start by creating a new Project**

	* Go to File > New Project
	* Select Microchip Embedded > Standalone Project
	* Enter the device name. In this case we are using the PIC18F47Q10
	* Name the project

2. **Configure the hardware peripherals**

	*  **Configure System Module**

		The system configuration window of MCC is used for MCU oscillator, PLL, Watchdog timer and low voltage programming configuration. Internal oscillator of 1MHz frequency with clock divider 1 is used as a system clock and the Watchdog timer is disabled in this example. Figure 3 shows the system configuration settings used in this example.

![](https://i.imgur.com/HOMyzF5.png)

Figure 3: System Configuration

 *  **Add peripherals to the project**

  Add CRC, EUSART2, Timer0 and memory peripherals to the project.

![](https://i.imgur.com/Ow3jTju.png)

Figure 4: Peripherals

* **Configure the CRC peripheral**

Check “Enable CRC”
Check “Use Pre-defined Polynomial”
Select “CRC-16-CCITT” from list of Pre-defined Polynomial
Confirm the “Seed” value is “0xFFFF”
Set “Data Word Width (bits)” to “16” (As Flash memory is in 16-bit registers)
Check “Enable Scanner” (We will use scanner to fetch data from memory)

![](https://i.imgur.com/NFzkJEM.png)

Figure 5: CRC Configuration

* **Configure Memory peripheral**

Check “Add DataEE Routines” (We will use these routines to write EEPROM data)

![](https://i.imgur.com/cyd7Zxw.png)

Figure 6: Memory Configuration

* **Configure Eusart2 Peripheral**

In this demo EUSART2 is used to transmit data on the terminal window to display the stored and computed CRC value as well as the error message if there is any mismatch in the CRC is detected. Figure 6 shows the EUSART2 module configuration settings.
* Configurations to be done on EUSART2 peripheral:
* Check the Enable EUSART checkbox
* Check the Enable Transmit checkbox
* Uncheck the Enable Receive checkbox
* Set the Baud Rate to 9600
* Check the Redirect STDIO to USART checkbox

![](https://i.imgur.com/5k7S1Gf.png)

Figure 7: EUSART2 Configuration

* **Configure Timer0 Peripheral**

In this demo Timer 0 is used to generate periodic event for checking the CRC of the program memory.
Timer 0 period can be changed to change the CRC calculation frequency.

* Configurations to be done on Timer0 peripheral:
* Check the Enable Timer checkbox
* Check the Enable Timer Interrupt checkbox
* Select the clock source as LFINTOSC
* Select the clock pre-scaler as 1:32768
* Set the desired timer period. The period selected in this example is 20 S

![](https://i.imgur.com/sE4YSjg.png)

Figure 8: Timer 0 Configuration

* **Configure the pin used on the device**

Set PE0 to output for LED indication using pin manager grid view.
Select RD0 as EUSART2:TX2 output.

![](https://i.imgur.com/3Fqjarx.png)

Figure 9: Pin Manager

Add custom name to the PE0 output pin as LED using Pin module. Check the Start High check box for LED pin PE0 for turning off the LED.

![](https://i.imgur.com/omO3qy5.png)

Figure 10: Pin Module

3. **Generate the Project files**
* Click the Generate button next to the project Resources heading to send the configured code back to MPLAB Xpress. Next step is to add custom code to complete the example as following.

**Open main.c file.**

Steps to calculate flash CRC using MCC generated APIs listed in crc.c file:

* Set memory SCAN address limits or block size of the program memory of which CRC to be calculated using following API:

`CRC_SCAN_SetAddressLimit(START_ADDRESS, END_ADDRESS);`

(Start address used in this demo is 0x00000 and end address used is 0x7FFE. So total block size of the memory used for CRC calculation is 32Kb.
*Note: If the program size exceeds 32kb then increase the block size by changing the End address)*

* Start the memory Scanner using the following MCC generated API:

`// CRC_SCAN_StartScanner();`

* Check if memory Scan is completed using the following MCC generated API:

`while (!CRC_SCAN_HasScanCompleted());`

* Stop the memory Scanner using the following MCC generated API:

`CRC_SCAN_StopScanner();`

* Calculate and read CRC using the following MCC generated API:

`CRCVal = CRC_CalculatedResultGet(NORMAL,0x00);`

4. **Build the project file and program the device**
* Program the MCU by clicking “Make and Program Device” icon on MPLAB XPRESS IDE as shown in below figure.
* IDE generates the .hex file, save the file in local PC
* Drag the .hex from the downloads section and drop the file on to the Curiosity drive. This should program the device

*Note: The additional last 2 steps required for MPLAB XPRESS IDE. For MPLAB X IDE, the only first step is sufficient to program the device*
![](https://i.imgur.com/tFBNCw1.jpg)

Figure 11: Make and Program the Device
# Demo operation
* For first power on OR whenever firmware is changed w.r.t earlier firmware , remove comment for the line `//#define ERASE_EEPROM_CRC`   in the code to erase the EEPROM location which stores the CRC for making sure that there isn’t any incorrect OR earlier calculated CRC value stored previously at that location. Build the project and program the device. Observe the message displayed on the terminal window. (Any terminal emulator can be used such as Tera Term. Set baud rate as 9600.)

![](https://i.imgur.com/SutgqIB.png)

Figure 12: EEPROM erase message

* For next power up, Comment the line “#define ERASE_EEPROM_CRC”.  Build the project and program the device.
* MCU calculates and displays the CRC of the program memory on the terminal window. The first calculated CRC is stored in the EEPROM location. The stored CRC then is compared with the calculated CRCs on subsequent power on of the devices as well as with the periodic calculated CRC. If there is mismatch in CRC, then program execution can be halted, and corresponding action can be taken.

![](https://i.imgur.com/BTeIoh5.png/)

Figure 13: First Time CRC calculation

* CRC of the program is calculated periodically and displayed on the terminal window.
* If there is mismatch in the stored CRC and computed CRC, then error message is displayed, and program execution is halted.

![](https://i.imgur.com/l0NW2sV.png)

Figure 14: Periodic CRC

*Note: CRC in Figure14 is computed using compiler v2.10 with -0 optimization level.*

# Conclusion
CRC module in PIC MCUs is hardware implemented checksum generator which can compute 16-bit CRC with programmable polynomial. It is also complimented with memory scanner feature which provides automatic flash read for CRC calculation. Configuring the CRC module is easy using the MCC GUI. Apart from the module configuration, MCC generates ready to use APIs for hassle free calculation of the CRC of the program memory using CRC and memory scan hardware peripheral in the PIC MCUs.
