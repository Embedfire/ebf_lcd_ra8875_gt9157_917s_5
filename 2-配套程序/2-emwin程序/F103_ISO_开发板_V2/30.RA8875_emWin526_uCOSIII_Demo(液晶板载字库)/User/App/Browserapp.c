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
#define _DF1S	0x81
#define BROWSER_FILE_NAME_LEN 	100							//文件名长度，如果检测到文件名超过50 则丢弃这个文件 
#define BROWSER_PATH_LEN		50
#define FILE_LIST_PATH 			"0:/system/filelist.txt"	//文件记录列表文件的目录
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
extern FIL	file;
extern FRESULT result;
extern UINT bw;	

static WM_HWIN hOpenfile,fileview;
static char *txtBuffer=0;
static char *_acBuffer=0;
static char* record_file=0;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateBrowser[] = {
  { FRAMEWIN_CreateIndirect, "Browser", 0, 0, 0, 800, 480, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "FileTree", GUI_ID_TREEVIEW0, 0, 0, 400-5, 440-5, 0, 0x0, 0 },

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

static uint8_t  txt2buffer(const char * sFilename) 
{
	OS_ERR      	err;
	txtBuffer = (char *)(0x680c0000);
	OSSchedLock(&err);
	
	result = f_open(&file, sFilename, FA_READ);
	if (result != FR_OK)
	{
		OSSchedUnlock(&err);
		return 0;
	}
	//printf("filesize=%d\n",file.fsize);
	if(file.fsize>=1024*50*2)
	{
		printf("TXT read fail!!!\n");
		OSSchedUnlock(&err);
		return 0;
  	}
	result = f_read(&file, txtBuffer, file.fsize, &bw);
	txtBuffer[file.fsize]='\0';
	if (result != FR_OK)
  	{
		OSSchedUnlock(&err);
		return 0;
  	}
	f_close(&file);
	OSSchedUnlock(&err);
	return 1;	
}
// USER START (Optionally insert additional static code)
static uint8_t  ShowBMP(WM_HWIN  hItem,const char * sFilename) 
{
	OS_ERR      	err;

	_acBuffer = (char *)(0x680c0000);
	
	OSSchedLock(&err);
	
	result = f_open(&file, sFilename, FA_READ);
	if (result != FR_OK||file.fsize>256*1024)
	{
		OSSchedUnlock(&err);
		return 0;
	}
	result = f_read(&file, _acBuffer, file.fsize, &bw);
	if (result != FR_OK)
  	{
		OSSchedUnlock(&err);
		return 0;
  	}
    f_close(&file);
	
	if(strstr(sFilename,".bmp")||strstr(sFilename,".BMP"))		
		IMAGE_SetBMP(hItem,_acBuffer,file.fsize);
	else if(strstr(sFilename,".jpg")||strstr(sFilename,".JPG"))
		IMAGE_SetJPEG(hItem,_acBuffer,file.fsize);
	else
		IMAGE_SetGIF(hItem,_acBuffer,file.fsize);
		
	OSSchedUnlock(&err);	

	return 1;
	
}

/**
  * @brief  OpenFileProcess打开文件	// TBD jpg显示不正常 
	*					
  * @param  none
  * @retval none
  */
static void OpenFileProcess(int sel_num,char* record_file)
{
	char openfile[BROWSER_FILE_NAME_LEN]={0};
	result = f_open (&file, FILE_LIST_PATH, FA_READ|FA_OPEN_EXISTING); //打开索引文件
	if(result != FR_OK)
      return ;
	result = f_lseek (&file, sel_num*BROWSER_FILE_NAME_LEN);
	if(result != FR_OK)
      return ;
	result = f_read (&file, openfile, BROWSER_FILE_NAME_LEN, &bw);	
	if(result != FR_OK)
      return ;
	//printf("\nfileItem=:%s",openfile);
	f_close (&file);
	WM_DeleteWindow(fileview);
	if(strstr(openfile,".txt")||strstr(openfile,".TXT")
			||strstr(openfile,".c")||strstr(openfile,".c")
				||strstr(openfile,".cpp")||strstr(openfile,".CPP")
						||strstr(openfile,".h")||strstr(openfile,".h"))//判断是否可以使用txt格式打开
	{
		//printf("\ntxt file");
		FRAMEWIN_SetText(hOpenfile,openfile);
		fileview=MULTIEDIT_CreateEx(5,30,400-10,440-35,hOpenfile,
							WM_CF_SHOW,MULTIEDIT_CF_AUTOSCROLLBAR_V|MULTIEDIT_CF_READONLY,0,0,0);
		MULTIEDIT_SetFont(fileview,&GUI_FontHZ24);
		MULTIEDIT_SetBkColor(fileview,MULTIEDIT_CI_READONLY,GUI_WHITE);
		MULTIEDIT_SetWrapWord(fileview);
		txt2buffer(openfile);
		MULTIEDIT_AddText(fileview,txtBuffer);
	}
	else if(strstr(openfile,".bmp")||strstr(openfile,".BMP")||
						strstr(openfile,".jpg")||strstr(openfile,".JPG")||
								strstr(openfile,".gif")||strstr(openfile,".GIF"))
	{
		//printf("\npicture file");
		FRAMEWIN_SetText(hOpenfile,openfile);
		fileview=IMAGE_CreateEx(5,30,400-10,440-35,hOpenfile,WM_CF_SHOW,0,0);
		ShowBMP(fileview,openfile);					
	}
	else
	{				
		GUI_MessageBox("Sorry,do not support open the file!","Sorry",GUI_MESSAGEBOX_CF_MOVEABLE);

	}
	
}
/**
  * @brief  scan_files 递归扫描sd卡内的文件
  * @param  path:初始扫描路径 file_name：指向用来存储文件名的一段空间 hFile:用于记录文件路径的文件指针 hTree 目录树 hNode 目录结点
	*					hTree == NULL &&	hNode == NULL 的话，不创建目录树			
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path,char* file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,int *fileNum) 
{ 
		
    FRESULT res; 		          //部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
	unsigned int rw_num;			//已读或已写的字节数
	char showname[60]={0};
    DIR dir; 
    int i; 
    char *fn; 	
	TREEVIEW_ITEM_Handle hItem=0;

	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path);                            //打开目录
    if (res == FR_OK) 
	{ 
        i = strlen(path); 
        for (;;) 
				{ 
            res = f_readdir(&dir, &fno); 										//读取目录下的内容
            if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
            fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
            fn = fno.fname; 
#endif 
            if (*fn == '.') continue; 											//点表示当前目录，跳过			
            if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
							
				if(hTree != NULL &&	hNode != NULL)
				{
//					gbk2utf8(fn,showname,60);
//					hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,showname,0);
					hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);						//目录，创建结点
					TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把结点加入到目录树中
				}
								
				sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path,file_name,hFile,hTree,hItem,fileNum);		//递归遍历 
                if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
			else 																														//是文件
			{ 				
				/* 根据要求是否创建目录树 */
				if(hTree != NULL &&	hNode != NULL)																			//创建目录树
				{
					//gbk2utf8(fn,showname,60);
					//hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,showname,0);
					hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);						//文件，创建树叶
					TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把树叶添加到目录树
			
				
					if (strlen(path)+strlen(fn)<BROWSER_FILE_NAME_LEN)
					{
						sprintf(file_name, "%s/%s", path,fn); 	
						
						//存储文件名到filelist(含路径)										
						res = f_lseek (hFile, hItem*BROWSER_FILE_NAME_LEN);  
						res = f_write (hFile, file_name, BROWSER_FILE_NAME_LEN, &rw_num);	
						//printf("\nfileItem=%ld:%s",hItem,file_name);
					}			
				}
           }//else
        } //for
    } 

    return res; 
} 


/**
  * @brief  Fill_FileList处理非递归过程，然后调用递归函数scan_files扫描目录
	*					
  * @param  path:初始扫描路径
  * @retval none
  */
void Fill_FileList(char* path,char* record_file,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,int *p)
{
	char  p_path[BROWSER_FILE_NAME_LEN]={0};									//目录名 指针
	char  file_name[BROWSER_FILE_NAME_LEN]={0};								//用于存储的目录文件名，
	result = f_unlink(record_file);//删除旧的filelist		// TBD 增加自建目录
	result = f_open (&file, record_file, FA_READ|FA_WRITE|FA_CREATE_ALWAYS ); //打开创建索引文件
     if(result != FR_OK)
      return ;

	strcpy(p_path,path);						//复制目录名到指针
	
	result = scan_files(p_path,file_name,&file,hTree,hNode,p);			//递归扫描文件		
	
	f_close (&file);					//关闭索引文件	

}
// USER END
/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogBrowser(WM_MESSAGE * pMsg) {
  TREEVIEW_ITEM_Handle hNode;			//结点句柄
  TREEVIEW_ITEM_INFO ItemInfo;
  WM_HWIN                hItem;
  int                    NCode;
  int                    Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  case WM_DELETE:
		OS_DEBUG("Browserapp delete\n");
		record_file=0;
		OnICON16 = 0;
		app_prevent_refresh = 0;
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Browser'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    FRAMEWIN_SetTitleHeight(hItem, 40);
    //
    // Initialization of 'FileTree'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TREEVIEW0);
	TREEVIEW_SetFont(hItem, &GUI_FontHZ32);
	TREEVIEW_SetDefaultFont(&GUI_FontHZ32);
	TREEVIEW_SetAutoScrollH(hItem,1);
	TREEVIEW_SetAutoScrollV(hItem,1);
	TREEVIEW_SetSelMode(hItem, TREEVIEW_SELMODE_ROW);
	hNode = TREEVIEW_InsertItem(hItem, TREEVIEW_ITEM_TYPE_NODE, 0, 0, "ExFlash");
    // USER START (Optionally insert additional code for further widget initialization)
	Fill_FileList("0:",record_file,hItem,hNode,NULL);
	TREEVIEW_ITEM_Expand(hNode);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_TREEVIEW0: // Notifications sent by 'FileTree'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//printf("\n release\n");				
		/* 查看选中了哪个项目 */
		hNode = TREEVIEW_GetSel(pMsg->hWinSrc);						
        /* 获取该项目的信息 */
		TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
				
		if(ItemInfo.IsNode == 0)        //点击的是目录树的叶子（即文件）
		{
			//printf("\r\nleaf num =%ld",hNode);
			OpenFileProcess(hNode,record_file);
		}
//		else										        //结点
//		{					
//			printf("\r\nnode num =%ld",hNode);
//		}
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
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
*       CreateBrowser
*/
void OnICON16Clicked(void)
{
	WM_HWIN    hWin;
	OS_DEBUG("Browserapp create\n");
	record_file=FILE_LIST_PATH;
	hWin=GUI_CreateDialogBox(_aDialogCreateBrowser, GUI_COUNTOF(_aDialogCreateBrowser), _cbDialogBrowser, WM_HBKWIN, 0, 0);
	hOpenfile=FRAMEWIN_CreateEx(400,40,400,440,hWin,WM_CF_SHOW,0,0,0,0);
	FRAMEWIN_SetTextColor(hOpenfile,GUI_DARKGRAY);
	FRAMEWIN_SetTitleHeight(hOpenfile, 25);
	FRAMEWIN_SetFont(hOpenfile, &GUI_FontHZ24);
	FRAMEWIN_SetText(hOpenfile,"");
	FRAMEWIN_SetTextAlign(hOpenfile, GUI_TA_LEFT | GUI_TA_VCENTER);
	
	while(OnICON16)
	{
		GUI_Delay(10); 
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
