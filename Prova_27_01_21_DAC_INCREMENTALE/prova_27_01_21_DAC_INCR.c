/* Realizzare uno script che genera una tensione analogica con il
 convertitore D/A pari a 0,1 V. Ad ogni pressione del tasto user,
 la tensione generata si incrementa di 0,1 V. Se il valore di 
tensione supera 3 V, riparte da 0,1 V. */

#include <stm32f30x.h>
#define Q 3.0/4096.0

unsigned int codice;
float tensione = 0;
int var=0;

void main(){
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  GPIOA->MODER |= GPIO_MODER_MODER4;
  DAC->CR |= DAC_CR_EN1;
  for(int i=0; i<8000; i++);
  while(1){
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
      var=1;
    while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
      if(var==1){ 
        var=0;
        tensione+= 100.00; // incrementi di 100mV (0.1V)
        codice = (unsigned int) (tensione/Q);
        DAC->DHR12R1 = codice;
        if(tensione> 3000.0)
          tensione=100.00;
      }
    }
}
  
  
