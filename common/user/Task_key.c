#include "includes.h"
#include "../../bsp_test/src/cxdish/cxdish.h"
#include "smt_adc.h"
#include "task_asr.h"
#include "user_main.h"


extern int user_code_updata_fromTF();
extern int cx2092x_upgrade(void);

extern void vol_up();
extern void  vol_dn();
void init_timer3_getresource(void);
void cpu_rate_print(void);
//extern void ASR_VAD_DNN_Init(void );


void start_updata()
{
    NVIC_DisableIRQ(VAD_IRQn);

    if((run_mode == 0)||(run_mode == 2))
    {
        xSemaphoreGive(xRecStopSemaphore);
    }

    IIS_EN(IIS0,DISABLE);
    //Scu_SetDeviceGate((unsigned int)IIS0,DISABLE);//有时语音采集可能会测试0x7050000~0x70600000 的ram，升级时关闭采音
    timer_stop(TIMER1);//
}


void end_updata()
{		
    ASR_VAD_DNN_Init(0);   

    IIS_EN(IIS0,ENABLE);
    //Scu_SetDeviceGate((unsigned int)IIS0,ENABLE);//有时语音采集可能会测试0x7050000~0x70600000 的ram，升级时关闭采音
    TIMx_us(TIMER1,SUSPEND_TIME);
}


void userapp_deal_key_msg(sys_key_msg_data_t  *key_msg)
{
    uint32_t key;
    uint32_t index;
    int32_t ver[4];
    (void)ver;
    
    key = key_msg->key;
    mprintf("recivce key 0x%x\n", key);
    switch(key)
    {
        case (KEY_SW2|C_KEY_HOLD):
            #if CX_20921_UPDATA 
            start_updata();
            vTaskDelay(pdMS_TO_TICKS(20));
            // cx2092x_upgrade();
            if(0 > cx2092x_upgrade())
            {
                index = sysinit_index+1;
                xQueueSend(play_Q, &index,200);
            }
            end_updata();
            #endif                    
            break;
        case (KEY_SW3|C_KEY_HOLD):
            start_updata();
            vTaskDelay(pdMS_TO_TICKS(20));
            if(0 > user_code_updata_fromTF())
            {
                index = sysinit_index+1;
                xQueueSend(play_Q, &index,200);
            }
            end_updata();
            break;
            /*	case KEY_SW3:
            //set_mode("ZMP8");
            sound_comm_start = 1;
            break;
            case KEY_SW4:
            sound_comm_start = 0;
            //set_mode("ZSW2");
            break;*/
        case KEY_SW5:
            #if CX_20921_UPDATA                  
            get_fw_version();
            getfwver(ver);
            get_mode();
            #endif                                            
            break;
        case KEY_SW6:
            vol_up();
            index = 0;
            xQueueSend(play_Q, &index,200);
            break;
        case KEY_SW7:
            vol_dn();
            index = 0;
            xQueueSend(play_Q, &index,200);
            break;
        case KEY_SW1:
            #if CPU_RATE_PRINT
            cpu_rate_print();
            #endif
            break;
        default:
            break;
    }
    mprintf("process key ok!\n");
}


#if CPU_RATE_PRINT
extern volatile unsigned int ulHighFrequencyTimerTicks;
char pbuf_cpu_print[400]={0};


void init_timer3_getresource(void)
{
    ulHighFrequencyTimerTicks = 0;
    Scu_SetDeviceGate((unsigned int)TIMER3 ,ENABLE);
    TIMx_us(TIMER3,200);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = TIMER3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void cpu_rate_print(void)
{
    mprintf("\n任务名\t任务状态\t优先级\t剩余栈\t任务序号\n");
    vTaskList((char*)pbuf_cpu_print);
    mprintf("%s\n",pbuf_cpu_print);
    mprintf("\n任务名\t\t运行计数\t使用率\n");
    vTaskGetRunTimeStats((char*)pbuf_cpu_print);
    mprintf("%s\n",pbuf_cpu_print);   
}
#endif    

