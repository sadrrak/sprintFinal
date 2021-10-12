/*
 * SlaveCod.c
 *
 * Created: 28/09/2021 23:14:26
 * Author : sadra
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1 //Convers�o p armanezar no registrador de baud (datasheet)
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

void acionaSinal(int8_t );
void semaforoEscravo(int8_t );
void cameraSemaforo();
void semaforoPedestre(int8_t);
int main(void)
{
    DDRB=0xff; //hablitando todos os pinos da porta B como sa�da
	DDRD=0x81; //habilitando o pno d7 e d0 como sa�da amarelo e camera
	DDRC=0b0000111; //c6 entrada c2..c0 sa�da
	PORTB=0x00; //Iniciando desligadas ja que depende da info do mestre
	PORTD=0x00; //amarelo desligado e d0 desligado (camera)
	PORTC=0b1000000;//borda de descida para carro que passa no vermelho
	//Usart Config
	UBRR0H= (unsigned char)(MYUBRR>>8); //ajusta taxa de transmiss�o, parte alta
	UBRR0L= (unsigned char)MYUBRR; //ajusta a taxa de transmiss�o, parte baixa
	UCSR0B= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //habilitando interrup��o do receptor, habilita transmissor e receptor
	UCSR0C= (3<<UCSZ00); //ajuste de formato do frame 8 bits de dados, 1 de parada, no parity
	
	sei();//habilitando as interrup��es
    while (1) 
    {
		
	}
}
void semaforoEscravo(int8_t received){
	
	int16_t estados[9]={0b011110000,0b001110000,0b000110000,0b000010000,0b000001111,0b000000111,0b000000011,0b000000001,0b100000000};
	PORTB= estados[received] & 0b011111111;	//Mascara and apagando o led correspondente ao estado
	acionaSinal(received);		//Aciona buzzer
	semaforoPedestre(received);
	if(estados[received] & 0b100000000)
		PORTD |= 0b10000000;	//ligando amarelo
	else
		PORTD &= 0b01111111;	//apagando amarelo
	cameraSemaforo();
}

ISR(USART0_RX_vect){
		
	semaforoEscravo(UDR0-'0');	
}

void semaforoPedestre( int8_t aux){	
	if(aux>3){
		PORTC &= 0b1111101;
		PORTC |= 0b0000100;	//C2 verde 	(escravo) vermelho (mestre)
	}
	else{
		PORTC |= 0b0000010 ; //C1 vermelho (escravo) verde(mestre)
		PORTC &= 0b1111011;		
	}
}
void acionaSinal(int8_t data){	//Recebe a posi��o de ESTADOS[] e realiza a opera��o de ativar o buzzer ou n�o
	if( data== 7 || data== 8)
		PORTC |= 0b0000001; //aciona C0 como sa�da de sinal
	else
		PORTC &= 0b1111110 ;	//desativa C0	
}

void cameraSemaforo(){ //Camera que capta se algum ve�culo passou no vermelho
	
	if(PORTB==0b011110000 || PORTB==0b001110000 || PORTB==0b000110000 || PORTB==0b000010000){
		if((PINC & 0b1000000) ==0)
			PORTD |= 0b00000001;	//aciona camera			
		else
			PORTD &= 0b11111110;//camera desativada
	}
}
	
	