#include "system/includes.h"
#include "app_main.h"
#include "asm/gpio.h"
#include "asm/spi.h"
#include "key_driver.h"



#include "app_task.h"
#include "debug.h"
#define LOG_TAG "DEBUG"


APP_VAR app_var;

void app_entry_idle()
{
    app_task_switch_to(APP_IDLE_TASK);
}

int16_t dac_buff_x[2048];

int16_t bafer[882];

void dac_timer(void *priv)
{
    if(audio_output_buf_time() < 21)
    {
        app_audio_output_write( bafer, 882*2);
    }
}

void app_main()
{
    clk_set("sys", 240 * 1000000L);
    //log_info("app_xmain1\n");
    wdt_disable();
    dac_init(48000);

    //key_driver_init();

    app_audio_output_init();

    app_audio_output_samplerate_set(44100);

    app_audio_output_start();

    int16_t b = -16383;
    for(int i=0; i<882; i++)
    {
        bafer[i] = b;

        if(i%21 == 0)
        {

            if(b==16383)
            {
                b = -16383;
            }
            else
            {
                b = 16383;
            }
        }
    }


    audio_dac_vol_set(0x01, BIT(0), 30, 1);
    audio_dac_vol_set(0x01, BIT(1), 0, 1);
    audio_dac_vol_set(0x02, BIT(0) | BIT(1), 16384, 1);

    app_audio_output_write( dac_buff_x, 1024*2);

    sys_s_hi_timer_add(NULL, dac_timer, 10);

    while(1)
    {

    }

}
