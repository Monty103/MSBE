#include "usb/device/usb_stack.h"
#include "usb_config.h"
#include "usb/device/msd.h"
#include "usb/scsi.h"
#include "usb/device/hid.h"
#include "usb/device/uac_audio.h"
#include "irq.h"
#include "init.h"
#include "gpio.h"
#include "app_config.h"

#if TCFG_USB_APPLE_DOCK_EN
#include "apple_dock/iAP.h"
#endif

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



static const u8 user_stirng[] =
{
    178,
    0x03,
    'X', 0x00,
    'b', 0x00,
    'o', 0x00,
    'x', 0x00,
    ' ', 0x00,
    'S', 0x00,
    'e', 0x00,
    'c', 0x00,
    'u', 0x00,
    'r', 0x00,
    'i', 0x00,
    't', 0x00,
    'y', 0x00,
    ' ', 0x00,
    'M', 0x00,
    'e', 0x00,
    't', 0x00,
    'h', 0x00,
    'o', 0x00,
    'd', 0x00,
    ' ', 0x00,
    '3', 0x00,
    ',', 0x00,
    ' ', 0x00,
    'V', 0x00,
    'e', 0x00,
    'r', 0x00,
    's', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00,
    ' ', 0x00,
    '1', 0x00,
    '.', 0x00,
    '0', 0x00,
    '0', 0x00,
    ',', 0x00,
    ' ', 0x00,
    0xA9, 0x00,
    ' ', 0x00,
    '2', 0x00,
    '0', 0x00,
    '0', 0x00,
    '5', 0x00,
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
    '.', 0x00,
    ' ', 0x00,
    'A', 0x00,
    'l', 0x00,
    'l', 0x00,
    ' ', 0x00,
    'r', 0x00,
    'i', 0x00,
    'g', 0x00,
    'h', 0x00,
    't', 0x00,
    's', 0x00,
    ' ', 0x00,
    'r', 0x00,
    'e', 0x00,
    's', 0x00,
    'e', 0x00,
    'r', 0x00,
    'v', 0x00,
    'e', 0x00,
    'd', 0x00,
    '.', 0x00,
};

#if TCFG_USB_APPLE_DOCK_EN
static const u8 IAP_interface_string[]  =
{
    0x1c, 0x03,
    //iAP Interface
    0x69, 0x00, 0x41, 0x00, 0x50, 0x00, 0x20, 0x00, 0x49, 0x00, 0x6e, 0x00, 0x74, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x66, 0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00
};
#endif

static u8 root2_testing;
u32 usb_root2_testing()
{
#if USB_ROOT2
    return root2_testing;
#else
    return 0;
#endif
}
u32 check_ep_vaild(u32 ep)
{
    u32 en = 0;
    switch (ep)
    {
    case 0:
        en = 1;
        break;
#if TCFG_USB_SLAVE_MSD_ENABLE
    case 1:
        en = 1;
        break;
#endif
#if TCFG_USB_SLAVE_HID_ENABLE
    case 2:
        en = 1;
        break;
#endif
#if TCFG_USB_SLAVE_AUDIO_ENABLE
    case 3:
        en = 1;
        break;
#endif
    case 4:
        break;
    }
    return en;
}
static u32 setup_endpoint(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    printf("setup endpoint");
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 tx_len = 0;
    u8 *tx_payload = usb_get_setup_buffer(usb_device);

#if TCFG_USB_SLAVE_AUDIO_ENABLE
    if (uac_setup_endpoint(usb_device, req))
    {
        return 1;
    }
#endif
    u32 ep = LOBYTE(req->wIndex) & 0x0f;
    printf("EP = %d", ep);
    if (check_ep_vaild(ep) == 0)
    {
        printf("VALIDAN");
        usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
        return 1;// not zero user handle this request
    }

    printf("NIJE VALIDAN");

    return 0;
}

enum
{
    XUSB_IDLE = 0U,
    XUSB_EP0_RX_WAITING,
    XUSB_EP0_RX_CHATPAD
};

extern uint32_t device_serial;
extern u8 state_chatpad;
extern u8 xusb_state;
extern u32 second_recv(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup);
extern u32 endpoint0_error(struct usb_device_t *usb_device);

static u32 setup_device(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    uint8_t recipient = req->bRequest & 0x03U;
    //printf("setup device req->bRequest = %02X recipient = %02X LOBYTE(req->wIndex) = %02X",req->bRequest, recipient, LOBYTE(req->wIndex));
    uint32_t dwInfo = 0;
    uint16_t status_info = 0;
    u32 ret = 0;
    static uint8_t temp[100];
    switch (req->bRequest)
    {
    case 0x01:
        if ((req->bRequestType & 0x80) == 0x80)
        {
           // printf("CASE 0x01");
            dwInfo = 0; // Serial number
            usb_set_data_payload(usb_device, req,  (uint8_t*)&dwInfo, 4);
            ret = 1;
        }
        else
        {
           printf("CASE 0x01 error");
            ret = 0;
        }
        break;

    case 0xA1:
        if ((req->bRequestType & 0x80) == 0x80)
        {
           // printf("CASE 0xA1 F");
            status_info = 0x09;
            usb_set_data_payload(usb_device, req,  (uint8_t*)&status_info, 2);
        }
        else
        {
           printf("CASE 0xA1 S");
            // Host to device request:
            xusb_state = XUSB_EP0_RX_CHATPAD;
            usb_set_setup_recv(usb_device, second_recv);
        }
        break;
    case 0xA9:
       // printf("CASE 0x09");
       //usb_set_data_payload(usb_device, req,  temp, 0);
        ret = 1;
        break;
    case USB_REQ_GET_DESCRIPTOR:
        switch (HIBYTE(req->wValue))
        {
        case USB_DT_STRING:
            switch (LOBYTE(req->wValue))
            {
            case SPEAKER_STR_INDEX:
            case 4:
                if (usb_device->bDeviceStates == USB_DEFAULT)
                {
                //    printf("OVDE USAO");
                    ret = 0;
                }
                else
                {
                //    printf("OVDE USAO1");
                    usb_set_data_payload(usb_device, req, user_stirng, sizeof(user_stirng));
                    ret = 1;
                }
                break;
            default:
                break;
            }
            break;
        case USB_DT_DEVICE:
#if USB_ROOT2
            if (req->wLength == 0xffff)
            {
                root2_testing = 1;
            }
#endif
            break;
        }
        break;
    case USB_REQ_SET_CONFIGURATION:
        break;
    case USB_REQ_SET_ADDRESS:
        /* if(req->wLength || req->wIndex){                        */
        /*     ret = 1;                                            */
        /*     usb_set_setup_phase(usb_device, USB_EP0_SET_STALL); */
        /*     dump_setup_request(req);                            */
        /*     log_debug_hexdump((u8 *)req, 8);                    */
        /* }                                                       */
        break;
    }
    return ret;
}
static u32 user_setup_filter(struct usb_device_t *usb_device, struct usb_ctrlrequest *request)
{
    /* dump_setup_request(request);
     printf_buf((u8 *)request, 8);*/
    u32 ret = 0;
    u32 recip = request->bRequestType & USB_RECIP_MASK;
    switch (recip)
    {
    case USB_RECIP_DEVICE:
        ret = setup_device(usb_device, request);
        break;
    case USB_RECIP_INTERFACE:
        break;
    case USB_RECIP_ENDPOINT:
        ret = setup_endpoint(usb_device, request);
        break;
    case USB_RECIP_OTHER:
        break;
    }
#if 0
    const char *statas[] = {"USB_ATTACHED", "USB_POWERED", "USB_DEFAULT",
                            "USB_ADDRESS", "USB_CONFIGURED", "USB_SUSPENDED"
                           };

    const char *phases[] = {"SETUP", "IN", "OUT",
                            "STALL",
                           };

    usb_log_d("state:%s phase: %s", statas[usb_device->bDeviceStates],
              phases[usb_device->bsetup_phase]);
#endif
    return ret;// not zero user handle this request
}
void user_setup_filter_install(struct usb_device_t *usb_device)
{
    usb_set_setup_hook(usb_device, user_setup_filter);
}
