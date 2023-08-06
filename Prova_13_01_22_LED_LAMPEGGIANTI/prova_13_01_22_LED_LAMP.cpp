/* Realizzare uno script che, alla pressione del tasto USER, faccia partire
 il lampeggio degli 8 led con un tempo di lampeggio pari alla propria
  matricola espressa in millisecondi
*/

#include <stm32f30x.h>

short int flag = 0;
int matricola = 46005124;
int var;

void main(){
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
  GPIOE->MODER = 0x55550000; // output di tutti i led
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // per il tim2
  
  TIM2->CR1 |= TIM_CR1_CEN;     // abilita il conteggio
  TIM2->ARR = matricola;        // tempo in ms della matricola
  TIM2->CNT = 0;        // azzera il conteggio
  
  while(1){
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0) // se premo, metti a 1 var
      var = 1;
    if(var==1){ // se il pulsante è stato premuto
      if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF && !flag)   // se ha raggiunto il conteggio
      {
        GPIOE->ODR = 0x0000FF00;  // accende i led
        TIM2->SR &= ~TIM_SR_UIF;  // azzera l'UIF (Update Interrupt Flag)
        flag = 1; // segnala che è acceso
      }
      if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF && flag)
      {
        GPIOE->ODR = 0;   // spegne i led
        TIM2->SR &= ~TIM_SR_UIF;   // azzera l'UIF
        flag = 0; // segnala che è spento 
      }
    } 
  }
}
