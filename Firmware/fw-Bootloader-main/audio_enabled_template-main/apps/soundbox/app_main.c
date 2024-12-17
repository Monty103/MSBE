#include "system/includes.h"
#include "app_config.h"
#include "asm/pwm_led.h"
#include "tone_player.h"
#include "ui_manage.h"
#include "app_main.h"
#include "app_task.h"
#include "asm/charge.h"
#include "app_power_manage.h"
#include "app_charge.h"
#include "user_cfg.h"
#include "power_on.h"
#include "bt.h"
#include "soundcard/peripheral.h"
#include "audio.h"
#include "vm.h"
#define delay_ms(n) delay(n*2000)
#define delay_us(n) delay(n*2)

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



APP_VAR app_var;


void app_entry_idle()
{
    app_task_switch_to(APP_IDLE_TASK);
}

u8 a=0;

void app_task_loop()
{
    while (1) {
        /*switch (app_curr_task) {
        case APP_POWERON_TASK:
            log_info("APP_POWERON_TASK \n");
            app_poweron_task();
            break;
        case APP_POWEROFF_TASK:
            log_info("APP_POWEROFF_TASK \n");
            app_poweroff_task();
            break;
        case APP_BT_TASK:
            log_info("APP_BT_TASK \n");
            app_bt_task();
            break;
        case APP_MUSIC_TASK:
            log_info("APP_MUSIC_TASK \n");
            app_music_task();
            break;
        case APP_FM_TASK:
            log_info("APP_FM_TASK \n");
            app_fm_task();
            break;
        case APP_RECORD_TASK:
            log_info("APP_RECORD_TASK \n");
            app_record_task();
            break;
        case APP_LINEIN_TASK:
            log_info("APP_LINEIN_TASK \n");
            app_linein_task();
            break;
        case APP_RTC_TASK:
            log_info("APP_RTC_TASK \n");
            app_rtc_task();
            break;
        case APP_PC_TASK:
            log_info("APP_PC_TASK \n");
            app_pc_task();
            break;
        case APP_SPDIF_TASK:
            log_info("APP_SPDIF_TASK \n");
            app_spdif_task();
            break;
        case APP_IDLE_TASK:
            log_info("APP_IDLE_TASK \n");
            app_idle_task();
            break;
        case APP_SLEEP_TASK:
            log_info("APP_SLEEP_TASK \n");
            app_sleep_task();
            break;
        case APP_SMARTBOX_ACTION_TASK:
            log_info("APP_SMARTBOX_ACTION_TASK \n");
            app_smartbox_task();
            break;
        }
        app_task_clear_key_msg();//清理按键消息
        //检查整理VM
        vm_check_all(0);*/

        /*app_pc_task();*/
        /*   hid_key_handler(0, BIT(1));
           delay_ms(1000);
           hid_key_handler(0, BIT(0));*/
           u8 buffer[20];
           memset(buffer, 0x00, 20);
           buffer[1] = 0x14;
           if(a==0)
           {
             buffer[2] = 0x40;
             a=1;
           }
           else
           {
                buffer[2] = 0x00;
               a=0;
           }

           usb_g_intr_write(0, 0x01, buffer, 20);
           //usb_g_intr_write(0, 0x03, buffer, 20);
           /* usb_g_intr_write(0, 0x02, buffer, 20);
             usb_g_intr_write(0, 0x03, buffer, 20);
              usb_g_intr_write(0, 0x04, buffer, 20);*/
           delay_ms(4);
           log_info("APP_LOOP \n");

            wdt_clear();
    }
}

void app_main()
{
    log_info("app_main\n");

   // app_curr_task = APP_PC_TASK;
    usb_start();
    app_task_loop();
}



