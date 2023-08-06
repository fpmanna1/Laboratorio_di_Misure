/*
Realizzare un firmware in cui si aspetta la pressione del pulsante User (PA0) e, 
e successivamente se ne attende il rilascio. Con un timer, 
misurare la durata dell’intervallo di tempo in cui il pulsante è stato mantenuto premuto.
Mostrare la durata, in secondi, in una variabile globale.
*/

#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define TCK 125*pow(10, -9)     // TCK = 125 ns

double secondi = 0;  // variabile globale per la live watch

void main()
{
  int conteggio;
  int i = 1; // indice del while(numero di conteggi)
  
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // per la porta A
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // per il TIM2
  
  while(1){
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0){ // se user è premuto
      TIM2->CR1 |= TIM_CR1_CEN; // abilita il conteggio
      TIM2->CNT = 0;    // azzera il conteggio
      
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0); // attende mentre user è premuto
      
      TIM2->CR1 &= ~TIM_CR1_CEN;        // disabilita il conteggio
      conteggio = TIM2->CNT;
      secondi = (float) conteggio * TCK;
      
      printf("conteggio %d:\t%f secondi\n", i, secondi);
      i++;
    }
  }
} 

  
  
  
