/*
 * nm_bsp_samd51.c
 *
 * Created: 4/20/2022 3:43:19 PM
 *  Author: David Pruitt
 */ 

#include "bsp/include/nm_bsp.h"
#include "bsp/include/nm_bsp_samd51.h"
#include "common/include/nm_common.h"
#include "atmel_start.h"
#include "winc_init.h"

#ifndef CONF_WINC_EXT_INT_PIN
#define CONF_WINC_EXT_INT_PIN PA02
#endif

static tpfNmBspIsr gpfIsr;

static void chip_isr(void)
{
	if (gpfIsr) {
		gpfIsr();
	}
}

sint8 nm_bsp_init(void)
{
	gpfIsr = NULL;

	/* Make sure a 1ms Systick is configured. */
	if (!(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk && SysTick->CTRL & SysTick_CTRL_TICKINT_Msk)) {
		delay_init(SysTick);
	}
	
	return M2M_SUCCESS;
}

sint8 nm_bsp_deinit(void)
{
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, false);
	gpio_set_pin_level(CONF_WINC_PIN_RESET, false);
	return M2M_SUCCESS;
}

void nm_bsp_reset(void)
{
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, false);
	gpio_set_pin_level(CONF_WINC_PIN_RESET, false);
	nm_bsp_sleep(1);
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, true);
	nm_bsp_sleep(5);
	gpio_set_pin_level(CONF_WINC_PIN_RESET, true);
}

void nm_bsp_sleep(uint32 u32TimeMsec)
{
	while (u32TimeMsec--) 
	{
		delay_ms(1);
	}
}

void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
	gpfIsr = pfIsr;

	ext_irq_register(CONF_WINC_EXT_INT_PIN, chip_isr);
}

void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
	_ext_irq_enable(CONF_WINC_EXT_INT_PIN, u8Enable);
}
