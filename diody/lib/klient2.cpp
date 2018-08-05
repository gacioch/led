#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/cvaux.h"
#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/cxcore.h"
#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/highgui.h"
#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/cv.h"

#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/cxmisc.h"
#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv/ml.h"

#include "opencv2/core/core.hpp"
#include "/home/pi/rpisss/rpi2/opencv/opencv-3.2.0/include/opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#include <stdio.h>
extern "C"{
#include "main.h"
}
using namespace cv;
using namespace std;

#include <stdlib.h>  
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<string>
#include<fstream>
#include<cstdlib>

//przesylanie zmeinnych miedzy watkami
class memory{
	public: int msg[9];
		char ssid[16];
		char haslo[16];
};

//wyszukiwanie i zamiana podanej opcji w podanym pliku konfiguracyjnym na podana wartosc
void zmiana(string opcja,string wartosc,string plik){
	string tekst;
	ifstream myfile;
	ofstream out;
	int flaga = 0;
	opcja += '=';	

  	myfile.open(plik.c_str());
	out.open("out.txt");

 	while(getline(myfile,tekst)){		
		if(tekst.find(opcja)!=string::npos){
			tekst = opcja + wartosc;
			flaga = 1;
		}
		out << tekst << endl;
		if(flaga == 1)
			goto maslo;
	}
	maslo:
  	myfile.close();	
	out.close();
	string komenda;
	komenda = "rm " + plik;
	system(komenda.c_str());
	komenda = "mv out.txt " + plik;
	system(komenda.c_str());
}

//zamiana ssid i hasla
void full(string ssid,string haslo){
	zmiana("ssid",ssid,"/etc/hostapd/hostapd.conf");
	zmiana("wpa_passphrase",haslo,"/etc/hostapd/hostapd.conf");
	system("systemctl stop hostapd");
	system("systemctl stop dnsmasq");
	system("systemctl start hostapd");
	system("systemctl start dnsmasq");
}

//ustawianie kontrastu pobieranego obrazu
Mat balans(float alpha, int beta, Mat img){
	Mat new_image = Mat::zeros(img.size(), img.type());
	for (int y=0;y<img.rows;y++){
		for (int x=0; x<img.cols; x++){
			for (int c=0; c<3;c++){
				new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>(alpha*(img.at<Vec3b>(y,x)[c] + beta));
			}
		}
	}
	return new_image;
}

//ustawianie gammy pobieranego obrazu
Mat correctGamma( Mat& img, double gamma ) {
	double inverse_gamma = 1.0 / gamma;
 
	Mat lut_matrix(1, 256, CV_8UC1 );
	uchar * ptr = lut_matrix.ptr();
 	for( int i = 0; i < 256; i++ )
   	ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma ) * 255.0 );
 
 	Mat result;
 	LUT( img, lut_matrix, result );
 
 	return result;
}

//przycinanie obrazu do podanych punktow
Mat trans(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, Mat image){
	Point2f srcTri[4];
	Point2f dstTri[4];

	Mat rot_mat(2, 3, CV_32FC1);
	Mat warp_mat(2, 3, CV_32FC1);
	Mat warp_dst, warp_rotate_dst;

	//punkty poczatkowe
	srcTri[0] = Point2f(x0, y0);
	srcTri[1] = Point2f(x1, y1);
	srcTri[2] = Point2f(x2, y2);
	srcTri[3] = Point2f(x3, y3);
	//punkty po przeksztalceniu
	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f(0, image.rows - 1);
	dstTri[2] = Point2f(image.cols - 1, 0);
	dstTri[3] = Point2f(image.cols - 1, image.rows -  1);
	//definuiowanie transformacji
	warp_mat = getPerspectiveTransform(srcTri, dstTri);
	//zastosowanie transformacji na danym obiekcie
	warpPerspective(image, warp_dst, warp_mat, warp_dst.size());

	return warp_dst;
}

//pobieranie dominujacego koloru w danym fragmecie obrazu
int *dominate(Mat image, int led_pion, int led_poz){
	int i;
	static int rgb_dom[147];

	int ity = 0;
	//zmienne szerokosc i wysokosc danego obdszaru
	int pix_pion, pix_poz;
	pix_pion = image.rows/led_pion;
	pix_poz = image.cols/led_poz;

	int red = 0, green = 0, blue = 0;
	//lewa krawedz ekranu
	for (i=led_pion; i>0; i--){
		for(int l=pix_pion*i; l>pix_pion*(i-1); l--){
			for (int k=0; k<pix_poz; k++){
				red = red + image.at<Vec3b>(l,k)[2];
				green = green + image.at<Vec3b>(l,k)[1];
				blue = blue + image.at<Vec3b>(l,k)[0];
			}
		}
		red = red/(pix_pion*pix_poz);
		green = green/(pix_pion*pix_poz);
		blue = blue/(pix_pion*pix_poz);

		rgb_dom[ity] = red;
		ity++;
		rgb_dom[ity] = green;
		ity++;
		rgb_dom[ity] = blue;
		ity++;

		red = 0;
		green = 0;
		blue = 0;
	}
	//gorna krawedz ekranu
	for (i=0; i<led_poz; i++){
		for(int l=0; l<pix_pion; l++){
			for (int k=i*pix_poz; k<(i+1)*pix_poz; k++){
				red = red + image.at<Vec3b>(l,k)[2];
				green = green + image.at<Vec3b>(l,k)[1];
				blue = blue + image.at<Vec3b>(l,k)[0];
			}
		}
		red = red/(pix_pion*pix_poz);
		green = green/(pix_pion*pix_poz);
		blue = blue/(pix_pion*pix_poz);

		rgb_dom[ity] = red;
		ity++;
		rgb_dom[ity] = green;
		ity++;
		rgb_dom[ity] = blue;
		ity++;

		red = 0;
		green = 0;
		blue = 0;
	}
	//prawa krawedz ekranu
	for (i=0; i<led_pion; i++){
		for(int l=pix_pion*i; l<pix_pion*(i+1); l++){
			for (int k=pix_poz*(led_poz-1); k<(pix_poz*led_poz); k++){
				red = red + image.at<Vec3b>(l,k)[2];
				green = green + image.at<Vec3b>(l,k)[1];
				blue = blue + image.at<Vec3b>(l,k)[0];
			}
		}
		red = red/(pix_pion*pix_poz);
		green = green/(pix_pion*pix_poz);
		blue = blue/(pix_pion*pix_poz);

		rgb_dom[ity] = red;
		ity++;
		rgb_dom[ity] = green;
		ity++;
		rgb_dom[ity] = blue;
		ity++;

		red = 0;
		green = 0;
		blue = 0;
	}
	//dolna krawedz ekranu
	for (i=led_poz-1; i>0; i--){
		for(int l=pix_pion*(led_pion-1); l<pix_pion*led_pion; l++){
			for (int k=i*led_poz; k<(i+1)*led_poz; k++){
				red = red + image.at<Vec3b>(l,k)[2];
				green = green + image.at<Vec3b>(l,k)[1];
				blue = blue + image.at<Vec3b>(l,k)[0];
			}
		}
		red = red/(pix_pion*pix_poz);
		green = green/(pix_pion*pix_poz);
		blue = blue/(pix_pion*pix_poz);

		rgb_dom[ity] = red;
		ity++;
		rgb_dom[ity] = green;
		ity++;
		rgb_dom[ity] = blue;
		ity++;

		red = 0;
		green = 0;
		blue = 0;
	}
	//zwraca tablice kolorow na krawedziach t[]={r1,g1,b1,...}
	return rgb_dom;
}

int main(int argc, char **argv){     
   	
   	//deklaracja led i wczytywanie kamery
	struct timeval czas;
    	memory* mem;
    	int id = shmget(98669,sizeof(mem),IPC_CREAT | 0666);
    	if(id < 0){
		cout << "shmget error" << endl;
    		return -1;
    	}
    //msg[0] i jej wartosci:
    //0 - wyloczone ledy
    //1 - pobieranie obrazu z kamery
    //2 - ledy swieca na wybrany kolor
	//3 - odbieranie punktow
	//msg[1], msg[2] i msg[3] to kolejno rgb
    	mem = (memory*)shmat(id,NULL,0);
    	if(mem == (memory*) -1){
    		cout << "shmat error" << endl;
		return -1;
    	}
    	mem->msg[0] = 1;
	//dekjlaracja tablicy t[ilosc led][3 - R,G,B]
	int t[76][3];
	int il_led_pion = 16, il_led_poz=22;
	int *tablica;
	int flaga=0;
	//flaga uzywana przy odbiorze kolorow z apki
	//0 - nic nie rob
	//1 - odbieramy czerwony
	//2 - odbieramy zielony
	//3 - odbieramy niebieski
	//4 - nic nie robi
	//5 - kalibracja
	ws2811_t diody;

	setup(76, &diody);
	
	//deklaracja klasy od kamery
	VideoCapture cap(0);
	//zmienne przechowujace klatki z kamery w postaci macierzy
	Mat src_klatka, dst_klatka;
	
	//sprawdzanie czy jest polaczenie z kamera
	if(!cap.isOpened()){
		cout << "blad kamery" << endl;
		return 0;
	}
	//ustawianie rozdzielczosci kamery
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	
	bool write = false;
   	int our_input_fifo_filestream = -1;
   	int result;
   	int pid;
   	int socket_desc, client_sock, c, read_size;
   	struct sockaddr_in server, client;
   	char client_message[128];
   	socket_desc=socket(AF_INET, SOCK_STREAM, 0);
   	server.sin_family=AF_INET;
   	server.sin_addr.s_addr=INADDR_ANY;
   	server.sin_port=htons(25005);
   	bind(socket_desc,(struct sockaddr *)&server, sizeof(server));
   	listen(socket_desc, 3);
   	pid = fork();
   	if(pid < 0){
   		cout << "fork error" << endl;
		return -1;
   	}
   	while(1){
		if(pid == 0){	
				//msg[0] = 0 - gaszenie led
				if(mem->msg[0]==0){
					int j=75;
					for(int i=(il_led_pion*2+il_led_poz*2)*3-1; i>=0; i--){
						t[j][2]=0;
						i--;
						t[j][1]=0;
						
						i--;
						t[j][0]=0;
						j--;
					}
					render(t, &diody);
					mem->msg[0] = 4;
				}
				
				//msg[0] = 1 - pobieranie obrazu i na jego podstawie uruchamianie kolorow led
	  			if(mem->msg[0]==1){
					//przekazanie kaltki do zmiennej Mat
					cap >> src_klatka;
					//zmiana gammy
					//src_klatka = correctGamma(src_klatka, 0.5);
					//src_klatka = balans(1, 10, src_klatka);
					//zapalanie led na dominujacy kolor w danym obszarze
					
				
					if (write == true){
						dst_klatka = trans(mem->msg[1],mem->msg[2],mem->msg[3],mem->msg[4],
							     mem->msg[5],mem->msg[6],mem->msg[7],mem->msg[8],src_klatka);
						tablica = dominate(dst_klatka, il_led_pion, il_led_poz);
					}else tablica = dominate(src_klatka, il_led_pion, il_led_poz);

					int j = 75;
					for (int i=(il_led_pion*2+il_led_poz*2)*3-1; i>=0; i--){
						
						if (*(tablica+i+3) > 40) t[j][2] = *(tablica+i+3) - 40;
						else t[j][2] = *(tablica+i+3);
						i--;
						if (*(tablica+i+3) > 40) t[j][1] = *(tablica+i+3) - 40;
						else t[j][1] = *(tablica+i+3);
						i--;
						if (*(tablica+i+3) > 40) t[j][0] = *(tablica+i+3) - 40;
						else t[j][0] = *(tablica+i+3);

						j--;
					}	
					render(t, &diody);
	  			}
				
				//msg[0] = 2 - wyswietlanie wszystkich led na dany kolor
				if(mem->msg[0]==2){
					//render rgb z msg
					int j=75;
					for(int i=(il_led_pion*2+il_led_poz*2)*3-3; i>=0; i--){
						t[j][0]=mem->msg[1];
						i--;
						t[j][1]=mem->msg[3];
						i--;
						t[j][2]=mem->msg[2];
						j--;
					}
					render(t, &diody);
					mem->msg[0]=4;
				}
				
				//msg[0] = 5 - zapisywanie obrazu klatki do kalibracji
				if(mem->msg[0] == 5){
					imwrite("/var/www/html/kalibracja.jpg",src_klatka);
					mem->msg[0] = 1;
					write = true;
				}
				//msg[0] = 3 - usuwanie wygenerowanego zdjecia do kalibracji
				if (mem->msg[0]==3){
					dst_klatka = trans(mem->msg[1],mem->msg[2],mem->msg[3],mem->msg[4],
							     mem->msg[5],mem->msg[6],mem->msg[7],mem->msg[8],src_klatka);
					
					mem->msg[0] = 1;
					write == true;
					
					remove("/var/www/html/kalibracja.jpg");
				}
				
				
				if (mem->msg[0]==4){
					sleep(2);
				}
				if (mem->msg[0]==6){
					full(mem->ssid,mem->haslo);	
				}


		}
		else if(pid > 0){

	   		c = sizeof(struct sockaddr_in);
               	client_sock=accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			recv(client_sock, &client_message, 128, 0);
			
			// WYWALENIE NA EKRAN TEGO CO PRZYSZLO Z APLIKACJI MOBILNEJ
			for(int k=0; k<strlen(client_message); k++){
				if(client_message[k] == '"'){
					client_message[k]=' ';
					for(int p=k+1; p<strlen(client_message); p++){
						if(client_message[p]=='"' && (p+1) <= strlen(client_message)){
							client_message[p]=' ';
							client_message[p+1]='\0';
							goto wroc;
						}
					}
				}
			}
			wroc:
			
				//cout << client_message << " " << strlen(client_message) << endl;	
				if(strcmp(client_message, " off ")==0) {
					cout << "wylaczono" << endl;
					mem->msg[0] = 0;
					continue;
				}
				if(strcmp(client_message," on ")==0){
					cout << "wlaczono" << endl;
 					mem->msg[0]=1;
					continue;
				}
				if(strcmp(client_message," jpg ")==0){
					cout << "jpg" << endl;
 					mem->msg[0]=5;
					continue;
				}
				int red,green,blue;
				char kolor[16];
				//pobieranie koloru z aplikacji do wyswietlenia led na jeden kolor
				sscanf(client_message," %s %d %d %d ",&kolor,&red,&green,&blue);
				if(strcmp(kolor,"kolor") == 0){
				
					mem->msg[1] = red;
					mem->msg[2] = green;
					mem->msg[3] = blue;
					mem->msg[0] = 2;
					continue;
				}
				int x1,x2,x3,x4,y1,y2,y3,y4;
				char punkt[16];	
				//pobieranie punktow do przyciecia obrazu z aplikacji
				sscanf(client_message," %s %d %d %d %d %d %d %d %d ",&punkt, &x1,&y1,&x2,&y2,&x3,&y3,&x4,&y4);
				if(strcmp(punkt,"kalibracja") == 0){
					cout<<"kalibracja"<<endl;
					mem->msg[1] = x1;
					mem->msg[2] = y1;
					mem->msg[3] = x2;
					mem->msg[4] = y2;
					mem->msg[5] = x3;
					mem->msg[6] = y3;
					mem->msg[7] = x4;
					mem->msg[8] = y4;
					mem->msg[0] = 3;
				}
				char ssid[16], haslo[16];
				char wifi[16];	
				//pobieranie nazwy wifi, ssid oraz hasla z aplikacji
				sscanf(client_message," %s %s %s ",&wifi,&ssid,&haslo);
				if(strcmp(wifi,"wifi") == 0){
					cout<<"wifi"<<endl;
					strcpy(mem->ssid,ssid);
					strcpy(mem->haslo,haslo);
					mem->msg[0] = 6;
				}
				
				
    	   	 }	
   
       	 }	
} 
