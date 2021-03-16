#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <queue>

int initial = rand() %  1024 + 1;


int main(){
	int rank,nproc;
	std::queue<int> myqueue;
	int flag = 0;
	MPI_Status status;
	int running = 1;
	int getting = 1;
	int recvMsg = 0;
	int otherproc = 0;
	int data = 0;
	int donedata = 0;
	int maxgen = rand() % 1024 + 1024;
	int gen = 0;
	int color = 1; // 0-black 1-white
	int ringpass = 0;
	int term = 0;
	int token = 0;
	int dest;
	int die = 3;
	int hastoken = 0;
	int tasksdone = 0;

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	srand(rank);

	if (rank==0) {
		myqueue.push(initial);
		std::cout<<"initial sent to 0"<<std::endl;
	}

// even processes create more work, odd do not

	if (rank % 2 == 0){

	while(running){
		MPI_Iprobe(MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&flag,&status);
		if (flag==1){
			while(flag==1){
				MPI_Recv(&recvMsg,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
				myqueue.push(recvMsg);
				MPI_Iprobe(MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&flag,&status);
			}
		}
		if (myqueue.size() > 16) {
			for (int i=0;i<2;i++){
			otherproc = rand() % nproc;
			data = myqueue.front();
			myqueue.pop();
			MPI_Send(&data,1,MPI_INT,otherproc,0,MPI_COMM_WORLD);
			if (otherproc < rank) { color = 0; }
			}
		}
		if (myqueue.size() > 0){
		data = myqueue.front();
		myqueue.pop();
		tasksdone++;
		for (int i = 0; i < data * data; i++){
			donedata++;
		}
		donedata = 0;

		}
		if (gen < maxgen){
			for (int i = 0; i < (rand() %3 + 1); i++){
				myqueue.push(rand() % 1024 + 1);
			}
			gen++;
		}
		if (myqueue.size() == 0 && rank == 0) {
			if (ringpass != 1){
			color = 1;
			token = 1;
			MPI_Send(&token,1,MPI_INT,rank+1,1,MPI_COMM_WORLD); // sends white token to next process
			ringpass = 1;
//			std::cout<<"ringpass started"<<std::endl;
			}
			term=0;
			MPI_Iprobe(MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&term,&status);
			if (term==1){
				MPI_Recv(&token,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&status);
//				std::cout<<"process : "<<rank<<" recieved "<<token<<" token from "<<nproc-1<<std::endl;
				if (token==1) {
					MPI_Send(&die,1,MPI_INT,rank+1,1,MPI_COMM_WORLD);
					std::cout<<"process "<<rank<<" tasks done "<<tasksdone<<std::endl;
					running=0;
				} else {
					token = 1;
					MPI_Send(&token,1,MPI_INT,rank+1,1,MPI_COMM_WORLD);}
			term=0;
			}
		}
		MPI_Iprobe(rank-1,1,MPI_COMM_WORLD,&term,&status);
		if (term==1 && rank != 0){
			hastoken=1;
			MPI_Recv(&token,1,MPI_INT,rank-1,1,MPI_COMM_WORLD,&status);
//			std::cout<<"process : "<<rank<<" recieved "<<token<<" token from "<<rank-1<<std::endl;
		}
		if (myqueue.size() == 0 && hastoken == 1&& rank!=0){
			if (token==3) {
				dest = (rank+1)%nproc;
				MPI_Send(&die,1,MPI_INT,dest,1,MPI_COMM_WORLD);
				std::cout<<"process "<<rank<<" tasks done "<<tasksdone<<std::endl;
				running=0;
			} else if (color == 1) {
				dest = (rank+1)%nproc;
				MPI_Send(&token,1,MPI_INT,dest,1,MPI_COMM_WORLD);
			} else {
				dest = (rank+1)%nproc;
				token = 0;
				MPI_Send(&token,1,MPI_INT,dest,1,MPI_COMM_WORLD);
//				std::cout<<"sent to "<<dest<<std::endl;
				color = 1;
			}
		hastoken=0;
		}
	}

	} else {
		while(running){
			MPI_Iprobe(MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&flag,&status);
			if (flag==1){
				while(flag==1){
					MPI_Recv(&recvMsg,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
					myqueue.push(recvMsg);
					MPI_Iprobe(MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&flag,&status);
				}
			}
			if (myqueue.size() > 16) {
				for (int i=0;i<2;i++){
					otherproc = rand() % nproc;
					data = myqueue.front();
					myqueue.pop();
					MPI_Send(&data,1,MPI_INT,otherproc,0,MPI_COMM_WORLD);
					if (otherproc < rank) { color = 0; }
				}
			}
			if (myqueue.size() > 0){
				data = myqueue.front();
				myqueue.pop();
				tasksdone++;
				for (int i = 0; i < data * data; i++){
					donedata++;
				}
			}
			donedata = 0;
			MPI_Iprobe(rank-1,1,MPI_COMM_WORLD,&term,&status);
			if (term==1){
				hastoken=1;
				MPI_Recv(&token,1,MPI_INT,rank-1,1,MPI_COMM_WORLD,&status);
//				std::cout<<"process : "<<rank<<" recieved "<<token<<" token from "<< rank-1<<std::endl;
			}
			if (myqueue.size() == 0 && hastoken == 1){
				if (token==3) {
					dest = (rank+1)%nproc;
					MPI_Send(&die,1,MPI_INT,dest,1,MPI_COMM_WORLD);
					running=0;
					std::cout<<"process "<<rank<<" tasks done "<<tasksdone<<std::endl;
				} else if (color == 1) {
					dest = (rank+1)%nproc;
					MPI_Send(&token,1,MPI_INT,dest,1,MPI_COMM_WORLD);
				} else {
					dest = (rank+1)%nproc;
					token = 0;
					MPI_Send(&token,1,MPI_INT,dest,1,MPI_COMM_WORLD);
					color = 1;
				}
			hastoken=0;
			}
		}
	}
  MPI_Finalize();
  return 0;
}
