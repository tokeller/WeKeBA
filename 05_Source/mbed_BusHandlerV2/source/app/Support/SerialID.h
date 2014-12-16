#ifndef SERIALID
#define SERIALID
#include "mbed.h"

// Location for IAP commands (like reading the serial number)
#define IAP_LOCATION 0x1FFF1FF1
typedef void (*IAP)(unsigned long [], unsigned long[] );



/**	Function returning the serial number of a device. 
  * @return 	8 bytes of the full serial that are unique for each device
  *
  */
uint32_t getSerialNumber(void);

#endif
