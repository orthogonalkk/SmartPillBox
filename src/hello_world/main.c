/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "fpioa.h"
#include <string.h>
#include "uart.h"
#include "gpiohs.h"
#include "sysctl.h"
#include <unistd.h>
#include <string.h>
#include <pwm.h>
#include<timer.h>

#define RECV_LENTH  4

#define CLOSLIGHT   0x55555555
#define OPENLIGHT   0xAAAAAAAA

#define UART_NUM    UART_DEVICE_3

volatile uint32_t s_count = 0;
volatile uint32_t s_recv_cnt = 0;
volatile uint32_t g_send_count = 0;
uint8_t recv_buf[128];

#define TIMER_NOR   0
#define TIMER_CHN   0
#define TIMER_PWM   1
#define TIMER_PWM_CHN 0


struct content
{
    /* data */
    char name[20];
    char how[40];
    int count_time;
};


void io_mux_init(void)
{
    fpioa_set_function(4, FUNC_UART1_RX + UART_NUM * 2);
    fpioa_set_function(5, FUNC_UART1_TX + UART_NUM * 2);
    fpioa_set_function(10, FUNC_TIMER1_TOGGLE1);
}

/*
UART INPUT
*/
void input_info(int medicine_count, struct content *total_medicine)
{
     char *hel_0 = {"please enter the medicine  \n"};
    char *hel_1 = {"please enter how you take medicine (once a week)  \n"};

    char recv = 0;
    int rec_flag = 0;
    char cmd[20];
    bool input_finish = false;
    int i = 0;

    while (1)
    {   
        if (rec_flag == 0)
        {
            uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, (uint8_t *)hel_0, strlen(hel_0));
        }
        else if(rec_flag == 1)
        {
            uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, (uint8_t *)hel_1, strlen(hel_1));
        }

        while (1)
        {
            uart_receive_data_dma(UART_NUM, DMAC_CHANNEL1, (uint8_t *)&recv, 1);    /*block wait for receive*/
            if (recv == '\n')
            {
                if (*cmd == 'q')
                {
                    input_finish = true;
                    break;
                }
                if (rec_flag == 0) // name
                {
                    sprintf(total_medicine[medicine_count].name, "%s", cmd);
                    // uart_send_data_dma(UART_NUM, DMAC_CHANNEL2, (uint8_t *)cmd, strlen(cmd));
                }
                else if (rec_flag == 1) // how
                {
                    sprintf(total_medicine[medicine_count].how, "%s", cmd);
                }    
                i =0;
                rec_flag ++;
                if (rec_flag >=2)
                {
                    rec_flag = 0;
                    medicine_count ++;
                }
                break;
            }
            cmd[i++] = recv;
            if(i >= RECV_LENTH)
            {
                i = 0;
                *cmd =0;
                rec_flag = 0;
            }

        }
       if (input_finish)
       {
           break;
       }
    }
}


void LCD_show()
{

}

void time_alarm()
{

}


struct content total_medicine[20];
int medicine_count = 0;

int main()
{
    io_mux_init();
    plic_init();
    sysctl_enable_irq();


    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    input_info(medicine_count, total_medicine);
   
}
