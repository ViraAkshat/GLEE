/*
 * SPI_Interrupt_Master.c
 *
 * Created: 09-07-2021 00:05:13
 * Author : acer
 */ 

#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#define FOSC 8000000 // 8MHz Clock frequency
#define BAUD 9600
#define BAUD_REGISTER FOSC/16/BAUD - 1

unsigned char int_data;

//UART setup -------------------------------------------------------------
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

// SPI ----------------------------------------------------------
void SPI_Master_init() {
	//enabling interrupts
	sei();
	
	//setting MOSI, SCK, SS pins to output
	DDRB |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);
	DDRB &=~(1<<DDB3);
	
	//Enabling master SPI
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPIE);
	
	PORTB &= ~(1<<DDB0);
}

void SPI_Master_transmit(unsigned char data) {
	//transmit the data
	SPDR = data;
	
	//wait till transmission is complete
	while(!(SPSR & (1<<SPIF)));
}

unsigned char SPI_Master_receive() {
	//wait till reception is complete
	while(!(SPSR & (1<<SPIF)));
	
	//return received data
	return SPDR;
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	SPI_Master_init();
	
	unsigned char uart_data = UART_receive();
	SPI_Master_transmit(uart_data);	
	
	while(1);
}

ISR(SPI_STC_vect) {
	DDRA = 0xFF;
	int_data = SPDR;
	UART_transmit(int_data);
}

