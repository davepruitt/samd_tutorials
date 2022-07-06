/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define USBD_CDC_BUFFER_SIZE CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define USBD_CDC_BUFFER_SIZE CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

/** Buffers to receive and echo the communication bytes. */
COMPILER_ALIGNED(4) uint8_t usbd_cdc_buffer[USBD_CDC_BUFFER_SIZE];

static uint8_t output_buffer[USBD_CDC_BUFFER_SIZE];
static uint32_t output_length = 0;

static uint8_t input_buffer[USBD_CDC_BUFFER_SIZE];
static uint32_t input_length = 0;

volatile bool pending_read = false;
volatile bool pending_write = false;
volatile bool cdc_connected = false;

static int32_t cdc_read_start()
{
    pending_read = true;
    return cdcdf_acm_read(usbd_cdc_buffer, USBD_CDC_BUFFER_SIZE);
}

/**
 * \brief Callback invoked when bulk OUT data received
 */
static bool cdc_read_finished(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
    bool result = false;

    if (rc == USB_XFER_DONE)
    {
        pending_read = false;
        uint8_t input_buffer_bytes_remaining = (uint8_t) (USBD_CDC_BUFFER_SIZE - input_length);

        volatile hal_atomic_t flags;
        atomic_enter_critical(&flags);

        if (count <= input_buffer_bytes_remaining)
        {
            for (uint16_t i = 0; i < count; i++)
            {
                uint8_t c = usbd_cdc_buffer[i];
                input_buffer[input_length] = c;
                input_length++;
                if (input_length == USBD_CDC_BUFFER_SIZE)
                {
                    break;
                }
            }
        }
        else
        {
            result = true;
        }

        memset(usbd_cdc_buffer, 0, USBD_CDC_BUFFER_SIZE);

        atomic_leave_critical(&flags);

        cdc_read_start();
    }

	/* No error. */
	return result;
}

/**
 * \brief Callback invoked when bulk IN data received
 */
static bool cdc_write_finished(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
    pending_write = false;

	/* No error. */
	return false;
}

static int32_t cdc_write (const char *const buf, const uint16_t length)
{
    for (int i = 0; i < length && cdc_connected; i++)
    {
        char p = buf[i];
		output_buffer[output_length++] = p;
		
        if ((p == '\n') || (output_length == USBD_CDC_BUFFER_SIZE))
        {
            pending_write = true;
            cdcdf_acm_write(output_buffer, output_length);

            while(pending_write && cdc_connected);
            output_length = 0;
        }
    }

    return length;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_state_changed_handler(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) 
    {
		/* Callbacks must be registered after endpoint allocation */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)cdc_read_finished);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)cdc_write_finished);
		/* Start Rx */
		cdc_read_start();
		cdc_connected = true;
	}
	else
	{
		cdc_connected = false;
	}

	/* No error. */
	return false;
}

/**
 * \brief CDC ACM Init
 */
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
}

void usb_init(void)
{
	cdc_device_acm_init();
}

void usb_serial_begin (void)
{
	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};

	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_state_changed_handler);
}

int32_t usb_serial_bytes_available (void)
{
    if (!pending_read)
    {
        cdc_read_start();
    }

    return input_length;
}

int32_t usb_serial_read (char *const user_input_buffer, const uint16_t user_input_buffer_length)
{
    int result = 0;

    if (input_length > 0)
    {
        int bytes_to_copy = 0;
        int bytes_remaining = 0;
        int idx_of_first_remaining = 0;
        if (input_length >= user_input_buffer_length)
        {
            bytes_to_copy = user_input_buffer_length;
            bytes_remaining = input_length - user_input_buffer_length;
            idx_of_first_remaining = user_input_buffer_length;
        }
        else
        {
            bytes_to_copy = input_length;
        }

        result = bytes_to_copy;

        CRITICAL_SECTION_ENTER()

        memcpy(user_input_buffer, input_buffer, bytes_to_copy);

        if (bytes_remaining > 0)
        {
            memcpy(input_buffer, (input_buffer + idx_of_first_remaining), bytes_remaining);
            memset((input_buffer + bytes_remaining), 0, (input_length - bytes_remaining));
            input_length = bytes_remaining;
        }
        else
        {
            memset(input_buffer, 0, input_length);
            input_length = 0;
        }

        CRITICAL_SECTION_LEAVE()

        if (!pending_read)
        {
            cdc_read_start();
        }
    }

    return result;
}

int32_t usb_serial_write (const char *const user_output_buffer, const uint16_t user_output_buffer_length)
{
    return cdc_write(user_output_buffer, user_output_buffer_length);
}
