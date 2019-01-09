/**
  ******************************************************************************
  * @file    task_asr.c 
  * @author  Chipintelli Technology Co., Ltd.
  * @version V1.0.0
  * @date    2017.05
  * @brief  
  ******************************************************************************
  **/

#ifndef _USER_MAIN_H_
#define _USER_MAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define key1evt 1
#define key2evt 2
#define key3evt 4

#define key12evt (key1evt|key2evt)
#define key13evt (key1evt|key3evt)
#define key23evt (key2evt|key3evt)

#define keyhold (1<<6)
#define keyrelease (1<<5)
#define keepholding (1<<4)

typedef enum
{
    MSG_TYPE_SYS_ASR = 0,
    MSG_TYPE_SYS_KEY,
    MSG_TYPE_SYS_COM,
}user_msg_type_t;

typedef enum
{
    MSG_ASR_STATUS_GOOD_RESULT = 0,
}user_asr_msg_status_t;

typedef struct
{
    uint32_t asr_status;
    uint32_t asr_index;
    float asr_score;
}sys_asr_msg_data_t;

typedef struct
{
    uint32_t key;
}sys_key_msg_data_t;

typedef struct
{
    unsigned char header0;//header
    unsigned char header1;
    unsigned char id;
    unsigned char cmd;
    unsigned char data0;
    unsigned char data1;
    unsigned char chksum;//check sum= add "header0~dta1"
    unsigned char end;	
}sys_com_msg_data_t;


typedef struct
{
    uint32_t msg_type;/*here will be modify use union*/
    union
    {
        sys_asr_msg_data_t asr_data;
        sys_key_msg_data_t key_data;
        sys_com_msg_data_t com_data;
    }msg_data;
}user_msg_t;

typedef struct
{
    int duty;
    int period;
}light;

typedef struct
{
    light r;
    light g;
    light b;
}color;

void userapp_initial(void);

#ifdef __cplusplus
}
#endif

#endif
