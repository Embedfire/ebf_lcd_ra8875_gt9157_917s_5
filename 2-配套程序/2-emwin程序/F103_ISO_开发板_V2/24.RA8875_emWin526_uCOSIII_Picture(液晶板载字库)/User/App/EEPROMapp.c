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
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio_ee.h"
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
static uint16_t messages[2];
static uint8_t eepromflag=0;

/*********** 输入法 ********/
extern OS_TCB	AppTaskGuiKeyPadTCB;
extern uint8_t  keyflag;
extern WM_HWIN hKeyPad;
/*************************/
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateEEPROM[] = {
  { FRAMEWIN_CreateIndirect,"EEPROM", 		0, 				0, 0, 800, 480, 0, 0x0, 0 },
  { TEXT_CreateIndirect, 	"init", 		GUI_ID_TEXT0, 	165, 20, 200, 30, 0, 0x0, 0 },//y
  { TEXT_CreateIndirect, 	"autotest", 	GUI_ID_TEXT1, 	560, 20, 200, 30, 0, 0x0, 0 },//y
  { EDIT_CreateIndirect, 	"Edit", 			GUI_ID_EDIT0, 	140, 104, 70, 35, 0, 0x3, 0 },
  { EDIT_CreateIndirect, 	"Edit", 			GUI_ID_EDIT1, 	140, 165, 70, 35, 0, 0x3, 0 },
  { BUTTON_CreateIndirect, 	"Write", 			GUI_ID_BUTTON0, 240, 115, 90, 50, 0, 0x0, 0 },

  { EDIT_CreateIndirect, 	"Edit", 			GUI_ID_EDIT2, 	540, 104, 70, 35, 0, 0x3, 0 },
  { TEXT_CreateIndirect, 	"Text", 			GUI_ID_TEXT2, 	540, 165, 70, 35, 0, 0x0, 0 },//y
  { BUTTON_CreateIndirect, 	"Read", 			GUI_ID_BUTTON1, 640, 115, 90, 50, 0, 0x0, 0 },
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
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogEEPROM(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  OS_ERR      err;
  uint8_t num[4]={0};
  static uint8_t writedata,writeadd,readadd,readdata;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  case WM_DELETE:
		OS_DEBUG("EEPROMapp delete\n");
		WM_DeleteWindow(hKeyPad);
		eepromflag=0;
		OnICON4 = 0;
		app_prevent_refresh = 0;
		break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'EEPROM TEST'
    //
    hItem = pMsg->hWin;
	FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    FRAMEWIN_SetTitleHeight(hItem, 40);
    //
    // Initialization of 'AUTOTEST'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, &GUI_FontHZ24);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, &GUI_FontHZ24);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
	TEXT_SetBkColor(hItem,GUI_WHITE);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_RED);
	TEXT_SetText(hItem, "***");
    //
    // Initialization of 'Edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetText(hItem, "123");
    EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, 0x00FF0000);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, &GUI_FontHZ32);
	EDIT_SetUlongMode(hItem,123,0,255);
    //
    // Initialization of 'Edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
    EDIT_SetText(hItem, "123");
    EDIT_SetFont(hItem, &GUI_FontHZ32);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, 0x00FF0000);
	EDIT_SetUlongMode(hItem,123,0,255);    
    //
    // Initialization of 'Edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
    EDIT_SetText(hItem, "123");
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, 0x00FF0000);
    EDIT_SetFont(hItem, &GUI_FontHZ32);
	EDIT_SetUlongMode(hItem,123,0,255);
	//
    // Initialization of 'Write'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, &GUI_FontHZ32);
	BUTTON_SetText(hItem, "写");
    //
    // Initialization of 'Read'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, &GUI_FontHZ32);
	BUTTON_SetText(hItem, "读");
    // USER START (Optionally insert additional code for further widget initialization)
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
	//i2c_CheckDevice_EE(EE_DEV_ADDR);
	if (ee_CheckOk())
	{	eepromflag=1;
		TEXT_SetText(hItem,"初始化通过");
	}
	else 
	{	
		TEXT_SetText(hItem,"初始化失败");
		eepromflag=0;
	}
	// USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_EDIT0: // Notifications sent by 'Edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		messages[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
		messages[1]=GUI_ID_EDIT0;
		if(!keyflag)OSTaskQPost(&AppTaskGuiKeyPadTCB,
					(void *)&messages,
					2,
					OS_OPT_POST_FIFO,
					&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT1: // Notifications sent by 'Edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		messages[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
		messages[1]=GUI_ID_EDIT1;
		if(!keyflag)OSTaskQPost(&AppTaskGuiKeyPadTCB,
					(void *)&messages,
					2,
					OS_OPT_POST_FIFO,
					&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT2: // Notifications sent by 'Edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		messages[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
		messages[1]=GUI_ID_EDIT2;
		if(!keyflag)OSTaskQPost(&AppTaskGuiKeyPadTCB,
					(void *)&messages,
					2,
					OS_OPT_POST_FIFO,
					&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	case GUI_ID_BUTTON0: // Notifications sent by 'Write'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		writeadd  = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
		writedata = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
		ee_WriteBytes(&writedata, writeadd, 1);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		readadd = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
		ee_ReadBytes(&readdata,readadd,1);
	    num[0]=readdata/100+'0';
		num[1]=readdata%100/10+'0';
		num[2]=readdata%100%10+'0';
	    num[3]='\0';
		//printf("readdata=%s\n",num);
	    TEXT_SetText(WM_GetDialogItem(pMsg->hWin,GUI_ID_TEXT2),(const char *)num);
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
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ24);
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("初始化结果：",30,25);
		GUI_DispStringAt("自动测试结果：",400,25);
		GUI_DispStringHCenterAt("写入数据(0-255)：",200,70);
		GUI_DispStringHCenterAt("读出数据(0-255)：",600,70);
		GUI_DispStringAt("地址：",60,107);
		GUI_DispStringAt("数据：",60,167);
	
		GUI_DispStringAt("地址：",460,107);
		GUI_DispStringAt("数据：",460,167);
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
*       CreateEEPROM TEST
*/
void OnICON4Clicked(void)
{
	WM_HWIN hWin;
	uint8_t flag=0;
	uint16_t i;
	uint8_t write_buf[EE_SIZE],read_buf[EE_SIZE];
	OS_DEBUG("EEPROMapp create\n");
	hWin = GUI_CreateDialogBox(_aDialogCreateEEPROM, GUI_COUNTOF(_aDialogCreateEEPROM), _cbDialogEEPROM, WM_HBKWIN, 0, 0);
	if(eepromflag)
	{
		/*------------------------------------------------------------------------------------*/  
		/* 填充测试缓冲区 */
		for (i = 0; i < EE_SIZE; i++)
		{		
			write_buf[i] = i;
		}
		/*------------------------------------------------------------------------------------*/  
		if (ee_WriteBytes(write_buf, 0, EE_SIZE))//写数据
		{
			i=250;
			while(--i);//短延时
			if (ee_ReadBytes(read_buf, 0, EE_SIZE))//读数据
			{	
				for (i = 0; i < EE_SIZE; i++)//数据比较
				{
					if(read_buf[i] != read_buf[i])
					{
						flag++;
					}
				}
			}
		}
		if(flag)
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"自动测试失败");
		else
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"自动测试成功");
	}
	else
		TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"自动测试失败");
	while(OnICON4)
	{
		GUI_Delay(10); 
	}
}
// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
