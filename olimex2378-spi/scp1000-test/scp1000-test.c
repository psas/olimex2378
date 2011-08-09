
/*
 * scp1000-test.c
 * testing SPI interface with pressure sensor scp1000-D01
 */

#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-spi.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"
#include "printf-lpc.h"

#include "olimex2378-util.h"
#include "scp1000-test.h"

/*
 * scp_create_register_write_address
 */
inline uint8_t scp_create_register_write_address(scp_regaddr regaddr) {
	return(  ( (regaddr << 2)| 0x1)   );
}

/*
 * scp_create_register_read_address
 */
inline uint8_t scp_create_register_read_address(scp_regaddr regaddr) {
	return( (regaddr << 2) );
}

/*
 * scp_read_revid
 */
uint16_t scp_read_revid() {
    uint8_t  cmdaddr;
    uint16_t data = 0;

    cmdaddr = scp_create_register_read_address(REVID);

    SSEL_LOW;

    spi_transact(cmdaddr, SPI_16BITS) ;

    spi_waitSPIF();

    data = S0SPDR ;

    SSEL_HIGH;

    return(data & 0xf);
}

/*
 * scp_read_data8
 */
uint16_t scp_read_data8() {
    uint8_t  cmdaddr;
    uint16_t data = 0;

    cmdaddr = scp_create_register_read_address(DATARD8);

    SSEL_LOW;

    spi_transact(cmdaddr<<8, SPI_16BITS) ;

    spi_waitSPIF();

    data = S0SPDR & 0x7 ;

    SSEL_HIGH;

    return(data);
}

/*
 * scp_read_data16
 */
uint16_t scp_read_data16() {
    uint8_t  cmdaddr;
    uint16_t data_low = 0;
    uint16_t data_high = 0;

    cmdaddr = scp_create_register_read_address(DATARD16);

    SSEL_LOW;

    spi_transact(cmdaddr<<8, SPI_16BITS) ;

    spi_waitSPIF();

    data_high = S0SPDR & 0xff ;

    spi_transact(cmdaddr<<8, SPI_16BITS) ;

    spi_waitSPIF();

    data_low = S0SPDR & 0xff ;


    SSEL_HIGH;

    return((data_high<<8) | data_low);
}



/*
 * scp_read_status
 */
uint16_t scp_read_status() {
    uint8_t  cmdaddr;
    uint16_t data = 0;

    cmdaddr = scp_create_register_read_address(STATUS);

    SSEL_LOW;

    spi_transact(cmdaddr<<8, SPI_16BITS) ;

    spi_waitSPIF();

    data = S0SPDR ;

    SSEL_HIGH;

    return(data & 0xff);
}

/*
 * scp_read_temperature
 */
uint16_t scp_read_temperature() {
    uint8_t  cmdaddr;
    uint16_t temp_highbits = 0;
    uint16_t temp_lowbits = 0;

    cmdaddr = scp_create_register_read_address(TEMPOUT);
    // printf_lpc(UART0,"cmd is: 0x%x\n", cmdaddr);

    SSEL_LOW;

    spi_transact(cmdaddr<< 8, SPI_16BITS) ;

    spi_waitSPIF();

    temp_highbits = S0SPDR; //& 0xf ; // low 8 bits is high 8 bits of temp

    spi_transact(cmdaddr<< 8, SPI_16BITS) ;

    spi_waitSPIF();

    temp_lowbits = S0SPDR; //& 0xf ; // low 8 bits is high 8 bits of temp

    SSEL_HIGH;

    return((temp_highbits << 8) | temp_lowbits);
}

/*
 * scp_task
 */
void scp_task() {

    uint16_t rev = 0;
    uint16_t pres_lsb = 0;
    uint16_t pres_msb = 0;
    uint16_t status = 0;
    uint16_t temp = 0;

    uint32_t pres = 0;

    uart0_putstring("scp task...\n");

    rev = scp_read_revid() ;
    printf_lpc(UART0,"rev is: %d\n", rev);

    status = scp_read_status() ;
    printf_lpc(UART0,"status is: %d\n", status);

    pres_msb = scp_read_data8();
    pres_lsb = scp_read_data16();
    pres     = (pres_msb << 16) | pres_lsb; 
    printf_lpc(UART0,"pressure is: %d\n",pres);

//    while(1) {
        temp = scp_read_temperature() ;
        printf_lpc(UART0,"temperature is: %d\n", (temp/20));
        util_waitticks(2000000);
 //   }


}

int main (void) {

    int32_t cycles = 10;

    // scp1000 breakout board has no reset use power off/on to reset.
    SCP1000_PWR_PINSEL;
    SCP1000_PWR_NOPULL;



    pllstart_seventytwomhz() ;
    //   pllstart_sixtymhz() ;
    //   pllstart_fourtyeightmhz() ;
    FIO_ENABLE;
    SCP1000_PWR_EN;
    SCP1000_PWR_ON;

    uart0_init_115200() ;

    uart0_putstring("\n***Starting olimex scp1000-D01 test***\n\n");

    spi_init(CCLK_DIV8, SPI_100KHZ) ;

    stat_led_flash_fast(cycles); // initial visual check

    scp_task() ;

    // stat_led_flash_slow(2);

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}

