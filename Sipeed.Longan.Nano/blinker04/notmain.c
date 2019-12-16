
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
#define RCU_CTL     (RCU_BASE + 0x00)
#define RCU_CFG0    (RCU_BASE + 0x04)
#define RCU_APB2EN  (RCU_BASE + 0x18)

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

    //52MHz from 8MHz crystal.  (8/1)*6.5 = 52
    ra=GET32(RCU_CFG0);
    ra&=(~(0xF<<18)); // times 6.5
    ra|=  (0xD<<18);  // times 6.5
    ra|=  (1<<16);    // PLLSRC HXTAL
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
    unsigned int ra;
    unsigned int rx;
    unsigned int ry;
    unsigned int rz;

    ra=GET32(RCU_APB2EN);
    ra|=1<<4;
    PUT32(RCU_APB2EN,ra);

    ra=GET32(GPIOC_CTL1);
    ra&=(~(0xF<<20));
    ra|=  (0x1<<20) ;
    PUT32(GPIOC_CTL1,ra);

    rx=GET32(0xD1000000);
    //while(1)
    for(ra=0;ra<3;ra++)
    {
        PUT32(GPIOC_BOP,(1<<(16+13)));
        while(1)
        {
            ry=GET32(0xD1000000);
            rz=ry-rx;
            if(rz>=2000000) break;
        }
        PUT32(GPIOC_BOP,(1<<( 0+13)));
        while(1)
        {
            rx=GET32(0xD1000000);
            rz=rx-ry;
            if(rz>=2000000) break;
        }
    }

    clock_init();

    while(1)
    {
        PUT32(GPIOC_BOP,(1<<(16+13)));
        while(1)
        {
            ry=GET32(0xD1000000);
            rz=ry-rx;
            if(rz>=2000000) break;
        }
        PUT32(GPIOC_BOP,(1<<( 0+13)));
        while(1)
        {
            rx=GET32(0xD1000000);
            rz=rx-ry;
            if(rz>=2000000) break;
        }
    }
    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/*

dfu-util -a 0 -D notmain.bin -s 0x08000000

*/
