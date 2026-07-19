#include "stm32f1xx.h"

#define BUFFER_SIZE 512
volatile uint8_t Buffer[BUFFER_SIZE];
uint8_t CompressedBuffer[BUFFER_SIZE];
volatile uint8_t HalfDataReady = 0;
volatile uint8_t FullDataReady = 0;
volatile uint8_t CaptureRunning = 0;

void INIT_RCC_72 (void);
void INIT_USART1 (void);
void INIT_GPIO (void);
void INIT_TIM2 (void);
void INIT_DMA (void);
void INIT_BUTTON_IE (void);
void StartCapture (void);
void SendChar (char c);
void SendString (const char *str);
uint16_t RLE (volatile uint8_t *inbuf, uint8_t *outbuf);
void SendHex (uint8_t byte);


int main ()
{
	INIT_RCC_72();
	INIT_USART1();
	INIT_GPIO();
	INIT_TIM2();
	INIT_DMA();
	INIT_BUTTON_IE();


	//SendString("LA is ready!");

	while (1)
	{
		if (HalfDataReady && CaptureRunning)
		{
			HalfDataReady = 0;

			uint16_t ComressedBufferLength = RLE(Buffer, CompressedBuffer);

			for (int i = 0; i < ComressedBufferLength; i++)
			{
				SendHex(CompressedBuffer[i]);
			}

			//SendString("");
		}

		//for (int i = 0; i < 1000000; i++) {};

		if (FullDataReady && CaptureRunning)
		{
			FullDataReady = 0;

			uint16_t ComressedBufferLength = RLE(&Buffer[BUFFER_SIZE / 2], CompressedBuffer);

			for (int i = 0; i < ComressedBufferLength; i++)
			{
				SendHex(CompressedBuffer[i]);
			}

			//SendString("");
		}

		//for (int i = 0; i < 1000000; i++) {};
	}

	return 0;
}

void INIT_RCC_72 (void)
{
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)) {};

	FLASH->ACR &= ~FLASH_ACR_PRFTBE;
	FLASH->ACR |=  FLASH_ACR_PRFTBE;
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2;

	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |=  RCC_CFGR_HPRE_DIV1;
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |=  RCC_CFGR_PPRE1_DIV2;
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |=  RCC_CFGR_PPRE2_DIV1;

	RCC->CFGR &= (uint32_t)((uint32_t) ~ (RCC_CFGR_PLLSRC |
										  RCC_CFGR_PLLXTPRE |
										  RCC_CFGR_PLLMULL));
	RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {};

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
}

void INIT_USART1 (void)
{

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// TX PA9
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
	GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9);

	// RX PA10
	GPIOA->CRH &= ~GPIO_CRH_CNF10_0;
	GPIOA->CRH |=  GPIO_CRH_CNF10_1;
	GPIOA->CRH &= ~GPIO_CRH_MODE10;
	GPIOA->BSRR |= GPIO_ODR_ODR10;

	// 115200
	USART1->CR1 = USART_CR1_UE;
	USART1->BRR = 625;
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART1->CR2 = 0;
	USART1->CR3 = 0;

}

void INIT_GPIO (void)
{
	// pins A0-A7 floating input
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL = 0x88888888;

	// Initialization button on PA0
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0;
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;
	GPIOA->CRL |=  GPIO_CRL_CNF0_1;

	// UASRT was initialized in void INIT_USART1 (void)

	// Initialization LED on PB2
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	GPIOB->CRL &= ~GPIO_CRL_MODE2_0;
	GPIOB->CRL |=  GPIO_CRL_MODE2_1;
	GPIOB->CRL &= ~GPIO_CRL_CNF2_0;
	GPIOB->CRL &= ~GPIO_CRL_CNF2_1;


}

void INIT_TIM2 (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 71;             // PSC for 1MHz
	TIM2->ARR = 999;            // Period 1ms
	//TIM2->DIER |= TIM_DIER_UIE; // Update Interrupt Enable
	TIM2->DIER |= TIM_DIER_UDE;
	//TIM2->CR2 |= TIM_CR2_MMS_1;

	//NVIC_SetPriority(TIM2_IRQn, 0);
	//NVIC_EnableIRQ(TIM2_IRQn);
	//TIM2->CR1 |= TIM_CR1_CEN;
}
/*
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;





        static uint32_t cnt = 0;
        cnt++;

        if (cnt >= 15)
        {
        	cnt = 0;
        	GPIOB->ODR ^= (1 << 2);
        	SendString("LED was changed!");
        }

    }

}
*/

void INIT_DMA (void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;
	DMA1_Channel2->CPAR = (uint32_t)&GPIOA->IDR;
	DMA1_Channel2->CMAR = (uint32_t)Buffer;
	DMA1_Channel2->CNDTR = BUFFER_SIZE;

	DMA1_Channel2->CCR = 0;
	DMA1_Channel2->CCR |= DMA_CCR_MINC;
	DMA1_Channel2->CCR |= DMA_CCR_CIRC;
	DMA1_Channel2->CCR |= DMA_CCR_PL_1;
	DMA1_Channel2->CCR |= DMA_CCR_TCIE;
	DMA1_Channel2->CCR |= DMA_CCR_HTIE;

	NVIC_SetPriority(DMA1_Channel2_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

void DMA1_Channel2_IRQHandler (void)
{
	if (DMA1->ISR & DMA_ISR_HTIF2)
	{
		DMA1->IFCR |= DMA_IFCR_CHTIF2;
		HalfDataReady = 1;
		GPIOB->ODR ^= (1 << 2);

	}

	if (DMA1->ISR & DMA_ISR_TCIF2)
	{
		DMA1->IFCR |= DMA_IFCR_CTCIF2;
		FullDataReady = 1;
		GPIOB->ODR ^= (1 << 2);
	}
}

void StartCapture (void)
{
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;
	while(DMA1_Channel2->CCR & DMA_CCR_EN) {};

	DMA1_Channel2->CNDTR = BUFFER_SIZE;
	HalfDataReady = 0;
	FullDataReady = 0;

	TIM2->CNT = 0;
	DMA1_Channel2->CCR |= DMA_CCR_EN;
	TIM2->CR1 |= TIM_CR1_CEN;

	CaptureRunning = 1;
}

void StopCapture(void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;
	while(DMA1_Channel2->CCR & DMA_CCR_EN) {};

	HalfDataReady = 0;
	FullDataReady = 0;
	CaptureRunning = 0;
}

void SendChar (char c)
{
	while ((USART1->SR & USART_SR_TXE) == 0) {};
	USART1->DR = c;
}

void SendString (const char *str)
{
	while (*str) {SendChar(*str++);};
	SendChar('\r');
	SendChar('\n');
}

uint16_t RLE (volatile uint8_t *inbuf, uint8_t *outbuf)
{
	uint16_t inbuf_idx = 0;
	uint16_t outbuf_idx = 0;
	//SendString("I'm in RLE (start)");

	while (inbuf_idx < BUFFER_SIZE / 2)
	{
		uint8_t cur_val = inbuf[inbuf_idx];
		uint8_t cur_len = 0;
		//SendString("log0");

		while (inbuf_idx < (BUFFER_SIZE / 2) &&
				cur_val == inbuf[inbuf_idx] &&
				cur_len < (BUFFER_SIZE / 2))
		{
			//SendString("log1");
			cur_len++;
			inbuf_idx++;
		}

		outbuf[outbuf_idx++] = cur_val;
		outbuf[outbuf_idx++] = (cur_len - 1);
		//SendString("log3");

	}
	//SendString("I'm in RLE (end)");

	return outbuf_idx;
}

void SendHex(uint8_t byte)
{
    const char hex[] = "0123456789ABCDEF";
    SendChar(hex[(byte >> 4) & 0x0F]);
    SendChar(hex[byte & 0x0F]);
}

void INIT_BUTTON_IE (void)
{
	EXTI->PR  |=  EXTI_PR_PR0;
	EXTI->IMR |=  EXTI_IMR_MR0;

	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0_PA;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	EXTI->FTSR |= EXTI_FTSR_TR0;

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 0);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)
    {
        EXTI->PR |= EXTI_PR_PR0;

        if (CaptureRunning)
        {
            StopCapture();
        }
        else
        {
            StartCapture();
        }
    }
}


