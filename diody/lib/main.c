/*
 * newtest.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "main.h"
#define TARGET_FREQ             WS2811_TARGET_FREQ

int kolor(int r,int g,int b){
	return (pow(16,4)*r) + (pow(16,2)*g) + b;
}

//ustawienie poczatkowych wartosci (ilosc led)
void setup(int liczba_diod,ws2811_t* ledstring){

	ledstring->freq = TARGET_FREQ;
	ledstring->dmanum = 5;
	ledstring->channel[0].count = liczba_diod;
	ledstring->channel[0].gpionum = 18;
	ledstring->channel[0].invert = 0;
	ledstring->channel[0].brightness = 255;
	ledstring->channel[0].strip_type = WS2811_STRIP_GRB;
	ledstring->channel[1].gpionum = 0;
	ledstring->channel[1].count = 0;
	ledstring->channel[1].invert = 0;
	ledstring->channel[1]. brightness = 0;
		
	ws2811_init(ledstring);
}

//wysylanie kolorow na doiody
void render(int data[][3],ws2811_t* ledstring){
//		printf("XXXX!!! %d %d %d\n", data[0][0], data[0][1], data[0][2]);
	for(int i=0;i < ledstring->channel[0].count;i++)
	{
		
		//r,g,b
		ledstring->channel[0].leds[i] =	kolor(data[i][0],data[i][1],data[i][2]);
		
	}
	ws2811_render(ledstring);
}

//zakonczenie dzialania diod
void finish(ws2811_t* ledstring){
	ws2811_fini(ledstring);
}
