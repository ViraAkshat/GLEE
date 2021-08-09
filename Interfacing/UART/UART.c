/*
 * UART.c
 *
 * Created: 07-07-2021 20:24:48
 * Author : acer
 */ 

#include <avr/io.h>
#define FOSC 8000000 // 16MHz Clock frequency
#define BAUD 9600
#define BAUD_REGISTER FOSC/16/BAUD - 1

//setup
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
	while(!(UCSR0A & (1<<UDRE)));
	
	//store the data in UDR (buffer)
	UDR0 = data;
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	
	unsigned char received_data;
	
	for(int i=5; i>=0; i--){
		received_data = UART_receive(); //receive data and store it
		UART_transmit(received_data); //transmit the same data to verify that protocol works
	}
	
	return 0;
}

