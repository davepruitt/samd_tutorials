#include <atmel_start.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

    usb_serial_begin();

    int user_buffer_length = 64;
    char user_buffer[64];
    memset(user_buffer, 0, 64);

    while(1)
    {
        if (usb_serial_bytes_available() > 0)
        {
            usb_serial_read(&user_buffer, user_buffer_length);
            usb_serial_write("Received: ", 10);
            usb_serial_write(&user_buffer, strlen(user_buffer));
            usb_serial_write("\r\n", 2);
            memset(user_buffer, 0, 64);
        }
    }
}
