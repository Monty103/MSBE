#include "device/key_driver.h"
#include "system/event.h"
#include "system/init.h"
#include "iokey.h"
#include "adkey.h"
#include "slidekey.h"
#include "irkey.h"
#include "touch_key.h"
#include "system/timer.h"
#include "asm/power_interface.h"
#include "app_config.h"
#include "rdec_key.h"
#if TCFG_KEY_TONE_EN
#include "tone_player.h"
#endif

#if(TCFG_IRSENSOR_ENABLE == 1)
#include "irSensor/ir_manage.h"
#endif

#define LOG_TAG_CONST       KEY
#define LOG_TAG             "[KEY]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
//#define LOG_DUMP_ENABLE
//#define LOG_CLI_ENABLE
#include "debug.h"

#define KEY_EVENT_CLICK_ONLY_SUPPORT	1 	//是否支持某些按键只响应单击事件


#if TCFG_SPI_LCD_ENABLE
#ifndef ALL_KEY_EVENT_CLICK_ONLY
#define ALL_KEY_EVENT_CLICK_ONLY	1 	//是否全部按键只响应单击事件
#endif
#else
#ifndef ALL_KEY_EVENT_CLICK_ONLY
#define ALL_KEY_EVENT_CLICK_ONLY	0 	//是否全部按键只响应单击事件
#endif
#endif

static volatile u8 is_key_active = 0;


extern u32 timer_get_ms(void);

//=======================================================//
// 按键值重新映射函数:
// 用户可以实现该函数把一些按键值重新映射, 可用于组合键的键值重新映射
//=======================================================//
int __attribute__((weak)) key_event_remap(struct sys_event *e)
{
    return true;
}

//=======================================================//
// 按键扫描函数: 扫描所有注册的按键驱动
//=======================================================//
static void key_driver_scan(void *_scan_para)
{
    struct key_driver_para *scan_para = (struct key_driver_para *)_scan_para;

    u8 key_event = 0;
    u8 cur_key_value = NO_KEY;
    u8 key_value = 0;
    struct sys_event e;
    static u8 poweron_cnt = 0;


    cur_key_value = scan_para->get_value();
     if (cur_key_value != NO_KEY) {
     //    printf(">>>cur_key_value: %d\n", cur_key_value);
     }


    return;
}


//wakeup callback
void key_active_set(u8 port)
{
    is_key_active = 35;      //35*10Ms
}

//=======================================================//
// 按键初始化函数: 初始化所有注册的按键驱动
//=======================================================//
int key_driver_init(void)
{
    int err;

    printf("KEY DRIVER INIT");
#if TCFG_IOKEY_ENABLE
    extern const struct iokey_platform_data iokey_data;
    extern struct key_driver_para iokey_scan_para;
    err = iokey_init(&iokey_data);
#ifdef TCFG_IOKEY_TIME_REDEFINE
    extern struct key_driver_para iokey_scan_user_para;
    if (err == 0) {
        sys_s_hi_timer_add((void *)&iokey_scan_user_para, key_driver_scan, iokey_scan_user_para.scan_time); //注册按键扫描定时器
    }
#else
    if (err == 0) {
        sys_s_hi_timer_add((void *)&iokey_scan_para, key_driver_scan, iokey_scan_para.scan_time); //注册按键扫描定时器
    }
#endif
#endif

#if TCFG_ADKEY_ENABLE
    extern const struct adkey_platform_data adkey_data;
    extern struct key_driver_para adkey_scan_para;
    err = adkey_init(&adkey_data);
    if (err == 0) {
    printf("KEY DRIVER AD INIT");
        sys_s_hi_timer_add((void *)&adkey_scan_para, key_driver_scan, adkey_scan_para.scan_time); //注册按键扫描定时器
    }
#endif

#if TCFG_IRKEY_ENABLE
    extern const struct irkey_platform_data irkey_data;
    extern struct key_driver_para irkey_scan_para;
    err = irkey_init(&irkey_data);
    if (err == 0) {
        sys_s_hi_timer_add((void *)&irkey_scan_para, key_driver_scan, irkey_scan_para.scan_time); //注册按键扫描定时器
    }
#endif

#if TCFG_TOUCH_KEY_ENABLE
    extern const struct touch_key_platform_data touch_key_data;
    extern struct key_driver_para touch_key_scan_para;
    err = touch_key_init(&touch_key_data);
    if (err == 0) {
        sys_s_hi_timer_add((void *)&touch_key_scan_para, key_driver_scan, touch_key_scan_para.scan_time); //注册按键扫描定时器
    }
#endif

#if TCFG_ADKEY_RTCVDD_ENABLE
    extern const struct adkey_rtcvdd_platform_data adkey_rtcvdd_data;
    extern struct key_driver_para adkey_rtcvdd_scan_para;
    err = adkey_rtcvdd_init(&adkey_rtcvdd_data);
    if (err == 0) {
        sys_s_hi_timer_add((void *)&adkey_rtcvdd_scan_para, key_driver_scan, adkey_rtcvdd_scan_para.scan_time); //注册按键扫描定时器
    }
#endif

#if TCFG_RDEC_KEY_ENABLE
    extern const struct rdec_platform_data rdec_key_data;
    extern struct key_driver_para rdec_key_scan_para;
    err = rdec_key_init(&rdec_key_data);
    if (err == 0) {
        sys_s_hi_timer_add((void *)&rdec_key_scan_para, key_driver_scan, rdec_key_scan_para.scan_time); //注册按键扫描定时器
    }
#endif

#if TCFG_CTMU_TOUCH_KEY_ENABLE
#include "asm/ctmu.h"
    extern const struct ctmu_touch_key_platform_data ctmu_touch_key_data;
    extern struct key_driver_para ctmu_touch_key_scan_para;
    extern int ctmu_touch_key_init(const struct ctmu_touch_key_platform_data * ctmu_touch_key_data);
    err = ctmu_touch_key_init(&ctmu_touch_key_data);
    if (err == 0) {
        sys_s_hi_timer_add((void *)&ctmu_touch_key_scan_para, key_driver_scan, ctmu_touch_key_scan_para.scan_time); //注册按键扫描定时器
    }
#endif /* #if TCFG_CTMU_TOUCH_KEY_ENABLE */

#if TCFG_SLIDE_KEY_ENABLE
    extern const struct slidekey_platform_data slidekey_data;
    extern int slidekey_init(const struct slidekey_platform_data * slidekey_data);
    err = slidekey_init(&slidekey_data);
    if (err == 0) {
    }
#endif//TCFG_SLIDE_KEY_ENABLE

#if TCFG_6083_ADKEY_ENABLE
    extern int adkey_init_6083();
    err = adkey_init_6083();
    if (err == 0) {
    }
#endif
    return 0;
}

static u8 key_idle_query(void)
{
    return !is_key_active;
}
#if !TCFG_LP_TOUCH_KEY_ENABLE
REGISTER_LP_TARGET(key_lp_target) = {
    .name = "key",
    .is_idle = key_idle_query,
};
#endif /* #if !TCFG_LP_TOUCH_KEY_ENABLE */

