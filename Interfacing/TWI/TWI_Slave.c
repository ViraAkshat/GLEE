/*
 * TWI_Slave.c
 *
 * Created: 09-07-2021 19:56:36
 * Author : acer
 */ 

#include <avr/io.h>
#define FOSC 8000000 // 8MHz Clock frequency
#define BAUD 9600
#define BAUD_REGISTER FOSC/16/BAUD - 1

//UART setup ------------------------------------------------
void UART_init(unsigned int baud_reg) {
	// setting up baud rate
	UBRR0H = (baud_reg >> 8);
	UBRR0L = baud_reg;
	
	//enabling RX TX
	UCSR0B = (1<<RXEN)|(1<<TXEN);
	
	//Frame size is by default set to 8 bit (UCSZ=011) and 1 stop bit (USBS=0)
}

unsigned char UART_receive(void) {
	//wait for the data reception to complete
	while(!(UCSR0A & (1<<RXC)));
	
	//return the data
	return UDR0;
}

void UART_transmit(unsigned char data){
	//wait for empty transmit buffer
	while(!(UCSR0A & (1<<UDRE)));
	
	//store the data in UDR (buffer)
	UDR0 = data;
}

//TWI -----------------------------------------------------
void TWI_Slave_init() {
	TWAR = 0b00000010;
	TWCR = (1<<TWEN)|(1<<TWEA);
	TWBR = 0x47;
}

void TWI_listen() {
	TWCR = (1<<TWEN)|(1<<TWEA);
	
	while (!(TWCR & (1<<TWINT)));
}

unsigned char TWI_Slave_receive(unsigned char isLast) {
	unsigned char received_data;
	if(isLast == 0) {
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	}
	else {
		TWCR = (1<<TWINT)|(1<<TWEN);
	}
	
	while(!(TWCR & (1<<TWINT)));
	received_data = TWDR;
	//PORTA = received_data;
	return received_data;
}

void TWI_Slave_transmit(unsigned char data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while(!(TWCR & (1<<TWINT)));
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	TWI_Slave_init();
	
	unsigned char twi_data;
	TWI_listen();
	
	//for(int i=3; i>0; i--) {
		//unsigned char isLast = i=1?1:0;
		//twi_data = TWI_Slave_receive(isLast);
		//UART_transmit(twi_data);
	//}
	
	twi_data = TWI_Slave_receive(1);
	UART_transmit(twi_data);
	
	DDRA = 0xFF;
	PORTA = twi_data;
	
	twi_data = UART_receive();
	TWI_listen();
	
	TWI_Slave_transmit(twi_data);
	
}

