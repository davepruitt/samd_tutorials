/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef USB_DEVICE_MAIN_H
#define USB_DEVICE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "cdcdf_acm.h"
#include "cdcdf_acm_desc.h"

void cdcd_acm_example(void);
void cdc_device_acm_init(void);

void usb_serial_begin (void);

int32_t usb_serial_bytes_available (void);

int32_t usb_serial_read (char *const input_buffer, const uint16_t input_buffer_length);

int32_t usb_serial_write (const char *const output_buffer, const uint16_t output_buffer_length);

/**
 * \berif Initialize USB
 */
void usb_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USB_DEVICE_MAIN_H
