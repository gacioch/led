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


#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>
#include <math.h>

#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

#include "ws2811.h"

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define MSG_SIZE		sizeof(int) * 3 * 64

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = 5,
    .channel =
    {
        [0] =
        {
            .gpionum = 18,
            .invert = 0,
            .brightness = 255,
            .strip_type = WS2811_STRIP_GRB,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

struct msgbuf{
	long mtype;
	int r[64];
	int g[64];
	int b[64];
};

int kolor(int r,int g,int b){
	return (pow(16,4)*r) + (pow(16,2)*g) + b;
}

int main(int argc, char *argv[])
{
	struct msgbuf buf;
    ws2811_return_t ret;
	int n,qid;
	sscanf(argv[1],"%d",&n);	
	ledstring.channel[0].count = n;
	
    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return ret;
    }
	if((qid = msgget(999,0666 | IPC_CREAT)) < 0){
		fprintf(stderr,"blad: nie mozna utworzyc kolejki\n");
		return -1;
	}
	while(1){
		msgrcv(qid,&buf,MSG_SIZE,1,0);
		for(int i = 0;i < n; i ++){
			ledstring.channel[0].leds[i] =	kolor(buf.r[i],buf.g[i],buf.b[i]);
		}
		ws2811_render(&ledstring);
	} 

    ws2811_fini(&ledstring);
    printf ("\n");    
    return ret;
}

