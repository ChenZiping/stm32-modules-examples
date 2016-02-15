/***********************************
** MPU6050软件I2C驱动程序         **
** by GDUTROBOT-陈子平 2016-2-12 **
** Github:ChenZiping            **
**********************************/
#include "MPU6050.h"

u8 LPF_sta = 0;

void MPU_I2C_init()
{
	RCC->APB1ENR |= 1 << 21;
	RCC->APB2ENR |= 1 << 3;							 
	GPIOB->CRL &= 0x00FFFFFF;
	GPIOB->CRL |= 0x33000000;//SDA默认输出
	GPIOB->ODR |= 3 << 6;
}

void MPU_I2C_start()
{
	MPU_SDA_OUT();
	MPU_I2C_SDA_W = 1;
	MPU_I2C_DELAY();
	MPU_I2C_SCL = 1;
	MPU_I2C_DELAY();
	MPU_I2C_SDA_W = 0;
	MPU_I2C_DELAY();
	//MPU_I2C_SCL = 0;//拉低SCL,等待发送或接收数据
}

void MPU_I2C_stop()
{
	MPU_SDA_OUT();
	MPU_I2C_SDA_W = 0;
	MPU_I2C_DELAY();
	MPU_I2C_SCL = 1;
	MPU_I2C_DELAY();
	MPU_I2C_SDA_W = 1;
	delayMs(10);
}

void MPU_I2C_sendByte(u8 txd)
{
	u8 i;
	MPU_SDA_OUT();
	MPU_I2C_SCL = 0;//拉低SCL,等待发送数据
	MPU_I2C_DELAY();
	for(i = 0;i < 8;i ++)
	{
		MPU_I2C_SDA_W = (txd & 0x80) >> 7;
		txd <<= 1;
		MPU_I2C_DELAY();
		MPU_I2C_SCL = 1;
		MPU_I2C_DELAY();
		MPU_I2C_SCL = 0;
		MPU_I2C_DELAY();
	}
}

u8 MPU_I2C_readByte(u8 ack)
{
	u8 i, recv = 0;
	MPU_SDA_IN();
	for(i = 0;i < 8;i ++)
	{
		MPU_I2C_SCL = 0;
		MPU_I2C_DELAY();
		MPU_I2C_SCL = 1;
		MPU_I2C_DELAY();
		if(MPU_I2C_SDA_R) recv ++;
		recv <<= 1;
		MPU_I2C_DELAY();
	}
	MPU_I2C_SCL = 0;
	MPU_I2C_respond(ack);
	return recv;
}

u8 MPU_I2C_waitACK()
{
	u8 errTime = 0;
	//MPU_SDA_OUT();
	//MPU_I2C_SCL = 0;
	//MPU_I2C_DELAY();
	//MPU_I2C_SDA_W = 1;
	//MPU_I2C_DELAY();
	MPU_I2C_SCL = 1;
	MPU_I2C_DELAY();
	MPU_SDA_IN();
	while(MPU_I2C_SDA_R)
	{
		errTime ++;
		if(errTime > 250)
		{
			MPU_I2C_stop();
			return 1;
		}
	}
	MPU_I2C_SCL = 0;//拉低SCL,等待发送或接收数据
	MPU_I2C_DELAY();
	return 0;
}

void MPU_I2C_respond(u8 ack)//应答信号:在第9个时钟周期SCL为高时,SDA为低
{
	MPU_SDA_OUT();
	MPU_I2C_SCL = 0;//拉低SCL,等待发送或接收数据
	MPU_I2C_DELAY();
	MPU_I2C_SDA_W = 1 - ack;
	MPU_I2C_DELAY();
	MPU_I2C_SCL = 1;
	MPU_I2C_DELAY();
	MPU_I2C_SCL = 0;
	MPU_I2C_DELAY();
}

u8 MPU_init()
{
	u8 res;
	MPU_I2C_init();
	delayMs(1000);
	MPU_writeByte(MPU_PWR_MGMT_1, 0x80);//reset MPU
	delayMs(100);
	MPU_writeByte(MPU_PWR_MGMT_1, 0x00);//awake MPU
	
	MPU_readByte(MPU_WHO_AM_I, &res); 
	if(res == MPU_WRITE)//器件ID正确
	{
		MPU_writeByte(MPU_PWR_MGMT_1, 0x01);//设置CLKSEL,PLL X轴为参考
		MPU_writeByte(MPU_PWR_MGMT_2, 0x00);//加速度与陀螺仪都工作
		MPU_setGyroFsr(3);//陀螺仪传感器,±2000dps
		MPU_setAccelFsr(3);//加速度传感器,±2g
		//MPU_setRate(100);//设置采样率50Hz
		MPU_writeByte(MPU_INT_EN, 0x00);	//关闭所有中断
		MPU_writeByte(MPU_USER_CTRL, 0x00);	//I2C主模式关闭
		MPU_writeByte(MPU_FIFO_EN, 0x00);	//关闭FIFO
		MPU_writeByte(MPU_INT_PIN_CFG, 0x80);	//INT引脚低电平有效
		//MPU_setRate(100);//设置采样率50Hz
		MPU_writeByte(MPU_SMPLRT_DIV, 0x07);//125Hz
		MPU_writeByte(MPU_CFG, 0x06);//5Hz
		return 0;
 	}
	else return 1;
	//return res;
}

u8 MPU_write(u8 len, u8 reg, u8 *buf)
{
	u8 i;
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		return 1;
	}
	MPU_I2C_sendByte(reg);
	if(MPU_I2C_waitACK())
	{
		return 2;
	}
	for(i = 0;i < len;i ++)
	{
		MPU_I2C_sendByte(buf[i]);
		if(MPU_I2C_waitACK())
		{
			return (i + 3);
		}	
	}    
	MPU_I2C_stop();	 
	return 0;	
}

u8 MPU_read(u8 len, u8 reg, u8 *buf)
{
	u8 i = len;
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		return 1;
	}
  MPU_I2C_sendByte(reg);
	if(MPU_I2C_waitACK())
	{
		return 2;
	}
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_READ);
	if(MPU_I2C_waitACK())
	{
		return 3;
	}
	while(len)
	{
		if(len == 1) buf[i - len] = MPU_I2C_readByte(0);//读数据,发送NACK 
		else buf[i - len] = MPU_I2C_readByte(1);//读数据,发送ACK  
		len --;
		//buf ++; 
	}   
	MPU_I2C_stop();
	return 0;	
}

u8 MPU_writeByte(u8 reg, u8 data)
{
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		return 1;
	}
	MPU_I2C_sendByte(reg);
	if(MPU_I2C_waitACK())
	{
		return 2;
	}
	MPU_I2C_sendByte(data);
	if(MPU_I2C_waitACK())
	{
		return 3;
	}	
	MPU_I2C_stop();	 
	return 0;	
}

u8 MPU_readByte(u8 reg,u8* buf)
{
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		return 1;
	}
  MPU_I2C_sendByte(reg);
	if(MPU_I2C_waitACK())
	{
		return 2;
	}
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_READ);
	if(MPU_I2C_waitACK())
	{
		return 3;
	}
	*buf = MPU_I2C_readByte(0);//读数据,发送NACK   
	MPU_I2C_stop();
	return 0;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
u8 MPU_setGyroFsr(u8 fsr)
{
	return MPU_writeByte(MPU_GYRO_CFG, (fsr << 3));
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
u8 MPU_setAccelFsr(u8 fsr)
{
	return MPU_writeByte(MPU_ACCEL_CFG, (fsr << 3));
}

//设置MPU6050的数字低通滤波器
//bandwidth:带宽5-260Hz
u8 MPU_setLPF(u16 bandwidth)
{	
	u8 tmp;
	if(bandwidth >= 256) tmp = 0;
	else if(bandwidth >= 184) tmp = 1;
	else if(bandwidth >= 94) tmp = 2;
	else if(bandwidth >= 42) tmp = 3;
	else if(bandwidth >= 20) tmp = 4;
	else if(bandwidth >= 10) tmp = 5;
	else if(bandwidth >= 5) tmp = 6;
	else
	{
		LPF_sta = 0;
		return 1;
	}
	if(!MPU_write(1, MPU_CFG, &tmp))
	{
		if(tmp == 0) LPF_sta = 0;
		else LPF_sta = 1;
		return 0;
	}
	return 2;
}

//设置MPU6050的采样率(假定Fs=1KHz)
//传感器寄存器输出，FITO输出，DMP采样，Motion检测，ZeroMotion检测和 Free Fall检测都基于这个采样频率。
//采样频率=陀螺仪输出频率/（1+SMPLRT_DIV）
//当 DLPF is disabled（DLPF_CFG=0 or 7），陀螺输出频率=8kHz；当 DLPF is enabled，陀螺仪输出频率=1KHz。
//注意：加速度计输出频率为 1KHz。这意味着，对于一个大于 1kHz的采样率，同个加速度计的采样，
//可能不止一次是输出到 FIFO，DMP，传感器寄存器。
u8 MPU_setRate(u16 rate)
{
	u8 tmp;
	MPU_setLPF(rate / 2);
	if(LPF_sta) tmp = 1000 / rate - 1;
	else tmp = 8000 / rate - 1;
	return MPU_write(1, MPU_SMPLRT_DIV, &tmp);
}

//u8 MPU_setFIFO(u8 sens)

u8 MPU_getTemp(float *temp)
{
	u8 tmp[2];
	short data;
	MPU_read(2, MPU_TEMP_OUT_H, tmp);
	data = ((s16)(tmp[0]) << 8) | tmp[1];
	*temp = 36.53 + ((double)data) / 340;
	delayMs(100);
	return 0;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
u8 MPU_getGyro(short *gx,short *gy,short *gz)
{
	u8 buf[6];
	if(MPU_read(6, MPU_GYRO_XOUT_H, buf)) return 1;
	else
	{
		*gx = (s16)((buf[0] << 8) | buf[1]);
		*gy = (s16)((buf[2] << 8) | buf[3]);
		*gz = (s16)((buf[4] << 8) | buf[5]);
		delayMs(100);
		return 0;
	}
}

//得到加速度值(原始值)
//gx,gy,gz:加速度x,y,z轴的原始读数(带符号)
u8 MPU_getAccel(short *ax,short *ay,short *az)
{
	u8 buf[6];
	if(MPU_read(6, MPU_ACCEL_XOUT_H, buf)) return 1;
	else
	{
		*ax = (s16)((buf[0] << 8) | buf[1]);
		*ay = (s16)((buf[2] << 8) | buf[3]);
		*az = (s16)((buf[4] << 8) | buf[5]);
		delayMs(100);
		return 0;
	}
}

/*u8 MPU_readAccel(u16* data)
{
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 1;
	}
	MPU_I2C_sendByte(MPU_ACCEL_XOUT_H);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 2;
	}
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_READ);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 3;
	}
	*data = MPU_I2C_readByte(0);
	(*data) <<= 8;
	MPU_I2C_stop();
	
	MPU_I2C_DELAY();
	
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_WRITE);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 1;
	}
	MPU_I2C_sendByte(MPU_ACCEL_XOUT_L);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 2;
	}
	MPU_I2C_start();
	MPU_I2C_sendByte(MPU_READ);
	if(MPU_I2C_waitACK())
	{
		MPU_I2C_stop();
		return 3;
	}
	(*data) |= MPU_I2C_readByte(0);
	MPU_I2C_stop();
	return 0;
}*/
