/**
  ******************************************************************************
  * @文件    ci100x_asrcmd.c
  * @作者    chipintelli软件团队
  * @版本    V1.0.0
  * @日期    2016-4-9
  * @概要    这个文件是chipintelli公司的CI100X芯片程序的命令词处理文件.
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */ 

#include "ci100x_asrcmd.h"
  
#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief Struct of asr cmd. Include cmd id and cmd and threshold score.
 */
AsrCmd_TypeDef AsrCmd_Struct[]=
{
{0XFFFE,"<Reserved>",},//我先离开稍后回来
{1,"智能管家",WAKEUP_SCORE_THRESHOLD},
{2,"打开空调",SCORE_THRESHOLD},
{3,"关闭空调",SCORE_THRESHOLD},
{4,"增大风速",SCORE_THRESHOLD},
{5,"减小风速",SCORE_THRESHOLD},
{6,"升高一度",SCORE_THRESHOLD},
{7,"降低一度",SCORE_THRESHOLD},
{8,"全自动",SCORE_THRESHOLD},
{9,"制热模式",SCORE_THRESHOLD},
{10,"制冷模式",SCORE_THRESHOLD},
{11,"送风模式",SCORE_THRESHOLD},
{12,"节能模式",SCORE_THRESHOLD},
{13,"关闭节能模式",SCORE_THRESHOLD},
{14,"除湿模式",SCORE_THRESHOLD},
{15,"关闭除湿",SCORE_THRESHOLD},
{16,"电加热",SCORE_THRESHOLD},
{17,"关闭电加热",SCORE_THRESHOLD},
{18,"空气清新",SCORE_THRESHOLD},
{19,"空气净化",SCORE_THRESHOLD},
{20,"关闭空气清新",SCORE_THRESHOLD},
{21,"睡眠模式",SCORE_THRESHOLD},
{22,"关闭睡眠模式",SCORE_THRESHOLD},
{23,"定时一小时",SCORE_THRESHOLD},
{24,"定时两小时",SCORE_THRESHOLD},
{24,"定时二小时",SCORE_THRESHOLD},
{25,"左右摆动",SCORE_THRESHOLD},
{26,"上下摆动",SCORE_THRESHOLD},
{27,"停止摆动",SCORE_THRESHOLD},
{28,"最小风",SCORE_THRESHOLD},
{29,"中等风",SCORE_THRESHOLD},
{30,"高速风",SCORE_THRESHOLD},
{31,"最大风",SCORE_THRESHOLD},
{32,"强劲风",SCORE_THRESHOLD},
{33,"风小点",SCORE_THRESHOLD},
{34,"风大点",SCORE_THRESHOLD},
{35,"十六度",SCORE_THRESHOLD},
{36,"十七度",SCORE_THRESHOLD},
{37,"十八度",SCORE_THRESHOLD},
{38,"十九度",SCORE_THRESHOLD},
{39,"二十度",SCORE_THRESHOLD},
{40,"二十一度",SCORE_THRESHOLD},
{41,"二十二度",SCORE_THRESHOLD},
{42,"二十三度",SCORE_THRESHOLD},
{43,"二十四度",SCORE_THRESHOLD},
{44,"二十五度",SCORE_THRESHOLD},
{45,"二十六度",SCORE_THRESHOLD},
{46,"二十七度",SCORE_THRESHOLD},
{47,"二十八度",SCORE_THRESHOLD},
{48,"二十九度",SCORE_THRESHOLD},
{49,"三十度",SCORE_THRESHOLD},
{50,"打开台灯",SCORE_THRESHOLD},
{51,"关闭台灯",SCORE_THRESHOLD},
{52,"最高亮度",SCORE_THRESHOLD},
{53,"中等亮度",SCORE_THRESHOLD},
{54,"最低亮度",SCORE_THRESHOLD},
{55,"调亮一点",SCORE_THRESHOLD},
{56,"调暗一点",SCORE_THRESHOLD},
{57,"照明模式",SCORE_THRESHOLD},
{58,"阅读模式",SCORE_THRESHOLD},
{59,"夜灯模式",SCORE_THRESHOLD},
{60,"红色模式",SCORE_THRESHOLD},
{61,"绿色模式",SCORE_THRESHOLD},
{62,"蓝色模式",SCORE_THRESHOLD},
{63,"彩色模式",SCORE_THRESHOLD},
{64,"打开主卧灯",SCORE_THRESHOLD},
{65,"关闭主卧灯",SCORE_THRESHOLD},
{66,"打开花园灯",SCORE_THRESHOLD},
{67,"关闭花园灯",SCORE_THRESHOLD},
{68,"打开客房灯",SCORE_THRESHOLD},
{69,"关闭客房灯",SCORE_THRESHOLD},
{70,"打开餐厅灯",SCORE_THRESHOLD},
{71,"关闭餐厅灯",SCORE_THRESHOLD},
{72,"打开厕所灯",SCORE_THRESHOLD},
{73,"关闭厕所灯",SCORE_THRESHOLD},
{74,"打开花园灯",SCORE_THRESHOLD},
{75,"关闭花园灯",SCORE_THRESHOLD},
{76,"打开阳台灯",SCORE_THRESHOLD},
{77,"关闭阳台灯",SCORE_THRESHOLD},
{78,"打开书房灯",SCORE_THRESHOLD},
{79,"关闭书房灯",SCORE_THRESHOLD},
{80,"大声点",SCORE_THRESHOLD},
{80,"音量增大",SCORE_THRESHOLD},
{80,"增大音量",SCORE_THRESHOLD},
{80,"大点声",SCORE_THRESHOLD},
{81,"小声点",SCORE_THRESHOLD},
{81,"音量减小",SCORE_THRESHOLD},
{81,"减小音量",SCORE_THRESHOLD},
{81,"小点声",SCORE_THRESHOLD},
{82,"最大声",SCORE_THRESHOLD},
{82,"音量最大",SCORE_THRESHOLD},
{82,"最大音量",SCORE_THRESHOLD},
{83,"最小声",SCORE_THRESHOLD},
{83,"音量最小",SCORE_THRESHOLD},
{83,"最小音量",SCORE_THRESHOLD},
{84,"打开语音",SCORE_THRESHOLD},
{84,"开启语音",SCORE_THRESHOLD},
{84,"启动语音",SCORE_THRESHOLD},
{85,"关闭语音",SCORE_THRESHOLD},
{85,"停止语音",SCORE_THRESHOLD},
{85,"结束语音",SCORE_THRESHOLD},
{200,"welcom"},//该词条为开机提示音对应的词条
{-1,}/*EOF 结束标识*/
};

const AsrSpecial_TypeDef specialwords_lst[] = 
{
  {20,"风小点"},
  {20,"关闭空调"},
  {20,"关闭台灯"},
  {20,"关闭除湿"},
  {20,"风大点"},
  {20,"最大风"},
  {20,"最小风"},
  {20,"中等风"},
  {-1,}
};


/**
 *@brief Wakeup words list. Be Used when USE_SEPARATE_WAKEUP is setted to 1.
 */
const char * wakewords_lst[] =
{
    "智能管家",
    "END"
};

/**
 *@brief Combo cmd first word list. Be used when USE_COMBO_CMD is setted to 1.
 */
#if USE_COMBO_CMD
const char * combo_cmd_first_word_list[] =
{
    "END"
};
#endif

#ifdef __cplusplus
}
#endif
/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
