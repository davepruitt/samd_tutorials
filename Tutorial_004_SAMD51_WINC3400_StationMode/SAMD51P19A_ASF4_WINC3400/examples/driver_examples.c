/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void button_on_PA02_pressed(void)
{
}

/**
 * Example of using EXTIRQ_INSTANCE
 */
void EXTIRQ_INSTANCE_example(void)
{

	ext_irq_register(PIN_PA02, button_on_PA02_pressed);
}

/**
 * Example of using SPI_INSTANCE to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_INSTANCE[12] = "Hello World!";

void SPI_INSTANCE_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_INSTANCE, &io);

	spi_m_sync_enable(&SPI_INSTANCE);
	io_write(io, example_SPI_INSTANCE, 12);
}

void delay_example(void)
{
	delay_ms(5000);
}
