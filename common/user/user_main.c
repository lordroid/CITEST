/**
  ******************************************************************************
  * @file    task_asr.c 
  * @author  Chipintelli Technology Co., Ltd.
  * @version V1.0.0
  * @date    2017.05
  * @brief
  ******************************************************************************
  **/
#include <stdint.h>
#include "user_main.h"
#include "FreeRTOS.h"
#include "ci100x_global.h"
#include "user_config.h"
#include "includes.h" 
#include "../../bsp_test/src/cxdish/cxdish.h"
#include "com2_task.h"
/**************************************************************************
                    type define
****************************************************************************/
typedef enum
{
    USERSTATE_WAIT_MSG = 0,
}USER_State_T;


struct user_manage_type
{
    uint32_t user_msg_state;
    uint32_t wakeup_voice_state;
}g_user_manage_t ;


/**************************************************************************
                    function prototype
****************************************************************************/
extern void userapp_deal_key_msg(sys_key_msg_data_t  *key_msg);
extern void userapp_deal_com_msg(sys_com_msg_data_t *com_data);
extern void Asr_Playback(int index);
extern void uart2_send_AsrResult(unsigned int index,float score);
extern void RGBChangeColor(int index);
extern void RGBDataInit(void);
#if USE_USER_UART
extern void Suart2_Init();
#endif
void userapp_deal_asr_msg(sys_asr_msg_data_t *asr_msg);
static void play_prompt(void);
void MainCallback(void *p);
void Asr_Playback(int function_index);
BaseType_t send_play_by_func_index(int32_t func_index);
/**************************************************************************
                    global 
****************************************************************************/

void Switch_Language_play_voice(void)
{
    
    //awaken = 1;
    //Swtich_Language_ASR_Play_If();
    
}

int vol_up()
{
       int IsMax = 0;
    if(nvdata_save.volset < VOLUME_MAX)
    {
        nvdata_save.volset++;
    }
    else
    {
        nvdata_save.volset = VOLUME_MAX;
        IsMax = 1;

    }
    
    vol_set(nvdata_save.volset);
    nvdata_write((char* )&nvdata_save,sizeof(nvdata_save));   
    return IsMax;

}

int vol_dn()
{
        int IsMin = 0;
    if(nvdata_save.volset > VOLUME_MIN)
    {
        nvdata_save.volset--;
    }
    else
    {
        nvdata_save.volset = VOLUME_MIN;
          IsMin = 2;

    }
    vol_set(nvdata_save.volset);
    nvdata_write((char* )&nvdata_save,sizeof(nvdata_save));   
    return IsMin;

}
int  deal_volume_control(int function_index)
{
    int ret = 0;
    int play_index = 0;

    if(function_index == VOLUME_UP_COMMAND_INDEX)
    {
        ret = vol_up();
    }
    else if(function_index == VOLUME_DOWN_COMMAND_INDEX) 
    {
        ret = vol_dn();
    }
    else if(function_index == VOLUME_MAX_COMMAND_INDEX)
    {
        nvdata_save.volset = VOLUME_MAX;
        ret = vol_up();
    }
    else if(function_index == VOLUME_MIN_COMMAND_INDEX)
    {
        nvdata_save.volset = VOLUME_MIN;
        ret = vol_dn();
    }
    else 
    {
       return -1;
    }
    
    if(ret == 1)    //max
    {
        play_index = PLAY_VOLMAX_VOICE_INDEX;
    }
    else if(ret == 2)   //min
    {
        play_index = PLAY_VOLMIN_VOICE_INDEX;
    }
    else
    {
        play_index = function_index;
    }
    mprintf("@@@@@@@@%s,%d,%d\n",__FUNCTION__,__LINE__,play_index);
    if(pdPASS!=send_play_by_func_index(play_index))
    {
        mprintf("send err %s,%d\n",__FUNCTION__,__LINE__);
    }
    return 0;
}

void GetNVUserData(void)
{
    nvdata_init();

    nvdat_read((char* )&nvdata_save,sizeof(nvdata_save));

    if(nvdata_save.inited != 0xa5)
    {//default init
        mprintf("--->first use spi,default init\r\n");
        nvdata_save.inited= 0xa5;//inited flag        
        nvdata_save.volset = VOLUME_DEFAULT;
        nvdata_save.voice_onoff = 1;    //默认打开播放
        nvdata_save.uLanguageId = SUPPORT_CHINESE_LANGUAGE;
        nvdata_write((char* )&nvdata_save,sizeof(nvdata_save));    
    }

    if((nvdata_save.volset>VOLUME_MAX)|| (nvdata_save.volset<VOLUME_MIN))
    {
        nvdata_save.volset = VOLUME_DEFAULT; 
    }
}

void  UserTaskManageProcess(void *p_arg)
{
    user_msg_t rev_msg;
    BaseType_t err = pdPASS;
    
    g_user_manage_t.user_msg_state = USERSTATE_WAIT_MSG;

    vol_set(nvdata_save.volset);

#if RGB_ENABLE
    RGBDataInit();
#endif

    while(1)
    {
        err = xQueueReceive(user_task_queue, &rev_msg, portMAX_DELAY);

        if(pdPASS == err)
        {
            switch (rev_msg.msg_type)
            {
                case MSG_TYPE_SYS_ASR:
                {
                    sys_asr_msg_data_t *asr_rev_data;
                    asr_rev_data = &(rev_msg.msg_data.asr_data);
                    userapp_deal_asr_msg(asr_rev_data);
                    break;
                }
                case MSG_TYPE_SYS_KEY:
                {
                    sys_key_msg_data_t *key_rev_data;
                    key_rev_data = &rev_msg.msg_data;
                    userapp_deal_key_msg(key_rev_data);
                    break;
                }
                #if USE_USER_UART
                case MSG_TYPE_SYS_COM:
                {
                    sys_com_msg_data_t *com_rev_data;
                    com_rev_data = &rev_msg.msg_data;
                    userapp_deal_com_msg(com_rev_data);
                    break;
                }
                #endif
                default:
                    break;
            }
        
        }
        else
        {

        }
    }
}

#if USE_COMBO_CMD
int combo_combo_cmd(int index)
{
    int combo_index=index;

    if(combo_cmd_result.first_word_valid==COMBO_CMD_FIRST_WORD_VALID)
    {
        combo_cmd_result.first_word_valid = COMBO_CMD_FIRST_WORD_INVALID;
        if(pdMS_TO_TICKS(xTaskGetTickCount()-combo_cmd_result.first_word_valid_tick)
            <=COMBO_CMD_VALID_TIME_IN_MS)
        {
            //组合播报有效，返回对应的index值，自行处理
            combo_index = index+1;
        }else
        {
            mprintf("combo cmd timeout.\n");
        }
    }
    return combo_index;
}
#endif

void userapp_deal_asr_msg(sys_asr_msg_data_t *asr_msg)
{
    int32_t index;
    int32_t function_index;
    if(MSG_ASR_STATUS_GOOD_RESULT == asr_msg->asr_status)
    {
        index = asr_msg->asr_index;
#if USE_COMBO_CMD
        index = combo_combo_cmd(index);
#endif
        function_index = AsrCmd_Struct[index].id;

        if(function_index >= 0)
        {
            Asr_Playback(function_index);
        }  

#if RGB_ENABLE
        if(function_index >= 0 && awaken)
        {
            RGBChangeColor(function_index);
        }
#endif

        #if USE_USER_UART
        if(awaken)
            uart2_send_AsrResult(function_index,asr_msg->asr_score);
        #endif
    }

}


void userapp_initial(void)
{
    #if CX_20921_UPDATA    
    cxdish_init();
    #endif    

    #if CPU_RATE_PRINT
    init_timer3_getresource();
    #endif

    #if USE_USER_UART
    Suart2_Init();
    #endif
}


/**
  * @功能: 退出识别模式，播放提示音
  * @注意: 无
  * @参数: 无
  * @返回值: 无
  */
static void play_prompt(void)
{
    int index;
    index = 0;
    
    if(nvdata_save.voice_onoff  ==1)
    {
        xQueueSend(play_Q, &index,200);
    }
    
    #if USE_USER_UART// quit awake state
    user_msg_t send_msg;
    memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
    send_msg.msg_type = MSG_TYPE_SYS_COM;
    send_msg.msg_data.com_data.header0 = UART_HEADER0;
    send_msg.msg_data.com_data.header1 = UART_HEADER1;    
    send_msg.msg_data.com_data.id = 0;
    send_msg.msg_data.com_data.cmd= CI_TX_UNWAKEN;
    send_msg.msg_data.com_data.data0= 0x01;  
    send_msg.msg_data.com_data.data1= 0;//score_conver;
    send_msg.msg_data.com_data.end= UART_END;
    
    xQueueSend(user_task_queue,&send_msg,200);
    #endif
}


/**
  * @功能: 软timer 回调函数 
  * @注意: 无
  * @参数: 无
  * @返回值: 无
  */
void MainCallback(void)
{   
    if(awaken)
    {
        awaken = 0;
        extern void start_lowpower_cout(void);
        start_lowpower_cout();

    #if USE_SEPARATE_WAKEUP
        int ret = asrSwitchWordsFst(handle,1);
        if(1 == ret)
        {
            mprintf("switch to wakeup\n");
        }
    #endif
        mprintf("go away from wakeup\n");
        play_prompt();  
        
    }
}
extern int get_PlayVoice_EndTime(void);
extern void set_PlayVoice_time(void);
extern void Reset_PlayVoice_EndTime(void);

void Check_Asr_Enter_Sleep(void)
{
    int dLasttime=get_PlayVoice_EndTime();
    int dCurrentTime = xTaskGetTickCount();
     if(awaken&&(dLasttime!=-1))
     {
        if((dCurrentTime - dLasttime)>pdMS_TO_TICKS(SLEEP_TIME))
        {
            if(1 == asr_decode_busy)
            {
                if((dCurrentTime - dLasttime)>pdMS_TO_TICKS(SLEEP_TIME+800))
                {
                    MainCallback();
                }
            }
            else
            {
                MainCallback();
            }
        }
     }
    

}


/**
* @功能: 播放对应命令词的语音
  * @注意: 无
  * @参数: 无
  * @返回值: 无
  */
void Asr_Playback(int function_index)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);
    set_PlayVoice_time();
    if((function_index == FUNC_ID_WAKE_WORD) && (!awaken))
    {
        
        awaken = 1;
        mprintf("进入识别模式...\n");
        send_play_by_func_index(function_index);
    }
    else
    {
        if(1==awaken)
        {        
            if((nvdata_save.voice_onoff ==0)&&(function_index == VOLUME_OPEN_COMMAND_INDEX))
            {
                nvdata_save.voice_onoff  = 1;

            }
            else if((function_index == VOLUME_CLOSE_COMMAND_INDEX))
            {
                if(nvdata_save.voice_onoff  == 1)
                {
                    if(pdPASS != send_play_by_func_index(function_index))
                    {
                        mprintf("send err %s,%d\n",__FUNCTION__,__LINE__);
                    }
                }
                nvdata_save.voice_onoff  = 0;
            }
            
            if(nvdata_save.voice_onoff  == 1)
            {
                if(deal_volume_control(function_index) == -1)
                {
                    if(pdPASS != send_play_by_func_index(function_index))
                    {
                        mprintf("send err %s,%d\n",__FUNCTION__,__LINE__);
                    }
                }
                
            }
            else
            {
            //Play Wakeup Word even in silence mode
                if(function_index == FUNC_ID_WAKE_WORD)
                {
                    if(pdPASS != send_play_by_func_index(function_index))
                    {
                        mprintf("send err %s,%d\n",__FUNCTION__,__LINE__);
                    }                    
                }
            }    
        }
        else
        {
            mprintf("如需进入识别模式,请说唤醒词\n");
        }
    }
    xSemaphoreGive(WakeupMutex);
}

int32_t GetIndexbyAsrIndex(int32_t index)
{
    int i = 0;

    while(1)
    {
        if(AsrCmd_Struct[i].id == -1)
        {
            break;
        }
        
        if(AsrCmd_Struct[i].id == index)
        {
            return i;
        }
        
        i++;
    }

    return -1;
}

BaseType_t send_play_by_func_index(int32_t func_index)
{
    int index;
    BaseType_t ret;
    
    index = GetIndexbyAsrIndex(func_index);
    ret = xQueueSend(play_Q, &index,200);
    return ret;
}
