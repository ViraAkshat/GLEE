/*
 * SPI_Interrupt_Slave.c
 *
 * Created: 09-07-2021 00:06:09
 * Author : acer
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000
#include <util/delay.h>
#define FOSC 8000000 // 8MHz Clock frequency
#define BAUD 9600
#define BAUD_REGISTER FOSC/16/BAUD - 1

unsigned char int_data;

//UART setup ---------------------------------------------------------
void UART_init(unsigned int baud_reg) {
	// setting up baud rate
	UBRR0H = (baud_reg >> 8);
	UBRR0L = baud_reg;
	
	//enabling RX TX
	UCSR0B = (1<<RXEN)|(1<<TXEN);
	
	//Frame size is by default set to 8 bit (UCSZ=011) and 1 stop bit (USBS=0)
}

//receiver function
unsigned char UART_receive(void) {
	//wait for the data reception to complete
	while(!(UCSR0A & (1<<RXC)));
	
	//return the data
	return UDR0;
}

//transmitter function
void UART_transmit(unsigned char data){
	//wait for empty transmit buffer
	while(!(UCSR0A & (1<<UDRE0)));
	
	//store the data in UDR (buffer)
	UDR0 = data;
}

//SPI ----------------------------------------------------------
void SPI_Slave_init() {
	
	sei();
	
	//Setting MISO as output
	DDRB |= (1<<DDB3);
	DDRB &= ~(1<<DDB1)&~(1<<DDB2);
	
	//Enabling SPI
	SPCR = (1<<SPE)|(1<<SPIE);
}

unsigned char SPI_Slave_receive() {
	//wait till reception is complete
	while(!(SPSR & (1<<SPIF)));
	
	//return received data
	return SPDR;
}

void SPI_Slave_transmit(unsigned char data) {
	//transmit the data
	SPDR = data;
	
	//wait till transmission is complete
	while(!(SPSR & (1<<SPIF)));
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	SPI_Slave_init();
	
	unsigned char spi_data = SPI_Slave_receive();
	UART_transmit(spi_data);
	
	while(1);
}

ISR(SPI_STC_vect) {
	int_data = UART_receive();
	SPDR = int_data;
}
