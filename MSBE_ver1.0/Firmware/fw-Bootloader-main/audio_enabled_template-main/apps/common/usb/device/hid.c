#include "os/os_api.h"
#include "usb/device/usb_stack.h"
#include "usb/device/hid.h"
#include "usb_config.h"

#include "app_config.h"

#ifdef TCFG_USB_SLAVE_USER_HID
#undef TCFG_USB_SLAVE_HID_ENABLE
#define TCFG_USB_SLAVE_HID_ENABLE           0
#endif


#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define printf_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#define  USB_LEN_DEV_QUALIFIER_DESC                     0x0AU
#define  USB_LEN_DEV_DESC                               0x12U
#define  USB_LEN_CFG_DESC                               0x09U
#define  USB_LEN_IF_DESC                                0x09U
#define  USB_LEN_EP_DESC                                0x07U
#define  USB_LEN_OTG_DESC                               0x03U
#define  USB_LEN_LANGID_STR_DESC                        0x04U
#define  USB_LEN_OTHER_SPEED_DESC_SIZ                   0x09U

#define  USBD_IDX_LANGID_STR                            0x00U
#define  USBD_IDX_MFC_STR                               0x01U
#define  USBD_IDX_PRODUCT_STR                           0x02U
#define  USBD_IDX_SERIAL_STR                            0x03U
#define  USBD_IDX_CONFIG_STR                            0x04U
#define  USBD_IDX_INTERFACE_STR                         0x05U

#define  USB_REQ_TYPE_STANDARD                          0x00U
#define  USB_REQ_TYPE_CLASS                             0x20U
#define  USB_REQ_TYPE_VENDOR                            0x40U
#define  USB_REQ_TYPE_MASK                              0x60U

#define  USB_REQ_RECIPIENT_DEVICE                       0x00U
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01U
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02U
#define  USB_REQ_RECIPIENT_MASK                         0x03U

#define  USB_REQ_GET_STATUS                             0x00U
#define  USB_REQ_CLEAR_FEATURE                          0x01U
#define  USB_REQ_SET_FEATURE                            0x03U
#define  USB_REQ_SET_ADDRESS                            0x05U
#define  USB_REQ_GET_DESCRIPTOR                         0x06U
#define  USB_REQ_SET_DESCRIPTOR                         0x07U
#define  USB_REQ_GET_CONFIGURATION                      0x08U
#define  USB_REQ_SET_CONFIGURATION                      0x09U
#define  USB_REQ_GET_INTERFACE                          0x0AU
#define  USB_REQ_SET_INTERFACE                          0x0BU
#define  USB_REQ_SYNCH_FRAME                            0x0CU

#define  USB_DESC_TYPE_DEVICE                           0x01U
#define  USB_DESC_TYPE_CONFIGURATION                    0x02U
#define  USB_DESC_TYPE_STRING                           0x03U
#define  USB_DESC_TYPE_INTERFACE                        0x04U
#define  USB_DESC_TYPE_ENDPOINT                         0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define  USB_DESC_TYPE_BOS                              0x0FU

#define USB_CONFIG_REMOTE_WAKEUP                        0x02U
#define USB_CONFIG_SELF_POWERED                         0x01U

#define USB_FEATURE_EP_HALT                             0x00U
#define USB_FEATURE_REMOTE_WAKEUP                       0x01U
#define USB_FEATURE_TEST_MODE                           0x02U

#define USB_DEVICE_CAPABITY_TYPE                        0x10U

#define USB_HS_MAX_PACKET_SIZE                          512U
#define USB_FS_MAX_PACKET_SIZE                          64U
#define USB_MAX_EP0_SIZE                                8U

/*  Device Status */
#define USBD_STATE_DEFAULT                              0x01U
#define USBD_STATE_ADDRESSED                            0x02U
#define USBD_STATE_CONFIGURED                           0x03U
#define USBD_STATE_SUSPENDED                            0x04U


/*  EP0 State */
#define USBD_EP0_IDLE                                   0x00U
#define USBD_EP0_SETUP                                  0x01U
#define USBD_EP0_DATA_IN                                0x02U
#define USBD_EP0_DATA_OUT                               0x03U
#define USBD_EP0_STATUS_IN                              0x04U
#define USBD_EP0_STATUS_OUT                             0x05U
#define USBD_EP0_STALL                                  0x06U

#define USBD_EP_TYPE_CTRL                               0x00U
#define USBD_EP_TYPE_ISOC                               0x01U
#define USBD_EP_TYPE_BULK                               0x02U
#define USBD_EP_TYPE_INTR                               0x03U

#define USB_XUSB_CONFIG_DESC_SIZ                     153U
#define USB_XUSB_DESC_SIZ                            9U

#define CONTROL_DATA_EPIN_ADDR                       0x81U
#define CONTROL_DATA_EPIN_SIZE                       0x20U
#define CONTROL_DATA_EPIN_BINTERVAL                  0x04U

#define CONTROL_DATA_EPOUT_ADDR                      0x01U
#define CONTROL_DATA_EPOUT_SIZE                      0x20U
#define CONTROL_DATA_EPOUT_BINTERVAL                 0x08U

#define CUSTOM_HID_DESCRIPTOR_TYPE                   0x21U
#define CUSTOM_HID_REPORT_DESC                       0x22U

#define CUSTOM_HID_REQ_SET_PROTOCOL                  0x0BU
#define CUSTOM_HID_REQ_GET_PROTOCOL                  0x03U

#define CUSTOM_HID_REQ_SET_IDLE                      0x0AU
#define CUSTOM_HID_REQ_GET_IDLE                      0x02U

#define CUSTOM_HID_REQ_SET_REPORT                    0x09U
#define CUSTOM_HID_REQ_GET_REPORT                    0x01U


#define HEADSET_EPIN_ADDR                            0x82U
#define HEADSET_EPIN_SIZE                            0x20U
#define HEADSET_EPIN_BINTERVAL                       0x02U

#define HEADSET_EPOUT_ADDR                           0x02U
#define HEADSET_EPOUT_SIZE                           0x20U
#define HEADSET_EPOUT_BINTERVAL                      0x04U

#define HEADSET_STATUS_EPIN_ADDR                     0x83U
#define HEADSET_STATUS_EPIN_SIZE                     0x20U
#define HEADSET_STATUS_EPIN_BINTERVAL                0x40U

#define HEADSET_CONFIG_EPOUT_ADDR                    0x03U
#define HEADSET_CONFIG_EPOUT_SIZE                    0x20U
#define HEADSET_CONFIG_EPOUT_BINTERVAL               0x10U

#define CHATPAD_EPIN_ADDR                            0x84U
#define CHATPAD_EPIN_SIZE                            0x20U
#define CHATPAD_EPIN_BINTERVAL                       0x10U

#ifndef USBD_CUSTOM_HID_REPORT_DESC_SIZE
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE             163U
#endif

#define CUSTOM_HID_DESCRIPTOR_TYPE                   0x21U
#define CUSTOM_HID_REPORT_DESC                       0x22U

#define CUSTOM_HID_REQ_SET_PROTOCOL                  0x0BU
#define CUSTOM_HID_REQ_GET_PROTOCOL                  0x03U

#define CUSTOM_HID_REQ_SET_IDLE                      0x0AU
#define CUSTOM_HID_REQ_GET_IDLE                      0x02U

#define CUSTOM_HID_REQ_SET_REPORT                    0x09U
#define CUSTOM_HID_REQ_GET_REPORT                    0x01U

#define EP0_BUFF_MAX_LEN 64

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define printf_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"
static const u8 sHIDDescriptor0[] =
{

    /*************** Interface 0: Control Data ********************************/
    0x09,                           // bLength: Interface Descriptor size
    0x04,        // bDescriptorType: Interface descriptor type
    0x00,                           // bInterfaceNumber: Number of Interface
    0x00,                           // bAlternateSetting: Alternate setting
    0x02,                           // bNumEndpoints
    0xFF,                           // bInterfaceClass: vendor defined (XUSB)
    0x5D,                           // bInterfaceSubClass
    0x01,                           // nInterfaceProtocol
    0x00,                           // iInterface: Index of string descriptor

    /*************** HID Class-Specific Descriptor (If0) **********************/
    0x11,                           // bLength
    0x21,     // bDescriptorType (HID)
    0x00,                           // ?
    0x01,                           // ?
    0x01,                           // ?
    0x25,                           // ?
    0x81,         // bEndpointAddress (IN, 1)
    0x14,                           // bMaxDataSize
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x13,                           // ?
    0x01,        // bEndpointAddress (OUT, 1)
    0x08,                           // bMaxDataSize
    0x00,                           // ?
    0x00,                           // ?

    /*************** Endpoint 1: Control Data In (to host) ********************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    CONTROL_DATA_EPIN_ADDR,         // bEndpointAddress (IN, 1)
    USBD_EP_TYPE_INTR,              // bmAttributes
    CONTROL_DATA_EPIN_SIZE,         // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    CONTROL_DATA_EPIN_BINTERVAL,    // bInterval

    /*************** Endpoint 2: Control Data Out (from host) *****************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    CONTROL_DATA_EPOUT_ADDR,        // bEndpointAddress (OUT, 1)
    USBD_EP_TYPE_INTR,              // bmAttributes
    CONTROL_DATA_EPOUT_SIZE,        // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    CONTROL_DATA_EPOUT_BINTERVAL,   // bInterval

};

static const u8 sHIDDescriptor1[] =
{

    /*************** Interface 1: Headset *************************************/
    0x09,                           // bLength
    USB_DESC_TYPE_INTERFACE,        // bDescriptorType (INTERFACE)
    0x01,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x04,                           // bNumEndpoints
    0xFF,                           // bInterfaceClass
    0x5D,                           // bInterfaceSubClass
    0x03,                           // bInterfaceProtocol
    0x00,                           // iInterface

    /*************** HID Class*Specific Descriptor (If1) **********************/
    0x1B,                           // bLength
    CUSTOM_HID_DESCRIPTOR_TYPE,     // bDescriptorType (HID)
    0x00,                           // ?
    0x01,                           // ?
    0x01,                           // ?
    0x01,                           // ?
    HEADSET_EPIN_ADDR,              // bEndpointAddress (IN, 2)
    0x40,                           // bMaxDataSize
    0x01,                           // ?
    HEADSET_EPOUT_ADDR,             // bEndpointAddress (OUT, 2)
    0x20,                           // bMaxDataSize
    0x16,                           // ?
    HEADSET_STATUS_EPIN_ADDR,       // bEndpointAddress (IN, 3)
    0x00,                           // bMaxDataSize
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x16,                           // ?
    HEADSET_CONFIG_EPOUT_ADDR,      // bEndpointAddress (OUT, 3)
    0x00,                           // bMaxDataSize
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?
    0x00,                           // ?

    /*************** Endpoint 82: Headset Microphone In (to host) *************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    HEADSET_EPIN_ADDR,              // bEndpointAddress (IN, 2)
    USBD_EP_TYPE_INTR,              // bmAttributes
    HEADSET_EPIN_SIZE,              // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    HEADSET_EPIN_BINTERVAL,         // bInterval

    /*************** Endpoint 2: Headset Audio Out (from host) ****************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    HEADSET_EPOUT_ADDR,             // bEndpointAddress (OUT, 2)
    USBD_EP_TYPE_INTR,              // bmAttributes
    HEADSET_EPOUT_SIZE,             // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    HEADSET_EPOUT_BINTERVAL,        // bInterval

    /*************** Endpoint 3: Headset Status (to host) *********************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    HEADSET_STATUS_EPIN_ADDR,       // bEndpointAddress (IN, 3)
    USBD_EP_TYPE_INTR,              // bmAttributes
    HEADSET_STATUS_EPIN_SIZE,       // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    HEADSET_STATUS_EPIN_BINTERVAL,  // bInterval

    /*************** Endpoint 83: Headset Configuration (from host) ***********/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    HEADSET_CONFIG_EPOUT_ADDR,      // bEndpointAddress (OUT, 3)
    USBD_EP_TYPE_INTR,              // bmAttributes
    HEADSET_CONFIG_EPOUT_SIZE,      // wMaxPacketSize
    0x00,                           // wMaxPacketSize...
    HEADSET_CONFIG_EPOUT_BINTERVAL, // bInterval


};

static const u8 sHIDDescriptor2[] =
{
    /*************** Interface 2: Chatpad *************************************/
    0x09,                           // bLength
    USB_DESC_TYPE_INTERFACE,        // bDescriptorType (INTERFACE)
    0x02,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints
    0xFF,                           // bInterfaceClass
    0x5D,                           // bInterfaceSubClass
    0x02,                           // bInterfaceProtocol
    0x00,                           // iInterface

    /*************** HID Class*Specific Descriptor (If2) **********************/
    0x09,                           // bLength
    CUSTOM_HID_DESCRIPTOR_TYPE,     // bDescriptorType (HID)
    0x00,                           // ?
    0x01,                           // ?
    0x01,                           // ?
    0x22,                           // ?
    CHATPAD_EPIN_ADDR, // bEndpointAddress (IN, 4)
    0x07,                           // bMaxDataSize
    0x00,                           // ?

    /*************** Endpoint 84: Chatpad In (to host) ************************/
    0x07,                           // bLength
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType (ENDPOINT)
    CHATPAD_EPIN_ADDR,              // bEndpointAddress (IN, 4)
    USBD_EP_TYPE_INTR,              // bmAttributes
    CHATPAD_EPIN_SIZE,
    0x00,                           // wMaxPacketSize
    CHATPAD_EPIN_BINTERVAL,         // bInterval

};

static const u8 sHIDDescriptor3[] =
{

    /*************** Interface 3: Security Method *****************************/
    0x09,                           // bLength
    USB_DESC_TYPE_INTERFACE,        // bDescriptorType (INTERFACE)
    0x03,                           // bInterfaceNumber
    0x00,                           // bAlternateSetting
    0x00,                           // bNumEndpoints
    0xFF,                           // bInterfaceClass
    0xFD,                           // bInterfaceSubClass
    0x13,                           // bInterfaceProtocol
    0x04,                           // iInterface

    /*************** Descriptor (If3) *****************************************/
    0x06,                           // bLength
    0x41,                           // bDescriptorType
    0x00,                           // ?
    0x01,                           // ?
    0x01,                           // ?
    0x03                            // ?
};

static const u8 USBD_CUSTOM_HID_Desc[] =
{
    0x09, /* bLength: CUSTOM_HID Descriptor size */
    CUSTOM_HID_DESCRIPTOR_TYPE, /* bDescriptorType: CUSTOM_HID */
    0x11, /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
    0x01,
    0x00, /* bCountryCode: Hardware target country */
    0x01, /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
    0x22, /* bDescriptorType */
    USBD_CUSTOM_HID_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,

};

#define MAX_EP1_SIZE 32
#define MAX_EP2_SIZE 64
#define MAX_EP3_SIZE 32
#define MAX_EP4_SIZE 32

enum
{
    CUSTOM_HID_IDLE = 0U,
    CUSTOM_HID_BUSY,
};

enum
{
    XUSB_IDLE = 0U,
    XUSB_EP0_RX_WAITING,
    XUSB_EP0_RX_CHATPAD
};
uint8_t  Report_buf[32];
uint8_t  headset_buf[20];
uint8_t  headset_conf_buf[20];
uint32_t Protocol;
uint32_t IdleState;
uint32_t AltSetting;
uint32_t IsReportAvailable;

u8 state;
u8 state_mic;
u8 state_headset;
u8 state_chatpad;
u8 state_chatpad;
u8 xusb_state;


static u32 endpoint0_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, 0x00, buffer, len);
}

u32 endpoint0_error(struct usb_device_t *usb_device)
{
    // usb_set_setup_phase(usb_device, USB_EP0_SET_STALL);
    usb_ep0_Set_Stall(usb_device2id(usb_device));
    return 0;
}

static u32 endpoint1_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, 0x01, buffer, len);
}

static void endpoint1_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    FirstCtrlMsgReceived();
//    printf("EP1 RX\n");
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x01);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 32, 0);
    //printf("EP1 RX LEN = %d\n", rx_len);

    for(int i=0; i< rx_len; i++)
    {
        //printf("%02X", ep_buffer[i]);
    }
    if (ep_buffer[0] == 0x00 && ep_buffer[1] == 8)
    {
        SetRumble(ep_buffer[3], ep_buffer[4]);
    }
    else if (ep_buffer[0] == 0x01 && ep_buffer[1] == 3)
    {
        SetPendingLEDPattern(ep_buffer[2]);
    }
}

static void endpoint1_init(const usb_dev usb_id)
{
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x01 | USB_DIR_IN);
    usb_g_ep_config(usb_id, 0x01 | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, ep_buffer, MAX_EP1_SIZE);

    ep_buffer = usb_get_ep_buffer(usb_id, 0x01);
    usb_g_set_intr_hander(usb_id, 0x01, endpoint1_rx_data);
    usb_g_ep_config(usb_id, 0x01, USB_ENDPOINT_XFER_INT, 1, ep_buffer, MAX_EP1_SIZE);
    usb_enable_ep(usb_id, 0x01);
}

static u32 endpoint2_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, 0x02, buffer, len);
}

static void endpoint2_rx_data(struct usb_device_t *usb_device, u32 ep)
{

    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x02);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 32, 0);
    //  printf("EP2 RX %d\n", rx_len);
    USB_AudioIn_NewPacket(ep_buffer);

}

static void endpoint2_init(const usb_dev usb_id)
{
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x02 | USB_DIR_IN);
    ASSERT(ep_buffer, "usb alloc ep buffer failed");
    //OVO JE ODLAZNI - JIELI SALJE
    usb_g_ep_config(usb_id, 0x02 | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, ep_buffer, 64);

    ep_buffer = usb_get_ep_buffer(usb_id, 0x02);
    ASSERT(ep_buffer, "usb alloc ep buffer failed");
    usb_g_set_intr_hander(usb_id, 0x02, endpoint2_rx_data);
    //OVO JE DOLAZNI - JIELI PRIMA
    usb_g_ep_config(usb_id, 0x02, USB_ENDPOINT_XFER_INT, 1, ep_buffer, 64);
    usb_enable_ep(usb_id, 0x02);

}

static u32 endpoint3_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, 0x03, buffer, len);
}
uint8_t start_mic = 0;

static void endpoint3_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    uint8_t stat[3] = {1,3,0};

    uint8_t headset_status0[] = {0, 4, 1, 0};
    uint8_t headset_status1[] = {1, 3, 0};
    uint8_t headset_status2[] = {2, 3, 0x0F};
    uint8_t headset_status3[] = {3, 5, 0xFF, 0, 0};
    uint8_t headset_status4[] = {4, 3, 0xFF};
    uint8_t headset_status5[] = {5, 3, 0};
    uint8_t *headset_status[] = { headset_status0, headset_status1, headset_status2,
                                  headset_status3, headset_status4, headset_status5,headset_status0, headset_status1, headset_status2,
                                  headset_status3, headset_status4, headset_status5
                                };
    uint8_t headset_status_lengths[12] = { 4, 3, 3, 5, 3, 3, 4, 3, 3, 5, 3, 3 };

    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x03);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 32, 0);
    start_mic=1;

    for(int i=0; i<6; i++)
    {
        if (USB_HeadsetStatusSend(headset_status[i],
                                  headset_status_lengths[i]) == 0)
        {
        }
    }

}

static void endpoint3_init(const usb_dev usb_id)
{
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x03 | USB_DIR_IN);
    usb_g_ep_config(usb_id, 0x03 | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, ep_buffer, MAX_EP3_SIZE);

    ep_buffer = usb_get_ep_buffer(usb_id, 0x03);
    usb_g_set_intr_hander(usb_id, 0x03, endpoint3_rx_data);
    usb_g_ep_config(usb_id, 0x03, USB_ENDPOINT_XFER_INT, 1, ep_buffer, MAX_EP3_SIZE);
    usb_enable_ep(usb_id, 0x03);
}

static u32 endpoint4_tx_data(struct usb_device_t *usb_device, const u8 *buffer, u32 len)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    return usb_g_intr_write(usb_id, 0x04, buffer, len);
}

static void endpoint4_rx_data(struct usb_device_t *usb_device, u32 ep)
{
    //printf("EP4 RX\n");
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x04);
    u32 rx_len = usb_g_intr_read(usb_id, ep, NULL, 32, 0);
}



static void endpoint4_init(const usb_dev usb_id)
{
    u8 *ep_buffer = usb_get_ep_buffer(usb_id, 0x04 | USB_DIR_IN);
    usb_g_ep_config(usb_id, 0x04 | USB_DIR_IN, USB_ENDPOINT_XFER_INT, 0, ep_buffer, MAX_EP4_SIZE);

    ep_buffer = usb_get_ep_buffer(usb_id, 0x04);
    usb_g_set_intr_hander(usb_id, 0x04, endpoint4_rx_data);
    usb_g_ep_config(usb_id, 0x04, USB_ENDPOINT_XFER_INT, 1, ep_buffer, MAX_EP4_SIZE);
    usb_enable_ep(usb_id, 0x04);


    state = CUSTOM_HID_IDLE;
    state_mic = CUSTOM_HID_IDLE;
    state_headset = CUSTOM_HID_IDLE;
    state_chatpad = CUSTOM_HID_IDLE;
    xusb_state = XUSB_IDLE;
}

u8 one = 0;
uint32_t device_serial = 31285053;
static u32 first_recv(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 ret = 0;
    //TODO PODESI BAFER
    u8 *read_ep = SecurityGetBuffer(setup->wLength);
    printf("First recv");
    if (xusb_state == XUSB_EP0_RX_WAITING)
    {
        usb_read_ep0(usb_id, read_ep, setup->wLength);

        //TODO UKLJUCI

        SecuritySendChallenge();



        xusb_state = XUSB_IDLE;
    }
    ret = USB_EP0_STAGE_SETUP;

    return ret;
}

static uint8_t temp[100];
u32 second_recv(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    printf("SECOND RECV");
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 ret = 0;
    u8 *read_ep = temp;

    usb_read_ep0(usb_id, read_ep, setup->wLength);

    printf("SECOND RECV LEN = %d", setup->wLength);

    xusb_state = XUSB_IDLE;
    ret = USB_EP0_STAGE_SETUP;

    return ret;
}

static u32 third_recv(struct usb_device_t *usb_device, struct usb_ctrlrequest *setup)
{
    printf("THIRD RECV");
    const usb_dev usb_id = usb_device2id(usb_device);
    u32 ret = 0;
    u8 *read_ep = Report_buf;

    usb_read_ep0(usb_id, read_ep, setup->wLength);

    ret = USB_EP0_STAGE_SETUP;

    return ret;
}
typedef u8 USB_STATUS;

enum USB_STATUS
{
    USB_OK = 0,
    USBD_FAIL
};

static u32 interface0_handler(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    printf("I0 REC %02x LOBYTE %02x", req->bRequest & 0x03, LOBYTE(req->wIndex));

}

static u32 interface1_handler(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    printf("I1 REC %02x LOBYTE %02x", req->bRequest & 0x03, LOBYTE(req->wIndex));
}

static u32 interface2_handler(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    //printf("I2 REC %02x LOBYTE %02x", req->bRequest & 0x03, LOBYTE(req->wIndex));
    if(LOBYTE(req->wIndex) == 2)
    {
        //  printf("CHATPAD I2 wValue = %d", req->wValue);
        chatpadUARTSendCmd(req->wValue);
    }
}

static u32 interface3_handler(struct usb_device_t *usb_device, struct usb_ctrlrequest *req)
{
    uint8_t  recipient = req->bRequest & USB_REQ_RECIPIENT_MASK;
    uint8_t *pbuf = NULL;
    uint16_t len = 0;
    uint16_t status_info = 0;
    uint32_t dwInfo = 0;
    USB_STATUS ret = USB_OK;
    printf("I3 REC %02x LOBYTE %02x", req->bRequest & 0x03, LOBYTE(req->wIndex));

    //printf("I3 req->bRequest = %02x", req->bRequest);

    // ------------------------------------------------------------
    // Security interface:
    // ------------------------------------------------------------
    switch (req->bRequest)
    {
    case 0x81: // Request Static Data
        if (SecurityGetStaticData(&pbuf, &len))
        {
            //printf("SecGetStaticData LEN=%d", len);
            usb_set_data_payload(usb_device, req, pbuf, len);
            // endpoint0_tx_data(usb_device, pbuf, len);
        }
        else
        {
            endpoint0_error(usb_device);
            ret = USBD_FAIL;
        }
        break;

    case 0x82: // Send Challenge 1
    case 0x87: // Send Challenge 2
        if (xusb_state == XUSB_IDLE && req->wLength <= EP0_BUFF_MAX_LEN)
        {
            xusb_state = XUSB_EP0_RX_WAITING;
            usb_set_setup_recv(usb_device, first_recv);
            // (void) USBD_CtlPrepareRx(pdev, SecurityGetBuffer(req->wLength), req->wLength);
        }
        else
        {
            endpoint0_error(usb_device);
            ret = USBD_FAIL;
        }
        break;

    case 0x83: // Get Response
        if (SecurityGetResponse(&pbuf, &len))
        {
            //printf("Get security response = %d", len);
            usb_set_data_payload(usb_device, req, pbuf, len);
            //endpoint0_tx_data(usb_device, pbuf, len);
        }
        else
        {
            //printf("Get security error");
            endpoint0_error(usb_device);
            ret = USBD_FAIL;
        }
        break;

    case 0x84: // Challenge Successful. Sent when the response was correct. If the response wasn't correct, the console just stops communicating with the device.
        u8 empty=0;
        usb_set_data_payload(usb_device, req, (uint8_t*) &empty, 0);
        break;

    case 0x86: // Get Response Status. It's always two bytes; the first is 01 if still busy, and 02 if ready. The second byte is always 00.

        if (SecurityGetResponseStatus())
        {
            //printf("STATUS 02");
            status_info = 2;
        }
        else
        {
            //printf("STATUS 01");
            status_info = 1;
        }
        usb_set_data_payload(usb_device, req, (uint8_t*) &status_info, 2);
        //endpoint0_tx_data(usb_device, (uint8_t*) &status_info, 2);
        break;
    }
    //printf("Returning ret=%02X", ret);
    return ret;

}


u32 hid_desc_config0(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    printf("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;
    memcpy(ptr, sHIDDescriptor0, sizeof(sHIDDescriptor0));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, interface0_handler) != *cur_itf_num)
    {
        ASSERT(0, "hid set interface_hander fail");
    }



    *cur_itf_num = *cur_itf_num + 1;
    endpoint1_init(usb_id);
    return sizeof(sHIDDescriptor0) ;
}

u32 hid_desc_config1(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{

    printf("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;
    memcpy(ptr, sHIDDescriptor1, sizeof(sHIDDescriptor1));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, interface1_handler) != *cur_itf_num)
    {
        ASSERT(0, "hid set interface_hander fail");
    }



    *cur_itf_num = *cur_itf_num + 1;
    endpoint2_init(usb_id);
    return sizeof(sHIDDescriptor1) ;
}

u32 hid_desc_config2(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    printf("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;
    memcpy(ptr, sHIDDescriptor2, sizeof(sHIDDescriptor2));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, interface2_handler) != *cur_itf_num)
    {
        ASSERT(0, "hid set interface_hander fail");
    }



    *cur_itf_num = *cur_itf_num + 1;
    endpoint3_init(usb_id);
    return sizeof(sHIDDescriptor2) ;
}

u32 hid_desc_config3(const usb_dev usb_id, u8 *ptr, u32 *cur_itf_num)
{
    //printf("hid interface num:%d\n", *cur_itf_num);
    u8 *_ptr = ptr;
    memcpy(ptr, sHIDDescriptor3, sizeof(sHIDDescriptor3));
    ptr[2] = *cur_itf_num;
    if (usb_set_interface_hander(usb_id, *cur_itf_num, interface3_handler) != *cur_itf_num)
    {
        ASSERT(0, "hid set interface_hander fail");
    }


    *cur_itf_num = *cur_itf_num + 1;

    endpoint4_init(usb_id);
    return sizeof(sHIDDescriptor3) ;
}

