
#include "stm32f407xx.h"

void SysClockConfig (void){
	//ENABLE HSE and wait for the HSE to become Ready
	/*UC: the 16th bit of the RCC's control register is set on.
	 * then loop until HSE is ready
	 */

	#define PLL_M 4
	#define PLL_N 168
	#define PLL_P 0 //PLLP = 2


	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

	/*UC: Enable Power interface bit of peripheral bus.
	 * Enable VOLTAGE REGULAR through PWR.
	 */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	//Configure FLASH PREFETCH and LATENCY related settings
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

	//Configure PRESCALARS for HCLK, PCLK1, AND PCLK2
	//AHB PR.
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//APB1 PR.
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	//APB2 PR.
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	//Configure main PLL
	RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6)| (PLL_P << 16) | (RCC_PLLCFGR_PLLSRC_HSE);

	//Enable PLL and wait for it to become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	//Select Clock Source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

}

void GPIO_Config(void){
	//Enable GPIO CLOCK
	/* UC: I am deviating here because NUCLEO LEDs are GPIOA while DISCOVERY LEDs are GPIOD
	 * and pin 12 vice pin 5.
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	//Setting GPIOA as OUTPUT
	/*UC: Here, I chose to use ODR as opposed to BSRR.
	 *
	 */
	GPIOD->MODER &= ~(3 << 24); // clear pin PD12 (bits 25:24) as Output (01)
	GPIOD->MODER |= (1 << 24); // set pin PD12 (bits 25:24) as Output (01)

}

void delay(uint32_t time){
	while(time > 0){
		time--;
	}
}

int main (void){

	SysClockConfig();
	GPIO_Config();
	while(1){
		GPIOD->ODR |= (1<<12); //set pin PD12 to turn green LED on.
		delay(10000000); //delay turning off for a second.
		GPIOD->ODR &= ~(1<<12); //reset pin PD12 to turn green LED off.
		delay(10000000); //delay turning off for a second.
	}
}

