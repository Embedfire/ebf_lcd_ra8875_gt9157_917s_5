/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.22                          *
*        Compiled Jul  4 2013, 15:16:01                              *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "includes.h"
#include  "app.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* 在中断处理函数中返回 */
extern  __IO uint32_t ret;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCAN[] = {
  { FRAMEWIN_CreateIndirect, "CAN LoopBack", 0, 0, 0, 800, 480, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", GUI_ID_BUTTON0, 300, 60, 200, 100, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text_1", GUI_ID_TEXT0, 300, 200, 200, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text_2", GUI_ID_TEXT1, 300, 250, 200, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text_3", GUI_ID_TEXT2, 300, 300, 200, 40, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
/*
 * 函数名：CAN_NVIC_Configuration
 * 描述  ：CAN RX0 中断优先级配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Configuration(FunctionalState able)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable CAN1 RX0 interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    // 主优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = able;
	NVIC_Init(&NVIC_InitStructure);
}
/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN GPIO 和时钟配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	                        											 
	
	/* CAN1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX */									               // PB8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1 本实验没有用到重映射I/O
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}
/*
 * 函数名：CAN_Polling
 * 描述  ：配置 CAN 的工作模式为 回环模式
 * 输入  ：无
 * 输出  : -PASSED   成功
 *         -FAILED   失败
 * 调用  ：内部调用
 */
TestStatus CAN_Polling(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CanTxMsg TxMessage;
	CanRxMsg RxMessage;
	uint32_t i = 0;
	uint8_t TransmitMailbox = 0;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;						 // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=DISABLE;						 // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;						 // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=DISABLE;						 // 自动重新传送豹纹，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;						 // FIFO没有锁定，新报文覆盖旧报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;						 // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;	 // 回环模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;				 // 1tq、BS1、BS2的值跟波特率有关
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=5;             // 分频系数为5
	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* transmit */
	TxMessage.StdId=0x11;				// 设定标准标识符（11位，扩展的为29位）
	TxMessage.RTR=CAN_RTR_DATA;	// 传输消息的帧类型为数据帧（还有远程帧）
	TxMessage.IDE=CAN_ID_STD;		// 消息标志符实验标准标识符
	TxMessage.DLC=2;					  // 发送两帧，一帧8位
	TxMessage.Data[0]=0xCA;			// 第一帧数据
	TxMessage.Data[1]=0xFE;			// 第二帧数据
	
	TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
	i = 0;
	// 用于检查消息传输是否正常
	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	
	i = 0;
	// 检查返回的挂号的信息数目
	while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
	{
		i++;
	}
	
	/* receive */
	RxMessage.StdId=0x00;
	RxMessage.IDE=CAN_ID_STD;
	RxMessage.DLC=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	
	if (RxMessage.StdId!=0x11)
	{
		return FAILED;  
	}
	
	if (RxMessage.IDE!=CAN_ID_STD)
	{
		return FAILED;
	}
	
	if (RxMessage.DLC!=2)
	{
		return FAILED;  
	}
	
	/* 判断发送的信息和接收的信息是否相等 */
	if ((RxMessage.Data[0]<<8|RxMessage.Data[1])!=0xCAFE)
	{
		return FAILED;
	}
	
	//printf("receive data:0X%X,0X%X",RxMessage.Data[0], RxMessage.Data[1]);  
	
	return PASSED; /* Test Passed */
}
/*
 * 函数名：CAN_Interrupt
 * 描述  ：配置 CAN 的工作模式为 中断模式
 * 输入  ：无
 * 输出  : -PASSED   成功
 *         -FAILED   失败
 * 调用  ：内部调用
 */
TestStatus CAN_Interrupt(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CanTxMsg TxMessage;
	uint32_t i = 0;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;           // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=DISABLE;	          // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;	          // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=DISABLE;	          // 自动重新传送豹纹，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;	          // FIFO没有锁定，新报文覆盖旧报文
	CAN_InitStructure.CAN_TXFP=DISABLE;           // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack; // 回环模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;        // 1tq、BS1、BS2的值跟波特率有关
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=1;					  // 分频系数为1
	CAN_Init(CAN1, &CAN_InitStructure);						// 初始化CAN1
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=1;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); //中断使能
	
	/* transmit 1 message */
	TxMessage.StdId=0x00;					 // 标准标识符为0
	TxMessage.ExtId=0x1234;				 // 设置扩展标示符（29位）
	TxMessage.IDE=CAN_ID_EXT;			 // 使用扩展标识符
	TxMessage.RTR=CAN_RTR_DATA;		 // 消息类型为数据帧，一帧8位
	TxMessage.DLC=2;							 // 发送两帧信息
	TxMessage.Data[0]=0xDE;				 // 第一帧信息
	TxMessage.Data[1]=0xCA;				 // 第二帧信息
	CAN_Transmit(CAN1, &TxMessage);
	
	/* initialize the value that will be returned */
	ret = 0xFF;
	
	/* receive message with interrupt handling */
	i=0;
	while((ret == 0xFF) && (i < 0xFFF))
	{
	i++;
	}
	
	if (i == 0xFFF)
	{
	ret=0;  
	}
	
	/* disable interrupt handling */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
	
	return (TestStatus)ret;
}

/*
 * 函数名：CAN_Init
 * 描述  ：CAN初始化，包括端口初始化和中断优先级初始化
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：外部调用
 */
void USER_CAN_Init(void)
{
	CAN_NVIC_Configuration(ENABLE);
	CAN_GPIO_Config();
}
void delCAN(void)
{
	CAN_NVIC_Configuration(DISABLE);
	/* CAN1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,DISABLE);
	
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogCAN(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	case WM_DELETE:
		OS_DEBUG("CANapp delete");
		delCAN();
		OnICON3 = 0;
		app_prevent_refresh = 0;
		break;
	case WM_INIT_DIALOG:
	//
	// Initialization of 'CAN TEST'
	//
	hItem = pMsg->hWin;
	FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
	FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
	FRAMEWIN_SetTitleHeight(hItem, 40);
	FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
	//
	// Initialization of 'Button'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
	BUTTON_SetText(hItem, "开始测试");
	BUTTON_SetFont(hItem, &GUI_FontHZ32);
	//
	// Initialization of 'Text_1'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
	TEXT_SetFont(hItem, &GUI_FontHZ32);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetTextColor(hItem, 0x00FF0000);
	TEXT_SetText(hItem, "");
	//
	// Initialization of 'Text_2'
	//
	hItem = WM_GetDialogItem(pMsg->hWin,GUI_ID_TEXT1);
	TEXT_SetText(hItem, "");
	TEXT_SetFont(hItem, &GUI_FontHZ32);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetTextColor(hItem, 0x00FF0000);
	//
	// Initialization of 'Text'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
	TEXT_SetFont(hItem, &GUI_FontHZ32);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetTextColor(hItem, 0x00FF0000);
	TEXT_SetText(hItem, "");
	// USER START (Optionally insert additional code for further widget initialization)
	// USER END
	break;
	case WM_NOTIFY_PARENT:
	Id    = WM_GetId(pMsg->hWinSrc);
	NCode = pMsg->Data.v;
	switch(Id) {
	case GUI_ID_BUTTON0: // Notifications sent by 'Button'
	  switch(NCode) {
	  case WM_NOTIFICATION_CLICKED:
		// USER START (Optionally insert code for reacting on notification message)
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0), "");
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), "");
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2), "");
		// USER END
		break;
	  case WM_NOTIFICATION_RELEASED:
		// USER START (Optionally insert code for reacting on notification message)
	    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0),"开始...");
		USER_CAN_Init();
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1),"初始化成功");
		if(CAN_Polling() && CAN_Interrupt())
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"CAN回路测试成功");
		else
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"CAN回路测试失败");		
		// USER END
		break;
	  // USER START (Optionally insert additional code for further notification handling)
	  // USER END
	  }
	  break;
	// USER START (Optionally insert additional code for further Ids)
	// USER END
	}
	break;
	// USER START (Optionally insert additional message handling)
	case WM_PAINT:	
		GUI_SetBkColor(APPBKCOLOR);
		GUI_SetColor(APPTEXTCOLOR);
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ32);
		GUI_DispStringHCenterAt("CAN回路测试",400,20);
		break;
	// USER END
	default:
		WM_DefaultProc(pMsg);
	break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateCAN TEST
*/
void OnICON3Clicked(void)
{
	GUI_CreateDialogBox(_aDialogCreateCAN, GUI_COUNTOF(_aDialogCreateCAN), _cbDialogCAN, WM_HBKWIN, 0, 0);
	OS_DEBUG("CANapp create");
	while(OnICON3)
	{					 				
		GUI_Delay(10); 				
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
