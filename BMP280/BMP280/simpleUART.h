/*
 * simpleUART.h
 *
 * Created: 25-Apr-19 4:39:30 PM
 *  Author: brankort8416
 */ 


#ifndef SIMPLEUART_H_
#define SIMPLEUART_H_
//#include <avr/io.h>
#include <string.h>
//#define  F_CPU 8000000UL
#define BAUDRATE 9600
#define BAUD_PRESACLE (((F_CPU/(BAUDRATE*16UL)))-1)

void initUART(unsigned int ubrr){
	//SET THE BAUD RATE
	UBRR0L = ubrr;
	UBRR0H = (ubrr << 8);
	//1 STOP BIT, 8 BIT DATA FRAME 
	UCSR0B = (1<<TXEN0 | 1<<RXEN0);
	UCSR0C = (3<<UCSZ00);
}

void sendByte(unsigned char data){
	while(!(UCSR0A & (1<< UDRE0)));
	UDR0 = data;
}
void sendString(char* str){
	for (int i = 0; i < strlen(str);i++)
	{
		sendByte(str[i]);
	}
}
char reciveByte(){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}



#endif /* SIMPLEUART_H_ */