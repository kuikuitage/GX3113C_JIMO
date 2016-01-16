/**
 *
 * @file        app_mg_cas_api_pop.h
 * @brief
 * @version     1.1.0
 * @date        04/15/2014 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_MG_CAS_API_POP_H__
#define __APP_MG_CAS_API_POP_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "app_common_play.h"
#include <gxos/gxcore_os_core.h>

#define MG_CA_MAX_LEN_OSD          240
#define MG_CA_OSD_TOP              0x01  /* OSD�����ʾ����Ļ�Ϸ� */
#define MG_CA_OSD_BOTTOM           0x02  /* OSD�����ʾ����Ļ�·� */

typedef enum
{
	MG_MSG_OSD=0,
	MG_MSG_MSG,
	MG_MSG_MAX
}MG_MSG_TYPE;

typedef struct {
	uint8_t wStatus; /* 0 --����Ҫ��ʾ��1-- ��Ҫ��ʾ*/
	uint8_t roll_status; /*0 -- ��ǰ����״̬��1-- ��ǰ�ǹ���״̬*/
	uint8_t byStyle; /*osd������Ϣ��ȫ������λ������*/
	uint8_t message_type; /* 0 --osd��1-- msg*/
	int32_t wTimes;       /*�ѹ�������*/
    handle_t mutex;
	uint8_t content[MG_CA_MAX_LEN_OSD];
} mg_cas_rolling_message_st; 

typedef struct {
	int32_t status; /*1 -- show, 0 -- hide*/
} mg_cas_pre_auth_st; 

typedef struct {
	int32_t status; /*1 -- show, 0 -- hide*/
} mg_cas_auth_st; 

typedef enum 
{
	MGCA_ENTITLE_INFO_ID1 = 0,
    MGCA_ENTITLE_INFO_ID2,
}MGCA_ENTITLE_INFO_ID_E;


int32_t app_mg_cas_gxmsg_ca_on_event_exec(GxMessage * msg);
uint8_t app_mg_cas_api_is_ca_pop_msg_type(uint8_t type);
int32_t app_mg_cas_api_pop_msg_exec(uint8_t type);
int32_t app_mg_cas_api_osd_exec(void);



#ifdef __cplusplus
}
#endif
#endif /*__APP_MG_CAS_API_POP_H__*/
