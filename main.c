#include "main.h"
#include "cfg.h"
#include "init.h"
#include "bldc.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

extern motor_ctrl motor;

int main(void)
{
	sys_init();
	motor.pulse = (TIMARR+1)/2;		//50%ռ�ձ�
	uhvl();

    uint8_t len;
    uint16_t times = 0;

    HAL_Init();                             /* ԵʼۯHALࠢ */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ʨ׃ʱדΪ72Mhz */
    delay_init(72);                         /* ғʱԵʼۯ */
    usart_init(115200);                     /* ԮࠚԵʼۯΪ115200 */
    while (1)
    {
        if (g_usart_rx_sta & 0x8000)      /* ޓ˕ս˽ߝ? */
        {
            len = g_usart_rx_sta & 0x3fff;  /* փսՋՎޓ˕սք˽ߝӤ׈ */
            printf("\r\nGoodMorning!\r\n");

            HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf, len, 1000);  
            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           
            printf("\r\n\r\n");           
            g_usart_rx_sta = 0;
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\r\nHello\r\n");
                printf("Ican\r\n\r\n\r\n");
            }

            if (times % 200 == 0)printf("Ohyes\r\n");

            delay_ms(10);
        }
    }
}



