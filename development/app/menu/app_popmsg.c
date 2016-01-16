#include "app_popmsg.h"


#define TEXT_WIN_POPMSG_YESNO		"win_popmsg_yesno"
#define TEXT_TEXT_POPMSG_YESNO		"win_popmsg_text_yesno"
#define TEXT_TEXT_POPMSG_TIME_OUT	"win_popmsg_time_out"
#define TEXT_BUTTON_POPMSG_YES		"win_popmsg_button_yes"
#define TEXT_BUTTON_POPMSG_NO		"win_popmsg_button_no"

#define TEXT_WIN_POPMSG_OK			"win_popmsg_ok"
#define TEXT_TEXT_POPMSG_OK			"win_popmsg_text_ok"
#define TEXT_BUTTON_POPMSG_OK		"win_popmsg_button_ok"


static popmsg_ret app_popmsg_result = POPMSG_RET_NONE;
static popmsg_type app_popmsg_type_sel = POPMSG_TYPE_YESNO;

static int32_t popmsg_yes_timeout = 0;

char* gs_popmsg_win = NULL;

#ifdef DVB_CA_TYPE_WF_CA_FLAG
extern void wf_cas_rolling_message_show(uint8_t byStyle,uint8_t showFlag,char* message);
extern void WFSTB_Set_roll_status(uint8_t flag);
#endif


void	app_set_popmsg_yes_timeout(int32_t time)
{
	popmsg_yes_timeout = time;
	return;
}

static status_t popmsg_ok_key(void)
{
	char* name = NULL;

	name = (char*)GUI_GetFocusWidget();

	switch(app_popmsg_type_sel)
	{
		case POPMSG_TYPE_YESNO:
			if(0 == strcasecmp(name, TEXT_BUTTON_POPMSG_YES))
			{
				app_popmsg_result = POPMSG_RET_YES;
			}
			else if(0 == strcasecmp(name, TEXT_BUTTON_POPMSG_NO))
			{
				app_popmsg_result = POPMSG_RET_NO;
			}
			break;
		case POPMSG_TYPE_OK:
			app_popmsg_result = POPMSG_RET_OK;
			break;

		default:
			return GXCORE_ERROR;
			break;
	}
	return GXCORE_SUCCESS;
}

static status_t popmsg_esc_key(void)
{
	switch(app_popmsg_type_sel)
	{
		case POPMSG_TYPE_YESNO:
			app_popmsg_result = POPMSG_RET_NO;
			break;

		case POPMSG_TYPE_OK:
			app_popmsg_result = POPMSG_RET_OK;
			break;

		default:
			return GXCORE_ERROR;
			break;
	}
	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int app_popmsg_init(const char* widgetname, void *usrdata)
{
	app_popmsg_result = POPMSG_RET_NONE;

	if(POPMSG_TYPE_YESNO == app_popmsg_type_sel)
	{
		GUI_SetFocusWidget(TEXT_BUTTON_POPMSG_NO);
	}

	return 0;
}

SIGNAL_HANDLER int app_popmsg_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{						
		case APPK_BACK:
		case APPK_MENU:
			popmsg_esc_key();
			break;

		case APPK_OK:
			popmsg_ok_key();
			break;

		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}



popmsg_ret app_popmsg(uint32_t x, uint32_t y, const char* context, popmsg_type type)
{
	char* xml_popmsg_win = NULL;
	char* xml_popmsg_text = NULL;
	char popmsg_win_yesno[] = {TEXT_WIN_POPMSG_YESNO};
	char popmsg_text_yesno[] = {TEXT_TEXT_POPMSG_YESNO};
	char popmsg_win_ok[] = {TEXT_WIN_POPMSG_OK};
	char popmsg_text_ok[] = {TEXT_TEXT_POPMSG_OK};

	GxTime nowtime={0};
	GxTime starttime={0};
	uint8_t time[10] = {0};
	int OkTimeout = 3;
	int iStartTimeStatus = 1;
	uint32_t startMs = 0, endMs= 0;
#ifdef DVB_CA_TYPE_WF_CA_FLAG
#ifdef DVB_CA_TYPE_WF
	wf_cas_rolling_message_show(0,3,NULL);
	WFSTB_Set_roll_status(0);
#endif	
#endif
	/*
	 * �����ǰ��ʾ��״̬������ʾ����Ϣ������ֱ�ӷ���
	 *��������ʾ������
	 */
	if (TRUE == app_get_pop_msg_flag_status())
	{
		popmsg_yes_timeout = 0;
		printf("\n[app_popmsg]:notshow[ %s ]tpye[%d] have return\n\n",context,type);
		switch(type)
		{
			case POPMSG_TYPE_YESNO:
				return POPMSG_RET_NO;
			case POPMSG_TYPE_OK:
				return POPMSG_RET_OK;
			default:
				return POPMSG_RET_NONE;
		}			
	}

	app_set_pop_msg_flag_status(TRUE);

	if (POPMSG_TYPE_YESNO == type)
	{
		app_play_set_popmsg_status(1);
	}




	app_popmsg_type_sel = type;

	switch(type)
	{
		case POPMSG_TYPE_YESNO:
			xml_popmsg_win = popmsg_win_yesno;
			xml_popmsg_text = popmsg_text_yesno;
			break;
		case POPMSG_TYPE_OK:
			xml_popmsg_win = popmsg_win_ok;
			xml_popmsg_text = popmsg_text_ok;
			break;

		default:
			return POPMSG_RET_NONE;
			break;
	}

	gs_popmsg_win = xml_popmsg_win;

	GUI_EndDialog(xml_popmsg_win);//fix create two and more
	GUI_CreateDialog(xml_popmsg_win);
	GUI_SetProperty(xml_popmsg_win, "move_window_x", &x);
	GUI_SetProperty(xml_popmsg_win, "move_window_y", &y);
	GUI_SetProperty(xml_popmsg_text, "string", (void*)context);
	if (type == POPMSG_TYPE_OK)
	{
		GUI_SetProperty("win_popmsg_button_ok", "state", "hide");
	}

	//app_msg_destroy(g_app_msg_self);
	if (popmsg_yes_timeout > 0)
	{
		sprintf((void*)time, "%02d",popmsg_yes_timeout);
		GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT, "state", "show");
		GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT, "string", time);
	}
	else
	{
		GxCore_GetTickTime(&starttime);
	}

	GUI_SetInterface("flush", NULL);

	while(POPMSG_RET_NONE == app_popmsg_result)
	{
		GUI_LoopEvent();
		GxCore_ThreadDelay(50);
		if (type == POPMSG_TYPE_OK)
		{
			GxCore_GetTickTime(&nowtime);	
			if (nowtime.seconds - starttime.seconds >= OkTimeout)
			{
				app_popmsg_result = POPMSG_RET_YES;
#ifdef CUST_JIMO
				if(0 == strcmp((void*)context,"        �ź��ж�        �벥��ά�޵绰��87568578"))
					app_popmsg_result = POPMSG_RET_NONE;
#endif
			}
		}
		else
		{
			if (popmsg_yes_timeout >0 )
			{
				if (iStartTimeStatus)
				{
					GxCore_GetTickTime(&starttime);
					iStartTimeStatus = 0;
					startMs = starttime.seconds * 1000 + starttime.microsecs / 1000;
				}

				GxCore_GetTickTime(&nowtime);	

				endMs = nowtime.seconds * 1000 + nowtime.microsecs / 1000;

				if (((endMs - startMs) / 1000) >= popmsg_yes_timeout)
					app_popmsg_result = POPMSG_RET_YES;
				else
				{
					char* focus_Window = (char*)GUI_GetFocusWindow();
					if ((NULL != focus_Window)&&(0 == strcasecmp(xml_popmsg_win, focus_Window)))
					{
						sprintf((void*)time,"%02d",popmsg_yes_timeout - ((endMs - startMs) / 1000));
						GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT,"state","show");
						GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT, "string", (void*)time);								
					}
				}
			}
		}
	}

	popmsg_yes_timeout = 0;
	GUI_EndDialog(xml_popmsg_win);
	GUI_SetInterface("flush", NULL);
	app_set_pop_msg_flag_status(FALSE);
	//app_msg_init(g_app_msg_self);
	app_play_set_popmsg_status(0);
	return app_popmsg_result;
}

/* Ϊ�˺�app_popmsg()��YES/NO dialogʹ��ͬһ��XML */
int app_ota_popmsg(uint32_t x, uint32_t y, const char* context, int32_t ri_Timeout)
{
	char* xml_popmsg_text = "win_popmsg_text_yesno";
	GxTime nowtime = {0};
	GxTime starttime={0};
	uint8_t time[10] = {0};

	app_set_pop_msg_flag_status(TRUE);

	GUI_EndDialog("win_popmsg_yesno");

	GUI_CreateDialog("win_popmsg_yesno");

	GUI_SetProperty("win_popmsg_yesno", "move_window_x", &x);
	GUI_SetProperty("win_popmsg_yesno", "move_window_y", &y);
	GUI_SetProperty(xml_popmsg_text, "string", (void*)context);

	GUI_SetProperty("win_popmsg_button_yes", "state", "hide");	
	GUI_SetProperty("win_popmsg_button_no", "state", "hide");

	GUI_SetInterface("flush", NULL);

	GxCore_GetTickTime(&starttime);

	while(POPMSG_RET_NONE == app_popmsg_result)
	{
		GUI_LoopEvent();
		GxCore_ThreadDelay(50);
		if (ri_Timeout > 0)
		{
			GxCore_GetTickTime(&nowtime);	
			printf("\n---------------------->[nowtime.seconds - starttime.seconds = %d].\n",nowtime.seconds - starttime.seconds);
			if (nowtime.seconds - starttime.seconds >= ri_Timeout)
			{
				app_popmsg_result = POPMSG_RET_YES;
			}
			else
			{
				char* focus_Window = (char*)GUI_GetFocusWindow();
				if ((NULL != focus_Window)&&(0 == strcasecmp("win_popmsg_yesno", focus_Window)))
				{
					sprintf((void*)time,"%02d",ri_Timeout - (nowtime.seconds - starttime.seconds));
					GUI_SetProperty("win_popmsg_time_out","state","show");
					GUI_SetProperty("win_popmsg_time_out", "string", (void*)time);								
				}
			}
		}
	}

	GUI_EndDialog("win_popmsg_yesno");
	GUI_SetInterface("flush", NULL);
	app_set_pop_msg_flag_status(FALSE);
	printf("\n------------------->[*******************************]");

	return POPMSG_RET_YES;
}
