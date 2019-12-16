
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
#define RCU_CTL     (RCU_BASE + 0x00)
#define RCU_CFG0    (RCU_BASE + 0x04)
#define RCU_APB2EN  (RCU_BASE + 0x18)

static void uart_init ( void )
{
    unsigned int ra;

    ra=GET32(RCU_APB2EN);
    ra|=1<<2;  //GPIOA
    ra|=1<<14; //USART0
    PUT32(RCU_APB2EN,ra);

    //USART0_TX  USART0_REMAP = 0  PA9
    //USART0_RX  USART0_REMAP = 0  PA10

    ra=GET32(GPIOA_CTL1);
    ra&=(~(0xF<<4)); //PA9
    ra|=  (0xA<<4) ; //PA9
    ra&=(~(0xF<<8)); //PA10
    ra|=  (0xA<<8) ; //PA10
    PUT32(GPIOA_CTL1,ra);

    PUT32(USART0_CTL0,0x00000000);
    PUT32(USART0_CTL1,0x00000000);
    PUT32(USART0_CTL2,0x00000000);
    //108000000 / (16*115200) = 58.59  0.59 * 16 = 9
    PUT32(USART0_BAUD,0x000003A9);
    PUT32(USART0_CTL0,0x0000000C); //RXTX
    PUT32(USART0_CTL0,0x0000200C); //RXTX
}

static void uart_send ( unsigned int data )
{
    unsigned int ra;

    while(1)
    {
        ra=GET32(USART0_STAT);
        if(ra&(1<<7)) break;
    }
    PUT32(USART0_DATA,data);
}

static unsigned int uart_recv ( void )
{
    unsigned int ra;
    unsigned int rb;
    while(1)
    {
        ra=GET32(USART0_STAT);
        if(ra&(1<<5)) break;
    }
    rb=GET32(USART0_DATA);
    return(rb);
}

static void hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}

static void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}


static void clock_init ( void )
{
    unsigned int ra;

    //first use the extneral 8MHz clock

    ra=GET32(RCU_CTL);
    ra|=1<<16;
    PUT32(RCU_CTL,ra);
    while(1)
    {
        ra=GET32(RCU_CTL);
        if(ra&(1<<17)) break;
    }

    ra=GET32(RCU_CFG0);
    ra&=(~3);
    ra|=(1);
    PUT32(RCU_CFG0,ra);

    while(1)
    {
        ra=GET32(RCU_CFG0);
        if(((ra>>2)&3)==1) break;
    }

    //second setup the PLL

    //108MHz from 8MHz crystal.  (8/2)*27 = 108
    //11010: (PLL source clock x 27)

    ra=GET32(RCU_CFG0);
    ra&=(~(0xF<<18)); // times 27
    ra|=  (0xA<<18);  // times 27
    ra|=  (1<<29);    // times 27
    ra|=  (1<<16);    // PLLSRC HXTAL
    ra|=  (1<<17);    // HXTAL / 2
    ra&=(~(0x7<<8));  // APB1 DIV2
    ra|=  (0x4<<8);   // APB1 DIV2
    PUT32(RCU_CFG0,ra);

    ra=GET32(RCU_CTL);
    ra|=1<<24; //PLL enable
    PUT32(RCU_CTL,ra);

    while(1)
    {
        ra=GET32(RCU_CTL);
        if(ra&(1<<25)) break;
    }

    ra=GET32(RCU_CFG0);
    ra&=(~3);
    ra|=(2);
    PUT32(RCU_CFG0,ra);

    while(1)
    {
        ra=GET32(RCU_CFG0);
        if(((ra>>2)&3)==2) break;
    }
}

int notmain ( void )
{
    unsigned int rx;

    clock_init();
    uart_init();
    hexstring(0x12345678);
    while(1)
    {
        rx=uart_recv();
        hexstring(rx);
    }
    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/*

dfu-util -a 0 -D notmain.bin -s 0x08000000

*/
