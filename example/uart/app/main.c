/********************************************************************************************************
 * @file     main.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "tl_common.h"
#include "drivers.h"

extern void app_uart_irq_proc(void);
extern void app_uart_init(void);
extern void app_uart_loop(void);



/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

void main_loop ()
{
	gpio_write(PWM_B, 1); 

    app_uart_loop();

    gpio_write(PWM_B, 0); 

}

// call interrupt process 
_attribute_ram_code_ void irq_handler(void)
{
	app_uart_irq_proc();
}

void user_init()
{
    app_uart_init();
    
    gpio_set_func(PWM_B, AS_GPIO);
	gpio_set_output_en(PWM_B, 1);
	gpio_set_input_en(PWM_B, 0); 
	
}

void system_init()
{
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init(!deepRetWakeUp);


    #if (CLOCK_SYS_CLOCK_HZ == 16000000)
        clock_init(SYS_CLK_16M_Crystal);
    #elif (CLOCK_SYS_CLOCK_HZ == 24000000)
        clock_init(SYS_CLK_24M_Crystal);
    #endif
        if(!deepRetWakeUp)
        {
            random_generator_init();
        }
}

int main (void)    //must run in ramcode
{
	system_init();

	user_init();

    irq_enable();

	while (1) 
	{
        #if (MODULE_WATCHDOG_ENABLE)
                wd_clear(); //clear watch dog
        #endif
                main_loop ();
            }
}


