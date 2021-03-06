/***********************************
** MPU6050软件I2C驱动程序头文件    **
** by GDUTROBOT-陈子平 2016-2-12 **
** Github:ChenZiping            **
**********************************/
#ifndef _CHENZIPING_MPU6050_H_
#define _CHENZIPING_MPU6050_H_

#include "chenziping.h"

//如果AD0脚(9脚)接地,I2C地址为0x68(不包含最低位).
//如果接V3.3,则I2C地址为0x69(不包含最低位).
//(不包含最低位!!)
#define MPU_ADDR 0x68 //器件地址
//因为模块AD0默认接GND,所以转为读写地址后,为0xD1和0xD0(如果接VCC,则为0xD3和0xD2) 
//(此处包含最低位!!) 
//0x68(0b1101 000) + 最低位(0\1) -> 0xD0(0b1101 0000)、0xD1(0b1101 0001)
//0x69(0b1101 001) + 最低位(0\1) -> 0xD2(0b1101 0010)、0xD3(0b1101 0011)
//这是软件i2c和硬件i2c的区别，硬件的地址是7位，而软件模拟的话需要是8位地址
#define MPU_WRITE (MPU_ADDR << 1)//0xD0
#define MPU_READ (MPU_WRITE + 1)//0xD1

//#define MPU_AD0_CTRL PBout(8)	//AD0接地(控制AD0电平,从而控制MPU地址)

#define MPU_I2C_DELAY() {delayUs(4);}

//I2C SDA:PB7 SCL:PB6
#define MPU_SDA_IN() {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= (u32)(1 << 31);}//上/下拉输入
#define MPU_SDA_OUT() {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= (u32)(3 << 28);}//推挽输出
 
#define MPU_I2C_SCL PBout(6) //SCL
#define MPU_I2C_SDA_W PBout(7) //输出SDA	 
#define MPU_I2C_SDA_R PBin(7) //输入SDA 

#define MPU_ACCEL_OFFS 0x06	//accel_offs寄存器,可读取版本号,寄存器手册未提到
#define MPU_PROD_ID 0x0C //Product ID Register,在寄存器手册未提到

#define MPU_SELF_TEST_X 0x0D //自检寄存器X
#define MPU_SELF_TEST_Y 0x0E //自检寄存器Y
#define MPU_SELF_TEST_Z 0x0F //自检寄存器Z
#define MPU_SELF_TEST_A 0x10 //自检寄存器A
#define MPU_SMPLRT_DIV 0x19	//采样频率分频器
#define MPU_CFG 0x1A	//配置寄存器
#define MPU_GYRO_CFG 0x1B //陀螺仪配置寄存器
#define MPU_ACCEL_CFG 0x1C	//加速度计配置寄存器
#define MPU_MOT_THR 0x1F //运动检测阀值设置寄存器
#define MPU_FIFO_EN 0x23 //FIFO使能寄存器
#define MPU_I2C_MST_CTRL 0x24	//I2C主机控制寄存器
#define MPU_I2C_SLV0_ADDR 0x25 //I2C从机0器件地址寄存器
#define MPU_I2C_SLV0_REG 0x26	//I2C从机0数据地址寄存器
#define MPU_I2C_SLV0_CTRL 0x27 //I2C从机0控制寄存器
#define MPU_I2C_SLV1_ADDR 0x28 //I2C从机1器件地址寄存器
#define MPU_I2C_SLV1_REG 0x29	//I2C从机1数据地址寄存器
#define MPU_I2C_SLV1_CTRL 0x2A //I2C从机1控制寄存器
#define MPU_I2C_SLV2_ADDR 0x2B //I2C从机2器件地址寄存器
#define MPU_I2C_SLV2_REG 0x2C	//I2C从机2数据地址寄存器
#define MPU_I2C_SLV2_CTRL 0x2D //I2C从机2控制寄存器
#define MPU_I2C_SLV3_ADDR 0x2E //I2C从机3器件地址寄存器
#define MPU_I2C_SLV3_REG 0x2F	//I2C从机3数据地址寄存器
#define MPU_I2C_SLV3_CTRL 0x30 //I2C从机3控制寄存器
#define MPU_I2C_SLV4_ADDR 0x31 //I2C从机4器件地址寄存器
#define MPU_I2C_SLV4_REG 0x32	//I2C从机4数据地址寄存器
#define MPU_I2C_SLV4_DO 0x33 //I2C从机4写数据寄存器
#define MPU_I2C_SLV4_CTRL 0x34 //I2C从机4控制寄存器
#define MPU_I2C_SLV4_DI 0x35 //I2C从机4读数据寄存器

#define MPU_I2C_MST_STA 0x36	//I2C主机状态寄存器
#define MPU_INT_PIN_CFG 0x37 //中断/旁路设置寄存器
#define MPU_INT_EN 0x38	//中断使能寄存器
#define MPU_INT_STA 0x3A //中断状态寄存器

#define MPU_ACCEL_XOUT_H 0x3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUT_L 0x3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUT_H 0x3D //加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUT_L 0x3E //加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUT_H 0x3F //加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUT_L 0x40 //加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUT_H 0x41	//温度值高八位寄存器
#define MPU_TEMP_OUT_L 0x42	//温度值低8位寄存器

#define MPU_GYRO_XOUT_H 0x43 //陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUT_L 0x44 //陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUT_H 0x45 //陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUT_L 0x46 //陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUT_H 0x47 //陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUT_L 0x48 //陀螺仪值,Z轴低8位寄存器

#define MPU_EXT_SENS_DATA_00 0x49
#define MPU_EXT_SENS_DATA_01 0x4A
#define MPU_EXT_SENS_DATA_02 0x4B
#define MPU_EXT_SENS_DATA_03 0x4C
#define MPU_EXT_SENS_DATA_04 0x4D
#define MPU_EXT_SENS_DATA_05 0x4E
#define MPU_EXT_SENS_DATA_06 0x4F
#define MPU_EXT_SENS_DATA_07 0x50
#define MPU_EXT_SENS_DATA_08 0x51
#define MPU_EXT_SENS_DATA_09 0x52
#define MPU_EXT_SENS_DATA_10 0x53
#define MPU_EXT_SENS_DATA_11 0x54
#define MPU_EXT_SENS_DATA_12 0x55
#define MPU_EXT_SENS_DATA_13 0x56
#define MPU_EXT_SENS_DATA_14 0x57
#define MPU_EXT_SENS_DATA_15 0x58
#define MPU_EXT_SENS_DATA_16 0x59
#define MPU_EXT_SENS_DATA_17 0x5A
#define MPU_EXT_SENS_DATA_18 0x5B
#define MPU_EXT_SENS_DATA_19 0x5C
#define MPU_EXT_SENS_DATA_20 0x5D
#define MPU_EXT_SENS_DATA_21 0x5E
#define MPU_EXT_SENS_DATA_22 0x5F
#define MPU_EXT_SENS_DATA_23 0x60

#define MPU_I2C_SLV0_DO 0x63 //I2C从机0数据寄存器
#define MPU_I2C_SLV1_DO 0x64 //I2C从机1数据寄存器
#define MPU_I2C_SLV2_DO 0x65 //I2C从机2数据寄存器
#define MPU_I2C_SLV3_DO 0x66 //I2C从机3数据寄存器

#define MPU_I2C_MST_DELAY_CTRL 0x67	//I2C主机延时管理寄存器
#define MPU_SIGNAL_PATH_RST 0x68 //信号通道复位寄存器
#define MPU_MOT_DETECT_CTRL 0x69 //运动检测控制寄存器
#define MPU_USER_CTRL 0x6A //用户控制寄存器
#define MPU_PWR_MGMT_1 0x6B	//电源管理寄存器1
#define MPU_PWR_MGMT_2 0x6C	//电源管理寄存器2 
#define MPU_FIFO_CNTH 0x72 //FIFO计数寄存器高八位
#define MPU_FIFO_CNTL 0x73 //FIFO计数寄存器低八位
#define MPU_FIFO_R_W 0x74	//FIFO读写寄存器
#define MPU_WHO_AM_I 0x75	//器件ID寄存器

void MPU_I2C_init(void); //初始化I2C的IO口				 
void MPU_I2C_start(void); //发送I2C开始信号
void MPU_I2C_stop(void); //发送I2C停止信号
void MPU_I2C_sendByte(u8 txd); //I2C发送一个字节
u8 MPU_I2C_readByte(u8 ack);//I2C读取一个字节
u8 MPU_I2C_waitACK(void); //I2C等待ACK信号,0成功,1失败
void MPU_I2C_respond(u8 ack);

//typedef enum {Temp, Gyro, Accel} Sensor_Type;

//u8 MPU_readData(Sensor_Type sensor, u16* data);

u8 MPU_init(void);
u8 MPU_write(u8 reg,u8 len,u8 *buf);
u8 MPU_read(u8 reg,u8 len,u8 *buf);
u8 MPU_writeByte(u8 reg, u8 data);
u8 MPU_readByte(u8 reg,u8* buf);
u8 MPU_dummyRead(void);

u8 MPU_setGyroFsr(u8 fsr);
u8 MPU_setAccelFsr(u8 fsr);
u8 MPU_setLPF(u16 lpf);
u8 MPU_setRate(u16 rate);
u8 MPU_setFIFO(u8 sens);

u8 MPU_getTemp(float *temp);
u8 MPU_getGyro(short *gx,short *gy,short *gz);
u8 MPU_getAccel(short *ax,short *ay,short *az);

#endif /* _CHENZIPING_MPU6050_H_ */
