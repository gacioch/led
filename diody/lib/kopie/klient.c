/* 
	program wysylajacy informacje do paska diod
	program dostaje dane na wejscie z lini komend
	format danych: wartosc_r1 wartosc_g1 wartosc_b1 wartosc_r2 wartosc_g2 wartosc_b2 (...) wartosc_rn wartosc_gn wartosc_bn
	przykladowe uzycie: ./program255 0 0 0 255 0 0 0 255
	(program wysle informacje na pin nr. 12 informacje by: 1 diona na pasku zapalila sie na czerwono 2 na zielono ,a 3 na niebiesko)
*/

#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/msg.h>

#define MSG_SIZE 	sizeof(int)* 64 * 3

struct msgbuf{
	long mtype;
	int r[64];
	int g[64];
	int b[64];
};

int main(int argc,char* argv[]){

	int i,j,qid;
	struct msgbuf buf;
		i = 1;
		j = 0;	
	while(i < argc){
	
		if((i-1)%3 == 0)
			sscanf(argv[i],"%d",&(buf.r[j]));
		if((i-1)%3 == 1)
			sscanf(argv[i],"%d",&(buf.g[j]));
		if((i-1)%3 == 2){
			sscanf(argv[i],"%d",&(buf.b[j]));
			j++;
		}
		
		i++;
	}
	buf.mtype = 1;
	if((qid = msgget(999,0666)) < 0){
		printf("blad: msgget\n");
		return -1;
	}
	msgsnd(qid,&(buf),MSG_SIZE,0);

}
