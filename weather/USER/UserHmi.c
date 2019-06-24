#include "UserHmi.h"
#include "UserCore.h"
#include "Delay.h"
#include "FlashDivide.h"
#include "GM3Dir.h"
//#include "ObstScan.h"
//#include "AGVMsg.h"

extern u8 g_u8LockScreenCnt;
extern u8 g_u8LockScreenF;
//u16 u16temp; 
extern OS_TMR	CallerTmr;
#define CMD_MAX_BUFFER 128
u8 Hmi_cmd_buffer[CMD_MAX_BUFFER];  //����������ջ�����
u16 s_screen_id;                  //ҳ��ID��ű���
u16 s_control_id;                 //�ؼ�ID��ű���

u16 s_TargetID;				//������ȷ��ת����zbz
u16 s_ReturnID;				//������淵����ת����zbz
StrategyStruct HMI_ProPara;
u8 WaterTimeH = 0,WaterTimeM = 0 , WaterTime = 0;
u8 FertilizerTimeH = 0,FertilizerTimeM = 0,FertilizerTime = 0;

void SignedNumberToASCII ( u8 *Arry , s16 Number);
/*******************************************************************************
����:�����ı������ȡ
����:�������е����ֽ�����������Ŀ�������ֵ
����: pDest:Ŀ����� msg:����
*******************************************************************************/
void FifureTextInput ( s16 *pDest , PCTRL_MSG msg )
{
	s16 datatxt = 0;
	u16 i = 0;
	u8 u8F = 0;
	while ( ( ( uint8 * ) ( &msg->param ) ) [i] )//ASCIIת��Ϊ����
	{
		if(( ( uint8 * ) ( &msg->param ) ) [i] != 0x2d )//0x2dΪ����
		{
			datatxt = datatxt * 10 + ( ( ( uint8 * ) ( &msg->param ) ) [i++] - 0x30 ); //�ӽ��ջ�����ȡ��һ���ֽ����ݣ���ת����ʮ��������
		}
		else
		{
			u8F = 1;
			i++;
		}
	}
	if(u8F == 1)
	{
	   datatxt = datatxt*(-1);
	}
	*pDest = datatxt;
}
/*******************************************************************************
����:�ı������ȡ
����:�������е����ֽ�����������Ŀ�������ֵ
����: pDest:Ŀ����� msg:����
*******************************************************************************/
void TextInput ( u8 *pDest , PCTRL_MSG msg )
{
	s16 datatxt = 0;
	u16 i = 0;
	while ( ( ( uint8 * ) ( &msg->param ) ) [i] )//ASCIIת��Ϊ����
	{
		*(pDest++) = ( ( uint8 * ) ( &msg->param ) ) [i++];
	}
	*pDest = datatxt;
}
/*******************************************************************************
����:����״̬��ȡ
����:�������еİ���״̬������������ֵ��Ŀ�Ĳ���
����: pDest:Ŀ����� msg:����
*******************************************************************************/
void ButtonStatueGet ( u8 *pDest , PCTRL_MSG msg )
{
	u8 buttonvalue = msg->param[1];
	*pDest = buttonvalue;
	
}
/*******************************************************************************
����:�б�ֵ��ȡ
����:�������еİ���״̬������������ֵ��Ŀ�Ĳ���
����: pDest:Ŀ����� msg:����
*******************************************************************************/
void MenuValueGet ( u8 *pDest , PCTRL_MSG msg )
{
	u8 value =  msg->param[0] ; //��ֵ
	*pDest = value;
}
//����ת����ASCII��
void NumberToASCII ( u8 *Arry , u16 Number)
{
	u8 i = 0 , j = 0;
	u8 temp = 0;
	u8 strtemp[16] = {0,0,0,0,0,0,0,0};
	do					//�����ַֿ���װ����ASCII��
	{
		temp = Number % 10;
		strtemp[i++] = temp + 0x30;
		Number = Number / 10;
	}
	while(Number);
	while(i)//����˳��
	{
		Arry[j++] =strtemp[--i];
	}
	Arry[j++] = 0;
	//Arry[j++] = 0x30;
	
}
//16λ�з�����ת����ASCII��
void SignedNumberToASCII ( u8 *Arry , s16 Number)//����������
{
	u8 i = 0 , j = 0;
	u8 temp = 0;
	u8 strtemp[16] = {0,0,0,0,0,0,0,0};
	u16 u16Number;
	if(Number >= 0)
	{ 
		u16Number = Number;
		do					//�����ַֿ���װ����ASCII��
		{
			temp = u16Number % 10;
			strtemp[i++] = temp + 0x30;
			u16Number = u16Number / 10;
		}
		while(u16Number);
		while(i)//����˳��
		{
			Arry[j++] =strtemp[--i];
		}
		Arry[j++] = 0;
	}
	else
	{
	  u16Number = -Number;
		do					//�����ַֿ���װ����ASCII��
		{
			temp = u16Number % 10;
			strtemp[i++] = temp + 0x30;
			u16Number = u16Number / 10;
		}
		while(u16Number);
//		strtemp[i++] = 0x2d;//����
		j = 1;
		while(i)//����˳��
		{
			Arry[j++] =strtemp[--i];
		}
		Arry[0] = 0x2d;//����
		Arry[j++] = 0;
	}
}
//��ʾ�ǵ���ʱ����
//���룺ʣ�����
//�������
//���ܣ���ҳ������ʾ����ʱ
void DisCountDown(u16 RemainderMinute)
{
	u16 ReHour = RemainderMinute/60;
	u16 ReMin = RemainderMinute%60;	
	u8 u8Str[16];
	
	NumberToASCII(u8Str,ReHour);
	SetTextValue(LCD_MAIN_PAGE,47,u8Str);
	
	NumberToASCII(u8Str,ReMin);
	SetTextValue(LCD_MAIN_PAGE,53,u8Str);
	
}
//�����濪��ѡ��
//���룺�ؼ�ID
//�������
//���ܣ����ݿؼ�ID���ж��û�ѡ���Ǹ����ؿ����ĸ����عء�
void MainPageSwitchSel(u8 CtrlID, u8 Value)
{
//	u8 i = 0;
//	s16 s16RevVal  = 0;
	if(Value == 1)
	{
		if(CtrlID == 28)
		{	
			HMI_ProPara.PumpFFlag = 1;
		}
		else if(CtrlID == 29)
		{
			HMI_ProPara.PumpWFlag = 1;
		}
		else if((CtrlID>=30) && (CtrlID<=32))
		{
			HMI_ProPara.Passageway[CtrlID-30] = 1;//alfred ��Ϊ��ʱֻ��3��ͨ����û�и���������ͨ������λ�ã�����ֻ��Ϊ<=32
		}
		else if((CtrlID>=33) && (CtrlID<=34))//alfred ��Ϊ��ʱֻ��3��ͨ����û�и���������ͨ������λ�ã�����ֻ��Ϊ<=34
		{
			HMI_ProPara.Zone = (CtrlID-32);//����
			//������ŷ�����
			if(CtrlID == 33)
			{
				SetButtonValue(LCD_MAIN_PAGE,34,0);
			}
			else if(CtrlID == 34)
			{
				SetButtonValue(LCD_MAIN_PAGE,33,0);
			}
		}
	}
	else if(Value == 0)
	{
		if(CtrlID == 28)
		{	
			HMI_ProPara.PumpFFlag = 0;
		}
		else if(CtrlID == 29)
		{
			HMI_ProPara.PumpWFlag = 0;
		}
		else if((CtrlID>=30) && (CtrlID<=32))// alfred ��Ҫȷ�ϴ˲������÷�
		{
			HMI_ProPara.Passageway[CtrlID-30] = 0;//alfred ��Ϊ��ʱֻ��3��ͨ����û�и���������ͨ������λ�ã�����ֻ��Ϊ<=32
		}
		else if((CtrlID>=33) && (CtrlID<=34))//alfred ��Ϊ��ʱֻ��3��ͨ����û�и���������ͨ������λ�ã�����ֻ��Ϊ<=34
		{
			HMI_ProPara.Zone = 0;
		}
	}
}
const u8 StaStr0[10] = "����";
const u8 StaStr1[10] = "��ˮ��";
const u8 StaStr2[10] = "ʩ����";
const u8* StaText[] = 
{
	StaStr0,
	StaStr1,
	StaStr2,
};
u8 WaterSta,OldWaterSta ;
//��ˮ״̬��ʾ����
void WaterStaShow(u8 State)
{
	WaterSta = State;
	//SetTextValue(LCD_MAIN_PAGE,7,(uchar *)StaText[State]);
}

u8 FertilizerSta,OldFertilizerSta;
//ʩ��״̬��ʾ����
void FertilizerStaShow(u8 State)
{
	FertilizerSta = State;
	//SetTextValue(LCD_MAIN_PAGE,8,(uchar *)StaText[State]);
}

//��ˮ����ʱ��ʾ����
void WaterTimeShow(u16 Time)
{
	u8 TimeH = 0 ;
	u8 TimeM = 0;
	u8 u8Str[7] = {0,0};
	
	TimeH = Time/60;
	TimeM = Time%60;
	
	NumberToASCII(u8Str,(u16)TimeH);
	SetTextValue(LCD_MAIN_PAGE,9,u8Str);
	delay_ms(10);
	NumberToASCII(u8Str,(u16)TimeM);
	SetTextValue(LCD_MAIN_PAGE,10,u8Str);
	
}
//ʩ�ʵ���ʱ��ʾ����
void FertilizerTimeShow(u16 Time)
{
	u8 TimeH = 0 ;
	u8 TimeM = 0;
	u8 u8Str[7] = {0,0};
	
	TimeH = Time/60;
	TimeM = Time%60;
	
	NumberToASCII(u8Str,(u16)TimeH);
	SetTextValue(LCD_MAIN_PAGE,11,u8Str);
	delay_ms(10);
	NumberToASCII(u8Str,(u16)TimeM);
	SetTextValue(LCD_MAIN_PAGE,12,u8Str);
	
}
//��ˮ���ý���
void WaterSetShow(void)
{
	u16 Time = 0;
	u8 TimeH = 0 ;
	u8 TimeM = 0;
	u8 u8Str[7] = {0,0};
	FlashReadWaterTime((u8*)&Time);
	TimeH = Time/60;
	TimeM = Time%60;
	NumberToASCII(u8Str,(u16)TimeH);
	SetTextValue(LCD_WARTERSET_PAGE,1,u8Str);
	NumberToASCII(u8Str,(u16)TimeM);
	SetTextValue(LCD_WARTERSET_PAGE,2,u8Str);
	WaterTimeH = TimeH;
	WaterTimeM = TimeM;
}
//ʩ�����ý���
void FertilizerSetShow(void) 
{
	u16 Time = 0;
	u8 TimeH = 0 ;
	u8 TimeM = 0;
	u8 u8Str[7] = {0,0};
	FlashReadFertilizerTime((u8*)&Time);
	TimeH = Time/60;
	TimeM = Time%60;
	NumberToASCII(u8Str,(u16)TimeH);
	SetTextValue(LCD_FERTILIZERSET_PAGE,1,u8Str);
	NumberToASCII(u8Str,(u16)TimeM);
	SetTextValue(LCD_FERTILIZERSET_PAGE,2,u8Str);
	FertilizerTimeH = TimeH;
	FertilizerTimeM = TimeM;
}
//��ˮ��ͣ��ť��������
//���룺��ֵ
//�������
//���ݼ�ֵ������Ӧ
void WaterButtonRev(u8 ButtonVal)
{
	MsgStruct Msgtemp;
	OS_CPU_SR  cpu_sr;
	//alfred //��������״̬��������Ϣ
	if(ButtonVal == 0)
	{
		Msgtemp.CmdType = MSG_WARTERING;
		Msgtemp.CmdSrc = SCREEN_TASK_CODE;
		Msgtemp.CmdData[0] = 0;
		OS_ENTER_CRITICAL();
		PackSendMasterQ(&Msgtemp);
		//SetStrategy(&HMI_ProPara);
		OS_EXIT_CRITICAL();
	}
	else if(ButtonVal == 1)
	{
		
		Msgtemp.CmdType = MSG_WARTERING;
		Msgtemp.CmdSrc = SCREEN_TASK_CODE;
		Msgtemp.CmdData[0] = 1;
		OS_ENTER_CRITICAL();
		PackSendMasterQ(&Msgtemp);
		OS_EXIT_CRITICAL();
	}
}
//ʩ����ͣ��ť��������
//���룺��ֵ
//�������
//���ݼ�ֵ������Ӧ
void FertilizerButtonRev(u8 ButtonVal)
{
	MsgStruct Msgtemp;
	OS_CPU_SR  cpu_sr;
	//alfred //��������״̬��������Ϣ
	if(ButtonVal == 0)
	{
		Msgtemp.CmdType = MSG_FERTILIZER;
		Msgtemp.CmdSrc = SCREEN_TASK_CODE;
		Msgtemp.CmdData[0] = 0;
		OS_ENTER_CRITICAL();
		PackSendMasterQ(&Msgtemp);
		SetStrategy(&HMI_ProPara);
		OS_EXIT_CRITICAL();
	}
	else if(ButtonVal == 1)
	{
		
		Msgtemp.CmdType = MSG_FERTILIZER;
		Msgtemp.CmdSrc = SCREEN_TASK_CODE;
		Msgtemp.CmdData[0] = 1;
		OS_ENTER_CRITICAL();
		PackSendMasterQ(&Msgtemp);
		OS_EXIT_CRITICAL();
	}
}
void SetRadiotube(u8 Num, u8 Value)
{
	SetButtonValue(LCD_MAIN_PAGE,Num+16,Value);
}
//���ս��������淢���ı���
void MainPage(PCTRL_MSG msg, qsize size )
{
	u16 u16Control_ID;				   //�ؼ�ID��ű���
//	s16 s16RevVal  = 0;
	u8	u8ButtonVaule = msg->param[1];
	MsgStruct Msgtemp;
	OS_CPU_SR  cpu_sr;
//	InitativeStruct InitiativeParaTemp;
	u16Control_ID = PTR2U16 ( &msg->control_id ); //�ؼ�ID
	if((u16Control_ID>=28) && (u16Control_ID<=34))//�Ϸ��İ���
	{
		MainPageSwitchSel(u16Control_ID , u8ButtonVaule);
	}
	switch(u16Control_ID)
	{
		case 1://��ˮ����
			WaterButtonRev(1);
			break;
		case 2://��ˮֹͣ
			WaterButtonRev(0);
			break;
		case 3://ʩ������
			FertilizerButtonRev(1);
			break;
		case 4://ʩ��ֹͣ
			FertilizerButtonRev(0);
			break;
		case 5:
			WaterSetShow();
			break;
		case 6:
			FertilizerSetShow();
			break;
		case 15://���翪��
			if(u8ButtonVaule == 1)
			{
				Msgtemp.CmdType = MSG_MODECHANGE;
				Msgtemp.CmdSrc = SCREEN_TASK_CODE;
				Msgtemp.CmdData[0] = 1;
				OS_ENTER_CRITICAL();
				PackSendMasterQ(&Msgtemp);
				OS_EXIT_CRITICAL();
			}
			else if(u8ButtonVaule == 0)
			{
				Msgtemp.CmdType = MSG_MODECHANGE;
				Msgtemp.CmdSrc = SCREEN_TASK_CODE;
				Msgtemp.CmdData[0] = 0;
				OS_ENTER_CRITICAL();
				PackSendMasterQ(&Msgtemp);
				OS_EXIT_CRITICAL();
			}
			break;
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
			if(u8ButtonVaule == 0)
			{
				Msgtemp.CmdType = MSG_RADIOTUBE;
				Msgtemp.CmdSrc = SCREEN_TASK_CODE;
				Msgtemp.CmdData[0] = 0;//��ŷ���
				Msgtemp.CmdData[1] = 0;//����(���㲻��������)
				OS_ENTER_CRITICAL();
				PackSendMasterQ(&Msgtemp);
				OS_EXIT_CRITICAL();
			}
			else if(u8ButtonVaule == 1)
			{
				Msgtemp.CmdType = MSG_RADIOTUBE;
				Msgtemp.CmdSrc = SCREEN_TASK_CODE;
				Msgtemp.CmdData[0] = u16Control_ID-16;//��ŷ���
				Msgtemp.CmdData[1] = 1;//����Ӧ��ŷ�,�ر�������ŷ�
				OS_ENTER_CRITICAL();
				PackSendMasterQ(&Msgtemp);
				OS_EXIT_CRITICAL();
			}
			break;
		default:
			break;
		
	}
}
void WaterSet(PCTRL_MSG msg, qsize size)
{
	u16 u16Control_ID;				   //�ؼ�ID��ű���
	s16 s16RevVal  = 0;
//	MsgStruct Msgtemp;
//	OS_CPU_SR  cpu_sr;
	u16Control_ID = PTR2U16 ( &msg->control_id ); //�ؼ�ID
	if(u16Control_ID == 1)
	{
		FifureTextInput ( &s16RevVal, msg );
		WaterTimeH =(u8) s16RevVal;
	}
	else if(u16Control_ID == 2)
	{
		FifureTextInput ( &s16RevVal, msg );
		WaterTimeM =(u8) s16RevVal;
	}
	else if(u16Control_ID == 3)
	{
		WaterTime = WaterTimeH*60+WaterTimeM;
		FlashWriteWaterTime((u8*)&WaterTime);
	}
	
}
void FertilizerSet(PCTRL_MSG msg, qsize size)
{
	u16 u16Control_ID;				   //�ؼ�ID��ű���
	s16 s16RevVal  = 0;
//	MsgStruct Msgtemp;
//	OS_CPU_SR  cpu_sr;
	u16Control_ID = PTR2U16 ( &msg->control_id ); //�ؼ�ID
	if(u16Control_ID == 1)
	{
		FifureTextInput ( &s16RevVal, msg );
		FertilizerTimeH =(u8) s16RevVal;
	}
	else if(u16Control_ID == 2)
	{
		FifureTextInput ( &s16RevVal, msg );
		FertilizerTimeM =(u8) s16RevVal;
	}
	else if(u16Control_ID == 3)
	{
		FertilizerTime = FertilizerTimeH*60+FertilizerTimeM;
		FlashWriteFertilizerTime((u8*)&FertilizerTime);
	}
}
//���ܴ�������Ϣ
void ReceiveFromMonitor( void )
{
	uint8 cmd_type = 0;
	uint8 msg_type = 0;
//	uint8 control_type = 0;
	static u16 IdleCnt = 0;
	PCTRL_MSG msg = 0;
	qsize size = 0;	

	//if(GetScreen()!= LCD_STANDBY_PAGE)
	{
		if(++IdleCnt > IDLECOUNTNUM)
		{
			SetScreen(LCD_STANDBY_PAGE);
			IdleCnt = 0;
		}
	}
	size = queue_find_cmd ( Hmi_cmd_buffer, CMD_MAX_BUFFER ); //�ӻ������л�ȡһ��ָ��
	if ( size <= 10 ) //û�н��յ�ָ��
		return;
//		continue;
	IdleCnt = 0;//�յ�������ռ���
	msg = ( PCTRL_MSG ) Hmi_cmd_buffer;
	cmd_type = msg->cmd_type;
	switch ( cmd_type ) //ָ������
	{
		case CMD_GIFMOVE_END:
			break;
		case CMD_HAND_SHAKE:
			break;
		case CMD_TYPE_UPDATE_CONTROL://�ؼ�������Ϣ
			msg_type = msg->ctrl_msg;
			switch ( msg_type ) //��Ϣ����
			{
				case kCtrlMsgAnimationEnd://��������
					break;
				case kCtrlMsgCountdownEnd://����ʱ�ؼ���
				
					break;
				case kCtrlMsgGetCurrentScreen:
					break;
				case kCtrlMsgGetMenuData:
				case kCtrlMsgGetData:
					
					s_screen_id = PTR2U16 ( &msg->screen_id );
					switch( s_screen_id )
					{
						case LCD_MAIN_PAGE:
							MainPage(msg, size);
							break;
						case LCD_WARTERSET_PAGE:
							WaterSet(msg, size);
							break;
						case LCD_FERTILIZERSET_PAGE:
							FertilizerSet(msg, size);
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		default:
			break;
	}
}
void SetSwitchAlarm(void)
{
	u8 Cnt =0 ;
	u8 Cnt1 = 0;
	for(Cnt1 = 0; Cnt1<2 ;Cnt1++)
	{
		for(Cnt = 0; Cnt<8 ; Cnt++)
		{
			if((ZoneAlarm[Cnt1]&(1<<Cnt)) == 0)
			{
				SetButtonValue(LCD_SWITCHALARM_PAGE,Cnt1*8+Cnt+1,0);
			}
			else 
			{
				SetButtonValue(LCD_SWITCHALARM_PAGE,Cnt1*8+Cnt+1,1);
			}
		}
	}
}
void TimerUpdate(void)
{
	WaterTimeShow(WarterRemainderTime);
	delay_ms(10);
	FertilizerTimeShow(FertilizerRemainderTime);
}
u8 TimeUpdateCnt = 0;
/*����AGV�Ĵ�������ʾ��������Ϣ*/
void Task_HMIMonitor ( void * parg )
{
	//uint8 cmd_type = 0;
	//uint8 msg_type = 0;
//	uint8 control_type = 0;
	//qsize size = 0;
//	uint8 update_en = 1;
	//u8 os_err;
	
	parg = parg;
	while ( 1 )
	{
		delay_ms(100);
		ReceiveFromMonitor();
		if(TimeUpdateCnt++>10)
		{
			TimerUpdate();
			TimeUpdateCnt = 0;
		}
		if(WaterSta!=OldWaterSta)
		{
			SetTextValue(LCD_MAIN_PAGE,7,(uchar *)StaText[WaterSta]);
			OldWaterSta = WaterSta;
		}
		if(FertilizerSta!=OldFertilizerSta)
		{
			SetTextValue(LCD_MAIN_PAGE,8,(uchar *)StaText[FertilizerSta]);
			OldFertilizerSta = FertilizerSta;
		}
		
	}
}
