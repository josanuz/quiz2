#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 struct thread{
	int PID;
	int HL;
	int DR;
	int PR;
	int TK;
 }; 
 typedef struct thread thread;
 struct vector{
	thread * vector[512];
	int pos;
 } procesos;//FIFO, SJF_NE, SJF_EX, SJF_NE, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
 struct amigo{
	thread * running;
	thread * ready[512];
	int top;
	int size;
 }FIFO, SJF_NE, SJF_EX, HPF_EX, HPF_NE, RR_2, RR_3, LTS;
 struct diagrama{
	char diagrama[512];
	int pos;
 } DFIFO, DSJF_NE, DSJF_EX, DHPF_EX, DHPF_NE, DRR_2, DRR_3, DLTS;
int tt = 0;
void expropiarSJF();
void swap(int i, int j, thread ** v){
		thread * temp = v[i];
		v[i] = v[j];
		v[j] = temp;
}
void swap(struct amigo * v, thread * r){
	thread * t = r;
	r = v->ready[v->size%512];
	v->ready[v->size%512] = t;
}
void expropiar(struct amigo * v){
	thread * t = v->running;
	v->running = v->ready[v->size%512];
	v->ready[v->size%512] = t;
}
void toReadyFIFO(thread p){
	*(FIFO.ready[FIFO.size%512]) = p;
	FIFO.size++;
}
void toReadySJF_Ex(thread p){
	*(SJF_EX.ready[SJF_EX.size%512]) = p;
	if(p.DR < SJF_EX.running->DR) expropiarSJF();
		SJF_EX.size++;
}
void toReadySJF_NE(thread p){
	*(SJF_NE.ready[SJF_NE.size%512]) = p;
	SJF_NE.size++;
}
void toReadyHPF_Ex(thread p){
	*(HPF_EX.ready[HPF_EX.size%512]) = p;
	if(HPF_EX.ready[HPF_EX.size]->PR > HPF_EX.running->PR) expropiar(&HPF_EX);
	HPF_EX.size++;
}
void toReadyHPF_NE(thread p){
	*(HPF_NE.ready[HPF_NE.size%512]) = p;
	HPF_EX.size++;
}
void toReadyRR_2(thread p){
	*(RR_2.ready[RR_2.size%512]) = p;
	RR_2.size++;
}
void toReadyRR_3(thread p){
		*(RR_3.ready[RR_3.size]) = p;
		RR_3.size++;
}
void toReadyLTS(thread p){
		*(LTS.ready[LTS.size]) = p;
		LTS.size++;
	}
void passToReady(thread p){
	
	toReadyFIFO(p);
	toReadySJF_Ex(p);
	toReadySJF_NE(p);
	toReadyHPF_Ex(p);
	toReadyHPF_NE(p);
	toReadyRR_2(p);
	toReadyRR_3(p);
	toReadyLTS(p);	
}

 /*thread * FIFO[512]; thread * SJF_NE[512]; thread * SJF_EX[512]; thread * HPF_NE[512]; thread * HPF_EX[512]; thread * RR_2[512]; thread * RR_3[512]; thread * LTS[512]; */
 void insertProceso(thread * t){
		procesos.vector[procesos.pos++] = t;
}
void avanzar(int x){
	int i = 0;
	for(i = 0; i < procesos.pos ; i++){
		if(procesos.vector[i]->HL == x) passToReady(*(procesos.vector[i]));
	}
}

void expropiarSJF(){
	thread * temp = SJF_EX.running;
	SJF_EX.running = SJF_EX.ready[SJF_EX.size];
	SJF_EX.ready[SJF_EX.size] = temp;
}

int main(int argc, char ** argv){
	FILE *fp;
	int _pid,_hl,_dr,_pr,_tk;
	//thread t;
	thread * pt;
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("No se pudo abrir el archivo\n");
		exit(0);
	}
	while (fscanf(fp, "%d %d %d %d %d\n", &_pid,&_hl,&_dr,&_pr,&_tk) == 5){
		printf("%d %d %d %d %d\n",_pid,_hl,_dr,_pr,_tk);
		pt = (thread*)malloc(sizeof(thread));
		pt->PID = _pid;
		pt->HL = _hl;
		pt->DR = _dr;
		pt->PR = _pr;
		pt->TK = _tk;
		tt+=_dr;
		//memcpy(pt, &t, sizeof(thread));
		procesos.vector[procesos.pos++] = pt;
	}
	
		
 }