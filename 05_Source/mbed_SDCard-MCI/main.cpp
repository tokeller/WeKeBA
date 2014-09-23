#include "mbed.h"
#include "MCIFileSystem.h"

DigitalInOut pin24(p24);
DigitalInOut pin25(p25);
DigitalInOut pin26(p26);
DigitalInOut pin27(p27);
DigitalOut pin28(p28);
DigitalOut pin29(p29);
DigitalOut pin30(p30);
Serial pcSerial(USBTX, USBRX);
MCIFileSystem mcifs("mci");

 int main() {
     pcSerial.baud(9600);
     pcSerial.printf("main()\n");
      
     pcSerial.printf("Please insert a SD/MMC card\n");
     while(!mcifs.cardInserted()) {
         wait(0.5);
     }
     
     pcSerial.printf("Found SD/MMC card, writing to /mci/myfile.txt ...\n");
     wait(5);
     pcSerial.printf("Wait done\n");
     FILE *fp = fopen("/mci/myfile.txt", "w");
     if (fp != NULL) {
         fprintf(fp, "Hello World!\n");
         fclose(fp);
         pcSerial.printf("Wrote to /mci/myfile.txt\n");
     } else {
         pcSerial.printf("Failed to open /mci/myfile.txt\n");
     }
 }