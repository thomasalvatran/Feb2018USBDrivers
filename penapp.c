//http://stackoverflow.com/questions/14772152/read-write-on-a-pen-drive-using-libusb-libusb-bulk-transfer
//http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
//http://www.microchip.com/forums/m340892.aspx
//gcc penapp1.c -o penapp1 -lusb-1.0


#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include </usr/include/libusb-1.0/libusb.h>
#include <usb.h>
/*#define BULK_EP_OUT     0x82*/
/*#define BULK_EP_IN      0x01*/
#define BULK_EP_OUT     0x02
#define BULK_EP_IN      0x82

int main(void)
{
    int r = 0, e = 0;
    struct libusb_device_handle *handle = NULL;
    struct libusb_device **devs;
    struct libusb_device *dev;
    struct libusb_device_descriptor desc;
    char str1[256], str2[256];

    /* Init libusb */
    r = libusb_init(NULL);
    if (r < 0)
    {
        printf("\nfailed to initialise libusb\n");
        return 1;
    }

    handle = libusb_open_device_with_vid_pid(NULL, 0x05dc, 0xa838);
    if(handle == NULL)
    {
        printf("\nError in device opening!");
    }
    else
        printf("\nDevice Opened");

    // Tell libusb to use the CONFIGNUM configuration of the device

    libusb_set_configuration(handle, 1);
    if(libusb_kernel_driver_active(handle, 0) == 1)
    {
        printf("\nKernel Driver Active");
        if(libusb_detach_kernel_driver(handle, 0) == 0)
            printf("\nKernel Driver Detached!");
    }

    e = libusb_claim_interface(handle, 0);
    if(e < 0)
    {
        printf("\nCannot Claim Interface");
    }
    else
        printf("\nClaimed Interface");

      /* Communicate */
	  int i; 
//    int bytes_read = 512;
    int nbytes = 64;
    int transferred = 0;
/*    unsigned char *my_string1, *my_string2;*/
/*    int transferred = 0;*/
/*    my_string1 = (unsigned char *) malloc (nbytes + 1);*/
/*    my_string2 = (unsigned char *) malloc (nbytes + 1);*/
			char string1[256] ={0};
			char string2[256] ={0};

    strcpy(string1, "123456789101102");
    printf("\nTo be sent : %s", string1);

    e = libusb_bulk_transfer(handle, 0x01, string1, 31, &transferred, 5000);
   printf("\nXfer returned with %d -> %s", e, libusb_error_name(e)); 
/*    */
    printf("\nSent CDB %d bytes with string: %s\n", transferred, string1);
/* for (i = 10; i > 0; i--)*/
/* {*/
	  
/*    e = libusb_bulk_transfer(handle, 0x82, my_string1, 7, &transferred, 5000);*/
   //usb_bulk_write(handle, 1, "\x2\x8\0\0\x3", 5, 100);

	//sleep(3);

 		   e = libusb_bulk_transfer(handle, /*(2 | LIBUSB_ENDPOINT_IN)*/0x82, string2, 13, &transferred, 500000);
    
   //sleep(3);
    printf("\nXfer returned with %d -> %s", e, libusb_error_name(e));     //It returns -1... This is an error, I guess.
// }
    //dump_error(e); translate error -7 ERROR TIMEOUT
    printf("\nReceived STB %d bytes with string: %s\n", transferred, string2);
    
printf("string2 = ");
for (i = 0; i < 64; i++)
    	printf("%c", string2[i]);
    printf("\n");   

    e = libusb_release_interface(handle, 0);
    libusb_reset_device(handle);
    //libusb_free_device_list(handle, 0);
    libusb_close(handle);
    libusb_exit(NULL);
    return 0;
}
