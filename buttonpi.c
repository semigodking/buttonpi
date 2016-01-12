/* buttonpi - A simple program that handles buttons on Raspberry Pi 2.
 * Copyright (C) 2015 Zhuofei Wang <semigodking@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define DEBOUNCE_MS 100

typedef struct button_info_t {
    int pin;
    unsigned int last_ms;
    int last_value;
    void (*interrupt)(void);
} button_info;

void button1_interrupt(void);
void button2_interrupt(void);

button_info buttons[] = {
    {.pin = 24,
     .interrupt = button1_interrupt,},
    {.pin = 25,
     .interrupt = button2_interrupt,}
};

int run_command(const char * cmd, int duration)
{
    char buff[256];
    snprintf(&buff[0], sizeof(buff), "%s %d", cmd, duration); 
    return system(buff);
}

unsigned int button_interrupt(button_info *bi)
{
    unsigned int now = millis();
    unsigned int duration = 0;
    int pin = bi->pin;
    int val = digitalRead(pin);
    if (val != bi->last_value)
    {
       // changed
       duration = now - bi->last_ms;
       if (duration > DEBOUNCE_MS)
       {
          if (val == 1)
          {
              return duration;
          }
          bi->last_ms = now;
          bi->last_value = val;
       }
    }
    return 0;
}


void button1_interrupt(void)
{
    unsigned int duration = button_interrupt(&buttons[0]);
    if (duration)
    {
        printf("pin %d\n", buttons[0].pin);
        run_command("/root/button1", duration);
    }
}


void button2_interrupt(void)
{
    unsigned int duration = button_interrupt(&buttons[1]);
    if (duration)
    {
        printf("pin %d\n", buttons[1].pin);
        run_command("/root/button2", duration);
    }
}



int main (void)
{
  // check wiringPi setup
  wiringPiSetup();
/*
  wiringPiISR(PIN_BUTTON2, INT_EDGE_FALLING, &button2_down);
  wiringPiISR(PIN_BUTTON2, INT_EDGE_RISING, &button2_up);
*/

  for (int i = 0; i < sizeof(buttons)/sizeof(button_info); i++)
  {
      button_info * bi = &buttons[i];
      int pin = bi->pin; 

      pinMode(pin, INPUT);
      pullUpDnControl(pin, PUD_UP);
      bi->last_value = digitalRead(pin);
      wiringPiISR(pin, INT_EDGE_BOTH, bi->interrupt);
  }
  for (;;)
     delay(1000);
  return 0;
}


