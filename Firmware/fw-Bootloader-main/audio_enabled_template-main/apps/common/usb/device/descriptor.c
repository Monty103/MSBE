/**
 * @file descriptor.c
 * @brief overwrite usb device descriptor
 * @version 1.00
 * @date 2019-05-06
 */

#include "usb/device/usb_stack.h"
#include "usb/device/descriptor.h"
#include "usb/device/uac_audio.h"

#include "app_config.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

//#if TCFG_USB_SLAVE_ENABLE

#define USBD_VID                       0x045E
#define USBD_LANGID_STRING             0x0409 // 1033 [dec]
#define USBD_MANUFACTURER_STRING       "\xA9" "Microsoft Corporation"
#define USBD_PID_HS                    0x028E
#define USBD_BCD_DEVICE                0x114
#define USBD_PRODUCT_STRING_HS         "Controller"
#define USBD_INTERFACE_STRING_HS       ""

#define USB_SIZ_BOS_DESC            0x0C

#define  USB_DESC_TYPE_DEVICE                           0x01U
#define USBD_LPM_ENABLED 0
#define USB_MAX_EP0_SIZE                                8U
#define LOBYTE(x)  ((uint8_t)((x) & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)(((x) & 0xFF00U) >> 8U))
#define  USBD_IDX_MFC_STR                               0x01U
#define  USBD_IDX_PRODUCT_STR                           0x02U
#define  USBD_IDX_SERIAL_STR                            0x03U
#define USBD_MAX_NUM_CONFIGURATION                      1U


static const u8 sDeviceDescriptor[] =   //<Device Descriptor
{
    USB_DT_DEVICE_SIZE,      // bLength: Size of descriptor
    USB_DT_DEVICE,       // bDescriptorType: Device
    0x00, 0x02,     // bcdUSB: USB 2.0
    0xFF,       // bDeviceClass: none
    0xFF,       // bDeviceSubClass: none
    0xFF,       // bDeviceProtocol: none
    EP0_SETUP_LEN,//EP0_LEN,      // bMaxPacketSize0: 8/64 bytes
    LOBYTE(USBD_VID), HIBYTE(USBD_VID),     // idVendor: 0x4a4c - JL
    LOBYTE(USBD_PID_HS), HIBYTE(USBD_PID_HS),     // idProduct: chip id
    LOBYTE(USBD_BCD_DEVICE), HIBYTE(USBD_BCD_DEVICE),     // bcdDevice: version 1.0
    USBD_IDX_MFC_STR,       // iManufacturer: Index to string descriptor that contains the string <Your Name> in Unicode
    USBD_IDX_PRODUCT_STR,       // iProduct: Index to string descriptor that contains the string <Your Product Name> in Unicode
    USBD_IDX_SERIAL_STR,       // iSerialNumber: none
    USBD_MAX_NUM_CONFIGURATION        // bNumConfigurations: 1
};

#if 0
static const u8 sDeviceDescriptor[] =   //<Device Descriptor
{
    0x12,                       /*bLength */
    USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
#if (USBD_LPM_ENABLED == 1)
    0x01,                       /*bcdUSB */ /* changed to USB version 2.01
											 in order to support LPM L1 suspend
											 resume test of USBCV3.0*/
#else
    0x00,                       /*bcdUSB */
#endif /* (USBD_LPM_ENABLED == 1) */

    0x02,
    0xFF,                       /*bDeviceClass*/
    0xFF,                       /*bDeviceSubClass*/
    0xFF,                       /*bDeviceProtocol*/
    USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
    LOBYTE(USBD_VID),           /*idVendor*/
    HIBYTE(USBD_VID),           /*idVendor*/
    LOBYTE(USBD_PID_HS),        /*idProduct*/
    HIBYTE(USBD_PID_HS),        /*idProduct*/
    LOBYTE(USBD_BCD_DEVICE),    /*bcdDevice*/
    HIBYTE(USBD_BCD_DEVICE),    /*bcdDevice*/
    USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
    USBD_IDX_PRODUCT_STR,       /*Index of product string*/
    USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
    USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};
#endif

static const u8 LANGUAGE_STR[] =
{
    0x04, 0x03, 0x09, 0x04
};


static const u8 product_string[] =
{
    22,
    0x03,
    'C', 0x00,
    'o', 0x00,
    'n', 0x00,
    't', 0x00,
    'r', 0x00,
    'o', 0x00,
    'l', 0x00,
    'l', 0x00,
    'e', 0x00,
    'r', 0x00,
};

static const u8 MANUFACTURE_STR[] =
{
    48,         //该描述符的长度为34字节
    0x03,       //字符串描述符的类型编码为0x03
    '\xA9', 0x00,
    ' ', 0x00,
    'M', 0x00,
    'i', 0x00,
    'c', 0x00,
    'r', 0x00,
    'o', 0x00,
    's', 0x00,
    'o', 0x00,
    'f', 0x00,
    't', 0x00,
    ' ', 0x00,
    'C', 0x00,
    'o', 0x00,
    'r', 0x00,
    'p', 0x00,
    'o', 0x00,
    'r', 0x00,
    'a', 0x00,
    't', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00,


};

static const u8 sConfigDescriptor[] =  	//<Config Descriptor
{
    0x09,                           // bLength: Configuration Descriptor size
    0x02,    // bDescriptorType: Configuration
    153,       // wTotalLength: Bytes returned
    0x00,                           // wTotalLength...
    0x04,                           // bNumInterfaces: 1 interface
    0x01,                           // bConfigurationValue: Configuration value
    0x00,                           // iConfiguration: Index of string descriptor describing the configuration
    0xA0,                           // bmAttributes: bus powered , remote wake up
    0xFA,                           // bMaxPower: 500 mA
};

static const u8 serial_string[] =
{
   /* 0x10, 0x03, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x36, 0x00, 0x46, 0x00, 0x36, 0x00,
    0x34, 0x00, 0x30, 0x00, 0x39, 0x00, 0x36, 0x00, 0x42, 0x00, 0x32, 0x00, 0x32, 0x00, 0x45, 0x00,
    0x37, 0x00*/

    0x10,
    0x03,
    '1',0x00,
    'D',0x00,
    'D',0x00,
    '5',0x00,
    'F',0x00,
    '3',0x00,
    'D',0x00,



};

void get_device_descriptor(u8 *ptr)
{
    memcpy(ptr, sDeviceDescriptor, USB_DT_DEVICE_SIZE);
}

void get_language_str(u8 *ptr)
{
    memcpy(ptr, LANGUAGE_STR, LANGUAGE_STR[0]);
}

void get_manufacture_str(u8 *ptr)
{
    memcpy(ptr, MANUFACTURE_STR, MANUFACTURE_STR[0]);
}

void get_iserialnumber_str(u8 *ptr)
{
//#if USB_ROOT2
    memcpy(ptr, serial_string, serial_string[0]);
 /*#else
   extern __attribute__((weak)) u8 *get_norflash_uuid(void);
    u8 flash_id[16] = {0};
    int i;
    u8 bcd;
    if (get_norflash_uuid && get_norflash_uuid())
    {
        ptr[0] = 0x22;
        ptr[1] = 0x03;
        memset(&ptr[2], 0, 0x20);
        memcpy(flash_id, get_norflash_uuid(), 16);
        //take 8 bytes from flash uuid
        for (i = 0; i < 8; i++)
        {
            bcd = flash_id[i] >> 4;
            if (bcd > 9)
            {
                bcd = bcd - 0xa + 'A';
            }
            else
            {
                bcd = bcd + '0';
            }
            ptr[2 + i * 4] = bcd;
            bcd = flash_id[i] & 0xf;
            if (bcd > 9)
            {
                bcd = bcd - 0xa + 'A';
            }
            else
            {
                bcd = bcd + '0';
            }
            ptr[2 + i * 4 + 2] = bcd;
        }
    }
    else
    {
        memcpy(ptr, serial_string, serial_string[0]);
    }
#endif*/
}

#if USB_ROOT2
static const u8 ee_string[] = {0x12, 0x03, 0x4D, 0x00, 0x53, 0x00, 0x46, 0x00, 0x54,
                               0x00, 0x31, 0x00, 0x30, 0x00, 0x30, 0x00, 0x90, 0x00
                              };
void get_string_ee(u8 *ptr)
{
    memcpy(ptr, ee_string, ee_string[0]);
}
#endif

void get_product_str(u8 *ptr)
{
    memcpy(ptr, product_string, product_string[0]);
}

const u8 *usb_get_config_desc()
{
    return sConfigDescriptor;
}

const u8 *usb_get_string_desc(u32 id)
{
    const u8 *pstr = uac_get_string(id);
    if (pstr != NULL)
    {
        return pstr;
    }
    return NULL;
}

//#endif
