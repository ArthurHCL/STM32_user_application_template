#include "usb_desc.h"
#include "usb_type.h"
/* user config include */
#include "main.h"



/* USB Standard Device Descriptor */
const u8 CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] = {
    0x12,                       /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
    0x00,                       /* bcdUSB */
    0x02,
    0x00,                       /* bDeviceClass */
    0x00,                       /* bDeviceSubClass */
    0x00,                       /* bDeviceProtocol */
    0x40,                       /* bMaxPacketSize40 */
    APPLICATION_CODE_VID_L,     /* idVendor */
    APPLICATION_CODE_VID_H,
    APPLICATION_CODE_PID_L,     /* idProduct */
    APPLICATION_CODE_PID_H,
    0x00,                       /* bcdDevice rel. 2.00 */
    0x02,
    1,                          /* Index of string descriptor describing manufacturer */
    2,                          /* Index of string descriptor describing product */
    3,                          /* Index of string descriptor describing the device serial number */
    0x01                        /* bNumConfigurations */
}; /* CustomHID_DeviceDescriptor */



/* USB Configuration Descriptor */
/* All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const u8 CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] = {
    0x09,                              /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    CUSTOMHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,                              /* bNumInterfaces: 1 interface */
    0x01,                              /* bConfigurationValue: Configuration value */
    0x00,                              /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,                              /* bmAttributes: Bus powered */
                                       /* Bus powered: 7th bit, Self Powered: 6th bit, Remote wakeup: 5th bit, reserved: 4..0 bits */
    0x96,                              /* MaxPower 100 mA: this current is used for detecting Vbus */
    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,                      /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE,       /* bDescriptorType: HID */
    0x10,                      /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                      /* bCountryCode: Hardware target country */
    0x01,                      /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                      /* bDescriptorType */
    CUSTOMHID_SIZ_REPORT_DESC, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
    0x82,                         /* bEndpointAddress: Endpoint Address (IN) */               
                                  // bit 3...0 : the endpoint number
                                  // bit 6...4 : reserved
                                  // bit 7     : 0(OUT), 1(IN)
    0x03,                         /* bmAttributes: Interrupt endpoint */
    0x40,//0x02,                  /* wMaxPacketSize: 20 Bytes max */
    0x00,
    0x01,                         /* bInterval: Polling Interval (1 ms) */
    /* 34 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
                                  /* Endpoint descriptor type */
    0x01,                         /* bEndpointAddress: */
                                  /* Endpoint Address (OUT) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    0x40,//0x02,                  /* wMaxPacketSize: 20 Bytes max  */
    0x00,
    0x01,                         /* bInterval: Polling Interval (1 ms) */
    /* 41 */
}; /* CustomHID_ConfigDescriptor */



const u8 CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] = {
    0x05, 0x8C,       // USAGE_PAGE (Bar Code Scanner page)
    0x09, 0x01,       // USAGE (Bar Code Badge Reader)
    0xA1, 0x01,       // COLLECTION (Application)

    // The Input report
    0x09, 0x03,       //     USAGE (Dumb Bar Code Scanner)
    0x15, 0x00,       //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00, //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,       //     REPORT_SIZE (8)
    0x95, CUSTOM_HID_IN_REPORT_SIZE,       //     REPORT_COUNT (...)
    0x81, 0x02,       //     INPUT (Data, Var, Abs)

    // The Output report
    0x09, 0x04,       //     USAGE (Cordless Scanner Base)
    0x15, 0x00,       //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00, //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,       //     REPORT_SIZE (8)
    0x95, CUSTOM_HID_OUT_REPORT_SIZE,       //     REPORT_COUNT (...)
    0x91, 0x02,       //     OUTPUT (Data, Var, Abs)

    // The Feature report
#if 0
    0x09, 0x05,       // USAGE ID - Vendor defined
    0x15, 0x00,       // LOGICAL_MINIMUM (0)
    0x26, 0x00, 0xFF, // LOGICAL_MAXIMUM (255)
    0x75, 0x08,       // REPORT_SIZE (8)
    0x95, 0x02,       // REPORT_COUNT (2)
    0xB1, 0x02, 
#endif

    0xC0 /* END_COLLECTION */
}; /* CustomHID_ReportDescriptor */



/* USB String Descriptors (optional) */
const u8 CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] ={
    CUSTOMHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
}; /* LangID = 0x0409: U.S. English */



const u8 CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] = {
    CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType */
    // Manufacturer: "STMicroelectronics" 
    'M', 0, 'y', 0, 'U', 0,'S', 0,'B', 0, '_', 0, 'H', 0,'I',0,'D',0
};



const u8 CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] = {
    CUSTOMHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'U', 0, 'S', 0, 'E', 0, 'R', 0, '_', 0, '_', 0,
    'A', 0, 'P', 0, 'P', 0, 'L', 0, 'I', 0, 'C', 0, 'A', 0, 'T', 0, 'I', 0, 'O', 0, 'N', 0
};



u8 CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] = {
    CUSTOMHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'x', 0, 'x', 0, 'x', 0,'x', 0,'x', 0, 'x', 0,
    'x', 0, 'x', 0, 'x', 0,'x', 0,'x', 0, 'x', 0
};

