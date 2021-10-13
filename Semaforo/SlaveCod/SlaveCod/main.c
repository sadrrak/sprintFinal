/*
 * SlaveCod.c
 *
 * Created: 28/09/2021 23:14:26
 * Author : sadra
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1 //Conversão p armanezar no registrador de baud (datasheet)
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

void acionaSinal(int8_t );
void semaforoEscravo(int8_t );
void cameraSemaforo();
void semaforoPedestre(int8_t);
void configRegSlave();

uint8_t flagShot=0;
uint32_t tempo_ms=0;
int main(void)
{
  	DDRB=0xff; //hablitando todos os pinos da porta B como saída
  	DDRD=0x81; //habilitando o pno d7 e d0 como saída amarelo e camera
	DDRC=0b0000111; //c6 entrada c2..c0 saída
	PORTB=0x00; //Iniciando desligadas ja que depende da info do mestre
	PORTD=0x04; //amarelo desligado e d0 desligado (camera) e d2 em NAA (borda de descida para carro que passa no vermelho)
	//PORTC=0b1000000;
	configRegSlave();	
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
	
}

ISR(USART0_RX_vect){
		
	semaforoEscravo(UDR0-'0');	//recebendo dado do mestre
}

ISR (TIMER0_COMPA_vect){
	tempo_ms++;//incrementando a váriavel
	if(tempo_ms%500 == 0)// tempo_ms eh meio segundo? se sim, desliga o "shot" da camera
		PORTD &= 0b11111110; //desligando a camera 
}

ISR (INT0_vect){	
	cameraSemaforo(); // chamando funcao dentro da interrupcao p verificar se passou veículo no vermelho			
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
void acionaSinal(int8_t data){	//Recebe a posição de ESTADOS[] e realiza a operação de ativar o buzzer ou não
	if( data== 7 || data== 8)
		PORTC |= 0b0000001; //aciona C0 como saída de sinal
	else
		PORTC &= 0b1111110 ;	//desativa C0	
}

void cameraSemaforo(){ //Camera que capta se algum veículo passou no vermelho
	if(PORTB==0b011110000 || PORTB==0b001110000 || PORTB==0b000110000 || PORTB==0b000010000)
		PORTD |= 0b00000001;	//aciona camera		
		
}

void configRegSlave(){
	//Para INT0
	EICRA= 0b00000010;	//interrupção externa INT0 na borda de descida
	EIMSK= 0b00000001; //habilitação das interrupção externa INT0
	
	//TIMER0
	TCCR0A= 0b00000010;
	TCCR0B= 0b00000011; //ligando TC0 com prescaler= 64
	OCR0A= 249; //define um limite de contagem para TC0
	TIMSK0= 0b00000010; //habilitando a interrupção na igualdade de OCR0A. A interrrupção chega em 1ms (64*(249+1)/16M Hz)
	
	//Usart Config
	UBRR0H= (unsigned char)(MYUBRR>>8); //ajusta taxa de transmissão, parte alta
	UBRR0L= (unsigned char)MYUBRR; //ajusta a taxa de transmissão, parte baixa
	UCSR0B= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //habilitando interrupção do receptor, habilita transmissor e receptor
	UCSR0C= (3<<UCSZ00); //ajuste de formato do frame 8 bits de dados, 1 de parada, no parity
	
	sei();//habilitando as interrupções
}
	
	
