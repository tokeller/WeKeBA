#include "SerialID.h"

IAP iap_entry = (IAP) IAP_LOCATION;
unsigned long command[5] = {0,0,0,0,0};
unsigned long result[5] = {0,0,0,0,0};


uint32_t getSerialNumber(void){
    command[0] = 58;  // read device serial number
    iap_entry(command, result);
    if (result[0] == 0) {
				// only take 8 relevant bytes to conserve memory
				return (((result[1] & 0x0000ffff)<< 16) | (result[3] & 0x0000ffff));
    } else {
        return 0;
    };
};
