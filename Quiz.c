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
	if(v->running == NULL || v->running->DR == 0) return;
	thread * t = v->running;
	v->running = NULL;//v->ready[v->size%512];
	v->ready[++v->size%512] = t;
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
	if(HPF_EX.running == NULL || (HPF_EX.ready[HPF_EX.size%512]->PR > HPF_EX.running->PR)) expropiar(&HPF_EX);
	HPF_EX.size++;
}
void toReadyHPF_NE(thread p)
{
	HPF_NE.ready[HPF_NE.size%512] = (thread*)malloc(sizeof(thread));
	*(HPF_NE.ready[HPF_NE.size%512]) = p;
	HPF_NE.size++;
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
void writeCasilla(diagrama * d, int i){
	d->diagrama[d->pos++] = '|';
	writecomposeint(d,i);d->diagrama[d->pos++] = 's';
	d->diagrama[d->pos++] = '|';
}
void runFIFO(int i)
{
	//if(FIFO.top >= FIFO.size) return;
	bool writed = false;
	if(FIFO.running == NULL || FIFO.running->DR == 0) {
		if(FIFO.running != NULL &&FIFO.running->DR == 0 ){ 
			writeCasilla(&DFIFO,i);
			writed = true;
		}
		free(FIFO.running);
		FIFO.running = FIFO.ready[FIFO.top];
		if(FIFO.running != NULL) {
			if(!writed) {writeCasilla(&DFIFO,i); writed = false;}
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
}
void runRR_2(int i)
{
	if(RR_2.running == NULL ||  qt2 % 2 == 0 || RR_2.running->DR == 0) {
		/*---------------------------------si expiro el tiempo----------------------------*/
		if(RR_2.running != NULL && RR_2.running->DR == 0) {
			free(RR_2.running);
			writeCasilla(&DRR_2,i);
			RR_2.running = RR_2.ready[RR_2.top];
			if(RR_2.running != NULL) {
				qt2 = 0;
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= 512;
			}
			/*----------------------------si hay cambio por quantum----------------------------*/
		} else if(RR_2.running != NULL && RR_2.running->DR != 0 && qt2 % 2 == 0 ) {
			writeCasilla(&DRR_2,i);
			thread * t = RR_2.running;
			RR_2.ready[RR_2.size++] = t;
			RR_2.running = RR_2.ready[RR_2.top];
			if(RR_2.running != NULL) {
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= 512;
			}
		/*---------------------------si no habia ninguno corriendo----------------------------*/
		} else if(RR_2.running == NULL ) {
			RR_2.running = RR_2.ready[RR_2.top];
			if(RR_2.running != NULL) {
				writeCasilla(&DRR_2,i);
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
void runRR_3(int i)
{
	if(RR_3.running == NULL ||  qt3 % 3 == 0 || RR_3.running->DR == 0) {
		/*---------------------------------si expiro el tiempo----------------------------*/
		if(RR_3.running != NULL && RR_3.running->DR == 0) {
			free(RR_3.running);
			writeCasilla(&DRR_3,i);
			RR_3.running = RR_3.ready[RR_3.top];
			qt3 = 0;
			if(RR_3.running != NULL) {
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= 512;
			}
			/*----------------------------si hay cambio por quantum----------------------------*/
		} else if(RR_3.running != NULL && RR_3.running->DR != 0 && qt3 % 3 == 0 ) {
			writeCasilla(&DRR_3,i);
			thread * t = RR_3.running;
			RR_3.ready[RR_3.size++] = t;
			RR_3.running = RR_3.ready[RR_3.top];
			qt3 = 0;
			if(RR_3.running != NULL) {
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= 512;
			}
		/*---------------------------si no habia ninguno corriendo----------------------------*/
		} else if(RR_3.running == NULL ) {
			RR_3.running = RR_3.ready[RR_3.top];
			if(RR_3.running != NULL) {
				writeCasilla(&DRR_3,i);
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= 512;
			}

		}
	}
	if(RR_3.running != NULL) {
		RR_3.running->DR--;
		DRR_3.diagrama[DRR_3.pos++] = '*';
		qt3++;
	}
}
void shortesToTop(amigo * a){
	int i;
	int mp = a->top;
	for(i = a->top%512; i != a->size%512 ; i++){
		i%=512;
		if((a->ready[i]->DR < a->ready[mp]->DR) && a->ready[i]->DR > 0) mp = i;
	}
	thread * t = a->ready[a->top%512];
	a->ready[a->top%512] = a->ready[mp];
	a->ready[mp] = t;
}
void runSJF_NE(int i){
	bool wwrited = false;
	if(SJF_NE.running == NULL || SJF_NE.running->DR == 0){
			if(SJF_NE.running == NULL){
				shortesToTop(&SJF_NE);
				SJF_NE.running = SJF_NE.ready[SJF_NE.top];
				if(SJF_NE.running != NULL){
						SJF_NE.top++; SJF_NE.top %=512;
						writeCasilla(&DSJF_NE,i);
						writecomposeint(&DSJF_NE,SJF_NE.running->PID);
				}
			}
			else if(SJF_NE.running->DR == 0){
				free(SJF_NE.running);
				shortesToTop(&SJF_NE);
				SJF_NE.running = SJF_NE.ready[SJF_NE.top];
				writeCasilla(&DSJF_NE,i);
				wwrited = true;
				if(SJF_NE.running != NULL){
						SJF_NE.top++; SJF_NE.top %=512;
						if(!wwrited) writeCasilla(&DSJF_NE,i);
						writecomposeint(&DSJF_NE,SJF_NE.running->PID);
				}
			}
	}
	if(SJF_NE.running != NULL)
	{
			DSJF_NE.diagrama[DSJF_NE.pos++] = '*';
			SJF_NE.running->DR--;
	}
}
void runSJF_EX(int i){
	bool wwrited = false;
	if(SJF_EX.running == NULL || SJF_EX.running->DR == 0){
			if(SJF_EX.running == NULL){
				shortesToTop(&SJF_EX);
				SJF_EX.running = SJF_EX.ready[SJF_EX.top];
				if(SJF_EX.running != NULL){
						SJF_EX.top++; SJF_EX.top %=512;
						writeCasilla(&DSJF_EX,i);
						writecomposeint(&DSJF_EX,SJF_EX.running->PID);
				}
			}
			else if(SJF_EX.running->DR == 0){
				free(SJF_EX.running);
				shortesToTop(&SJF_EX);
				SJF_EX.running = SJF_EX.ready[SJF_EX.top];
				writeCasilla(&DSJF_EX,i);
				wwrited = true;
				if(SJF_EX.running != NULL){
						SJF_EX.top++; SJF_EX.top %=512;
						if(!wwrited) writeCasilla(&DSJF_EX,i);
						writecomposeint(&DSJF_EX,SJF_EX.running->PID);
				}
			}
	}
	if(SJF_EX.running != NULL)
	{
			DSJF_EX.diagrama[DSJF_EX.pos++] = '*';
			SJF_EX.running->DR--;
	}
}
void priorToTop(amigo * a){
	int i;
	int mp = a->top;
	for(i = a->top%512; i != a->size%512 ; i++){
		i%=512;
		if((a->ready[i]->PR > a->ready[mp]->PR) && a->ready[i]->DR > 0) mp = i;
	}
	thread * t = a->ready[a->top%512];
	a->ready[a->top%512] = a->ready[mp];
	a->ready[mp] = t;
}
void runHPF_EX(int i){
	bool wwrited = false;
	if(HPF_EX.running == NULL || HPF_EX.running->DR == 0){
			if(HPF_EX.running == NULL){
				priorToTop(&HPF_EX);
				HPF_EX.running = HPF_EX.ready[HPF_EX.top];
				if(HPF_EX.running != NULL){
						HPF_EX.top++; HPF_EX.top %=512;
						writeCasilla(&DHPF_EX,i);
						writecomposeint(&DHPF_EX,HPF_EX.running->PID);
				}
			}
			else if(HPF_EX.running->DR == 0){
				free(HPF_EX.running);
				priorToTop(&HPF_EX);
				HPF_EX.running = HPF_EX.ready[HPF_EX.top];
				writeCasilla(&DHPF_EX,i);
				wwrited = true;
				if(HPF_EX.running != NULL){
						HPF_EX.top++; HPF_EX.top %=512;
						if(!wwrited) writeCasilla(&DHPF_EX,i);
						writecomposeint(&DHPF_EX,HPF_EX.running->PID);
				}
			}
	}
	if(HPF_EX.running != NULL)
	{
			DHPF_EX.diagrama[DHPF_EX.pos++] = '*';
			HPF_EX.running->DR--;
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
void printThread(thread * t)
{
	printf("PID: %9d\nLlegada %6d\nDuracion: %4d\nPrioridad: %3d\nTicketes: %4d\n---------------------\n", t->PID,t->HL,t->DR,t->PR,t->TK);
}
void correr(int i)
{
	runFIFO(i);
	runRR_2(i);
	runRR_3(i);
	runSJF_NE(i);
	runSJF_EX(i);
	runHPF_EX(i);
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
	DRR_3.diagrama[DRR_3.pos++] = '\0';
	DSJF_NE.diagrama[DSJF_NE.pos++] = '\0';
	DSJF_EX.diagrama[DSJF_EX.pos++] = '\0';
	DHPF_EX.diagrama[DHPF_EX.pos++] = '\0';
	printf(" FIFO: %s\n",DFIFO.diagrama);
	printf(" RR2: %s\n",DRR_2.diagrama);
	printf(" RR3: %s\n",DRR_3.diagrama);
	printf(" SJF_NE: %s\n",DSJF_NE.diagrama);
	printf(" SJF_EX: %s\n",DSJF_EX.diagrama);
	printf(" HPF_EX: %s\n",DHPF_EX.diagrama);
	/*for(s = 0; s<FIFO.size ; s++){
		printThread(FIFO.ready[s]);
		printThread(SJF_EX.ready[s]);
	}*/

}
