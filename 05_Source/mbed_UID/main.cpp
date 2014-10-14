/*
 * How to read LPC1768 part ID and device serial number
 * Dan Minear
 * 2012-11-08
 */
 
#include "mbed.h"
 
#define IAP_LOCATION 0x1FFF1FF1
 
typedef void (*IAP)(unsigned long [], unsigned long[] );
IAP iap_entry = (IAP) IAP_LOCATION;
 
DigitalOut myled(LED1);
 
int main() {
    unsigned long command[5] = {0,0,0,0,0};
    unsigned long result[5] = {0,0,0,0,0};
 
    printf("\r\nStart...\r\n");
    
    // See User Manual section 32.8.5
    command[0] = 54;    // partID
    printf("\r\nPart ID: (should be 0x2601 3F37 for LPC1768)\r\n");
    iap_entry(command, result);
    if (result[0] == 0) {
        for(int i = 1; i < 2; i++) {
            printf( "0x%x\r\n", result[i] );
        }
    } else {
        printf("Status error!\r\n");
    }
    
    // See User Manual section 32.8.7
    command[0] = 58;  // read device serial number
    printf("\r\nSerial number:\r\n");
    iap_entry(command, result);
    if (result[0] == 0) {
        for(int i = 1; i < 5; i++) {
            printf( "0x%x\r\n", result[i] );
        }
    } else {
        printf("Status error!\r\n");
    }
        
    printf( "\r\nEnd\r\n" );
        
    // and now back to the default new project, just flash a LED
    while(1) {
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
    }
}