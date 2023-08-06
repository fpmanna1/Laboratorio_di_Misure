/*
Acquisire con il convertitore A/D dal canale PA1 (Canale IN2 di ADC1) a 
intervalli di tempo regolari pari a 0.5 secondi. 
Realizzare la base dei tempi con Timer 2.
*/

#include "stm32f30x.h"
#include <stdio.h>

#define Q 3.0/4096  // VDD/2^n

unsigned int codice;
float tensione;
double tempo;

void ADCsetup();

void main()
{
  
  // Abilitazione ADC12, GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_ADC12EN;       // Abilitazione ADC12, GPIOA
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;    // abilitazione TIM2
  GPIOA->MODER |= GPIO_MODER_MODER0;    // PA0 in modalità analogica
  
  ADCsetup();
  
  TIM2->ARR = 8000000/2; //Base dei tempi a 0.5 s
  TIM2->CR2 |= (1<<5); //Abilito la linea TRGO sull'UPDATE (MMS=010)
  tempo=0;
  TIM2->CR1 |= TIM_CR1_CEN;     // Abilito il conteggio del contatore del timer
  ADC1->CR |= ADC_CR_ADSTART;   // Abilito la ricezione dei trigger esterni
  
  while(1){
    while((ADC1->ISR & ADC_ISR_EOC)==0); // Aspetto la fine della conversione
    tempo+= 1.000;
    codice = ADC1->DR;  // leggo il risultato
    tensione = codice * Q;
    printf("tempo = %f, codice = %d, tensione = %f\n", tempo, codice, tensione);
  }  
}

void ADCsetup(){
  // Abilitazione regolatore di tensione
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;       // ADVREGEN = 10->00
  ADC1->CR |= ADC_CR_ADVREGEN_0;        // ADVREGEN = 00->01
  for(int i=0; i<8000; i++);             // attesa di 10 us
    
  // Configurazione clock
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;    // CKMODE=01, clock del bus
  
  // Calibrazione ADC
  ADC1_2->CCR |= ADC_CR_ADCAL;  // ADCAL=1, avvio calibrazione
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);     // attesa ADCAL=0
  
  // Abilitazione ADC
  ADC1->CR |= ADC_CR_ADEN;      // ADEN=1, abilitazione ADC
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);    // attesa ADRDY=1
  
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;       // Abilitazione trigger esterno fronte di salita
  ADC1->CFGR |= ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0;  
  // Trigger sul TRGO di Timer 2 (da tab. 90 ref manual, EXTSEL = 1011;)
  
  // Configurazione ADC
  ADC1->CFGR |= ADC_CFGR_CONT;  // CONT=1, conversione continua
  ADC1->SQR1  = ADC_SQR1_SQ1_0; // SQ1=00001: canale 1 (PA0)
  ADC1->SQR1 &= ~ADC_SQR1_L;    // L=0: 1 conversione
  ADC1->SMPR1 |= ADC_SMPR1_SMP1;    // SMP1 = 111, 601.5 CK
} 

/* -----VERSIONE PROF------
#include "stm32f30x.h"
#include <stdio.h>

#define Q 3.0/4096


unsigned int codice;
float tensione;
float tempo;

void main(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_ADC12EN;  //Abilitazione Periferiche
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  GPIOA->MODER |= GPIO_MODER_MODER0;   //Configuro la linea PA0 analogica (ADC1_IN1)
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;        //Abilitazione Regolatore tensione (ADVREGEN)
  for (int i=0;i<8000;i++);            //Attesa di almeno 10 us
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;   //Impostazione clock ADC = clock di sistema
  ADC1->CR |= ADC_CR_ADCAL;  //Avvio calibrazione ADC
  while ((ADC1->CR & ADC_CR_ADCAL)!=0); //Attesa fine calibrazione
  ADC1->CR |= ADC_CR_ADEN;     //Abilitazione ADC
  while ((ADC1->ISR & ADC_ISR_ADRD)==0); //Attesa fine abilitazione
                                         //Configurazioni ADC (RES, ALIGN, CONT)
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;            //Abilitazione Trigger esterno fronte di salita
  ADC1->CFGR |= ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0; //Trigger sul TRGO di Timer 2 (da tab. 90 Ref manual, EXTSEL=1011;
  ADC1->SQR1 |= (1<<6);    //Definizione sequenza = IN1
  ADC1->SMPR1 |= (5<<3);   //Definizione tempo di sampling (SMP)
  TIM2->ARR = 8000000; //Base dei tempi a 1 s
  TIM2->CR2 |= (1<<5); //Abilito la linea TRGO sull'UPDATE (MMS=010)
  tempo=0;
  TIM2->CR1 |= TIM_CR1_CEN; //Abilito il conteggio del contatore del timer
  ADC1->CR |= ADC_CR_ADSTART;   //Abilito la ricezione dei trigger esterni
  while(1)
  {
    while ((ADC1->ISR & ADC_ISR_EOC)==0);      //Aspetto la fine della conversione
    tempo=tempo+1.000;
    codice = ADC1->DR;//Leggo il risultato
    tensione = codice *Q;
    printf("tempo = %f, codice = %d, tensione = %f\n",tempo,codice,tensione);
 
  }
  
} */





    
 
  
  
