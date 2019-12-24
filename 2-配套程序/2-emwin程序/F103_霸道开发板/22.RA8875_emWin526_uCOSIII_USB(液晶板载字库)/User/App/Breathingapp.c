/*********************************************************************
*	所有例程文件都是以UTF-8编码格式，不然中文显示出现乱码。
*	如果只是个别例程出现中文显示乱码，
*	把对应的例程文件(比如LEDapp.c)用电脑的记事本软件打开，然后选择另
*	存为，在弹出对话框中“保存(S)"按钮的左边有个"编码(E)"选项，选择
*	"UTF-8"，然后同样保存为同名称的C语言文件(覆盖原来文件)。
*
*	论坛    :http://www.firebbs.cn
*	淘宝    :http://fire-stm32.taobao.com
*	邮箱	:wildfireteam@163.com
*
*
**********************************************************************
*/
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
// USER END
#include "includes.h"
#include  "app.h"

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
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateBLN[] = {
  { FRAMEWIN_CreateIndirect, "Breathing",0, 0, 0, 800, 480, 0, 0x0, 0 },
//  { TEXT_CreateIndirect, "Breathing", GUI_ID_TEXT0, 15, 80, 200, 20, 0, 0x0, 0 },
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
 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

  /* 配置呼吸灯用到的PB0引脚 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
   
  /* 配置TIM3_IRQ中断为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  配置TIM3输出的PWM信号的模式，如周期、极性
  * @param  无
  * @retval 无
  */

/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> 中断 & TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 *
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 
 */

/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* 设置TIM3CLK 时钟为72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 			//使能TIM3时钟

	/* 基本定时器配置 */		 
	TIM_TimeBaseStructure.TIM_Period = 255;       					//当定时器从0计数到255，即为266次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 1999;	    				//设置预分频：
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;		//设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    		//配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;								//设置初始PWM脉冲宽度为0	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  	//当定时器计数值小于CCR1_Val时为低电平

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 					//使能通道3
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);				//使能预装载
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 					//使能TIM3重载寄存器ARR	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);                   						//使能定时器3	
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);						//使能update中断		
	NVIC_Config_PWM();												//配置中断优先级		
	
}
static void stopbreathing(void)
{
	TIM_Cmd(TIM3, DISABLE);                   						//禁用定时器3	
	TIM_ITConfig(TIM3,TIM_IT_Update, DISABLE);						//禁用update中断
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); 			//禁用TIM3时钟
}

/**
  * @brief  TIM3 呼吸灯初始化
  *         配置PWM模式和GPIO
  * @param  无
  * @retval 无
  */
static void TIM3_Breathing_Init(void)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config();	
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogBLN(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  case WM_DELETE:
		    OS_DEBUG("Breathingapp delete\n");
			stopbreathing();
			LED_GPIO_Config(); 
		    OnICON8 = 0;
			app_prevent_refresh = 0;
			LED1_OFF;
		    break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Breathing'
    //
    hItem = pMsg->hWin;
	FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    FRAMEWIN_SetTitleHeight(hItem, 40);
//    //
//    // Initialization of 'This is the Breathing Test'
//    //
//    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
//    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//    TEXT_SetFont(hItem, &XBF_Font);
//    TEXT_SetTextColor(hItem, GUI_BLUE);
//	TEXT_SetText(hItem,"观察LED1现象");
    // USER START (Optionally insert additional code for further widget initialization)
    TIM3_Breathing_Init();
	// USER END
    break;
  // USER START (Optionally insert additional message handling)
     case WM_PAINT:	
		GUI_SetBkColor(APPBKCOLOR);
		GUI_SetColor(APPTEXTCOLOR);
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ32);
		GUI_DispStringHCenterAt("呼吸灯",400,20);
		GUI_DispStringHCenterAt("利用PWM控制LED1亮度",400,80);
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
*       CreateBreathing
*/
void OnICON8Clicked(void){
	GUI_CreateDialogBox(_aDialogCreateBLN, GUI_COUNTOF(_aDialogCreateBLN), _cbDialogBLN, WM_HBKWIN, 0, 0);
	OS_DEBUG("Breathingapp create\n");
	while(OnICON8)
	{					 				
		GUI_Delay(10); 				
	}
}

// USER START (Optionally insert additional public code)
/* 呼吸灯中断服务函数 */
void TIM3_IRQHandler(void)
{	
	static uint8_t pwm_index = 0;			//用于PWM查表
	static uint8_t period_cnt = 0;		//用于计算周期数
	/* LED亮度等级 PWM表 */
	uint8_t indexWave[] = {1,1,2,2,3,4,6,8,10,14,19,25,33,44,59,80,107,143,191,255,
							255,191,143,107,80,59,44,33,25,19,14,10,8,6,4,3,2,2,1,1};
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;
			if(period_cnt >= 10)										//若输出的周期数大于10，输出下一种脉冲宽的PWM波
			{
				
				TIM3->CCR3 = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
				pwm_index++;												//标志PWM表的下一个元素
			
				if( pwm_index >=  40)								//若PWM脉冲表已经输出完成一遍，重置PWM查表标志
				{
					pwm_index=0;								
				}
				
				period_cnt=0;												//重置周期计数标志
		}
		TIM_ClearITPendingBit (TIM3, TIM_IT_Update);	//必须要清除中断标志位
	}
}
// USER END

/*************************** End of file ****************************/
