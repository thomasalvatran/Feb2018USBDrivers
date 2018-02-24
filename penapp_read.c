//http://stackoverflow.com/questions/14772152/read-write-on-a-pen-drive-using-libusb-libusb-bulk-transfer
//http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
// http://git.libusb.org/?p=libusb-pbatard.git;a=blob;f=examples/xusb.c
//gcc penapp1.c libusb-1.0.20/libusb/libusb_1_0_la-strerror.o  -o penapp1 -lusb-1.0
// gcc penapp.c -o penapp -lusb-1.0

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include </usr/include/libusb-1.0/libusb.h> //libusb1.0
#include <usb.h> //libusb0.1
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
/*#define BULK_EP_OUT     0x82*/
/*#define BULK_EP_IN      0x01*/
#define BULK_EP_OUT     0x01
#define BULK_EP_IN      0x82


#if defined(_WIN32)
#define msleep(msecs) Sleep(msecs)
#else
#include <time.h>
#define msleep(msecs) nanosleep(&(struct timespec){msecs / 1000, (msecs * 1000000) % 1000000000UL}, NULL);
#endif

#if defined(_MSC_VER)
#define snprintf _snprintf
#define putenv _putenv
#endif

#if !defined(bool)
#define bool int
#endif
#if !defined(true)
#define true (1 == 1)
#endif
#if !defined(false)
#define false (!true)
#endif

// Future versions of libusb will use usb_interface instead of interface
// in libusb_config_descriptor => catter for that
#define usb_interface interface

// Global variables
static bool binary_dump = false;
static bool extra_info = false;
static bool force_device_request = false;   // For WCID descriptor queries
static const char* binary_name = NULL;

static int perr(char const *format, ...)
{
    va_list args;
    int r;

    va_start (args, format);
    r = vfprintf(stderr, format, args);
    va_end(args);

    return r;
}

#define ERR_EXIT(errcode) do { perr("   %s\n", libusb_strerror((enum libusb_error)errcode)); return -1; } while (0)
#define CALL_CHECK(fcall) do { r=fcall; if (r < 0) ERR_EXIT(r); } while (0);
#define B(x) (((x)!=0)?1:0)
#define be_to_int32(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|(buf)[3])

#define RETRY_MAX                     5
#define REQUEST_SENSE_LENGTH          0x12
#define INQUIRY_LENGTH                0x24
#define READ_CAPACITY_LENGTH          0x08

// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

// Mass Storage Requests values. See section 3 of the Bulk-Only Mass Storage Class specifications
//Universal Serial Bus Mass Storage Class Bulk-Only Transport
//K:\J\Study-Ebook\Reilly_linux_kernel_driver\USB\usbmassbulk_10.pdf
#define BOMS_RESET                    0xFF
#define BOMS_GET_MAX_LUN              0xFE

// Section 5.1: Command Block Wrapper (CBW)
//K:\J\Study-Ebook\Reilly_linux_kernel_driver\USB\usbmassbulk_10.pdf
struct command_block_wrapper {
    uint8_t dCBWSignature[4];
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
};

// Section 5.2: Command Status Wrapper (CSW)
struct command_status_wrapper {
    uint8_t dCSWSignature[4];
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
};

union int_thing {
    char usb_data[4];
    uint32_t number;
};

union int_thing number;

static uint8_t cdb_length[256] = {
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  F
};

static int send_mass_storage_command(libusb_device_handle *handle, uint8_t endpoint, uint8_t lun,
    uint8_t *cdb, uint8_t direction, int data_length, uint32_t *ret_tag)
{
    static uint32_t tag = 0x00000045;
    uint8_t cdb_len;
    int i, r, size;
    struct command_block_wrapper cbw;

    if (cdb == NULL) {
        return -1;
    }

    if (endpoint & LIBUSB_ENDPOINT_IN) {
        perr("send_mass_storage_command: cannot send command on IN endpoint\n");
        return -1;
    }

    cdb_len = cdb_length[cdb[0]];
    printf("cdb_len ===========%d\n", cdb_len);
    if ((cdb_len == 0) || (cdb_len > sizeof(cbw.CBWCB))) {
        perr("send_mass_storage_command: don't know how to handle this command (%02X, length %d)\n",
            cdb[0], cdb_len);
        return -1;
    }

    memset(&cbw, 0, sizeof(cbw));
    cbw.dCBWSignature[0] = 'U';
    cbw.dCBWSignature[1] = 'S';
    cbw.dCBWSignature[2] = 'B';
    cbw.dCBWSignature[3] = 'C';
    *ret_tag = tag;
    cbw.dCBWTag = tag++;
    cbw.dCBWDataTransferLength = data_length;
    cbw.bmCBWFlags = direction;
    cbw.bCBWLUN = lun;
    // Subclass is 1 or 6 => cdb_len
    cbw.bCBWCBLength = cdb_len;
    memcpy(cbw.CBWCB, cdb, cdb_len);

    i = 0;
    do {
        // The transfer length must always be exactly 31 bytes.
        r = libusb_bulk_transfer(handle, endpoint, (unsigned char*)&cbw, 31, &size, 1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
    if (r != LIBUSB_SUCCESS) {
        perr("   send_mass_storage_command: %s\n", libusb_error_name((enum libusb_error)r));
        return -1;
    }

    printf("   sent %d CDB bytes\n", cdb_len);
    return 0;
}

static int get_mass_storage_status(libusb_device_handle *handle, uint8_t endpoint, uint32_t expected_tag)
{
    int i, r, size;
    struct command_status_wrapper csw;

    // The device is allowed to STALL this transfer. If it does, you have to
    // clear the stall and try again.
    i = 0;
    do {
        r = libusb_bulk_transfer(handle, endpoint, (unsigned char*)&csw, 13, &size, 1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
    if (r != LIBUSB_SUCCESS) {
        // perr("   get_mass_storage_status: %s\n", libusb_strerror((enum libusb_error)r));
        perr("   get_mass_storage_status: %s\n", libusb_error_name((enum libusb_error)r));
        return -1;
    }
    if (size != 13) {
        perr("   get_mass_storage_status: received %d bytes (expected 13)\n", size);
        return -1;
    }
    if (csw.dCSWTag != expected_tag) {
        perr("   get_mass_storage_status: mismatched tags (expected %08X, received %08X)\n",
            expected_tag, csw.dCSWTag);
        return -1;
    }
    // For this test, we ignore the dCSWSignature check for validity...
    printf("   Mass Storage Status: %02X (%s)\n", csw.bCSWStatus, csw.bCSWStatus?"FAILED":"Success");
    if (csw.dCSWTag != expected_tag)
        return -1;
    if (csw.bCSWStatus) {
        // REQUEST SENSE is appropriate only if bCSWStatus is 1, meaning that the
        // command failed somehow.  Larger values (2 in particular) mean that
        // the command couldn't be understood.
        if (csw.bCSWStatus == 1)
            return -2;  // request Get Sense
        else
            return -1;
    }

    // In theory we also should check dCSWDataResidue.  But lots of devices
    // set it wrongly.
    return 0;
}

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
    // handle = libusb_open_device_with_vid_pid(NULL, 0x05dc, 0xa205);  
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

    uint32_t expected_tag = 0x00000045;
    uint8_t cdb[16] = {0};   // SCSI Command Descriptor Block
    memset(cdb, 0 , sizeof(cdb));
    //printf("Sending Inquiry:\n");
    
    //cdb[0] = 0x25;  // Inquiry
    cdb[0] = 0x28;  // Read
    // cdb[0] = 0x2a;  // Write
    //cdb[0] = 0x12;    // Inquiry
    //cdb[4] = INQUIRY_LENGTH;
    cdb[4] = 0x00; //MSB

    // cdb[5] = 0x02;
    cdb[5] = 0x02;  //1st sector sudo dd if=/dev/sdb bs=512 count=1 skip=1 | xxd  LSB
    // cdb[8] = 0x01;
    // cdb[5] = 0x01;
    cdb[7] = 0x00;
    cdb[8] = 0x01;  //1 sector 512 bytes
    send_mass_storage_command(handle, 0x01, 0, cdb, /*(0x01 | LIBUSB_ENDPOINT_OUT) */ 0x80, 512, &expected_tag);

    char string1[512] ={0};
    char string2[512] ={0};
     strcpy(string1, "abcdefghxxxx");   
    int transferred = 0, i;
    // e = libusb_bulk_transfer(handle, /*(1 | LIBUSB_ENDPOINT_IN)*/  0x01, string1, 512, &transferred, 5000);
    e = libusb_bulk_transfer(handle, /*(2 | LIBUSB_ENDPOINT_IN)*/0x82, string2, sizeof(string2), &transferred, 5000);
    printf("\nXfer returned with %d -> %s", e, libusb_error_name(e));     //It returns -1... This is an error, I guess.
// }
    printf("\nReceived STB %d bytes with string: %s\n", transferred, string2);
    
    printf("string2 = ");
    for (i = 0; i < transferred; i++)
        // printf("%c", string2[i]);
        printf("%x", string2[i]);
    printf("\n");   

    e = libusb_release_interface(handle, 0);
    libusb_reset_device(handle);
    //libusb_free_device_list(handle, 0);
    libusb_close(handle);
    libusb_exit(NULL);
    return 0;
}
