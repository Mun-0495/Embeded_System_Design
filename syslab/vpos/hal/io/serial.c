#include "serial.h"
#include "vh_cpu_hal.h"
#include "vh_variant_hal.h"
#include "vh_io_hal.h"
#include "vh_variant_hal.h"
#include "dd.h"
#include "printk.h"

void vh_serial_interrupt_handler(void);

char getc(void)
{
	char c;
	unsigned long rxstat;
	/* Write getc func */
	
	//while(UARTFR & UARTFR_RXFE);
	
	//c = UARTDR;
	//rxstat = UARTFR & UARTFR_RXFE;
	
	//return c;


	/* End getc func */
	
	while (pop_idx == push_idx);
	
	c = serial_buff[pop_idx++];

	if (pop_idx == SERIAL_BUFF_SIZE)
		pop_idx = 0;
	
	return c;
}

void putc(char c)
{
	while (UARTFR & UARTFR_TXFF);
	
	UARTDR = c;
}

void vh_serial_init(void)
{
	// set baud rate
	unsigned int idiv, fdiv;
	double f;
	/*  baud rate Here  */
	idiv = UART_CLK / (16 * UART_BAUDRATE);
	f = UART_CLK / (16 * UART_BAUDRATE);
	f = f - idiv;
	fdiv = (unsigned int)(f*64 + 0.5);


	/*  baud rate End  */
    UARTIBRD = idiv;
    UARTFBRD = fdiv;

	// set UART ctrl regs
    //UARTLCR_H = 0x76;
	UARTLCR_H = 0x66;
    UARTCR = 0x301;
	//UARTIMSC = 0xF9EF;
	//UARTIFLS = 0x04;
	
	// clear buffer
	push_idx = 0;
	pop_idx = 0;
	for(int i=0; i<SERIAL_BUFF_SIZE; i++)
		serial_buff[i] = '\0';
}

void vh_serial_irq_enable(void)
{	
	/* enable GIC & interrupt */
	int m = 33;

	GICD_ICACTIVER(m / 32) = (1 << (m % 32));
	GICD_ICPENDR(m / 32) = (1 << (m % 32));
	
	GICD_ISENABLER(m / 32) = (1 << (m % 32));
	
	GICD_ITARGETSR(m / 4) = (0b00000001 << (m % 4));

	GICD_ICFGR(m / 16) = (1 << (m % 16));

	UARTICR = 0x07FF;

	UARTIMSC = 0x0010;
}

void vk_serial_push(void)
{
	char c = UARTDR;
	
	serial_buff[push_idx++] = c;

	if (push_idx == SERIAL_BUFF_SIZE){	// buffer is full
		push_idx = 0;
	}	
}

void vh_serial_interrupt_handler(void)
{
	vk_serial_push();	
}

