#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ADCSWTrigger.h"
#include "inc/ST7735.h"

void ADC_Init(void){
// write this
	  SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10;    // 2) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};  // 3 for stabilization
  GPIO_PORTE_DIR_R &= ~0x10;    // 4) make PE4 input
  GPIO_PORTE_AFSEL_R |= 0x10;   // 5) enable alternate function on PE4
  GPIO_PORTE_DEN_R &= ~0x10;    // 6) disable digital I/O on PE4
  GPIO_PORTE_AMSEL_R |= 0x10;   // 7) enable analog functionality on PE4
// while((SYSCTL_PRADC_R&0x0001) != 0x0001){}; // good code, but not implemented in simulator
  ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 9;           // 12) set channel
  ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3


}
int32_t ADC_In(void){
uint32_t result;

  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3

  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done

  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result

  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion

  return result;

}




uint32_t bass_amplitude;

uint32_t xpos;
void Visualize(void){
	
  
  
  bass_amplitude= ADC_In();
  //pos_y= (150*bass_amplitude)/4095;
  
  //ST7735_DrawFastVLine(xpos, 0, 160, ST7735_BLUE)
	
	  uint32_t pos_y= (150*bass_amplitude)/4095;

	ST7735_DrawFastVLine(xpos, 0, 160, ST7735_BLACK); 
  ST7735_DrawPixel(xpos, pos_y, ST7735_BLUE);           

	
	
	
	xpos++;
	if(xpos>=128){

    xpos=0;
  }
	
	
	
}


int main(void) {
  PLL_Init(80000000);
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);
  ADC_Init();
  while(1){

bass_amplitude=ADC_In();

  }


}