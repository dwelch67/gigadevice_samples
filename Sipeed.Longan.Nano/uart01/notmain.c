
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void PUT32( unsigned int, unsigned int);
unsigned int GET32 ( unsigned int );
void  dummy ( unsigned int );
unsigned int read_mtime ( void );

#define USART0_BASE 0x40013800
#define USART0_STAT (USART0_BASE+0x00)
#define USART0_DATA (USART0_BASE+0x04)
#define USART0_BAUD (USART0_BASE+0x08)
#define USART0_CTL0 (USART0_BASE+0x0C)
#define USART0_CTL1 (USART0_BASE+0x10)
#define USART0_CTL2 (USART0_BASE+0x14)
#define USART0_GP   (USART0_BASE+0x18)

//#define AFIO_BASE   0x40010000

#define GPIOA_BASE  0x40010800
#define GPIOA_CTL1  (GPIOA_BASE + 0x04)

#define RCU_BASE    0x40021000
#define RCU_APB2EN  (RCU_BASE + 0x18)

//-------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    ra=GET32(RCU_APB2EN);
    ra|=1<<2;  //GPIOA
    ra|=1<<14; //USART0
    PUT32(RCU_APB2EN,ra);

    //USART0_TX  USART0_REMAP = 0  PA9
    //USART0_RX  USART0_REMAP = 0  PA10

    ra=GET32(GPIOA_CTL1);
    ra&=(~(0xF<<4)); //PA9
    ra|=  (0xA<<4) ; //PA9
    //ra&=(~(0xF<<8)); //PA10
    //ra|=  (0xA<<8) ; //PA10
    PUT32(GPIOA_CTL1,ra);

    PUT32(USART0_CTL0,0x00000000);
    PUT32(USART0_CTL1,0x00000000);
    PUT32(USART0_CTL2,0x00000000);
    //8000000 / (16*115200) = 4.34  0.34*16 = 5
    PUT32(USART0_BAUD,0x00000045);
    //8000000 / (16*9600) = 52.08 0.08*16 = 1
    //PUT32(USART0_BAUD,0x00000341);
    PUT32(USART0_CTL0,0x00000008); //TX only
    PUT32(USART0_CTL0,0x00002008); //TX only

if(0)
{
    while(1)
    {
        PUT32(USART0_DATA,0x55);
        while(1)
        {
            rx=GET32(0xD1000000);
            if((rx&0x400000)!=0) break;
        }
        PUT32(USART0_DATA,0x56);
        while(1)
        {
            rx=GET32(0xD1000000);
            if((rx&0x400000)==0) break;
        }
    }
}

    for(rx=0x30;;rx++)
    {
        rx&=0x37;
        while(1)
        {
            ra=GET32(USART0_STAT);
            if(ra&(1<<7)) break;
        }
        PUT32(USART0_DATA,rx);
    }

    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/*

dfu-util -a 0 -D notmain.bin -s 0x08000000

*/
