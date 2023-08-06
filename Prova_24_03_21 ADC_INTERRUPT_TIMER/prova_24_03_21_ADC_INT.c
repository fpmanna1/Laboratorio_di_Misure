/* Realizzare uno script che acquisisce e converte in digitale dal
 pin PA2 (IN3 di ADC1). Le conversioni devono avvenire ad
 intervalli regolari, pari alla seconda parte della propria matricola espressa in ms.
L’update del timer deve attivare una routine di interrupt, nella quale 
viene avviata la conversione A/D. */

#include "stm32f30x.h"
#include <stdio.h>

#define Q 3.0/4096

unsigned int codice;
float tensione;
float tempo;

void ADCsetup();

void main(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_ADC12EN;  //Abilitazione Periferiche
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  GPIOA->MODER |= GPIO_MODER_MODER0;   //Configuro la linea PA0 analogica (ADC1_IN1)
  
  ADCsetup();
  
  //TIM2->ARR = 72000000; //Base dei tempi a 1 s
  TIM2->ARR = (5124/1000)*72000000;
  TIM2->DIER |= TIM_DIER_UIE; //Abilitazione interrupt di TIM2 sull'Update
  NVIC->ISER[0] |= (1<<28); //Abilitazione l'interrupt in NVIC
  tempo=0;
  TIM2->CR1 |= TIM_CR1_CEN; //Abilito il conteggio del contatore del timer

  while(1)
  {
  }
}

void TIM2_IRQHandler (void)
{
  TIM2->SR &= ~TIM_SR_UIF; //Abbaso il flag
  tempo=tempo+1.000;
  ADC1->CR |= ADC_CR_ADSTART;   //Avvio la conversione (ADSTART)
  while ((ADC1->ISR & ADC_ISR_EOC)==0);      //Aspetto la fine della conversione
  codice = ADC1->DR;//Leggo il risultato
  tensione = codice *Q;
  printf("tempo = %f, codice = %d, tensione = %f\n",tempo,codice,tensione);
}

void ADCsetup()
{
  // Abilitazione regolatore di tensione
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;       // ADVREGEN = 10->00
  ADC1->CR |= ADC_CR_ADVREGEN_0;        // ADVREGEN = 00->01
  for(int i=0; i<72000; i++);             // attesa di 10 us
    
  // Configurazione clock
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;    // CKMODE=01, clock del bus
  
  // Calibrazione ADC
  //ADC1_2->CCR |= ADC_CR_ADCAL;  // ADCAL=1, avvio calibrazione
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);     // attesa ADCAL=0
  
  // Abilitazione ADC
  ADC1->CR |= ADC_CR_ADEN;      // ADEN=1, abilitazione ADC
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);    // attesa ADRDY=1
  
  // Configurazione ADC
  ADC1->CFGR |= ADC_CFGR_CONT;  // CONT=1, conversione continua
  ADC1->SQR1  = ADC_SQR1_SQ1_0; // SQ1=00001: canale 1 (PA0)
  ADC1->SQR1 &= ~ADC_SQR1_L;    // L=0: 1 conversione
  ADC1->SMPR1 |= ADC_SMPR1_SMP1;    // SMP1 = 111, 601.5 CK
}
