#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#define bool char
#define true 1
#define false 0
#define MAX 1024

struct thread {
	int PID;
	int HL;
	int DR;
	int PR;
	int TK;
};

typedef struct thread thread;
struct vector {
	thread * vector[MAX];
	int pos;
} procesos;//FIFO, SJF_NE, SJF_EX, SJF_NE, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
struct amigo {
	thread * running;
	thread * ready[MAX];
	int top;
	int size ;
} FIFO, SJF_NE, SJF_EX, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
struct diagrama {
	char diagrama[4000];
	int pos;
} DFIFO, DSJF_NE, DSJF_EX, DHPF_EX, DHPF_NE, DRR_2, DRR_3, DLTS;
typedef struct diagrama diagrama;
int tt = 0, qt2, qt3, highest;
bool istrue(int x);
bool used = false;
char relleno = '';
void expropiarSJF();
void expropiar(struct amigo * v)
{
	if(v->running == NULL || v->running->DR == 0) return;
	thread * t = v->running;
	v->running = NULL;//v->ready[v->size%MAX];
	v->ready[++v->size%MAX] = t;
}
void toReadyFIFO(thread p)
{
	FIFO.ready[FIFO.size%MAX] = (thread*)malloc(sizeof(thread));
	*(FIFO.ready[FIFO.size%MAX]) = p;
	FIFO.size++;
}
void toReadySJF_Ex(thread p)
{
	SJF_EX.ready[SJF_EX.size%MAX] = (thread*)malloc(sizeof(thread));
	*(SJF_EX.ready[SJF_EX.size%MAX]) = p;
	if(SJF_EX.running == NULL || p.DR < SJF_EX.running->DR) expropiar(&SJF_EX);
	SJF_EX.size++;
}
void toReadySJF_NE(thread p)
{
	SJF_NE.ready[SJF_NE.size%MAX] = (thread*)malloc(sizeof(thread));
	*(SJF_NE.ready[SJF_NE.size%MAX]) = p;
	SJF_NE.size++;
}
void toReadyHPF_Ex(thread p)
{
	HPF_EX.ready[HPF_EX.size%MAX] = (thread*)malloc(sizeof(thread));
	*(HPF_EX.ready[HPF_EX.size%MAX]) = p;
	if(HPF_EX.running == NULL || (HPF_EX.ready[HPF_EX.size%MAX]->PR > HPF_EX.running->PR)) expropiar(&HPF_EX);
	HPF_EX.size++;
}
void toReadyHPF_NE(thread p)
{
	HPF_NE.ready[HPF_NE.size%MAX] = (thread*)malloc(sizeof(thread));
	*(HPF_NE.ready[HPF_NE.size%MAX]) = p;
	HPF_NE.size++;
}
void toReadyRR_2(thread p)
{
	RR_2.ready[RR_2.size%MAX] = (thread*)malloc(sizeof(thread));
	*(RR_2.ready[RR_2.size%MAX]) = p;
	RR_2.size++;
}
void toReadyRR_3(thread p)
{
	RR_3.ready[RR_3.size%MAX] = (thread*)malloc(sizeof(thread));
	*(RR_3.ready[RR_3.size%MAX]) = p;
	RR_3.size++;
}
void toReadyLTS(thread p)
{
	LTS.ready[LTS.size%MAX]= (thread*)malloc(sizeof(thread));
	*(LTS.ready[LTS.size%MAX]) = p;
	if(LTS.running == NULL || istrue(p.TK)) expropiar(&LTS);
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
	int at = 0,s[6] , z;
	do {
		s[at++] = i%10 + '0';
		i /=10;
	} while(i!=0);
	for(z = at-1; z >=0 ; z--) d->diagrama[d->pos++] = s[z];
}
void writeCasilla(diagrama * d, int i)
{
	d->diagrama[d->pos++] = '|';
	writecomposeint(d,i);
	d->diagrama[d->pos++] = 's';
	d->diagrama[d->pos++] = '|';
}
void runFIFO(int i)
{
	bool writed = false;
	if(FIFO.running == NULL || FIFO.running->DR == 0) {
		if(FIFO.running != NULL &&FIFO.running->DR == 0 ) {
			writeCasilla(&DFIFO,i);
			writed = true;
		}
		free(FIFO.running);
		FIFO.running = FIFO.ready[FIFO.top%MAX];
		if(FIFO.running != NULL) {
			if(!writed) {
				writeCasilla(&DFIFO,i);
				writed = false;
			}
			FIFO.top++;
			FIFO.top %= MAX;
			writecomposeint(&DFIFO,FIFO.running->PID);
		}
	}
	if(FIFO.running != NULL) {
		FIFO.running->DR--;
		DFIFO.diagrama[DFIFO.pos++] = relleno;
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
			RR_2.running = RR_2.ready[RR_2.top%MAX];
			if(RR_2.running != NULL) {
				//qt2 = 0;
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= MAX;
			}
			/*----------------------------si hay cambio por quantum----------------------------*/
		} else if(RR_2.running != NULL && RR_2.running->DR != 0 && qt2 % 2 == 0 ) {
			writeCasilla(&DRR_2,i);
			thread * t = RR_2.running;
			RR_2.ready[RR_2.size++%MAX] = t;
			RR_2.running = RR_2.ready[RR_2.top%MAX];
			if(RR_2.running != NULL) {
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= MAX;
			}
			/*---------------------------si no habia ninguno corriendo----------------------------*/
		} else if(RR_2.running == NULL ) {
			RR_2.running = RR_2.ready[RR_2.top%MAX];
			if(RR_2.running != NULL) {
				writeCasilla(&DRR_2,i);
				writecomposeint(&DRR_2,RR_2.running->PID);
				RR_2.top++;
				RR_2.top %= MAX;
			}

		}
	}
	if(RR_2.running != NULL) {
		RR_2.running->DR--;
		DRR_2.diagrama[DRR_2.pos++] = relleno;
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
			RR_3.running = RR_3.ready[RR_3.top%MAX];
			//qt3 = 0;
			if(RR_3.running != NULL) {
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= MAX;
			}
			/*----------------------------si hay cambio por quantum----------------------------*/
		} else if(RR_3.running != NULL && RR_3.running->DR != 0 && qt3 % 3 == 0 ) {
			writeCasilla(&DRR_3,i);
			thread * t = RR_3.running;
			RR_3.ready[RR_3.size++%MAX] = t;
			RR_3.running = RR_3.ready[RR_3.top%MAX];
			//qt3 = 0;
			if(RR_3.running != NULL) {
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= MAX;
			}
			/*---------------------------si no habia ninguno corriendo----------------------------*/
		} else if(RR_3.running == NULL ) {
			RR_3.running = RR_3.ready[RR_3.top%MAX];
			if(RR_3.running != NULL) {
				writeCasilla(&DRR_3,i);
				writecomposeint(&DRR_3,RR_3.running->PID);
				RR_3.top++;
				RR_3.top %= MAX;
			}

		}
	}
	if(RR_3.running != NULL) {
		RR_3.running->DR--;
		DRR_3.diagrama[DRR_3.pos++] = relleno;
		qt3++;
	}
}
void shortesToTop(amigo * a)
{
	int i;
	int mp = a->top;
	for(i = a->top%MAX; i != a->size%MAX ; i++) {
		i%=MAX;
		if((a->ready[i]->DR < a->ready[mp]->DR) && a->ready[i]->DR > 0) mp = i;
	}
	thread * t = a->ready[a->top%MAX];
	a->ready[a->top%MAX] = a->ready[mp];
	a->ready[mp] = t;
}
void runSJF_NE(int i)
{
	bool wwrited = false;
	if(SJF_NE.running == NULL || SJF_NE.running->DR == 0) {
		if(SJF_NE.running == NULL) {
			shortesToTop(&SJF_NE);
			SJF_NE.running = SJF_NE.ready[SJF_NE.top%MAX];
			if(SJF_NE.running != NULL) {
				SJF_NE.top++;
				SJF_NE.top %=MAX;
				writeCasilla(&DSJF_NE,i);
				writecomposeint(&DSJF_NE,SJF_NE.running->PID);
			}
		} else if(SJF_NE.running->DR == 0) {
			free(SJF_NE.running);
			shortesToTop(&SJF_NE);
			SJF_NE.running = SJF_NE.ready[SJF_NE.top%MAX];
			writeCasilla(&DSJF_NE,i);
			wwrited = true;
			if(SJF_NE.running != NULL) {
				SJF_NE.top++;
				SJF_NE.top %=MAX;
				if(!wwrited) writeCasilla(&DSJF_NE,i);
				writecomposeint(&DSJF_NE,SJF_NE.running->PID);
			}
		}
	}
	if(SJF_NE.running != NULL) {
		DSJF_NE.diagrama[DSJF_NE.pos++] = relleno;
		SJF_NE.running->DR--;
	}
}
void runSJF_EX(int i)
{
	bool wwrited = false;
	if(SJF_EX.running == NULL || SJF_EX.running->DR == 0) {
		if(SJF_EX.running == NULL) {
			shortesToTop(&SJF_EX);
			SJF_EX.running = SJF_EX.ready[SJF_EX.top%MAX];
			if(SJF_EX.running != NULL) {
				SJF_EX.top++;
				SJF_EX.top %=MAX;
				writeCasilla(&DSJF_EX,i);
				writecomposeint(&DSJF_EX,SJF_EX.running->PID);
			}
		} else if(SJF_EX.running->DR == 0) {
			free(SJF_EX.running);
			shortesToTop(&SJF_EX);
			SJF_EX.running = SJF_EX.ready[SJF_EX.top%MAX];
			writeCasilla(&DSJF_EX,i);
			wwrited = true;
			if(SJF_EX.running != NULL) {
				SJF_EX.top++;
				SJF_EX.top %=MAX;
				if(!wwrited) writeCasilla(&DSJF_EX,i);
				writecomposeint(&DSJF_EX,SJF_EX.running->PID);
			}
		}
	}
	if(SJF_EX.running != NULL) {
		DSJF_EX.diagrama[DSJF_EX.pos++] = relleno;
		SJF_EX.running->DR--;
	}
}
void priorToTop(amigo * a)
{
	int i;
	int mp = a->top;
	for(i = a->top%MAX; i != a->size%MAX ; i++) {
		i%=MAX;
		if((a->ready[i]->PR > a->ready[mp]->PR) && a->ready[i]->DR > 0) mp = i;
	}
	thread * t = a->ready[a->top%MAX];
	a->ready[a->top%MAX] = a->ready[mp];
	a->ready[mp] = t;
}
void runHPF_EX(int i)
{
	bool wwrited = false;
	if(HPF_EX.running == NULL || HPF_EX.running->DR == 0) {
		if(HPF_EX.running == NULL) {
			priorToTop(&HPF_EX);
			HPF_EX.running = HPF_EX.ready[HPF_EX.top%MAX];
			if(HPF_EX.running != NULL) {
				HPF_EX.top++;
				HPF_EX.top %=MAX;
				writeCasilla(&DHPF_EX,i);
				writecomposeint(&DHPF_EX,HPF_EX.running->PID);
			}
		} else if(HPF_EX.running->DR == 0) {
			free(HPF_EX.running);
			priorToTop(&HPF_EX);
			HPF_EX.running = HPF_EX.ready[HPF_EX.top];
			writeCasilla(&DHPF_EX,i);
			wwrited = true;
			if(HPF_EX.running != NULL) {
				HPF_EX.top++;
				HPF_EX.top %=MAX;
				if(!wwrited) writeCasilla(&DHPF_EX,i);
				writecomposeint(&DHPF_EX,HPF_EX.running->PID);
			}
		}
	}
	if(HPF_EX.running != NULL) {
		DHPF_EX.diagrama[DHPF_EX.pos++] = relleno;
		HPF_EX.running->DR--;
	}
}
void runHPF_NE(int i)
{
	bool wwrited = false;
	if(HPF_NE.running == NULL || HPF_NE.running->DR == 0) {
		if(HPF_NE.running == NULL) {
			priorToTop(&HPF_NE);
			HPF_NE.running = HPF_NE.ready[HPF_NE.top%MAX];
			if(HPF_NE.running != NULL) {
				HPF_NE.top++;
				HPF_NE.top %=MAX;
				writeCasilla(&DHPF_NE,i);
				writecomposeint(&DHPF_NE,HPF_NE.running->PID);
			}
		} else if(HPF_NE.running->DR == 0) {
			free(HPF_NE.running);
			priorToTop(&HPF_NE);
			HPF_NE.running = HPF_NE.ready[HPF_NE.top%MAX];
			writeCasilla(&DHPF_NE,i);
			wwrited = true;
			if(HPF_NE.running != NULL) {
				HPF_NE.top++;
				HPF_NE.top %=MAX;
				if(!wwrited) writeCasilla(&DHPF_NE,i);
				writecomposeint(&DHPF_NE,HPF_NE.running->PID);
			}
		}
	}
	if(HPF_NE.running != NULL) {
		DHPF_NE.diagrama[DHPF_NE.pos++] = relleno;
		HPF_NE.running->DR--;
	}
}
bool istrue(int x)
{
	//bool a;
	srand(time(0));
	//for(i = 0; i<x ; i++ ) a = rand() % 2;
	if(x > rand() % highest)
		return true;
	return false;
}
void randToTop(amigo * a)
{
	int m = a->top, i=0;
	for(i = 0; i != a->size%MAX ; i++) {
		i%=MAX;
		if(istrue(a->ready[i]->TK)) m = i;
	}
	while(a->ready[m]!= NULL && a->ready[m]->DR == 0 ) m++;
	thread * t = a->ready[a->top%MAX];
	a->ready[a->top%MAX] = a->ready[m];
	a->ready[m] = t;
}
void runLts(int i)
{
	bool writtes;
	if(LTS.running == NULL || LTS.running->DR == 0) {
		if(LTS.running == NULL) {
			randToTop(&LTS);
			LTS.running = LTS.ready[LTS.top%MAX];
			if(LTS.running != NULL) {
				writeCasilla(&DLTS, i);
				LTS.top++;
				LTS.top %= MAX;
				writecomposeint(&DLTS,LTS.running->PID);
			}
		} else if(LTS.running->DR == 0) {
			free(LTS.running);
			writeCasilla(&DLTS, i);
			randToTop(&LTS);
			LTS.running = LTS.ready[LTS.top%MAX];
			if(LTS.running != NULL) {
				LTS.top++;
				LTS.top %= MAX;
				writecomposeint(&DLTS,LTS.running->PID);
			}
		}
	}
	if(LTS.running != NULL) {
		DLTS.diagrama[DLTS.pos++] = relleno;
		LTS.running->DR--;
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
	runHPF_NE(i);
	runLts(i);
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
	highest = 0;
	while (fscanf(fp, "%d %d %d %d %d\n", &_pid,&_hl,&_dr,&_pr,&_tk) == 5) {
		//printf("%d %d %d %d %d\n",_pid,_hl,_dr,_pr,_tk);
		pt = (thread*)malloc(sizeof(thread));
		pt->PID = _pid;
		pt->HL = _hl;
		pt->DR = _dr;
		pt->PR = _pr;
		pt->TK = _tk;
		tt+=_dr;
		//minmaxvector.vt[pos++] = {maxTickets , maxTickets+_tk, _pid};
		//maxTickets += _tk;
		if(_tk > highest) highest = _tk;
		procesos.vector[procesos.pos++] = pt;
	}
	int s = 0;
	printf("%d\n",tt);
	for(s = 0; s<tt + 1024; s++) {
		avanzar(s);
		correr(s);
	}
	FILE * fs = fopen(argv[2], "ab+");
	DFIFO.diagrama[DFIFO.pos++] = '\0';
	DRR_2.diagrama[DRR_2.pos++] = '\0';
	DRR_3.diagrama[DRR_3.pos++] = '\0';
	DSJF_NE.diagrama[DSJF_NE.pos++] = '\0';
	DSJF_EX.diagrama[DSJF_EX.pos++] = '\0';
	DHPF_EX.diagrama[DHPF_EX.pos++] = '\0';
	DHPF_NE.diagrama[DHPF_NE.pos++] = '\0';
	DLTS.diagrama[DLTS.pos++] = '\0';
	fprintf(fs," FIFO: %s\n",DFIFO.diagrama);
	fprintf(fs," RR2: %s\n",DRR_2.diagrama);
	fprintf(fs," RR3: %s\n",DRR_3.diagrama);
	fprintf(fs," SJF_NE: %s\n",DSJF_NE.diagrama);
	fprintf(fs," SJF_EX: %s\n",DSJF_EX.diagrama);
	fprintf(fs," HPF_EX: %s\n",DHPF_EX.diagrama);
	fprintf(fs," HPF_NE: %s\n",DHPF_NE.diagrama);
	fprintf(fs," Lottery: %s\n",DLTS.diagrama);
}
