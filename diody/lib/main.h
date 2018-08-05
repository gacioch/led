
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

//Sposob uzycia:
//do poprawnego dzialania potrzebna jest 1 instancja struktury ws2811_t
//instancje struktury (te sama) uzywa sie w kazdej funkcji biblioteki

//nic ciekawego mozna ZIGNOROWAC
int kolor(int r,int g,int b);
//funkcja potrzebna do uruchomienia by program poprawnie dzialal
//slyzy do przygotowania diod do pracy
void setup(int liczba_diod,ws2811_t* ledstring);
//funkcja wyswietlajaca kolory na pasku diod
//format tablicy data: pierwszy wymiar to kolejne diody na pasku zaczynajac od tych najblizej usb, drugi to kolejno wartosc red, green i blue
void render(int data[][3],ws2811_t* ledstring);
//funkcja odlaczajaca strukture ledstring od paska diod
//wykonac ja nalezy po zakonczeniu pracy diod i przed wyjsciem z programu
void finish(ws2811_t* ledstring);


