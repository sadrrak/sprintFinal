/*
 * 
 *
 * Created: 25/07/2021 14:42:02
 * Author : sadrak max da silva Mat: 118210365
 */ 
#define F_CPU 16000000UL
#define BAUD 9600 
#define MYUBRR F_CPU/16/BAUD-1 //Conversão p armanezar no registrador de baud (datasheet)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "PCD8544\nokia5110.h"


void opSemaforo();
void animaLCD();
void verificaLuz();
void USART_transmit(unsigned char);
void configuracoesRegistradores();


//variáveis globais para os leds e cursor do lcd
uint8_t contaLed=0,cursor=1, flagTempo=0, flagAuto=0,flagLux=0, flagPessoa=0, modo=0, contSlave=0;  // varia de 0 a 8 para representar os 9 leds do semaforo
uint16_t redTime=2, yellowTime=1, greenTime=1, contCar=0;
uint32_t  timer_ms=0;
float taxa=0;



int main(void)
{
	//DDRC definidos em nokia5110.h
	DDRB= 0b11111111;	// definindo os pinos B como saída
	DDRD = 0b10001000;	//definindo os botoes de entrada D2,D4,D5,D6 e utilizando uma das saidas para o led amarelo(D7) e D3 luz
	PORTD= 0b01110100; //NA em D2,D4,D5 e D6
	DDRC=  0b00000000;	//definindo a porta C como entrada 
	PORTC= 0b01000000;// C6 como NA
	PORTB= 0b00001111;
	configuracoesRegistradores();	
	nokia_lcd_init(); //inicialização do LCD
	animaLCD();	
	while (1)
	{		
		opSemaforo();	// chama a funcao responsavel pelas operacoes		
	}
}



void opSemaforo(){
	static int8_t  aux= 1;	// variaveis do tipo static que "guardam" o valor e so podem ser acessadas dentro desse escopo
	//	PORTB= leds; //portB recebe o valor em binario de leds que devem estar acesos
	
	if(flagTempo){
		if(aux){ // Nessa rotina, contaLed será incrementado e usado na interrupção do timer
			
			if(PORTD== 0b11110100 ){
				PORTD= 0b01110100; //apagando amarelo
				PORTB=0xf0;	//acende vermelho
				aux=0;
				}else{
				PORTB= 0x0f & (PORTB>>1); //mascara and para apagar os verdes
				if(PORTB==0)	//se igual a zero, acende o amarelo D7
				PORTD=0b11110100;
			}
			
			}else{
			PORTB= 0xf0 & (PORTB<<1); //mascara and para apagar os vermelhos
			if(PORTB==0){	//se igual a 0, acende os verdes e altera o valor de descendo
				PORTB=0x0f;
				aux=1;
			}
		}
		flagTempo=0;
		
		if(contaLed<8)
		contaLed++;
		else
		contaLed=0;
		
	}
	
}

void animaLCD(){
	unsigned char red[2], yellow[2], green[2], mode[7]= "MODO M", taxaC[4], lux[4] ;
	uint16_t taxaAux= (uint16_t)taxa;
	
	//convertendo os numeros em caracteres
	
	sprintf(red, "%u" ,redTime);
	sprintf(yellow,"%u",yellowTime);
	sprintf(green,"%u",greenTime);
	sprintf(taxaC,"%u",taxaAux);
	sprintf(lux,"%u",((1023000/ADC)-1000)); //valor de lux
	
	
	if(cursor==2){		//Para cada valor do cursor, uma série de chamada eh ativada, tudo isso para mostrar a "seta" do cursor 1: modo 2:vermelho 3:amarelo e 4:verde
		nokia_lcd_clear();
		nokia_lcd_write_string("PAINEL", 1);
		
		
		//C/min
		nokia_lcd_set_cursor(55,0);
		nokia_lcd_write_string(taxaC, 2);
		nokia_lcd_set_cursor(55,15);
		nokia_lcd_write_string("C/min", 1);
		
		//Lux
		nokia_lcd_set_cursor(53.7,25);
		nokia_lcd_write_string(lux, 2);
		nokia_lcd_set_cursor(60,40);
		nokia_lcd_write_string("LUX", 1);
		
		//modo
		nokia_lcd_set_cursor(0,10);
		if(modo){
			nokia_lcd_write_string("MODO A", 1);
			}else{
			nokia_lcd_write_string("MODO M", 1);
		}
		nokia_lcd_set_cursor(45,10);
		nokia_lcd_write_string(" |",1);
		
		//vermelho
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("red", 1);
		nokia_lcd_set_cursor(40,20);
		nokia_lcd_write_string(red, 1);
		nokia_lcd_set_cursor(45,20);
		nokia_lcd_write_string("<|",1);
		
		
		//amarelo
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("yellow", 1);
		nokia_lcd_set_cursor(40,30);
		nokia_lcd_write_string(yellow, 1);
		nokia_lcd_set_cursor(45,30);
		nokia_lcd_write_string(" |",1);
		
		//verde
		nokia_lcd_set_cursor(0,40);
		nokia_lcd_write_string("green", 1);
		nokia_lcd_set_cursor(40,40);
		nokia_lcd_write_string(green, 1);
		nokia_lcd_set_cursor(45,40);
		nokia_lcd_write_string(" |",1);
	}
	if(cursor==3){
		nokia_lcd_clear();
		nokia_lcd_write_string("PAINEL", 1);
		
		//C/min
		nokia_lcd_set_cursor(55,0);
		nokia_lcd_write_string(taxaC, 2);
		nokia_lcd_set_cursor(55,15);
		nokia_lcd_write_string("C/min", 1);
		
		//Lux
		nokia_lcd_set_cursor(53.7,25);
		nokia_lcd_write_string(lux, 2);
		nokia_lcd_set_cursor(60,40);
		nokia_lcd_write_string("LUX", 1);
		
		//modo
		nokia_lcd_set_cursor(0,10);
		if(modo){
			nokia_lcd_write_string("MODO A", 1);
			}else{
			nokia_lcd_write_string("MODO M", 1);
		}
		nokia_lcd_set_cursor(45,10);
		nokia_lcd_write_string(" |",1);
		
		//vermelho
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("red", 1);
		nokia_lcd_set_cursor(40,20);
		nokia_lcd_write_string(red, 1);
		nokia_lcd_set_cursor(45,20);
		nokia_lcd_write_string(" |",1);
		
		//amarelo
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("yellow", 1);
		nokia_lcd_set_cursor(40,30);
		nokia_lcd_write_string(yellow, 1);
		nokia_lcd_set_cursor(45,30);
		nokia_lcd_write_string("<|",1);
		
		nokia_lcd_set_cursor(0,40);
		nokia_lcd_write_string("green", 1);
		nokia_lcd_set_cursor(40,40);
		nokia_lcd_write_string(green, 1);
		nokia_lcd_set_cursor(45,40);
		nokia_lcd_write_string(" |",1);
	}
	if(cursor==4){
		
		nokia_lcd_clear();
		nokia_lcd_write_string("PAINEL", 1);
		
		//C/min
		nokia_lcd_set_cursor(55,0);
		nokia_lcd_write_string(taxaC, 2);
		nokia_lcd_set_cursor(55,15);
		nokia_lcd_write_string("C/min", 1);
		
		//Lux
		nokia_lcd_set_cursor(53.7,25);
		nokia_lcd_write_string(lux, 2);
		nokia_lcd_set_cursor(60,40);
		nokia_lcd_write_string("LUX", 1);
		
		//modo
		nokia_lcd_set_cursor(0,10);
		if(modo){
			nokia_lcd_write_string("MODO A", 1);
			}else{
			nokia_lcd_write_string("MODO M", 1);
		}
		nokia_lcd_set_cursor(45,10);
		nokia_lcd_write_string(" |",1);
		
		//vermelho
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("red", 1);
		nokia_lcd_set_cursor(40,20);
		nokia_lcd_write_string(red, 1);
		nokia_lcd_set_cursor(45,20);
		nokia_lcd_write_string(" |",1);
		
		//amarelo
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("yellow", 1);
		nokia_lcd_set_cursor(40,30);
		nokia_lcd_write_string(yellow, 1);
		nokia_lcd_set_cursor(45,30);
		nokia_lcd_write_string(" |",1);
		
		//verde
		nokia_lcd_set_cursor(0,40);
		nokia_lcd_write_string("green", 1);
		nokia_lcd_set_cursor(40,40);
		nokia_lcd_write_string(green, 1);
		nokia_lcd_set_cursor(45,40);
		nokia_lcd_write_string("<|",1);
	}
	if(cursor==1){
		nokia_lcd_clear();
		nokia_lcd_write_string("PAINEL", 1);
		
		//C/min
		nokia_lcd_set_cursor(55,0);
		nokia_lcd_write_string(taxaC, 2);
		nokia_lcd_set_cursor(55,15);
		nokia_lcd_write_string("C/min", 1);
		
		//Lux
		nokia_lcd_set_cursor(53.7,25);
		nokia_lcd_write_string(lux, 2);
		nokia_lcd_set_cursor(60,40);
		nokia_lcd_write_string("LUX", 1);
		
		//modo
		nokia_lcd_set_cursor(0,10);
		if(modo){
			nokia_lcd_write_string("MODO A", 1);
			}else{
			nokia_lcd_write_string("MODO M", 1);
		}
		nokia_lcd_set_cursor(45,10);
		nokia_lcd_write_string("<|",1);
		
		//vermelho
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("red", 1);
		nokia_lcd_set_cursor(40,20);
		nokia_lcd_write_string(red, 1);
		nokia_lcd_set_cursor(45,20);
		nokia_lcd_write_string(" |",1);
		
		//amarelo
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("yellow", 1);
		nokia_lcd_set_cursor(40,30);
		nokia_lcd_write_string(yellow, 1);
		nokia_lcd_set_cursor(45,30);
		nokia_lcd_write_string(" |",1);
		
		//verde
		nokia_lcd_set_cursor(0,40);
		nokia_lcd_write_string("green", 1);
		nokia_lcd_set_cursor(40,40);
		nokia_lcd_write_string(green, 1);
		nokia_lcd_set_cursor(45,40);
		nokia_lcd_write_string(" |",1);
		
		
		
	}
	nokia_lcd_render();
	
}

void verificaLux(){
	
	if(((1023000/ADC)-1000) <300)//valor de lux comparado com a condição
	flagLux=1;	//se NA, acende a luminária
	else
	OCR2B=0;	//zerando o duty (apaga luz)
	
	if(flagLux){
		if((PINC & 0b001000000) == 0b00000000  || taxa!=0){ //flagPessoa NA oou houver fluxo de carros, acende.
			OCR2B=255;
			//flagPessoa=0;
			}else{
			OCR2B=85;
		}
		flagLux=0;	//flag zerada
	}
}

void USART_transmit( unsigned char cont){
	while(!(UCSR0A & (1<<UDRE0))); //esperando limpeza do registrador de transmissão
	UDR0=cont; //enviando dado
}

void configuracoesRegistradores(){
	//config interrupt
	
	//para INT2
	PCICR= 0b00000110;	//configuração de seleção INT2
	PCMSK2=0b01110000; //pino D4, D5 E D6
	PCMSK1=0b1000000; //pino C6 SENSOR DE PESSOAS
	
	//Para INT0 e INT1
	EICRA= 0b00001010;	//interrupção externa INT0 e INT1 na borda de descida
	EIMSK= 0b00000011; //habilitação das interrupções externas INT0 E INT1
	
	//config ADC
	ADMUX=0b01000000;	//vcc como referência
	ADCSRA=0b11100111;	// habilitando ADC, conversão contínua , prescaler=128
	ADCSRB=0b00000000;	//conversão contínua
	DIDR0= 0b00000001; //desabilitando porta c0 como entrada dgital
	//config timers
	
	//TIMER0
	TCCR0A= 0b00000010;
	TCCR0B= 0b00000011; //ligando TC0 com prescaler= 64
	OCR0A= 249; //define um limite de contagem para TC0
	TIMSK0= 0b00000010; //habilitando a interrupção na igualdade de OCR0A. A interrrupção chega em 1ms (64*(249+1)/16M Hz)
	
	//TIMER2
	TCCR2A= 0b00100011;
	TCCR2B= 0b00000110;
	OCR2B=128;
	
	//Usart Config
	UBRR0H= ( unsigned char)(MYUBRR>>8); //ajusta taxa de transmissão, parte alta
	UBRR0L= ( unsigned char)MYUBRR; //ajusta a taxa de transmissão, parte baixa
	UCSR0B= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //habilitando interrupção do receptor, habilita transmissor e receptor
	UCSR0C= (3<<UCSZ00); //ajuste de formato do frame 8 bits de dados, 1 de parada, no parity
	
	sei();//habilitando interrupções globais
	
}

ISR (PCINT2_vect){
	//------------------------------------botão seleção----------------------------------------------
	if(!(PIND & (1<<4))){	//verificando para pegar apenas a borda de descida de D4
		
		cursor++;
		
		if(cursor>4){
			cursor=1;
		}
		animaLCD();
		
	}
	//-------------------------------------acrescenta tempo D5-----------------------------------------
	if(!(PIND & (1<<5))){
		if(!modo){
			if(cursor==2){
				if(redTime<9)
				redTime++;
				animaLCD();
			}
			
			if(cursor==4){
				if(greenTime<9)
				greenTime++;
				animaLCD();
			}
		}
		if(cursor==1){
			modo=1; //auto
			flagAuto=1;
			animaLCD();
		}
		if(cursor==3){
			if(yellowTime<9 && yellowTime+1<redTime)
			yellowTime++;
			animaLCD();
		}
	}
	//--------------------------------------diminui tempo D6-----------------------------------
	if(!(PIND & (1<<6))){
		if(!modo){
			
			if(cursor==2){
				if(redTime>1 && redTime>yellowTime+1)
				redTime--;
				animaLCD();
			}
			
			if(cursor==4){
				if(greenTime>1)
				greenTime--;
				animaLCD();
			}
		}
		if (cursor==1){
			modo=0; //manual
			flagAuto=0;
			animaLCD();
		}
		if(cursor==3){
			if(yellowTime>1)
			yellowTime--;
			animaLCD();
		}
	}
}


ISR (TIMER0_COMPA_vect){
	
	timer_ms++; //incremento do timer
	
	if(flagAuto){
		greenTime= (uint16_t)((taxa/60)+1);		//calcula o tempo do verde baseado na taxa de veículos que passam em 5 seg;
		redTime= 10-greenTime; //o tempo do vermelho, dado as funções, será sempre o complementar do verde p 10
	}
	//--------------------------Condicionais tempo para leds Mestre----------------------------------------------------
	if(contaLed<4){ //0 a 3, referente aos leds verdes. No ultimo ciclo, amarelo acende
		if((timer_ms % (greenTime*250)) == 0)	//capta o tempo em milisegundo corespondente a um led
			flagTempo=1;
		}else if(contaLed==4){ //tempo, amarelo apaga
			if(timer_ms % (yellowTime*1000) == 0)
				flagTempo=1;
		
		}else{ //5 a 8, referente ao vermelho, apaga amarelo no primeiro ciclo e acende verde no ultimo
		if((timer_ms % (redTime*250)) == 0)
		flagTempo=1;
		
	}
	//------------------------------------------------------------------------------------------------
	//contSlave como o nome sugere, eh um contador de estados do escravo
	
	if(contSlave<=3){
		if(timer_ms % ((greenTime+yellowTime)*250)==0){ //VERMELHO DO ESCRAVO
			contSlave++;
			USART_transmit('0'+contSlave);
			
		}
		}else{	//verde
		if(contSlave<=7){
			if(timer_ms % ((redTime-yellowTime)*250)==0){ //VERDE
				contSlave++;
				USART_transmit('0'+contSlave);
				
			}
		}
		else{	//Amarelo
			
			if(contSlave<=8){
				if(timer_ms % (yellowTime*1000)==0){
					
					contSlave++;
					USART_transmit('0'+contSlave);
				}
			}
			else{
				contSlave=0;
				USART_transmit('0'+contSlave);
			}
			
		}
		
	}
	
	if(timer_ms%5000==0){
		taxa=contCar*12; //calculando a taxa que posteriormente será usada no automatico contCar*60/5
		contCar=0;
	}
	if(timer_ms%500==0){
		verificaLux();
		animaLCD();
	}
}

ISR (INT0_vect){
	contCar++;  //contando os carros borda de descida pino D2
}
