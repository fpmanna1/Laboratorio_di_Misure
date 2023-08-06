/* Attendere la pressione del tasto USER. Quando la pressione si verifica, 
generare con il convertitore D/A un segnale costante pari alla seconda 
parte della propria matricola espressa in mV. 
Se questo numero supera 3000 mV, sottrarre 3000. 
(ESEMPIO: N39001500 deve generare 1500 mV; N46004000 deve generare 1000 mV).
*/

#include "stm32f30x.h"
#define Q 3.0/4096.0

unsigned int codice;
float tensione;
int var=0;
int matricola = 5124;

void main(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //Abilitazione Periferiche
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  GPIOA->MODER |= GPIO_MODER_MODER4;   //Configuro la linea PA4 analogica (DAC1_OUT1)
  DAC->CR |= DAC_CR_EN1;   //Abilito il canale 1 del DAC
  for (int i=0;i<8000;i++);            //Attesa di almeno 10 us (T_wakeup)
  while(1){
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0) // se premo, metti a 1 var
      var = 1;
    if(var==1){  
      if(matricola>3000)
          matricola-=3000;
      tensione = (float) matricola / 1000; // tensione da generare 2100mV
      
      codice = tensione / Q; //codice da dare in input al DAC = tensione/quanto
      DAC->DHR12R1 = codice;  //Scrivo il codice nel Data holding register
    }
  }
}
