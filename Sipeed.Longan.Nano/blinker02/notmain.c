
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
#define RCU_APB2EN  (RCU_BASE + 0x18)

//-------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    ra=GET32(RCU_APB2EN);
    ra|=1<<4;
    PUT32(RCU_APB2EN,ra);

    ra=GET32(GPIOC_CTL1);
    ra&=(~(0xF<<20));
    ra|=  (0x1<<20) ;
    PUT32(GPIOC_CTL1,ra);

    while(1)
    {
        PUT32(GPIOC_BOP,(1<<(16+13)));
        while(1)
        {
            rx=GET32(0xD1000000);
            if((rx&0x100000)!=0) break;
        }
        PUT32(GPIOC_BOP,(1<<( 0+13)));
        while(1)
        {
            rx=GET32(0xD1000000);
            if((rx&0x100000)==0) break;
        }
    }
    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/*

dfu-util -a 0 -D notmain.bin -s 0x08000000

*/
