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
#include <stdlib.h>
#include "fpioa.h"
#include <string.h>
#include "uart.h"
#include "gpiohs.h"
#include "sysctl.h"
#include <unistd.h>
#include <string.h>
#include <pwm.h>
#include<timer.h>
#include <rtc.h>
#include "board_config.h"
#include "nt35310.h"
#include "lcd.h"

#define RECV_LENGTH  20

#define UART_NUM    UART_DEVICE_3

uint8_t recv_buf[128];

#define TIMER_NOR   0
#define TIMER_CHN   0
#define TIMER_PWM   1
#define TIMER_PWM_CHN 0

uint16_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX ];

struct content
{
    /* data */
    char name[20];
    char how[40];
    int count_time;
};


struct content total_medicine[20];
int medicine_count = 0;
int medicine_interval = 0;

static void io_set_power(void)
{
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

void io_mux_init(void)
{
    fpioa_set_function(4, FUNC_UART1_RX + UART_NUM * 2);
    fpioa_set_function(5, FUNC_UART1_TX + UART_NUM * 2);
    fpioa_set_function(10, FUNC_TIMER1_TOGGLE1);

    // LCD
    fpioa_set_function(LCD_DC_PIN, FUNC_GPIOHS0 + LCD_DC_IO);
    fpioa_set_function(LCD_CS_PIN, FUNC_SPI0_SS3);
    fpioa_set_function(LCD_RW_PIN, FUNC_SPI0_SCLK);
    fpioa_set_function(LCD_RST_PIN, FUNC_GPIOHS0 + LCD_RST_IO);

    sysctl_set_spi0_dvp_data(1);

    // LCD Backlight
    fpioa_set_function(LCD_BLIGHT_PIN, FUNC_GPIOHS0 + LCD_BLIGHT_IO);
    gpiohs_set_drive_mode(LCD_BLIGHT_IO, GPIO_DM_OUTPUT);
    gpiohs_set_pin(LCD_BLIGHT_IO, GPIO_PV_LOW);
}

/*
UART INPUT
*/
void input_info()
{
     char *hel_0 = {"please enter the medicine  \n"};
    char *hel_1 = {"please enter how you take medicine (once a week)  \n"};

    char *hel = {"please enter the interval you take medicine (seconds)  \n"};

    char recv = 0;
    int rec_flag = 0;
    char cmd[20] = {};
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
            if (recv == '\r') {
                continue;
            }
            else if (recv == '\n')
            {
                uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, (uint8_t *)cmd, strlen(cmd));
                if (strcmp(cmd, "q") == 0)
                {
                    uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, (uint8_t *)cmd, strlen(cmd));
                    input_finish = true;
                    rec_flag =2;
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
                i = 0;
                cmd[0] = 0;
                rec_flag ++;
                if (rec_flag >=2)
                {
                    rec_flag = 0;
                    medicine_count ++;
                }
                break;
            }
            else {
                cmd[i++] = recv;
                cmd[i] = 0;
                if(i >= RECV_LENGTH)
                {
                    i = 0;
                    *cmd =0;
                    rec_flag = 0;
                } 
            }
        }
       if (input_finish)
       {
            *cmd = 0;
           break;
       }
    }
    // get time
    uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, (uint8_t *)hel, strlen(hel));
    while(1)
    {
        uart_receive_data_dma(UART_NUM, DMAC_CHANNEL1, (uint8_t *)&recv, 1);    /*block wait for receive*/
        if (recv == '\n')
        {
            medicine_interval = atoi(cmd);
            break;
        }
        cmd [i++] = recv;
    }

}

void LCD_timer()
{
    int t = medicine_interval;
    t=60;
    while (t>=0)
    {
        char time[10];
        itoa(t, time, 10 );
        ram_draw_string((uint32_t* ) g_lcd_gram, 50, 50, time, RED);
        lcd_draw_picture(0,0, LCD_Y_MAX, LCD_X_MAX,(uint32_t*) g_lcd_gram);
        sleep(1);
        memset(g_lcd_gram, 0, sizeof(g_lcd_gram));
        t--;
    }
    
}

void LCD_show()
{

}


// alarm

uint16_t hsv_to_rgb(int h,int s,int v)
{
    float R = 0, G =0, B =0;

    float C = 0,X = 0,Y = 0,Z = 0;
    int i=0;
    float H=(float)(h);
    float S=(float)(s)/100.0;
    float V=(float)(v)/100.0;
    if(S == 0)
        R = G = B = V;
    else
    {
        H = H/60;
        i = (int)H;
        C = H - i;
 
        X = V * (1 - S);
        Y = V * (1 - S*C);
        Z = V * (1 - S*(1-C));
        switch(i){
            case 0 : R = V; G = Z; B = X; break;
            case 1 : R = Y; G = V; B = X; break;
            case 2 : R = X; G = V; B = Z; break;
            case 3 : R = X; G = Y; B = V; break;
            case 4 : R = Z; G = X; B = V; break;
            case 5 : R = V; G = X; B = Y; break;
        }
    }
    uint16_t r = (uint16_t) (R * 255.0);
    uint16_t g = (uint16_t) (G * 255.0);
    uint16_t b = (uint16_t) (B * 255.0);
    return ((b >> 3) << 0) | ((g >> 2) << 5) | ((r >> 3) << 11);
}

void time_alarm()
{
    uint16_t colors[360];
    for (size_t i = 0; i < 360; i++)
    {
        colors[i] = hsv_to_rgb(i, 100, 100);
    }
    while (true)
    {
        for(int i=0;i<360;i++)
        {
            for(int j = 0; j < sizeof(g_lcd_gram) / sizeof(g_lcd_gram[0]); j++)
            {
                g_lcd_gram[j] = colors[i];
            }
            lcd_draw_picture(0,0, LCD_Y_MAX, LCD_X_MAX,(uint32_t*) g_lcd_gram);
        }
    }
    
}


int main()
{
    io_mux_init();
    plic_init();
    sysctl_enable_irq();
    io_set_power();
    lcd_init();
    lcd_set_direction(DIR_YX_RLDU);
    lcd_clear(BLACK);


    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    time_alarm();
    input_info();
    LCD_timer();
   
}
