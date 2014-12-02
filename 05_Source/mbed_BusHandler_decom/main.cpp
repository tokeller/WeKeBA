#include "mbed.h"
#include "BusHandler.h"
#include "cmsis_os.h"


//#define SENSOR
#define LOGGER

//CAN can1(p9, p10);
Serial pcSerial(USBTX, USBRX);

char array[16]  = {65,65,65,65,65,65,65,65,66,66,66,66,66,66,66,66};

char array1[16] = {67,67,67,67,67,67,67,67,68,68,68,68,68,68,68,68};

char array2[16] = {69,69,69,69,69,69,69,69,70,70,70,70,70,70,70,70};

char array3[16] = {65,65,65,65,65,65,65,65};

char array4[16] = {67,67,67,67,67,67,67,67};

char array5[16] = {69,69,69,69,69,69,69,69};

DigitalOut myled(LED1);

int main() {
	
		pcSerial.baud(115200);
		pcSerial.printf("starting\n");
		#ifdef LOGGER
			osThreadDef(processMessagesLogger, osPriorityNormal, DEFAULT_STACK_SIZE);
			pcSerial.printf("thread creating\n");
			osThreadCreate(osThread(processMessagesLogger), NULL);  
			pcSerial.printf("thread created\n");
			osDelay(1000);
		#endif
		#ifdef SENSOR
			osThreadDef(processMessagesSensor, osPriorityNormal, DEFAULT_STACK_SIZE);
			pcSerial.printf("thread creating\n");
			osThreadCreate(osThread(processMessagesSensor), NULL);  
			pcSerial.printf("thread created\n");
			osDelay(5000);
			/*sendMessage(16, array, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
			sendMessage(16, array1, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
			sendMessage(16, array2, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
	*/
			sendMessage(8, array3, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
		/*	sendMessage(8, array4, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
			sendMessage(8, array5, 0x01, IMPACT_STD_SINGLE);
			osDelay(2000);
			*/printf("done\n");
		#endif
		while(1){
			#ifdef LOGGER
			msgData_t inbound = getInboundMessage();
			//printf("checked queue\n");
			// Message from outbound queue received
			if (inbound.msgType != NULL_MESSAGE){
				printf("inbound: %s\n",inbound.payload+0x00);
			} else {
				//printf("nothing received...\n");
			}
			#endif
			//printf("status %d\n",ev.status);
		}
		//deviceType_t log = LOGGER;
		/*deviceType_t log = SENSOR;
		init(log);
	
		message_t msg;
		msg.msgType = GET_SENSOR_SERIAL_BC;
		sendMsg(msg);
		CANMessage mesg;
		while(1) {
				if(can1.read(mesg)) {
						pcSerial.printf("Message received: %s\n", mesg.data);
						myled = !myled;
				} else {
					setFilterForID(log,0x0000);
				}
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
    }*/
}
