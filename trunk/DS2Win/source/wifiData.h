#if !defined(WIFIDATA_H)
#define WIFIDATA_H
typedef struct wifid {
  char ssid[3][32];
  char wepKey[3][13];
  unsigned long ipAddress[3];
  unsigned long  gateway[3];
  unsigned long  dns1[3];
  unsigned long  dns2[3];
  unsigned char subnetLength[3];
  bool read;
  } wifidata;

/* Address of the shared CommandControl structure */
#define wifiData ((wifidata*)((uint32)(IPC) + sizeof(TransferRegion)))
//#define timeData ((timedata*)((uint32)(wifiData) + sizeof(wifidata)))
#endif 
