#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define bool char
#define true 1
#define false 0

struct thread {
	int PID;
	int HL;
	int DR;
	int PR;
	int TK;
};
typedef struct thread thread;
struct vector {
	thread * vector[512];
	int pos;
} procesos;//FIFO, SJF_NE, SJF_EX, SJF_NE, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
struct amigo {
	thread * running;
	thread * ready[512];
	int top;
	int size ;
} FIFO, SJF_NE, SJF_EX, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
struct diagrama {
	char diagrama[4000];
	int pos;
} DFIFO, DSJF_NE, DSJF_EX, DHPF_EX, DHPF_NE, DRR_2, DRR_3, DLTS;
typedef struct diagrama diagrama;
int tt = 0, qt2, qt3;
bool used = false;
void expropiarSJF();
void expropiar(struct amigo * v)
{
	if(v->running == NULL ) return;
	thread * t = v->running;
	v->running = v->ready[v->size%512];
	v->ready[v->size%512] = t;
}
void toReadyFIFO(thread p)
{
	FIFO.ready[FIFO.size%512] = (thread*)malloc(sizeof(thread));
	*(FIFO.ready[FIFO.size%512]) = p;
	FIFO.size++;
}
void toReadySJF_Ex(thread p)
{
	SJF_EX.ready[SJF_EX.size%512] = (thread*)malloc(sizeof(thread));
	*(SJF_EX.ready[SJF_EX.size%512]) = p;
	if(SJF_EX.running == NULL || p.DR < SJF_EX.running->DR) expropiar(&SJF_EX);
	SJF_EX.size++;
}
void toReadySJF_NE(thread p)
{
	SJF_NE.ready[SJF_NE.size%512] = (thread*)malloc(sizeof(thread));
	*(SJF_NE.ready[SJF_NE.size%512]) = p;
	SJF_NE.size++;
}
void toReadyHPF_Ex(thread p)
{
	HPF_EX.ready[HPF_EX.size%512] = (thread*)malloc(sizeof(thread));
	*(HPF_EX.ready[HPF_EX.size%512]) = p;
	if(HPF_EX.running == NULL || HPF_EX.ready[HPF_EX.size]->PR > HPF_EX.running->PR) expropiar(&HPF_EX);
	HPF_EX.size++;
}
void toReadyHPF_NE(thread p)
{
	HPF_NE.ready[HPF_NE.size%512] = (thread*)malloc(sizeof(thread));
	*(HPF_NE.ready[HPF_NE.size%512]) = p;
	HPF_EX.size++;
}
void toReadyRR_2(thread p)
{
	RR_2.ready[RR_2.size%512] = (thread*)malloc(sizeof(thread));
	*(RR_2.ready[RR_2.size%512]) = p;
	RR_2.size++;
}
void toReadyRR_3(thread p)
{
	RR_3.ready[RR_3.size%512] = (thread*)malloc(sizeof(thread));
	*(RR_3.ready[RR_3.size%512]) = p;
	RR_3.size++;
}
void toReadyLTS(thread p)
{
	LTS.ready[LTS.size%512]= (thread*)malloc(sizeof(thread));
	*(LTS.ready[LTS.size%512]) = p;
	LTS.size++;
}
void passToReady(thread p)
{

	toReadyFIFO(p);
	toReadySJF_Ex(p);
	toReadySJF_NE(p);
	toReadyHPF_Ex(p);
	toReadyHPF_NE(p);
	toReadyRR_2(p);
	toReadyRR_3(p);
	toReadyLTS(p);
}
void writecomposeint(diagrama * d, int i)
{
	//char i = 0;
	int at = 0,s[6] , z;
	do {
		s[at++] = i%10 + '0';
		i /=10;
	} while(i!=0);
	for(z = at-1; z >=0 ; z--) d->diagrama[d->pos++] = s[z];
}
void runFIFO(int i)
{
	//if(FIFO.top >= FIFO.size) return;
	if(FIFO.running != NULL &&FIFO.running->DR == 0 ) {
		DFIFO.diagrama[DFIFO.pos++] = '|';
		writecomposeint(&DFIFO,i);
		DFIFO.diagrama[DFIFO.pos++] = 's';
		DFIFO.diagrama[DFIFO.pos++] = '|';
	}
	if(FIFO.running == NULL || FIFO.running->DR == 0) {
		free(FIFO.running);
		FIFO.running = FIFO.ready[FIFO.top];

		if(FIFO.running != NULL) {
			DFIFO.diagrama[DFIFO.pos++] = '|';
			writecomposeint(&DFIFO,i);
			DFIFO.diagrama[DFIFO.pos++] = 's';
			DFIFO.diagrama[DFIFO.pos++] = '|';
			FIFO.top++;
			FIFO.top %= 512;
			writecomposeint(&DFIFO,FIFO.running->PID);
		}
	}
	if(FIFO.running != NULL) {
		FIFO.running->DR--;
		DFIFO.diagrama[DFIFO.pos++] = '*';
		used = true;
	}
	//else DFIFO.diagrama[DFIFO.pos++] = '/';
}
void runRR_2(int i)
{
	if(RR_2.running != NULL &&  qt2 % 2 == 0) {
		DRR_2.diagrama[DRR_2.pos++] = '|';
		writecomposeint(&DRR_2,i);
		DRR_2.diagrama[DRR_2.pos++] = 's';
		DRR_2.diagrama[DRR_2.pos++] = '|';
	}
	if(RR_2.running == NULL ||  qt2 % 2 == 0 || RR_2.running->DR == 0) {
		if(RR_2.running != NULL && RR_2.running->DR == 0) {
			free(RR_2.running);
			DRR_2.diagrama[DRR_2.pos++] = '|';writecomposeint(&DRR_2,i);	DRR_2.diagrama[DRR_2.pos++] = 's';DRR_2.diagrama[DRR_2.pos++] = '|';
			RR_2.running = RR_2.ready[RR_2.top];
			if(RR_2.running != NULL) {
				//DRR_2.diagrama[DRR_2.pos++] = '|';writecomposeint(&DRR_2,i);DRR_2.diagrama[DRR_2.pos++] = 's';DRR_2.diagrama[DRR_2.pos++] = '|';
				qt2 = 2;
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= 512;
			}

		} else if(RR_2.running != NULL && RR_2.running->DR != 0 && qt2 % 2 == 0 ) {
			thread * t = RR_2.running;
			DRR_2.diagrama[DRR_2.pos++] = '|';
			writecomposeint(&DRR_2,i);
			DRR_2.diagrama[DRR_2.pos++] = 's';
			DRR_2.diagrama[DRR_2.pos++] = '|';
			RR_2.running = RR_2.ready[RR_2.top];
			RR_2.ready[RR_2.size++] = t;
			if(RR_2.running != NULL) {
				//DRR_2.diagrama[DRR_2.pos++] = '|';writecomposeint(&DRR_2,i);DRR_2.diagrama[DRR_2.pos++] = 's';DRR_2.diagrama[DRR_2.pos++] = '|';
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= 512;
			}

		} else if(RR_2.running == NULL) {
			RR_2.running = RR_2.ready[RR_2.top];
			if(RR_2.running != NULL) {
				DRR_2.diagrama[DRR_2.pos++] = '|';
				writecomposeint(&DRR_2,i);
				DRR_2.diagrama[DRR_2.pos++] = 's';
				DRR_2.diagrama[DRR_2.pos++] = '|';
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= 512;
			}

		}
	}
	if(RR_2.running != NULL) {
		RR_2.running->DR--;
		DRR_2.diagrama[DRR_2.pos++] = '*';
		qt2++;
	}
}

void insertProceso(thread * t)
{
	procesos.vector[procesos.pos++] = t;
}
void avanzar(int x)
{
	int i = 0;
	for(i = 0; i < procesos.pos ; i++) {
		if(procesos.vector[i]->HL == x) passToReady(*(procesos.vector[i]));
	}
}
/*
void expropiarSJF(){
	if(SJF_EX.running == NULL) SJF_EX.size--;
	thread * temp = SJF_EX.running;
	SJF_EX.running = SJF_EX.ready[SJF_EX.size];
	SJF_EX.ready[SJF_EX.size] = temp;

}*/
void printThread(thread * t)
{
	printf("PID: %9d\nLlegada %6d\nDuracion: %4d\nPrioridad: %3d\nTicketes: %4d\n---------------------\n", t->PID,t->HL,t->DR,t->PR,t->TK);
}
void correr(int i)
{
	runFIFO(i);
	runRR_2(i);
}

int main(int argc, char ** argv)
{
	FILE *fp;
	int _pid,_hl,_dr,_pr,_tk;//, m;
	//thread t;
	thread * pt;
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("No se pudo abrir el archivo\n");
		exit(0);
	}
	while (fscanf(fp, "%d %d %d %d %d\n", &_pid,&_hl,&_dr,&_pr,&_tk) == 5) {
		//printf("%d %d %d %d %d\n",_pid,_hl,_dr,_pr,_tk);
		pt = (thread*)malloc(sizeof(thread));
		pt->PID = _pid;
		pt->HL = _hl;
		pt->DR = _dr;
		pt->PR = _pr;
		pt->TK = _tk;
		tt+=_dr;
		//memcpy(pt, &t, sizeof(thread));
		procesos.vector[procesos.pos++] = pt;
		//printThread(pt);
		//toReadyFIFO(*pt);
	}
	int s = 0;
	printf("%d\n",tt);
	for(s = 0; s<tt + 1024; s++) {
		avanzar(s);
		correr(s);
	}
	DFIFO.diagrama[DFIFO.pos++] = '\0';
	DRR_2.diagrama[DRR_2.pos++] = '\0';
	printf(" FIFO: %s\n",DFIFO.diagrama);
	printf(" RR2: %s\n",DRR_2.diagrama);
	/*for(s = 0; s<FIFO.size ; s++){
		printThread(FIFO.ready[s]);
		printThread(SJF_EX.ready[s]);
	}*/

}
