/*
 * TWI_Master.c
 *
 * Created: 09-07-2021 19:55:41
 * Author : acer
 */ 

#include <avr/io.h>
#define FOSC 8000000 // 8MHz Clock frequency
#define BAUD 9600
#define BAUD_REGISTER FOSC/16/BAUD - 1

//UART setup ---------------------------------------------------------
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

//TWI ------------------------------------------------------
void TWI_Master_init() {
	//CPU Clock = 8MHz, SCL Freq = 50kHz
	TWBR = 0x47; //Bit rate
	TWSR = (0<<TWPS0) | (0<<TWPS1); //Prescalar 
	TWCR = (1<<TWEN); //TWI enable
}

void TWI_start() {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //send start condition
	while(!(TWCR & (1<<TWINT))); //wait for TWINT to set
}

//Slave Address and W/R
void TWI_SLA_W(unsigned char sla_w) {
    TWDR = sla_w;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while(!(TWCR & (1<<TWINT)));
}

void TWI_Master_transmit(unsigned char data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while(!(TWCR & (1<<TWINT)));
}

unsigned char TWI_Master_receive(unsigned char isLast) {
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

void TWI_stop() {
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

int main(void)
{
	UART_init(BAUD_REGISTER);
	TWI_Master_init();
	
	unsigned char uart_data = UART_receive();
	//unsigned char uart_data = 2;
	TWI_start();
	TWI_SLA_W(0b00000010);
	
	//for(int i=3; i>0; i--) {
		//uart_data = UART_receive();
		//TWI_Master_transmit(uart_data);
		//uart_data++;
	//}
	
	TWI_Master_transmit(uart_data);
	TWI_stop();
	
	DDRA = 0xFF;
	PORTA = uart_data;
	
	TWI_start();
	TWI_SLA_W(0b00000011);
	
	uart_data = TWI_Master_receive(1);
	TWI_stop();
	
	UART_transmit(uart_data);
}

