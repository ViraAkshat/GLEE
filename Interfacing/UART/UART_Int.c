/*
 * UART_Interrupt.c
 *
 * Created: 07-07-2021 20:26:16
 * Author : acer
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000 // 8MHz Clock frequency
#include <util/delay.h>

#define BAUD 9600
#define BAUD_REGISTER F_CPU/16/BAUD - 1

unsigned char received_data = 0;

//setup
void UART_init(unsigned int baud_reg) {
	
	sei(); //global interrupt enable
	
	// setting up baud rate
	UBRR0H = (baud_reg >> 8);
	UBRR0L = baud_reg;
	
	//enabling RX, TX & RX-Interrupt
	UCSR0B = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	
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
	while(!(UCSR0A & (1<<UDRE)));
	
	//store the data in UDR (buffer)
	UDR0 = data;
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	
	//unsigned char received_data;
	
	while(1) {
		//received_data = UART_receive(); //receive data and store it
		if (!(UCSR0A & (0<<UDRE))) {
		    UART_transmit(received_data); //transmit the same data to verify that protocol works
		}
		_delay_ms(1000);
	}
	
	return 0;
}

ISR(USART0_RX_vect) {
	received_data = UDR0;
}
