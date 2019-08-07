/*
 * BMP280.c
 *
 * Created: 05-Mar-19 6:11:31 PM
 * Author : brankort8416
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "simpleUART.h"
#include "types.h"
#define SS 2


void Init_SPI(void);
uint8 Transmit_SPI(uint8);
sint32 CompTemp(sint32);
uint8 ReadTemp(void);
void Init_BPM280(void);
void BurstRead(uint8**,sint8,uint8);

int main(void)
{
	/*
	char str[32];

	Init_SPI();
	*/
	
	DDRD |= 0xFF;
	
	/*
	Init_BPM280();
	
	_delay_ms(1);
	
	PORTD = ReadTemp();
	*/
	
    while (1) 
    {
		PORTD |= 0xff;
		//8 bit value of the temperature
		_delay_ms(500);
    }
}
void Init_SPI(void){
	//SCK MOSI output
	DDRB = ((1<<5) | (1<<3) | (1<<2));
	//PRSC f/16, 1MHz
	SPCR = ((1<<SPE) | (1<<MSTR) | (1<<SPR0));
	
}

uint8 Transmit_SPI(uint8 data){

	SPDR = data;
	_delay_us(30);
	while(!(SPSR & (1<<SPIF)));
	data = SPDR;
	return(data);
	
}
sint32 CompTemp(sint32 adc_temp){
	sint32 t_fine,var1,var2,T;
	uint16 dig_t1;
	sint16 dig_t2,dig_t3;
	PORTB &= ~(1<<SS); //Turn the SS line low
	
	//Fill the dig Values
	uint8* buffer;
	BurstRead(&buffer,5,0x88);
	
	
	dig_t1 = buffer[5];
	dig_t1 |= (uint16)buffer[4] << 8; 
	
	dig_t2 = buffer[3];
	dig_t2 |= (sint16)buffer[2] << 8;
	
	dig_t3 = buffer[1];
	dig_t3 |= (sint16)buffer[0] << 8;

	free(buffer);
	
	PORTB |= (1<<SS);

	//Temperature compensation formula
	var1 = ((((adc_temp >> 3)-((sint32)dig_t1 << 1)))*((sint32)dig_t2)) >> 11;
	var2 = (((((adc_temp >> 4) - ((sint32)dig_t1)) * ((adc_temp >> 4)-((sint32)dig_t1))) >> 12) * ((sint32)dig_t3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	
	return (T/100);
}
void Init_BPM280(){
	
	PORTB &= ~(1<<SS); //Turn the SS line low
	//Tell the sensor to wake up
	Transmit_SPI(0x74);
	Transmit_SPI(0xff);
	PORTB |= (1<<SS);
}
uint8 ReadTemp(){
	sint32 raw_adc_data = 0x00;
	sint32 final_temp = 0x00;
	PORTB &= ~(1<<SS); //Turn the SS line low
	
	uint8* buffer;
	BurstRead(&buffer,2,0xfa);
	uint32 msb_t = buffer[2];
	uint32 lsb_t = buffer[1];
	uint32 xlsb_t =buffer[0];
	
	PORTB |= (1<<SS);
	
	free(buffer);
	
	raw_adc_data = ((msb_t << 16) | (lsb_t << 8) | (xlsb_t));
	raw_adc_data >>= 4; // get the 32bit value
	
	final_temp = CompTemp(raw_adc_data);
	
	return (uint8)final_temp;
}

//Return buffer of the SPI read
void BurstRead(uint8** buffer,sint8 n,uint8 adr){

	*buffer = malloc((n+1) * sizeof(uint8));
	Transmit_SPI(adr);

	while(n >= 0){
		(*buffer)[n--] = Transmit_SPI(0xff);
	}
	
}

