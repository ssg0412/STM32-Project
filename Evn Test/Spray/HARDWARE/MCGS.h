#include "sys.h"
#include "rs485.h"
#include "check.h"
#include "delay.h"
#include "main.h"

void	HDMI_Set_Plan(u16 addr)					//���ô���������ʱʱ��
{
	u8 Plancmd[]={01,06,00,01,00,00,0x7C,0x0E};
	u16 crc;
	Plancmd[2]=addr>>8;
	Plancmd[3]=addr&0xff;
	Plancmd[5]=1;
	crc=mc_check_crc16(Plancmd,6);
	Plancmd[6]=crc>>8;
	Plancmd[7]=crc&0xff;
	comSendBuf(COM5,Plancmd,sizeof(Plancmd));
	delay_ms(100);
}

void	HDMI_Set_Log(u8	Num,u16	addr)			//��ʾ��־
{
	u8 Logcmd[]={01,16,01,0x91,00,17,34,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,0x7C,0x0E};
	u16 crc;
	Logcmd[2]=addr>>8;
	Logcmd[3]=addr&0xff;
	
	Logcmd[8]=Num;
	Logcmd[10]=Log.StartTime[0];
	Logcmd[12]=Log.StartTime[1];
	Logcmd[14]=Log.StartTime[2];
	Logcmd[16]=Log.StartTime[3];
	Logcmd[18]=Log.StartTime[4];
	Logcmd[20]=Log.StartMode;
	Logcmd[22]=Log.IrrMode;
	Logcmd[24]=Log.Partition;
	Logcmd[25]=Log.Irrtime>>8;
	Logcmd[26]=Log.Irrtime&0xff;
	Logcmd[28]=Log.StopTime[0];
	Logcmd[30]=Log.StopTime[1];
	Logcmd[32]=Log.StopTime[2];
	Logcmd[34]=Log.StopTime[3];
	Logcmd[36]=Log.StopTime[4];
	Logcmd[38]=Log.StopMode;
	Logcmd[39]=Log.RemTime>>8;
	Logcmd[40]=Log.RemTime&0xff;

	crc=mc_check_crc16(Logcmd,sizeof(Logcmd)-2);
	Logcmd[sizeof(Logcmd)-2]=crc>>8;
	Logcmd[sizeof(Logcmd)-1]=crc&0xff;
	comSendBuf(COM5,Logcmd,sizeof(Logcmd));
	delay_ms(200);
}

void	HDMI_Set_Fer()							//��ʾ��������
{
	u8 Fercmd[23]={01,16,00,10,00,07,14,00,00};
	u16 crc;
	
	Fercmd[8]=Run_SetMessage.Mu;
	Fercmd[10]=Run_SetMessage.AverageMu;
	Fercmd[12]=Run_SetMessage.SpecialFer;
	Fercmd[14]=Run_SetMessage.Pre;
	Fercmd[16]=Run_SetMessage.Behind;
	Fercmd[18]=Run_SetMessage.Terminaladdr;
	Fercmd[20]=Run_SetMessage.TerminalNum;

	crc=mc_check_crc16(Fercmd,sizeof(Fercmd)-2);
	Fercmd[sizeof(Fercmd)-2]=crc>>8;
	Fercmd[sizeof(Fercmd)-1]=crc&0xff;
	comSendBuf(COM5,Fercmd,sizeof(Fercmd));
	delay_ms(200);
}

void	HDMI_Set_Error()						//��������ʾ�쳣
{
	u8 Errorcmd[]={01,6,01,0x8F,00,00,01,00};
	u16 crc;
	Errorcmd[5]=Error;
	crc=mc_check_crc16(Errorcmd,6);
	Errorcmd[6]=crc>>8;
	Errorcmd[7]=crc&0xff;
	delay_ms(50);
	comSendBuf(COM5,Errorcmd,sizeof(Errorcmd));
	delay_ms(1000);
}

void	HDMI_Set_Button(u8 value)				//���ð���״̬
{
	u8 Partitioncmd[]={01,15,00,00,00,8,01,00,0x7C,0x0E};
	u16 crc;
	Partitioncmd[7]=value;
	crc=mc_check_crc16(Partitioncmd,8);
	Partitioncmd[8]=crc>>8;
	Partitioncmd[9]=crc&0xff;
	delay_ms(50);
	comSendBuf(COM5,Partitioncmd,sizeof(Partitioncmd));
	delay_ms(1000);
}

void	HDMI_Set_Remaining_time(u16 time)		//���ô���������ʱʱ��
{
	u8 Timecmd[]={01,06,00,01,00,00,0x7C,0x0E};
	u16 crc;
	Timecmd[4]=time>>8;
	Timecmd[5]=time&0xff;
	crc=mc_check_crc16(Timecmd,6);
	Timecmd[6]=crc>>8;
	Timecmd[7]=crc&0xff;
	comSendBuf(COM5,Timecmd,sizeof(Timecmd));
	delay_ms(100);
}

void	HDMI_Set_Partition(u8 value)			//���ô���������ѡ����ʾ
{
	u8 Partitioncmd[]={01,15,00,8,00,8,01,00,0x7C,0x0E};
	u16 crc;
	Partitioncmd[7]=value;
	crc=mc_check_crc16(Partitioncmd,8);
	Partitioncmd[8]=crc>>8;
	Partitioncmd[9]=crc&0xff;
	comSendBuf(COM5,Partitioncmd,sizeof(Partitioncmd));
	delay_ms(1000);
}

void	HDMI_Set_Current_Partition(u8 value)	//���õ�ǰ����
{
	u8 Currentcmd[]={01,06,00,00,00,00,0x7C,0x0E};
	u16 crc;
	Currentcmd[5]=value;
	crc=mc_check_crc16(Currentcmd,6);
	Currentcmd[6]=crc>>8;
	Currentcmd[7]=crc&0xff;
	comSendBuf(COM5,Currentcmd,sizeof(Currentcmd));
	delay_ms(100);
}

void	HDMI_SetSysTime()						//���ô�����ʱ��
{
	u8 SysTimecmd[21]={01,16,00,03,00,06,12,0x7C,0x0E};
	u8 Stimecmd[]={01,05,00,16,0xFF,00,0x8D,0XFF};
	u16 crc;
	SysTimecmd[7]=STIME.year>>8;
	SysTimecmd[8]=STIME.year&0xff;
	SysTimecmd[9]=STIME.month>>8;
	SysTimecmd[10]=STIME.month&0xff;
	SysTimecmd[11]=STIME.day>>8;
	SysTimecmd[12]=STIME.day&0xff;
	SysTimecmd[13]=STIME.hour>>8;
	SysTimecmd[14]=STIME.hour&0xff;
	SysTimecmd[15]=STIME.minute>>8;
	SysTimecmd[16]=STIME.minute&0xff;
	SysTimecmd[17]=STIME.second>>8;
	SysTimecmd[18]=STIME.second&0xff;
	crc=mc_check_crc16(SysTimecmd,19);
	SysTimecmd[19]=crc>>8;
	SysTimecmd[20]=crc&0xff;
	comSendBuf(COM5,SysTimecmd,sizeof(SysTimecmd));
	delay_ms(100);
	
	if(TimePopup)return;

	comSendBuf(COM5,Stimecmd,sizeof(Stimecmd));
	delay_ms(100);
}

void	HDMI_Set_Stage()						//���ô������׶�
{
	u8 Stagecmd[]={01,06,00,06,00,00,0x7C,0x0E};
	u16 crc;
	Stagecmd[5]=stage;
	crc=mc_check_crc16(Stagecmd,6);
	Stagecmd[6]=crc>>8;
	Stagecmd[7]=crc&0xff;
	comSendBuf(COM5,Stagecmd,sizeof(Stagecmd));
	delay_ms(100);
}

void	HDMI_Set_Water_Fer()					//���ô�����ʵ����ˮ�����÷���������ʱ
{
	u8 WF[15]={01,16,00,07,00,03,06,0x7C,0x0E};
	u16 crc;
	WF[7]=Actual_Water>>8;
	WF[8]=Actual_Water&0xff;
	WF[9]=Actual_Fer>>8;
	WF[10]=Actual_Fer&0xff;
	WF[11]=Remaining>>8;
	WF[12]=Remaining&0xff;
	crc=mc_check_crc16(WF,15);
	WF[13]=crc>>8;
	WF[14]=crc&0xff;
	comSendBuf(COM5,WF,sizeof(WF));
	delay_ms(100);
}

void	HDMI_Check_Acquisition_Frequency()		//��ȡ�������ɼ�Ƶ��
{
	u8 Acquisitioncmd[]={01,03,00,02,00,01,0x25,0xCA};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Acquisitioncmd,sizeof(Acquisitioncmd));
	delay_ms(100);
	COM5GetBuf(buf,7);
	crc=mc_check_crc16(buf,5);
	if((buf[5]==(crc>>8))&(buf[6]==(crc&0xff)))
		up_time=buf[3]<<8|buf[4];
}

void	HDMI_Read_SetHouseNum(u8 *Num)			//��ȡ�������
{
	u8 SetHouseNum[]={01,03,00,9,00,01,0x54,0x08};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,SetHouseNum,sizeof(SetHouseNum));
	delay_ms(100);
	COM5GetBuf(buf,7);
	crc=mc_check_crc16(buf,5);
	if((buf[5]==(crc>>8))&(buf[6]==(crc&0xff)))
		*Num=buf[4];
}

void	HDMI_Check_Net(u16 CMD,char *data)		//��ȡ�������
{
	u8 Netcmd[]={01,03,00,02,00,01,0x25,0xCA};
	u8 buf[100];
	u8 len;
	u16 crc;
	char hc[30];
	comClearRxFifo(COM5);
	Netcmd[2]=CMD>>8;
	Netcmd[3]=CMD&0xff;
	crc=mc_check_crc16(Netcmd,6);
	Netcmd[6]=crc>>8;
	Netcmd[7]=crc&0xff;
	comSendBuf(COM5,Netcmd,sizeof(Netcmd));
	delay_ms(100);
	len=COM5GetBuf(buf,100);
	crc=mc_check_crc16(buf,len-2);
	if((buf[len-2]==(crc>>8))&(buf[len-1]==(crc&0xff)))
	{
		buf[len-2]=0;
		sprintf(hc,(char *)buf+9);
		strcpy(data,hc);
	}
}

void	HDMI_Check_Button()						//��ȡ��ť���ذ���״̬
{
	u8 Buttoncmd[]={01,01,00,00,00,0x08,0x3D,0xCC};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Buttoncmd,sizeof(Buttoncmd));
	delay_ms(100);
	COM5GetBuf(buf,6);
	crc=mc_check_crc16(buf,4);
	if((buf[4]==(crc>>8))&(buf[5]==(crc&0xff)))
		MCGS_Button=buf[3];
}

void	HDMI_Check_Irrigation_time()			//��ȡ���õĹ��ʱ��
{
	u8 Irrigationcmd[]={01,04,00,00,00,01,0x31,0xCA};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Irrigationcmd,sizeof(Irrigationcmd));
	delay_ms(100);
	COM5GetBuf(buf,7);
	crc=mc_check_crc16(buf,5);
	if((buf[5]==(crc>>8))&(buf[6]==(crc&0xff)))
		IrrTime=(buf[3]<<8)|buf[4];
}

void	HDMI_Read_Greenhouse()					//��ȡ���õ����в���
{
	u8 Greenhousecmd[]={01,04,00,00,00,04,0xF1,0xC9};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Greenhousecmd,sizeof(Greenhousecmd));
	delay_ms(100);
	COM5GetBuf(buf,13);
	crc=mc_check_crc16(buf,11);
	if((buf[11]==(crc>>8))&(buf[12]==(crc&0xff)))
		{HouseNum=buf[4];MODEL=buf[6];Remaining=(buf[7]<<8)|buf[8];PublicFer=buf[10];}
}

void	HDMI_Read_HouseSet()					//��ȡ���õ����Ҳ���
{
	u8 HouseSetcmd[]={01,03,00,0x0A,00,0x07,0x24,0x0A};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,HouseSetcmd,sizeof(HouseSetcmd));
	delay_ms(100);
	COM5GetBuf(buf,19);
	crc=mc_check_crc16(buf,17);
	if((buf[17]==(crc>>8))&(buf[18]==(crc&0xff)))
	{
		Run_SetMessage.Mu=buf[4];
		Run_SetMessage.AverageMu=buf[6];
		Run_SetMessage.SpecialFer=buf[8];
		Run_SetMessage.Pre=buf[10];
		Run_SetMessage.Behind=buf[12];
		Run_SetMessage.Terminaladdr=buf[14];
		Run_SetMessage.TerminalNum=buf[16];
		FlashWriteFer(&Run_SetMessage.Mu,buf[4]*10,10);
	}	
}

void	HDMI_Read_Fer()							//��ȡ����Ͱ������
{
	u8 Fercmd[]={01,01,00,48,00,0x08,0x3D,0xC3};
	u8 Fercmd1[]={01,03,00,29,00,07,0x94,0x0E};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Fercmd,sizeof(Fercmd));
	delay_ms(100);
	COM5GetBuf(buf,6);
	crc=mc_check_crc16(buf,4);
	if((buf[4]==(crc>>8))&(buf[5]==(crc&0xff)))
		MCGS_Fer=buf[3];
	
	comClearRxFifo(COM5);
	comSendBuf(COM5,Fercmd1,sizeof(Fercmd1));
	delay_ms(200);
	COM5GetBuf(buf,19);
	crc=mc_check_crc16(buf,17);
	if((buf[17]==(crc>>8))&(buf[18]==(crc&0xff)))
	{
		for(crc=0;crc<7;crc++)Concentration[crc]=buf[4+crc];
		FlashWriteFer0(Concentration,0,10);
	}
}

void	HDMI_Check_SysTime()					//��ȡ������ϵͳʱ��
{
	u8 Syscmd[]={01,04,00,04,00,06,0x31,0xC9};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,Syscmd,sizeof(Syscmd));
	delay_ms(100);
	COM5GetBuf(buf,18);
	crc=mc_check_crc16(buf,15);
	if((buf[15]==(crc>>8))&(buf[16]==(crc&0xff)))
	{
		STIME.year=buf[3]<<8|buf[4];
		STIME.month=buf[6];
		STIME.day=buf[8];
		STIME.hour=buf[10];
		STIME.minute=buf[12];
		STIME.second=buf[14];
	}
}

void	HDMI_Check_LogPage()					//��ȡ��־ҳ��
{
	u8 LogPagecmd[]={01,04,00,0X0A,00,01,0x11,0xC8};
	u8 buf[100];
	u16 crc;
	comClearRxFifo(COM5);
	comSendBuf(COM5,LogPagecmd,sizeof(LogPagecmd));
	delay_ms(100);
	COM5GetBuf(buf,7);
	crc=mc_check_crc16(buf,5);
	if((buf[5]==(crc>>8))&(buf[6]==(crc&0xff)))
		LogPage=(buf[3]<<8)|buf[4];
}