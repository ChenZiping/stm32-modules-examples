/***********************************
** RC522软件SPI驱动程序           **
** by GDUTROBOT-陈子平 2016-2-14 **
** Github:ChenZiping            **
**********************************/
#include "RC522.h"

void RC522_SPI_init()
{
	RCC->APB2ENR |= 1 << 2;
	//RCC->APB2ENR |= 1 << 12;
	GPIOA->CRL &= 0x0000FFF0;
	GPIOA->CRL |= 0x34330003;
}

void RC522_init()
{
	RC522_SPI_init();
	RC522_reset();
	RC522_disableAntenna();
	RC522_enableAntenna();
	RC522_cfgISOType('A');
}

s8 RC522_reset()
{
	RC522_RST = 1;
	delayMs(10);
	RC522_RST = 0;
	delayMs(10);
	RC522_RST = 1;
	delayMs(100);
	RC522_SPI_read(0x02);
	RC522_SPI_write(CommandReg, Cmd_SoftReset);
	RC522_SPI_write(ModeReg, 0x3D);//和Mifare卡通讯，CRC初始值0x6363
	RC522_SPI_write(TReloadReg_L, 30);           
	RC522_SPI_write(TReloadReg_H, 0);
	RC522_SPI_write(TModeReg, 0x8D);
	RC522_SPI_write(TPrescalerReg, 0x3E);
	RC522_SPI_write(TxASKReg, 0x40);     
	return MI_OK;
}
	
void RC522_disableAntenna()
{
	RC522_clearBitMask(TxControlReg, 0x03);
}

void RC522_enableAntenna()
{
	u8 i;
	i = RC522_SPI_read(TxControlReg);
	if(!(i & 0x03)) RC522_setBitMask(TxControlReg, 0x03);
}

////////////////////////////////////////////
//寻卡
//参数：req_code 寻卡方式
//0x52 = 寻感应区内所有符合14443A标准的卡，0x26 = 寻未进入休眠状态的卡
//输出：*pTagType 卡片类型
//0x4400 = Mifare_UltraLight
//0x0400 = Mifare_One(S50)
//0x0200 = Mifare_One(S70)
//0x0800 = Mifare_Pro(X)
//0x4403 = Mifare_DESFire
//返回MI_OK表示成功
s8 RC522_request(u8 req_code,u8 *pTagType)
{
	s8 status;  
	u32 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	RC522_clearBitMask(Status2Reg, 0x08);
	RC522_SPI_write(BitFramingReg, 0x07);
	RC522_setBitMask(TxControlReg, 0x03);
	ucComMF522Buf[0] = req_code;
	status = RC522_cmd(Cmd_Transceive, ucComMF522Buf, 1, ucComMF522Buf, &unLen);
	if((status == MI_OK) && (unLen == 0x10))
	{    
		 *pTagType     = ucComMF522Buf[0];
		 *(pTagType+1) = ucComMF522Buf[1];
	}
	else status = MI_ERR;
	return status;
}

////////////////////////////////////////////
//防冲撞
//输出：*pSnr 4字节卡片序列号
//返回MI_OK表示成功
s8 RC522_anticoll(u8 *pSnr)
{
	s8 status;
	u8 i, snr_check = 0, ucComMF522Buf[MAXRLEN];
	u32 unLen;
	RC522_clearBitMask(Status2Reg, 0x08);
	RC522_SPI_write(BitFramingReg, 0x00);
	RC522_clearBitMask(CollReg, 0x80);
	ucComMF522Buf[0] = MI_ANTICOLL;
	ucComMF522Buf[1] = 0x20;
	status = RC522_cmd(Cmd_Transceive, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
	if(status == MI_OK)
	{
		 for(i = 0;i < 4;i ++)
		 {   
				 *(pSnr + i)  = ucComMF522Buf[i];
				 snr_check ^= ucComMF522Buf[i];
		 }
		 if(snr_check != ucComMF522Buf[i]) status = MI_ERR;
	}
	RC522_setBitMask(CollReg,0x80);
	return status;
}

////////////////////////////////////////////
//选定卡片
//输出：*pSnr 4字节卡片序列号
//返回MI_OK表示成功
s8 RC522_select(u8 *pSnr)
{
	s8 status;
	u8 i, ucComMF522Buf[MAXRLEN];
	u32  unLen;
	ucComMF522Buf[0] = MI_SELECTTAG;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	for (i = 0;i < 4;i ++)
	{
		ucComMF522Buf[i + 2] = *(pSnr + i);
		ucComMF522Buf[6]  ^= *(pSnr + i);
	}
	RC522_calcCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
	RC522_clearBitMask(Status2Reg,0x08);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,9,ucComMF522Buf,&unLen);
	if((status == MI_OK) && (unLen == 0x18)) status = MI_OK;
	else status = MI_ERR;
	return status;
}

////////////////////////////////////////////
//验证卡片密码
//参数：auth_mode 密码验证模式
//0x60 = 验证A密钥,0x61 = 验证B密钥
//addr 块地址
//*pKey 密码
//*pSnr 4字节卡片序列号
//返回MI_OK表示成功
s8 RC522_auth(u8 auth_mode,u8 addr,u8 *pKey,u8 *pSnr)
{
	s8 status;
	u32 unLen;
	u8 i, ucComMF522Buf[MAXRLEN]; 
	ucComMF522Buf[0] = auth_mode;
	ucComMF522Buf[1] = addr;
	for (i=0; i<6; i++) ucComMF522Buf[i+2] = *(pKey+i);
	for (i=0; i<6; i++) ucComMF522Buf[i+8] = *(pSnr+i);
	//memcpy(&ucComMF522Buf[2], pKey, 6); 
	//memcpy(&ucComMF522Buf[8], pSnr, 4); 
	status = RC522_cmd(Cmd_MFAuthent,ucComMF522Buf,12,ucComMF522Buf,&unLen);
	if((status != MI_OK) || (!(RC522_SPI_read(Status2Reg) & 0x08))) status = MI_ERR;
	return status;
}

//读取MI卡一块数据
s8 RC522_read(u8 addr,u8 *pData)
{
	s8 status;
	u32 unLen;
	u8 i, ucComMF522Buf[MAXRLEN]; 
	ucComMF522Buf[0] = MI_READ;
	ucComMF522Buf[1] = addr;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status == MI_OK) && (unLen == 0x90))
	//   {   memcpy(pData, ucComMF522Buf, 16);   }
	{
			for(i = 0;i < 16;i ++) *(pData + i) = ucComMF522Buf[i];
	}
	else status = MI_ERR;
	return status;
}

//写数据到MI卡一块
s8 RC522_write(u8 addr,u8 *pData)
{
	s8 status;
	u32 unLen;
	u8 i, ucComMF522Buf[MAXRLEN]; 
	ucComMF522Buf[0] = MI_WRITE;
	ucComMF522Buf[1] = addr;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if(status == MI_OK)
	{
			//memcpy(ucComMF522Buf, pData, 16);
			for (i = 0;i < 16;i ++) ucComMF522Buf[i] = *(pData + i);
			RC522_calcCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
			status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,18,ucComMF522Buf,&unLen);
			if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	}

	return status;
}

////////////////////////////////////////////////////
//扣款和充值
//参数：dd_mode 命令字
//0xC0 = 扣款,0xC1 = 充值
//addr 钱包地址
//*pValue 4字节增(减)值，低位在前
//返回MI_OK表示成功
s8 RC522_val(u8 dd_mode, u8 addr, u8 *pValue)
{
	s8 status;
	u8 i, ucComMF522Buf[MAXRLEN];
	u32 unLen;
	ucComMF522Buf[0] = dd_mode;
	ucComMF522Buf[1] = addr;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if (status == MI_OK)
	{
		// memcpy(ucComMF522Buf, pValue, 4);
		for(i = 0;i < 16;i ++) ucComMF522Buf[i] = *(pValue + i);
		RC522_calcCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
		unLen = 0;
		status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if(status != MI_ERR) status = MI_OK;
	}
	if (status == MI_OK)
	{
		ucComMF522Buf[0] = MI_TRANSFER;
		ucComMF522Buf[1] = addr;
		RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
		status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
		if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	}
	return status;
}

////////////////////////////////////////////////////
//备份钱包
//参数：srcaddr 源地址
//goaladdr 目标地址
//返回MI_OK表示成功
s8 RC522_bakVal(u8 srcaddr, u8 goaladdr)
{
	s8 status;
	u32 unLen;
	u8 ucComMF522Buf[MAXRLEN];
	ucComMF522Buf[0] = MI_RESTORE;
	ucComMF522Buf[1] = srcaddr;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if(status == MI_OK)
	{
		ucComMF522Buf[0] = 0;
		ucComMF522Buf[1] = 0;
		ucComMF522Buf[2] = 0;
		ucComMF522Buf[3] = 0;
		RC522_calcCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
		status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if(status != MI_ERR) status = MI_OK;
	}
	if(status != MI_OK) return MI_ERR;
	ucComMF522Buf[0] = MI_TRANSFER;
	ucComMF522Buf[1] = goaladdr;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) status = MI_ERR;
	return status;
}

//命令卡片进入休眠状态
s8 RC522_halt()
{
	u32 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	ucComMF522Buf[0] = MI_HALT;
	ucComMF522Buf[1] = 0;
	RC522_calcCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	RC522_cmd(Cmd_Transceive,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	return MI_OK;
}
	
s8 RC522_cmd(u8 cmd, u8 *pInData, u8 InLenByte, u8 *pOutData, u32 *pOutLenBit)
{
	s8 status = MI_ERR;
	u8 irqEn = 0x00, waitFor = 0x00, lastBits, n;
	u32 i;
	switch(cmd)
	{
		case Cmd_MFAuthent:
		{
			irqEn   = 0x12;
			waitFor = 0x10;
			break;
		}
		case Cmd_Transceive:
		{
			irqEn   = 0x77;
			waitFor = 0x30;
			break;
		}
		default: break;
	}
	RC522_SPI_write(ComlEnReg, irqEn | 0x80);
	RC522_clearBitMask(ComIrqReg,0x80);
	RC522_SPI_write(CommandReg,Cmd_Idle);
	RC522_setBitMask(FIFOLevelReg,0x80);
	for(i = 0;i < InLenByte;i ++) RC522_SPI_write(FIFODataReg, pInData[i]);
	RC522_SPI_write(CommandReg, cmd);
	if(cmd == Cmd_Transceive) RC522_setBitMask(BitFramingReg,0x80);
	//i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
	i = 2000;
	do 
	{
		n = RC522_SPI_read(ComIrqReg);
		i--;
	}
	while((i != 0) && !(n & 0x01) && !(n & waitFor));
	RC522_clearBitMask(BitFramingReg, 0x80);
	if(i != 0)
	{    
		if(!(RC522_SPI_read(ErrorReg) & 0x1B))
		{
			 status = MI_OK;
			 if(n & irqEn & 0x01) status = MI_NOTAGERR;
			 if(cmd == Cmd_Transceive)
			 {
					n = RC522_SPI_read(FIFOLevelReg);
					lastBits = RC522_SPI_read(ControlReg) & 0x07;
					if(lastBits) *pOutLenBit = (n-1)*8 + lastBits;
					else *pOutLenBit = n*8;
					if(n == 0) n = 1;
					if(n > MAXRLEN) n = MAXRLEN;
					for(i = 0;i < n;i ++) pOutData[i] = RC522_SPI_read(FIFODataReg);
			}
		}
		else status = MI_ERR;
	}
	RC522_setBitMask(ControlReg, 0x80);           // stop timer now
	RC522_SPI_write(CommandReg, Cmd_Idle); 
	return status;
}

//计算CRC16
void RC522_calcCRC(u8 *pIndata,u8 len,u8 *pOutData)
{
	u8 i, n;
	RC522_clearBitMask(DivIrqReg,0x04);
	RC522_SPI_write(CommandReg,Cmd_Idle);
	RC522_setBitMask(FIFOLevelReg,0x80);
	for (i=0; i<len; i++) RC522_SPI_write(FIFODataReg, *(pIndata + i));
	RC522_SPI_write(CommandReg, Cmd_CalcCRC);
	i = 0xFF;
	do 
	{
		n = RC522_SPI_read(DivIrqReg);
		i--;
	}
	while((i != 0) && !(n & 0x04));
	pOutData[0] = RC522_SPI_read(CRCResultReg_L);
	pOutData[1] = RC522_SPI_read(CRCResultReg_H);
}

void RC522_SPI_writeByte(u8 data)
{
	u8 i;
	RC522_SPI_SCK = 0;
	for(i = 0;i < 8;i ++)
	{
		if(data & 0x80) RC522_SPI_MOSI = 1;
		else RC522_SPI_MOSI = 0;
		RC522_SPI_SCK = 1;
		data <<= 1;
		RC522_SPI_SCK = 0;
	}
}

void RC522_SPI_readByte(u8* data)
{
	u8 i;
	RC522_SPI_SCK = 0;
	for(i = 0;i < 8;i ++)
	{
		(*data) <<= 1;
		RC522_SPI_SCK = 1;
		if(RC522_SPI_MISO) (*data) |= 1;
		//else (*data) &= ~1;
		RC522_SPI_SCK = 0;
	}
}

void RC522_SPI_write(u8 addr, u8 data)
{
	u8 reg;
	RC522_SPI_SCK = 0;
	RC522_SPI_NSS = 0;
	reg = ((addr << 1) & 0x7E);
	RC522_SPI_writeByte(reg);
	RC522_SPI_writeByte(data);
	RC522_SPI_NSS = 1;
	RC522_SPI_SCK = 1;
}

u8 RC522_SPI_read(u8 addr)
{
	u8 reg, res = 0;
	RC522_SPI_NSS = 0;
	reg = ((addr << 1) & 0x7E) | 0x80;
	RC522_SPI_writeByte(reg);
	RC522_SPI_readByte(&res);
	RC522_SPI_NSS = 1;
	RC522_SPI_SCK = 1;
	return res;
}

//置RC522寄存器某些位为1
void RC522_setBitMask(u8 reg,u8 mask)
{
	s8 tmp;
	tmp = RC522_SPI_read(reg);
	RC522_SPI_write(reg, tmp | mask);
}

//置RC522寄存器某些位为0
void RC522_clearBitMask(u8 reg,u8 mask)
{
	s8 tmp;
	tmp = RC522_SPI_read(reg);
	RC522_SPI_write(reg, tmp & ~mask);
}

s8 RC522_cfgISOType(u8 type)
{
	if (type == 'A')//ISO14443_A
	{ 
		RC522_clearBitMask(Status2Reg,0x08);
		/*     RC522_SPI_write(CommandReg,0x20);    //as default   
		RC522_SPI_write(ComIEnReg,0x80);     //as default
		RC522_SPI_write(DivlEnReg,0x0);      //as default
		RC522_SPI_write(ComIrqReg,0x04);     //as default
		RC522_SPI_write(DivIrqReg,0x0);      //as default
		RC522_SPI_write(Status2Reg,0x0);//80    //trun off temperature sensor
		RC522_SPI_write(WaterLevelReg,0x08); //as default
		RC522_SPI_write(ControlReg,0x20);    //as default
		RC522_SPI_write(CollReg,0x80);    //as default
		*/
		RC522_SPI_write(ModeReg,0x3D);//3F
		/*	   RC522_SPI_write(TxModeReg,0x0);      //as default???
		RC522_SPI_write(RxModeReg,0x0);      //as default???
		RC522_SPI_write(TxControlReg,0x80);  //as default???
		RC522_SPI_write(TxSelReg,0x10);      //as default???
		*/
		RC522_SPI_write(RxSelReg,0x86);//84
		//      RC522_SPI_write(RxThresholdReg,0x84);//as default
		//      RC522_SPI_write(DemodReg,0x4D);      //as default

		//      RC522_SPI_write(ModWidthReg,0x13);//26
		RC522_SPI_write(RFCfgReg,0x7F);   //4F
		/*   RC522_SPI_write(GsNReg,0x88);        //as default???
		RC522_SPI_write(CWGsCfgReg,0x20);    //as default???
		RC522_SPI_write(ModGsCfgReg,0x20);   //as default???
		*/
		RC522_SPI_write(TReloadReg_L,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		RC522_SPI_write(TReloadReg_H,0);
		RC522_SPI_write(TModeReg,0x8D);
		RC522_SPI_write(TPrescalerReg,0x3E);
		//PcdSetTmo(106);
		delayMs(10);
		RC522_enableAntenna();
	}
	else return MI_NOTAGERR;
	return MI_OK;
}

void RC522_wait4CardOff()
{
	s8 status;
  u8 TagType[2];
	while(1)
	{
		status = RC522_request(MI_REQSTD, TagType);
		if(status)
		{
			status = RC522_request(MI_REQSTD, TagType);
			if(status)
			{
				status = RC522_request(MI_REQSTD, TagType);
				if(status) return;
			}
		}
		delayMs(1000);
	}
}

s8 RC522_unlock0(u8 cmd)
{
	s8 status;  
	u32 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	RC522_clearBitMask(Status2Reg,0x08);
	RC522_SPI_write(BitFramingReg,0x07);
	RC522_setBitMask(TxControlReg,0x03);
	ucComMF522Buf[0] = cmd;
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,1,ucComMF522Buf,&unLen);
	if (status == MI_OK && ucComMF522Buf[0] == 0x0A) status = MI_OK;
	else status = MI_ERR;
	return status;
}

s8 RC522_unlock1(u8 cmd)
{
	s8 status;
	u32 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	RC522_clearBitMask(Status2Reg,0x08);
	RC522_SPI_write(BitFramingReg,0x00);
	RC522_clearBitMask(CollReg,0x80);
	ucComMF522Buf[0] = cmd;
	status = RC522_cmd(Cmd_Transceive,ucComMF522Buf,1,ucComMF522Buf,&unLen);
	if(status == MI_OK && ucComMF522Buf[0] == 0x0A) status = MI_OK;
	else status = MI_ERR;
	RC522_setBitMask(CollReg,0x80);
	return status;
}

void RC522_writeMI(u8 block, u8 *pKey, u8* data, u8 *pTagType, u8 *SelectedSnr)
{
	s8 status;
	status = RC522_request(MI_REQSTD, pTagType);
	if(!status)
	{
		status = RC522_anticoll(SelectedSnr);
		if(!status)
		{
			status = RC522_select(SelectedSnr);
			if(!status)
			{
				if(block == 0)
				{
					status = RC522_halt();
					if(!status)
					{
						status = RC522_unlock0(MI_UNLOCK0);
						if(!status) status = RC522_unlock1(MI_UNLOCK1);
						else printf("Unlock I Error\r\n");	
					}
					else printf("Halt Error\r\n");
				}
				else status = RC522_auth(MI_AUTHENT1A, block, pKey, SelectedSnr);
				if(!status)
				{
					status = RC522_write(block, data);
					if(!status) 
					{
						printf("Write Succeed\r\n");
						RC522_wait4CardOff();
					}
					else printf("Write Error\r\n");
				}
				else printf("Auth Error\r\n");	
			}
			else printf("Select Error\r\n");
		}
		else printf("Anticoll Error\r\n");
	}
	//else printf("Request Error\r\n"); 
}

u8 RC522_readMI(u8 block, u8 *pKey, u8* data, u8 *pTagType, u8 *SelectedSnr)
{
	s8 status;
	status = RC522_request(MI_REQSTD, pTagType);
	if(!status)
	{
		status = RC522_anticoll(SelectedSnr);
		if(!status)
		{
			status = RC522_select(SelectedSnr);
			if(!status)
			{
				status = RC522_auth(MI_AUTHENT1A, block, pKey, SelectedSnr);
				if(!status)
				{
					status = RC522_read(block, data);
					if(!status)
					{
						printf("Read Succeed\r\n");
						RC522_wait4CardOff();
						return *data;
					}
					else printf("Read Error\r\n");	
				}
				else printf("Auth Error\r\n");	
			}
			else printf("Select Error\r\n");
		}
		else printf("Anticoll Error\r\n");
	}
	//else printf("Request Error\r\n"); 
	return 0;
}
