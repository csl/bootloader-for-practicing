/////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2005 2111010@uestc. All rights reserved.
//
// Module name:
//      scc.c
//
// Description:
//
//
// Author:
//      lijilong
//
// Created:
//      2005
//
///////////////////////////////////////////////////////////////////////////////

#include "net.h"
#include "scc.h"

uchar SendFrameBuf[100];
uchar ReadFrameBuf[100];
void wait(int sec)
{
     long timeout;
     timeout = GetTime() + sec;
     while(GetTime()<timeout);
}

bool EthTx(void *txPktBuf, int len)
{
	short  *endPtr;
        uchar *s;
        long timeout;
        int numPages;
        char alloc;
        int i,k,cntWord;
        numPages=(len&(~1)+5)>>8;
        // Allocate memory for TX
        BANK_SELECT = BANK2;
        while((IO_MMUCR&0x0001));	
        IO_MMUCR = ALLOCMEM|numPages;
        for(k=0;k<5;k++);
        while(!(IO_ISR&0x08))
           {
               IO_MMUCR=0x0040;
               for(k=0;k<10;k++);
               IO_MMUCR=ALLOCMEM|numPages;
               for(k=0;k<5;k++);
           }
        //printf("allocate memmory sucessfully\n");

        // copy allocated packet number into packet number reg.
        alloc = IO_ARR;
        if((alloc&0x80))
           printf("fail to allocate memort\n");
        IO_PNR=alloc&0x3f;
        
        // load pointer register,aoutincrease from 0x00000000 address.
        IO_PTR = 0x4000;
        IO_DATA = 0x0000;            //status word
        IO_DATA = len + 6;           //len word;
        cntWord = len>>1;
        s=txPktBuf;
	for (i=0;i<cntWord;i++){
		IO_DATA = *((short *)s);
                for(k=0;k<5;k++);
                s=s+2;
	}
        //process odd byte
         if((len &1))
             IO_DATA=0x2000|(*s);
         else
             IO_DATA=0x0000;
        // enqueue packet number into tx fifo to start transition.
        IO_MMUCR = ENQPACKET;
        
        // wait for transmit successfully
        //BANK_SELECT=BANK0;
        timeout=GetTime()+5;
        while(!(IO_ISR&0x04)){
             if(GetTime()>timeout)
                 printf("transmit failure\n");
               break;
        }
        IO_ISR=0x04;
        //printf("transmit successfully\n");
        printf(".");
        BANK_SELECT=BANK2;
        while((IO_MMUCR&0x0001));//wait for releasing completesy;
        for(k=0;k<5;k++);
        /*if(IO_ISR&0x04)   //transmit successfully,TXEMPTY is set;
           {
              printf("transmit successfully\n");
              IO_ISR = 0x04; //clear TXEMPTY bit.
           }
        else              //fail to transmit
          {
             printf("transmit failure,release the failure packet\n then renable txen bit");
             
             IO_TCR = IO_TCR | 0x0001;  //reenable txen bit;
          }*/
             
          
	return true;
}	// EthTx.


int EthRx(void *rxPktBuf)
{
	int		i;
	char	*s;
        long timeout;
        unsigned long cnt;
        short rcvdata;
        unsigned long length;
        unsigned short leftshort;
        unsigned char leftchar;

        //enable TXEN to start to receive.
        BANK_SELECT = BANK0;
        IO_RCR = IO_RCR | RXEN;
        
        //loop until something has been received in 5 second.
        BANK_SELECT = BANK2;
        timeout = GetTime() + 5;
        while(!(IO_ISR & 0x01))
        {
            if(GetTime()>timeout)
            {
                printf("receive has time out\n");
                return false;
            }
        } 
                           
	// receive packets into rxPktBuf.
        MemSet(rxPktBuf,0,MAX_PKT_SIZE);
        IO_PTR = (RCV | AUTOINCR | READ);
        rcvdata = IO_DATA;
        rcvdata = IO_DATA;
        length = (rcvdata&0x07ff);
        cnt = length>>1;
        cnt = cnt-3;
        s = (char *)rxPktBuf;
        for(i=0;i<cnt;i++)
        {
            rcvdata = IO_DATA;
            *((short *)s) = rcvdata;
            s=s+2;
        }
        rcvdata = IO_DATA;
        if(rcvdata&0x2000){
             length=length-5;
             *s=rcvdata&0x00ff;
        }
        else
             length=length-6;
        //receive completely,release memory;
        IO_MMUCR = 0x0080;
        for(i=0;i<5;i++);
        RxPacketHandle((char *)rxPktBuf,length);               
	return true;
}	// EthRx.

bool EthInit()
{
        int i;
        short  iobase;
        ushort phyid1=0;
        ushort phyid2=0;
        ushort phycfg1=0;
        ushort temp,status=0;
        uchar ch;

        //#define GAFREG0_VALUE 0x80000000
        GAFREG0_L = GAFREG0_L | GAFREG0_VALUE;	//GP15 alternate function
        //#define GAFREG1_VALUE 0x00000A2A
        GAFREG1_U = GAFREG1_U | GAFREG1_VALUE;	//GP48,49,50,52,53 alternate function
        //#define GAFREG2_VALUE 0x80000000
        GAFREG2_L = GAFREG2_VALUE;			//GPIO 79 alternate function
        GPDREG0 = GPDREG0 | GPD0_VALUE;		//GP15 out direction
        GPDREG1 = GPDREG1 | GPD1_VALUE;		//GP49 out direction
        GPDREG2 = GPD2_VALUE;			//GP79 out direction

        //Enable chip to start to work if it exists.
	MemSet((char *)0x04000000, 0x0, 2);

        /*//read BANK_SELECT register.
        //check if lan91 exist.
        BANK_SELECT = BANK0;
	if ((BANK_SELECT&0xFF00) != 0x3300){
		printf("Can't access to Memory of lan91.\n");
		return false;
	}
	else 
        printf("can access to memory of lan91.\n\n");
        
        // check lan91 Chip ID .
        BANK_SELECT = BANK3;
        if ((IO_REV&0xffff)!=0x3391){
                printf("\nEthernet Chip is not lan91. Are you use lan991.\n");
                return false;
        }
        else
           printf("lan91 chip id is %x\n\n",IO_REV);*/
        
        ReadReg(PHY_CFG1,&phycfg1);
        phycfg1=phycfg1|0x8000;

        WriteReg(PHY_CFG1,phycfg1);	//Enter dislink mode
        WriteReg(PHY_CNTR,0x2100);
        
        //set STRIP_CRC,DUPLEX,100M SPEED bit
     
        BANK_SELECT=BANK0;
        IO_RCR = 0x0300;       //STRIP_CRC,RXEN.
        //Tx enable.enable SWFDUP mode.PAD_EN enable.
        IO_TCR = 0x8081;
        IO_RPCR = 0x3000;      //100M,full duplex mode
        
	//EPH_POWER enable;add wait states on ardy.
        BANK_SELECT = BANK1;
	IO_CR = 0x8000;        //EPH_POWER enabled

        /*//check BASE address register value(default to 0x1801h)
        iobase = IO_BAR;
        printf("the base address register is %x\n",iobase);*/

	//set MAC address register.
        MAC_ADDR0 = clientEther[0];
        MAC_ADDR1 = clientEther[1];
        MAC_ADDR2 = clientEther[2];
        MAC_ADDR3 = clientEther[3];
        MAC_ADDR4 = clientEther[4];
        MAC_ADDR5 = clientEther[5];
        ch = MAC_ADDR5;

        printf("the mac address is %x,%x,%x,%x,%x,%x\n\n",MAC_ADDR0,MAC_ADDR1,MAC_ADDR2,MAC_ADDR3,MAC_ADDR4,MAC_ADDR5);
        
	//Enable Autorelease,enbable transmit error interrupt bit into EPH interrupt status.
	IO_CTR = 0x08E0;
        printf("the value of IO_CTR is %x\n",IO_CTR);
       
        //Enable multicast address.
        BANK_SELECT = BANK3; 
        IO_MT01 = 0xffff;
        IO_MT23 = 0xffff;
        IO_MT45 = 0xffff;
        IO_MT67 = 0xffff;
	
        //Generate interrupt when the number of bytes in memory for received packets exceed 64*threshold
        IO_ERCV = 0x001f;
    
        //test PHY ID
         ReadReg(PHY_ID1,&phyid1);
         ReadReg(PHY_ID2,&phyid2);
         printf("phyid1 is %x\n",phyid1);
         printf("phyid2 is %x\n",phyid2);

        //PHY is set as 100m speed,duplex
        /* WriteReg(PHY_CNTR,0x2100);
         ReadReg(PHY_CNTR,&phyid1);
         printf("PHY_CNTR is %x\n",phyid1);
         WriteReg(PHY_CFG1,0x0020);
         WriteReg(PHY_CFG2,0x0000);*/

         BANK_SELECT=BANK2;
         while((IO_MMUCR&0x0001));
         IO_MMUCR=0x0040;
         while((IO_MMUCR&0x0001));

         for(i=0;i<5;i++);

         /*ReadReg(STATUS_OUT,&phyid1);
         if(phyid1&0x4000)
             printf("link fail\n");       
         ReadReg(PHY_STATUS,&phyid2);
         if(phyid2&0x0004)
             printf("link successfully\n");*/
	return true;
}	// EthInit.

void SetMiiFrame(uchar * framebuf,uchar read,uchar write,uchar regadd,ushort data)
{
     uchar * ptr;
     ptr = framebuf;
     *((ulong *)ptr) = 0xffffffff;//IDLE
     ptr+=4;
     if(read){
        *ptr = 0x60;//read
        ptr+=1;
        *ptr=(regadd << 2);
        ptr+=1;
        *((ushort *)ptr)=0x0000;
     }
     else if(write){
        *ptr = 0x50;//write
        ptr+=1;
        *ptr=(regadd << 2)|0x02;
        ptr+=1;
        *ptr=(uchar)(data>>8);
        ptr+=1;
        *ptr=(uchar)(data&0x00ff);
     }
}

void SendReadFrame(uchar *framebuf,ushort * ptr)
{
     int i,j,k;
     ushort temp=0;
     uchar read=0;
     BANK_SELECT = BANK3;
     *ptr=0;
     MemSet(ReadFrameBuf,0x0,100);
     //enable MDO
     IO_MGMT = 0x0000;
     //first send 5 bytes
     for(i=0;i<8;i++,framebuf++){
        for(j=0;j<8;j++){
            IO_MGMT=IO_MGMT | MDOE;
            IO_MGMT=~MDCLK&IO_MGMT;
            temp=((*framebuf)>>(7-j))&0x01;
            IO_MGMT=temp | IO_MGMT;
            
            for(k=0;k<10;k++);//delay
            temp = IO_MGMT|MDCLK|MDOE|temp;
            IO_MGMT=temp;//set mkclk to 1 generate rise edge
            for(k=0;k<10;k++);//delay to send successfully
            read=IO_MGMT&0x0002;
            read=read>>1;
            read=read<<(7-j);
            ReadFrameBuf[i]=ReadFrameBuf[i]|read;
            IO_MGMT=IO_MGMT&~MDOE;
            IO_MGMT=IO_MGMT&~MDO;
     }
       
    }
   *ptr=((ushort)((ReadFrameBuf[5]&0x01)|*ptr))<<15;
   *ptr=(((ushort)ReadFrameBuf[6])<<7)|*ptr;
   *ptr=*ptr|(ReadFrameBuf[7]>>1);   
 
}


void SendWriteFrame(uchar *SendFrame)
{
     int i,j,k;
     ushort temp;
     BANK_SELECT=BANK3;
     IO_MGMT=0x0000;
     for(i=0;i<8;i++,SendFrame++){
         for(j=0;j<8;j++){
            IO_MGMT=IO_MGMT | MDOE;
            IO_MGMT=~MDCLK&IO_MGMT;
            temp=((*SendFrame)>>(7-j))&0x01;
            IO_MGMT=temp|IO_MGMT;
            for(k=0;k<10;k++);
            temp=IO_MGMT|MDCLK|MDOE|temp;
            IO_MGMT=temp;
            for(k=0;k<10;k++);
            IO_MGMT=IO_MGMT&~MDOE;
            IO_MGMT=IO_MGMT&~MDO;
         }
    }
    //final clock
    IO_MGMT=IO_MGMT|MDOE;
    IO_MGMT=~MDCLK&IO_MGMT;
    for(k=0;k<10;k++);
    IO_MGMT=IO_MGMT|MDCLK|MDOE;
}
    
void ReadReg(uchar reg,ushort * ptr)
{
    int i;
    SetMiiFrame(SendFrameBuf,1,0,reg,0);
    //printf("Read frame to be send is");
    for(i=0;i<8;i++)
       printf("%x ",SendFrameBuf[i]);
    SendReadFrame(SendFrameBuf,ptr);
    
}
void WriteReg(uchar reg,ushort data)
{
   int i;
   SetMiiFrame(SendFrameBuf,0,1,reg,data);
  // printf("write frame is");
   for(i=0;i<8;i++)
     printf("%x",SendFrameBuf[i]);
   SendWriteFrame(SendFrameBuf);
   
}
 
void EthHalt(void)
{
}

void EthRestart(void)
{
}
