all: dma.o mailbox.o main.o pcm.o pwm.o rpihw.o ws2811.o klient2.o
	g++ -O3 -o klient2 klient2.o dma.o mailbox.o main.o pcm.o pwm.o rpihw.o ws2811.o `pkg-config --libs opencv`
klient2: klient2.o
	g++ -O3 -c klient2.cpp


