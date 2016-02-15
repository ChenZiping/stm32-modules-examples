/***********************************
** RC522软件SPI驱动程序头文件     **
** by GDUTROBOT-陈子平 2016-2-14 **
** Github:ChenZiping            **
**********************************/
#ifndef _CHENZIPING_RC522_H_
#define _CHENZIPING_RC522_H_

#include "chenziping.h"

///////////////////////////////////////////////////
//MFRC522 register overview

//Reserved 0x00 0x0F 0x10 0x1A 0x1B 0x1E 0x20 0x23 0x25 0x30 0x3C 0x3D 0x3E 0x3F

//Page 0: Command and status
#define CommandReg 0x01//Starts and stops command execution.
#define ComlEnReg 0x02//Control bits to enable and disable the passing of interrupt requests.
#define DivlEnReg 0x03//Control bits to enable and disable the passing of interrupt requests.
#define ComIrqReg 0x04//Interrupt request bits.
#define DivIrqReg 0x05//Interrupt request bits.
#define ErrorReg 0x06//Error bit register showing the error status of the last command executed.
#define Status1Reg 0x07//Contains status bits of the CRC, interrupt and FIFO buffer.
#define Status2Reg 0x08//Contains status bits of the receiver, transmitter and data mode detector.
#define FIFODataReg 0x09//Input and output of 64 byte FIFO buffer.
#define FIFOLevelReg 0x0A//Indicates the number of bytes stored in the FIFO.
#define WaterLevelReg 0x0B//Defines the level for FIFO under- and overflow warning.
#define ControlReg 0x0C//Miscellaneous control bits.
#define BitFramingReg 0x0D//Adjustments for bit-oriented frames.
#define CollReg 0x0E//Defines the first bit-collision detected on the RF interface.

//Page 1: Command
#define ModeReg 0x11//Defines general mode settings for transmitting and receiving.
#define TxModeReg 0x12//Defines the data rate during transmission.
#define RxModeReg 0x13//Defines the data rate during reception.
#define TxControlReg 0x14//Controls the logical behavior of the antenna driver pins TX1 and TX2.
#define TxASKReg 0x15//Controls transmit modulation settings.
#define TxSelReg 0x16//Selects the internal sources for the analog module.
#define RxSelReg 0x17//Selects internal receiver settings.
#define RxThresholdReg 0x18//Selects thresholds for the bit decoder.
#define DemodReg 0x19//Defines demodulator settings.
#define MfTxReg 0x1C//Controls some MIFARE communication transmit parameters.
#define MfRxReg 0x1D
#define SerialSpeedReg 0x1F//Selects the speed of the serial UART interface.

//Page 2: Configuration
#define CRCResultReg_H 0x21//Shows the MSB values of the CRC calculation.
#define CRCResultReg_L 0x22//Shows the LSB values of the CRC calculation.
#define ModWidthReg 0x24//Sets the modulation width.
#define RFCfgReg 0x26//Configures the receiver gain.
#define GsNReg 0x27//Defines the conductance of the antenna driver pins TX1 and TX2 for the n-driver when the driver is switched on.
#define CWGsPReg 0x28//Defines the conductance of the p-driver output during periods of no modulation.
#define ModGsPReg 0x29//Defines the conductance of the p-driver output during modulation.
#define TModeReg 0x2A//These registers define the timer settings.
#define TPrescalerReg 0x2B
#define TReloadReg_H 0x2C//Defines the 16-bit timer reload value.
#define TReloadReg_L 0x2D//Defines the 16-bit timer reload value.
#define TCounterValReg_H 0x2E//Contains the timer value.
#define TCounterValReg_L 0x2F//Contains the timer value.

//Page 3: Test register
#define TestSel1Reg 0x31//General test signal configuration.
#define TestSel2Reg 0x32//General test signal configuration and PRBS control.
#define TestPinEnReg 0x33//Enables the test bus pin output driver.
#define TestPinValueReg 0x34//Defines the HIGH and LOW values for the test port D1 to D7 when it is used as I/O.
#define TestBusReg 0x35//Shows the status of the internal test bus.
#define AutoTestReg 0x36//Controls the digital self-test.
#define VersionReg 0x37//Shows the MFRC522 software version.
#define AnalogTestReg 0x38//Determines the analog output test signal at, and status of, pins AUX1 and AUX2.
#define TestDAC1Reg 0x39//Defines the test value for TestDAC1.
#define TestDAC2Reg 0x3A//Defines the test value for TestDAC2.
#define TestADCReg 0x3B//Shows the values of ADC I and Q channels.
////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//MFRC522 command overview .CommandReg register[3:0].Reserved 0x0D
#define Cmd_Idle 0x00//no action, cancels current command execution
#define Cmd_Mem 0x01//stores 25 bytes into the internal buffer
#define Cmd_GenerateRandomID 0x02//generates a 10-byte random ID number
#define Cmd_CalcCRC 0x03//activates the CRC coprocessor or performs a self test
#define Cmd_Transmit 0x04//transmits data from the FIFO buffer
#define Cmd_NoCmdChange 0x07//no command change, can be used to modify CommandReg register bits without affecting the command
#define Cmd_Receive 0x08//activates the receiver circuits
#define Cmd_Transceive 0x0C//transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define Cmd_MFAuthent 0x0E//performs the MIFARE standard authentication as a reader
#define Cmd_SoftReset 0x0F//resets the MFRC522
///////////////////////////////////////////////////////


//////////////////////////////////////////////////////
//ErrorReg register[7:0] descriptions
typedef enum {
	ProtocolErr = 1,//the SOF is incorrect
	ParityErr = 2,//parity check failed
	CRCErr = 4,//the RxModeReg register’s RxCRCEn bit is set and the CRC calculation fails
	CollErr = 8,
	BufferOvfl = 16,//the FIFO buffer is already full
	TempErr = 64,//internal temperature sensor detects overheating
	WrErr = 128
} ErrType;
//////////////////////////////////////////////////////

/////////////////////////////////////////////////////
//Status2Reg register[2:0]:ModemState[2:0]
typedef enum {
	idle = 0,
	wait4StartSend = 1,//wait for the BitFramingReg register’s StartSend bit
	TxWait = 2,
	transmitting = 3,
	RxWait = 4,
	wait4data = 5,
	receiving = 6
} ModemState;
//////////////////////////////////////////////////////

///////////////////////////////////////////////////////
//Mifare One command word
#define MI_REQSTD 0x26//寻天线区内未进入休眠状态
#define MI_REQALL 0x52//寻天线区内全部卡
#define MI_ANTICOLL 0x93//防冲撞
//#define MI_ANTICOLL2 0x95//防冲撞
#define MI_SELECTTAG 0x93//选卡
#define MI_AUTHENT1A 0x60//验证A密钥
#define MI_AUTHENT1B 0x61//验证B密钥
#define MI_READ 0x30//读块
#define MI_WRITE 0xA0//写块
#define MI_DECREMENT 0xC0//减值
#define MI_INCREMENT 0xC1//加值
#define MI_RESTORE 0xC2//恢复（调块数据到缓冲区）
#define MI_TRANSFER 0xB0//转存（保存缓冲区中数据）
#define MI_HALT 0x50//休眠
#define MI_UNLOCK0 0x40
#define MI_UNLOCK1 0x43
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Error code
#define MI_OK (s8)0
#define MI_NOTAGERR (s8)(-1)
#define MI_ERR (s8)(-2)
/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
//测试平台：stm32f103zet6
//SPI_NSS：PA4,SPI_SCK:PA5,SPI_MISO:PA6,SPI_MOSI:PA7
//RST:PA0
#define RC522_RST PAout(0)
#define RC522_SPI_NSS PAout(4)
#define RC522_SPI_SCK PAout(5)//3
#define RC522_SPI_MISO PAin(6)//4
#define RC522_SPI_MOSI PAout(7)

#define DEF_FIFO_LENGTH 64//FIFO size=64byte
#define MAXRLEN 18

void RC522_SPI_init(void);
void RC522_init(void);

s8 RC522_reset(void);
void RC522_disableAntenna(void);
void RC522_enableAntenna(void);
s8 RC522_request(u8 req_code,u8 *pTagType);
s8 RC522_anticoll(u8 *pSnr);
s8 RC522_select(u8 *pSnr);
s8 RC522_auth(u8 auth_mode,u8 addr,u8 *pKey,u8 *pSnr);
s8 RC522_read(u8 addr,u8 *pData);
s8 RC522_write(u8 addr,u8 *pData);
s8 RC522_val(u8 dd_mode,u8 addr, u8 *pValue);
s8 RC522_bakVal(u8 srcaddr, u8 goaladdr);
s8 RC522_halt(void);
s8 RC522_cmd(u8 cmd, u8 *pInData, u8 InLenByte, u8 *pOutData, u32 *pOutLenBit);
void RC522_calcCRC(u8 *pIndata,u8 len,u8 *pOutData);

void RC522_SPI_writeByte(u8 data);
void RC522_SPI_readByte(u8* data); 

void RC522_SPI_write(u8 addr,u8 data);
u8 RC522_SPI_read(u8 addr); 

void RC522_setBitMask(u8 reg,u8 mask); 
void RC522_clearBitMask(u8 reg,u8 mask); 
s8 RC522_cfgISOType(u8 type);
void RC522_wait4CardOff(void);

s8 RC522_unlock0(u8 cmd);
s8 RC522_unlock1(u8 cmd);

void RC522_writeMI(u8 block, u8 *pKey, u8* data, u8 *pTagType, u8 *SelectedSnr);
u8 RC522_readMI(u8 block, u8 *pKey, u8* data, u8 *pTagType, u8 *SelectedSnr);
/////////////////////////////////////////////////////////////////



#endif /* _CHENZIPING_RC522_H_ */
