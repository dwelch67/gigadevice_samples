
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void PUT32( unsigned int, unsigned int);
unsigned int GET32 ( unsigned int );
void  dummy ( unsigned int );
unsigned int read_mtime ( void );

#define GPIOC_BASE  0x40011000
#define GPIOC_CTL1  (GPIOC_BASE + 0x04)
#define GPIOC_BOP   (GPIOC_BASE + 0x10)

#define RCU_BASE    0x40021000
#define RCU_CFG0    (RCU_BASE + 0x04)
#define RCU_APB2EN  (RCU_BASE + 0x18)
#define RCU_APB1EN  (RCU_BASE + 0x1C)
#define RCU_APB1RST (RCU_BASE + 0x10)

#define TIMER5_BASE 0x40001000
#define TIMER5_CTL0 (TIMER5_BASE + 0x00)
#define TIMER5_CTL1 (TIMER5_BASE + 0x04)
#define TIMER5_INTF (TIMER5_BASE + 0x10)
#define TIMER5_CNT  (TIMER5_BASE + 0x24)
#define TIMER5_PSC  (TIMER5_BASE + 0x28)
#define TIMER5_CAR  (TIMER5_BASE + 0x2C)


int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    ra=GET32(RCU_APB2EN);
    ra|=1<<4; //GPIOC
    PUT32(RCU_APB2EN,ra);

    ra=GET32(RCU_APB1EN);
    ra|=1<<4; //TIMER5
    PUT32(RCU_APB1EN,ra);

    ra=GET32(GPIOC_CTL1);
    ra&=(~(0xF<<20));
    ra|=  (0x1<<20) ;
    PUT32(GPIOC_CTL1,ra);

    PUT32(RCU_APB1RST,1<<4);
    PUT32(RCU_APB1RST,0);

    PUT32(TIMER5_PSC ,0xFFFF);
    PUT32(TIMER5_CAR ,0xFFFF);
    PUT32(TIMER5_CNT ,0xFFFF);
    PUT32(TIMER5_CTL0,0x0001);

    //while(1)
    for(ra=0;ra<4;ra++)
    {
        PUT32(GPIOC_BOP,(1<<(16+13)));
        while(1)
        {
            rx=GET32(TIMER5_CNT);
            if((rx&0x40)!=0) break;
        }
        PUT32(GPIOC_BOP,(1<<( 0+13)));
        while(1)
        {
            rx=GET32(TIMER5_CNT);
            if((rx&0x40)==0) break;
        }
    }

    PUT32(RCU_APB1RST,1<<4);
    PUT32(RCU_APB1RST,0);

    //8000000 = 0x7A1200
    //0x7A1200 / 0x200 = 0x3D09

    PUT32(TIMER5_PSC ,0x200-1);
    PUT32(TIMER5_CAR ,0x3D09-1);
    PUT32(TIMER5_CNT ,0x3D09-1);
    PUT32(TIMER5_CTL0,0x0001);
    PUT32(TIMER5_INTF,0);

    while(1)
    {
        PUT32(GPIOC_BOP,(1<<(16+13)));
        while(1)
        {
            rx=GET32(TIMER5_INTF);
            if(rx&1) break;
        }
        PUT32(TIMER5_INTF,0);
        PUT32(GPIOC_BOP,(1<<( 0+13)));
        while(1)
        {
            rx=GET32(TIMER5_INTF);
            if(rx&1) break;
        }
        PUT32(TIMER5_INTF,0);
    }

    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/*

dfu-util -a 0 -D notmain.bin -s 0x08000000

*/
