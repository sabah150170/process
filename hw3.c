// BUSE NUR SABAH - 150170002
// compile with "gcc -std=c99 son.c -o o -lm"
// if you have problem about ipc.h when compile. Complie with "gcc -D_SVID_SOURCE -std=c99 son.c -o o -lm"
// When running in a different environment, the program hangs on the first starts. I have not had such a problem in ssh. but if you live please run the code again.

#define _SVID_SOURCE // for ipc.h
//#define _XOPEN_SOURCE // if problem with ipc.h
//#define _DEFAULT_SOURCE //if problem with ipc.h
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // fork()
#include <sys/wait.h>
#include <sys/ipc.h> // shm(), sem(), ftok()
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/types.h> // sem(), ftok()
#include<math.h> // ceil()
#include<stdbool.h> // for true

void sem_signal(int semid, int val){
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=val;
	semaphore.sem_flg=1;
	if(semop(semid, &semaphore, 1)!=0) perror("semop0");
 }

void sem_wait(int semid, int val){
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=(-1*val);
	semaphore.sem_flg=1;
	if((semop(semid, &semaphore, 1))==-1) perror("semop1");
}

void sem_multi_signal(int semid, int nsems, int n){ //values of all of the semaphore in sem[2]/sem[3] except semaphore which has number of process calling this increase as much as 1, end of processes. nsems: which process call it. n: how many semaphore are exist
	struct sembuf semaphore[n-1];
	int k, h=0;
	for(k=0; k<n; k++){
		if(k!=nsems){
			semaphore[h].sem_num=k;
			semaphore[h].sem_op=1;
			semaphore[h].sem_flg=1;
			h++;
		}
	}
	if((semop(semid, semaphore, n-1))==-1) perror("semop2");
}

void sem_multi_wait(int semid, int val, int nsems){ //value of semaphore which has number of process calling this in sem[2]/sem[3] decreases as much as ni-1/nd-1, head of related process.  nsems: which process call it
	struct sembuf semaphore;
	semaphore.sem_num=nsems;
	semaphore.sem_op=(-1*val);
	semaphore.sem_flg=1;
	
	if((semop(semid, &semaphore, 1))==-1) perror("semop3");
}

void sem_multi_signal2(int semid, int val, int nsems){ //semaphore which has number of process calling this in sem[4]/sem[5] increase as much as nd/ni.  nsems: which process call it
	struct sembuf semaphore;
		semaphore.sem_num=nsems;
		semaphore.sem_op=val;
		semaphore.sem_flg=1;		
	if((semop(semid, &semaphore, 1))==-1) perror("semop4");
}

void sem_multi_wait2(int semid, int n){ //values of all of the semaphore in sem[4]/sem[5] decreases as much as 1,  n: how many semaphore are exist
	struct sembuf semaphore[n];
	for(int k=0; k<n; k++){
	    semaphore[k].sem_num=k;
	    semaphore[k].sem_op=-1;
	    semaphore[k].sem_flg=1;
    }
	if((semop(semid, semaphore, n))==-1) perror("semop5");
}

int getFibonacci(int x){ // calculate fibonacci number
		if (x == 0) return 0;
		if (x == 1) return 1;
		else return getFibonacci(x - 1) + getFibonacci(x - 2);
}

int main(int argc, char* argv[] ) {
    key_t key1= ftok("/dev/null", 150171); if(key1==(key_t)-1) perror("k1:"); // for money in moneybox - shared memory
    key_t key6= ftok("/dev/null", 1501721); if(key6==(key_t)-1) perror("k6:"); // for number of turn in inc. pr. - shared memory
    key_t key7= ftok("/dev/null", 1501725); if(key7==(key_t)-1) perror("k7:"); // for number of turn in dec. pr. - shared memory
    key_t key2= ftok("/dev/null", 150887175); if(key2==(key_t)-1) perror("k2:"); // for syn. between main and child processes
    key_t key3= ftok("/dev/null", 150179); if(key3==(key_t)-1) perror("k3:"); // for syn. when changing money in the moneybox
    key_t key4= ftok("/dev/null", 1501713); if(key4==(key_t)-1) perror("k4:"); // for syn. inter inc. processes
    key_t key5= ftok("/dev/null", 1501717); if(key5==(key_t)-1) perror("k5:"); // for syn. inter dec. processes
    key_t key8= ftok("/dev/null", 1501729); if(key8==(key_t)-1) perror("k8:"); // for syn. between inc-dec.
    key_t key9=ftok("/dev/null", 10210); if(key9==(key_t)-1) perror("k9:"); // for syn. between inc-dec.
    
	int i,j, f;
	int N=atoi(argv[1]);
	int ni=atoi(argv[2]);
	int nd=atoi(argv[3]);
	int ti=atoi(argv[4]);
	int td=atoi(argv[5]);
	
	int counter=1; // for fibonacci
	
	int shm[3];
	int sem[6];

	//calculate first increaser how many tour will be worked
	double x = N / (((ni * 10.0 / 2) + (ni * 15.0 / 2))*ti);
	int tour = ceil(x);	

	for(i=0; i<ni; i++)	{ // increaser processes are created
		f=fork();
		if(f<0) printf("UNSUCCESS FORK");
		else if(f==0) break;
		else if(f>0);
	}
	if(f>0){
		for(j=0; j<nd; j++)	{ // decreaser processes are created
			f=fork();
			if(f<0) printf("UNSUCCESS FORK");
			else if(f==0) break;
			else if(f>0);
		}
	}
	
	if(f>0){ //main process
		int* shared[3];
		
		shm[0]=shmget(key1, sizeof(int), IPC_CREAT|0666); if(shm[0]<0) perror("m1:");  // points to shared memory address, allocate shared memory segment
		shared[0]=(int*)shmat(shm[0], 0,0); if(shared[0]==(int*)-1) perror("m2:");  // attach shared memory segment
		(*shared[0])=0; // money in the moneybox=0;
		
		shm[1]=shmget(key6, sizeof(int), IPC_CREAT|0666); if(shm[1]<0) perror("m3:");  
		shared[1]=(int*)shmat(shm[1], 0,0); if(shared[1]==(int*)-1) perror("m4:"); 
		(*shared[1])=0; // it keeps number of turn of inc. process
		
		shm[2]=shmget(key7, sizeof(int), IPC_CREAT|0666); if(shm[2]<0) perror("m5:");  
		shared[2]=(int*)shmat(shm[2], 0,0); if(shared[2]==(int*)-1) perror("m6:"); 
		(*shared[2])=0; // it keeps number of turn of dec. process

		sem[0]=semget(key2, 1, 0700|IPC_CREAT); if(sem[0]<0) perror("s1:"); // inter main pr. and child pr. sem_id = second parameter
		if((semctl(sem[0], 0, SETVAL, 0))<0) perror("s2:"); 
		
		sem[1]=semget(key3, 1, 0700|IPC_CREAT); if(sem[1]<0) perror("s3:"); // it provide syncronization when changing money in the moneybox
		if((semctl(sem[1], 0, SETVAL, 1))<0) perror("s4:");
		
		sem[2]=semget(key4, ni, 0700|IPC_CREAT); if(sem[2]<0) perror("s5:"); // all of the inc. pr. work and then a process start next turn
		for( int k=0; k<ni; k++){
			if((semctl(sem[2], k, SETVAL, ni-1))<0)  perror("s6:");
		}
		
		sem[3]=semget(key5, nd, 0700|IPC_CREAT); if(sem[3]<0) perror("s7:"); // all of the dec. pr. work and then a process start next turn
		for( int k=0; k<nd; k++){
			if((semctl(sem[3], k, SETVAL, nd-1))<0)  perror("s8:");
		}
		
		sem[4]=semget(key8, ni, 0700|IPC_CREAT); if(sem[4]<0) perror("s9:"); // it send permission from inc. pr. to dec. pr.
		for( int k=0; k<ni; k++){
			if((semctl(sem[4], k, SETVAL, 0))<0)  perror("s10:");
		}
		
		sem[5]=semget(key9, nd, 0700|IPC_CREAT); if(sem[5]<0) perror("s11:"); // it send permission from dec. pr. to inc. pr.
		for( int k=0; k<nd; k++){
			if((semctl(sem[5], k, SETVAL, 0))<0)  perror("s12:");
		}
		
		printf("Master Process: Current money is %d\n", *shared[0]);
		
		//wait all of the processes finish their job
		sem_wait(sem[0], nd+ni); //perror("ERROR::");
			
		for( int k=0; k<3; k++) {if((shmdt(shared[k]))<0) perror("parent_dt:");} // deatch shared memory seg.   (void*)-1 cast if problem is exist
		for( int k=0; k<3; k++) {if((shmctl(shm[k], IPC_RMID, NULL))<0) perror("parent_shmctl:");}// destroy s.m.seg. return -1
		for( int k=0; k<6; k++) {if((semctl(sem[k],0,IPC_RMID, 0))<0) perror("parent_semctl");}
		
		printf("Master Process: Killing all children and terminating the program\n");

		return 0;
	}
	
	else if(f==0){ //child processes
		sleep(3); // wait until semaphore and shared memory are created by main process
		
		int* shared[3];
		
		shm[0]=shmget(key1, sizeof(int), 0); if(shm[0]<0) perror("m20:"); 
		shared[0]=(int*)shmat(shm[0], 0,0); if(shared[0]==(int*)-1) perror("m21:");  // it keeps money in the moneybox
		
		sem[0]=semget(key2, 1, 0); if(sem[0]<0) perror("s20"); // it provide syncronization with main and child process
		sem[1]=semget(key3, 1, 0); if(sem[1]<0) perror("s21:"); // it provide syncronization when changing money in the moneybox
		sem[4]=semget(key8, ni ,0); if(sem[4]<0) perror("s22:"); // it send permission from inc. pr. to dec. pr.
		sem[5]=semget(key9, nd, 0); if(sem[5]<0) perror("s23:"); // it send permission from dec. pr. to inc. pr.
		
		if(i<ni){ //increaser processes
			shm[1]=shmget(key6, sizeof(int), 0); if(shm[1]<0) perror("m22:");  
			shared[1]=(int*)shmat(shm[1], 0,0); if(shared[1]==(int*)-1) perror("m23:"); // it is increased as much as 1 by every inc. pr.
			
			sem[2]=semget(key4, ni, 0); if(sem[2]<0) perror("s24:"); //it provide syncronization inter inc. pr.; a process work 2. turn after all of the processes finish 1. turn
							
			for(int k=0; k<ti*tour; k++){ // run just one time
				//if other processes finish this turn, it starts next turn
				sem_multi_wait(sem[2], ni-1, i); //perror("----------103:"); 
				
				//lock
				sem_wait(sem[1], 1); //perror("----------104:");
				(*shared[1])++;		
				if(i<ni/2){ //increaser 10
					(*shared[0])+=10;
				}	
				else if(i>=ni/2 && i<ni){ // increaser 15
					(*shared[0])+=15;		
				}
				printf("Increaser Process %d: Current Money is %d", i, *shared[0]);
				if( ((*shared[1])%ni)==0 ) printf(", increaser processes finished their turn %d",(*shared[1])/ni);
				printf("\n");
				//unlock
				sem_signal(sem[1], 1); //perror("----------107:"); 
				
				//it contribute to start for other process' next turn
				sem_multi_signal(sem[2], i, ni); //perror("----------108:"); 
			}	
			//send signal to dec. pr. - if every inc. pr. send to signal, dec. pr. will start
			sem_multi_signal2(sem[4], nd, i); //perror("----------109:"); 
			
			while(true){ 
				// wait for dec. signal .....
				sem_multi_wait2(sem[5], nd); // perror("---------152"); 
				
				for(int k=0; k<ti; k++){	
					//if other processes finish this turn, it starts next turn
					sem_multi_wait(sem[2], ni-1, i); //perror("----------153:"); 
					
					//lock	
					sem_wait(sem[1], 1); //perror("----------154:");
					if((*shared[0])<=0) break;
					(*shared[1])++;		
					if(i<ni/2){ //increaser 10
						(*shared[0])+=10;
					}	
					else if(i>=ni/2 && i<ni){ // increaser 15
						(*shared[0])+=15;		
					}
					printf("Increaser Process %d: Current Money is %d", i, *shared[0]);
					if( ((*shared[1])%ni)==0 ) printf(", increaser processes finished their turn %d",(*shared[1])/ni);
					printf("\n");
					//unlock
					sem_signal(sem[1], 1); //perror("----------155:");
					
					//it contribute to start for other process' next turn
					sem_multi_signal(sem[2], i, ni); //perror("----------156:"); 
				}
				if((*shared[0])<=0) {
					//unlock
					sem_signal(sem[1], 1); //perror("----------157:");
					sem_multi_signal(sem[2], i, ni); //perror("----------158:"); 
					
					//send signal to dec. pr. - if every inc. pr. send to signal, dec. pr. will start
					sem_multi_signal2(sem[4], nd, i); //perror("----------159:");
					
					break;
				}
				//send signal to dec. pr. - if every inc. pr. send to signal, dec. pr. will start
				sem_multi_signal2(sem[4], nd, i); //perror("----------159:");
			}	
			
			if((shmdt(shared[1]))<0) perror("*");
			if((shmdt(shared[0]))<0) perror("****");
			
			//send signal to main pr.
			sem_signal(sem[0], 1); //perror("----------160:");
		}
		else if(i==ni){ // decreaser processes
			shm[2]=shmget(key7, sizeof(int), 0); if(shm[2]<0) perror("m24:");  
			shared[2]=(int*)shmat(shm[2], 0,0); if(shared[2]==(int*)-1) perror("m25:"); // it is increased as much as 1 by every inc. pr.
			
			sem[3]=semget(key5, nd, 0); if(sem[3]<0) perror("s25:");//it provide syncronization inter dec. pr.; a process work 2. turn after all of the processes finish 1. turn
			
			while(true){
				// wait for inc. signal .....
				sem_multi_wait2(sem[4], ni); //perror("----------110");	
				
				for(int k=0; k<td; k++){
					//if other processes finish this turn, it starts next turn
					sem_multi_wait(sem[3], nd-1, j); //perror("----------111");
					
					//lock
					sem_wait(sem[1],1); //perror("----------112:");	
					if((*shared[0])<=0) break;
					(*shared[2])++;
					if(j<nd/2){ // decreaser even
						if((*shared[0])%2==0)	{
							(*shared[0])-=getFibonacci(counter);	
							if((*shared[0])<=0) {
								printf("Decreaser Process %d: Current money is less than %d, signaling master to finish (%d. fibonacci number for decreaser (%d))\n", j, getFibonacci(counter), counter, j);								
								break; // for ti den çıkar
							}
							printf("Decreaser Process %d: Current money is %d (%d. fibonacci number for decreaser (%d))\n", j, *shared[0], counter, j);
							counter++;
						}	
					}
					else if (j>=nd/2 && j<nd){ // decreaser odd
						if((*shared[0])%2==1)	{
							(*shared[0])-=getFibonacci(counter);	
							if((*shared[0])<=0) {
								printf("Decreaser Process %d: Current money is less than %d, signaling master to finish (%d. fibonacci number for decreaser (%d))\n", j, getFibonacci(counter), counter,j);								
								break; // for ti den çıkar
							}
							printf("Decreaser Process %d: Current money is %d (%d. fibonacci number for decreaser (%d))\n", j, *shared[0], counter, j);	
							counter++;
						}	
					}
					if( ((*shared[2])%nd)==0 ) printf(", decreaser processes finished their turn %d\n",(*shared[2])/nd);
					//unlock
					sem_signal(sem[1], 1); //perror("----------113");				
					
					//it contribute to start for other process' next turn
					sem_multi_signal(sem[3], j, nd); //perror("----------107"); // diğer semaforları 1 arttır			
				}			
					
				if((*shared[0])<=0) {
					//unlock
					sem_signal(sem[1], 1); //perror("----------113");			
					sem_multi_signal(sem[3], j, nd); //perror("----------107"); // diğer semaforları 1 arttır
					
					//send signal to inc. pr. - if every dec. pr. send to signal, inc. pr. will start	
					sem_multi_signal2(sem[5], ni, j); //perror("----------109:");
					break;
				}

				//send signal to inc. pr. - if every dec. pr. send to signal, inc. pr. will start	
				sem_multi_signal2(sem[5], ni, j); //perror("----------109:");
			}
			if((shmdt(shared[0]))<0) perror("**");
			if((shmdt(shared[2]))<0) perror("***");	
			
			//send signal to main pr.
			sem_signal(sem[0], 1); //perror("----------91:"); 
			
		}
	}
	sleep(2);
	return 0;
}
