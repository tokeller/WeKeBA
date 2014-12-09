#include "BusProtocol.h"


CAN can1(p9, p10);
CANMessage msgIn;	

outputRingbuf recMsgs;

extern Queue <CANMessage, 50> inQueue;
extern Queue <CANMessage, 50> outQueue;
extern MemoryPool<CANMessage, 50> mpoolOutQueue;
extern MemoryPool<CANMessage, 50> mpoolInQueue;

extern Serial pcSerial;


int CAN_init(void){
	can1.reset();
	if(can1.frequency(1000000)){
		pcSerial.printf("Freq 1MHz\n");
	}
	can1.mode(CAN::Normal);
	can1.attach(&CANIRQHandler);
	recMsgs.count = 0;
	recMsgs.read_pos = 0;
	recMsgs.write_pos = 0;
	return 1;	
}

void CANIRQHandler(void){
  __disable_irq;
	if(can1.read(msgIn)){
		storeRecMessages(msgIn);
	}
	__enable_irq;
}

uint32_t storeRecMessages(CANMessage sentMsg){
	if (recMsgs.count < BUFFER_SIZE){
		recMsgs.queue[recMsgs.write_pos].len = sentMsg.len;
		recMsgs.queue[recMsgs.write_pos].format = sentMsg.format;
		recMsgs.queue[recMsgs.write_pos].id = sentMsg.id;
		memcpy(recMsgs.queue[recMsgs.write_pos].data,sentMsg.data,sentMsg.len);
		recMsgs.count++;
		if (recMsgs.write_pos < BUFFER_SIZE - 1) {
			recMsgs.write_pos++;
		} else {
			recMsgs.write_pos = 0;
		}
		
		
	} else {
		//pcSerial.printf("Sent messages buffer overflow\n");
	}	
	return 1;
}

CANMessage dequeueOutput (void){
	CANMessage msg;
	if (recMsgs.count > 0){
		__disable_irq();
		msg.format = recMsgs.queue[recMsgs.read_pos].format;
		msg.id = recMsgs.queue[recMsgs.read_pos].id;
		msg.len = recMsgs.queue[recMsgs.read_pos].len;
		msg.type = recMsgs.queue[recMsgs.read_pos].type;
		memcpy(msg.data,recMsgs.queue[recMsgs.read_pos].data,recMsgs.queue[recMsgs.read_pos].len);
		recMsgs.count--;
		if (recMsgs.read_pos < BUFFER_SIZE - 1){
			recMsgs.read_pos++;
		} else {
			recMsgs.read_pos = 0;
		}
		__enable_irq();
	} else {
		msg.id = 0;
	}
	return msg;
}

int sendMessage(CANMessage msg){
	return (can1.write(msg));
}

/**
  * Setup acceptance filter on CAN
  */
void setExtGrpCANFilter (uint32_t id_low, uint32_t id_high)  {
  static int CAN_std_cnt = 0;
  static int CAN_ext_cnt = 0;
         uint32_t buf0, buf1;
         int cnt1, cnt2, bound1;

  /* Acceptance Filter Memory full */
  if ((((CAN_std_cnt + 1) >> 1) + CAN_ext_cnt) >= 512)
    return;                                       /* error: objects full */

  /* Setup Acceptance Filter Configuration 
    Acceptance Filter Mode Register = Off  */                                 
  LPC_CANAF->AFMR = 0x00000001;
	
                                   /* Add mask for extended identifiers */
  id_low  |= (0 << 29);                        /* Add controller number, 0 for CAN1, 1 for CAN2 */
  id_high |= (0 << 29);                        /* Add controller number, 0 for CAN1, 1 for CAN2 */

  cnt1 = ((CAN_std_cnt + 1) >> 1);
  cnt2 = 0;
  while (cnt2 < CAN_ext_cnt)  {                /* Loop through extended existing masks */
    if (LPC_CANAF_RAM->mask[cnt1] > id_low)
      break;
    cnt1++;                                    /* cnt1 = U32 where to insert new mask */
    cnt2++;
  }

  buf0 = LPC_CANAF_RAM->mask[cnt1];            		 /* Remember current entry */
  LPC_CANAF_RAM->mask[cnt1] = id_low;              /* Insert lower bound */
	LPC_CANAF_RAM->mask[cnt1 + 1] = id_high;				 /* Insert upper bound */
	printf("lower bound %d %x\n",cnt1,LPC_CANAF_RAM->mask[cnt1]);
	printf("upper bound %d %x\n",cnt1 + 1,LPC_CANAF_RAM->mask[cnt1 + 1]);
  CAN_ext_cnt++;

  bound1 = CAN_ext_cnt - 1;
  /* Move all remaining extended mask entries one place up */
  while (cnt2 < bound1)  {
    cnt1++;
    cnt2++;
    buf1 = LPC_CANAF_RAM->mask[cnt1];
    LPC_CANAF_RAM->mask[cnt1] = buf0;
    buf0 = buf1;
  }        
   
  /* Calculate std ID start address (buf0) and ext ID start address (buf1) */
  buf0 = ((CAN_std_cnt + 1) >> 1) << 2;
  buf1 = buf0 + (CAN_ext_cnt << 3);

  /* Setup acceptance filter pointers */
  LPC_CANAF->SFF_sa     = 0;
  LPC_CANAF->SFF_GRP_sa = buf0;
  LPC_CANAF->EFF_sa     = buf0;
  LPC_CANAF->EFF_GRP_sa = buf0;
  LPC_CANAF->ENDofTable = buf1;

  LPC_CANAF->AFMR = 0x00000000;                  /* Use acceptance filter */
}
