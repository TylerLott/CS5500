#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

#define MCW MPI_COMM_WORLD

using namespace std;

// The integer variable is equal to the rank of the process


//MPI_Allreduce

void allReduce(){
	int rank, size;
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	int reduction_res = 0;
	MPI_Allreduce(&rank,&reduction_res,1,MPI_INT,MPI_SUM,MCW);

	MPI_Barrier(MCW);

	cout<<"Allreduce: Process "<<rank<<": The sum of all processes is: "<<reduction_res<<endl;

	return;
}

//MPI_Gather and MPI_Bcast
void gatherBcast(){
	int rank, size;
	int data;
	int sum = 0;
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	data = rank;

	if(rank==0){
		int buf[size];
		MPI_Gather(&data,1,MPI_INT,buf,1,MPI_INT,0,MCW);
		for(int i=0;i<size;++i){
			sum+=buf[i];
		}
	}else{
		MPI_Gather(&data,1,MPI_INT,NULL,0,MPI_INT,0,MCW);
	}

	MPI_Bcast(&sum,1,MPI_INT,0,MCW);

	MPI_Barrier(MCW);

	cout<<"Gather Bcast: Process "<<rank<<" : The sum of all processes is: "<<sum<<endl;

	return;
}

//MPI_Send and Recv
void sendRecv(){
	int rank, size;
	int data;
	int data2;
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	//master
	if(rank==0){
		for(int i=1;i<size;++i){
			data = i;
			MPI_Send(&data,1,MPI_INT,i,0,MCW);
		}
		data = 0;
		for(int i=1;i<size;++i){
			MPI_Recv(&data2,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
			data=data+data2;
		}
		for(int i=1;i<size;++i){
			MPI_Send(&data,1,MPI_INT,i,0,MCW);
		}
	// slave
	}else{
		MPI_Recv(&data,1,MPI_INT,0,0,MCW,MPI_STATUS_IGNORE);
		MPI_Send(&data,1,MPI_INT,0,0,MCW);
		MPI_Recv(&data,1,MPI_INT,0,0,MCW,MPI_STATUS_IGNORE);
	}

	cout<<"Send - Recieve: Process "<<rank<<" : The sum of all processes is: "<<data<<endl;

	MPI_Barrier(MCW);
	return;
}



//MPI_Send and Recv in ring topology
void ring(int *data,int *sum, int plusMinus){
	int rank, size;
	int dest;
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);
	if(plusMinus==1){
		dest = (rank+1)%size;
	}else{
		dest = ((rank-1)+size)%size;
	}

	MPI_Send(data,1,MPI_INT,dest,0,MCW);
	MPI_Recv(data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

	*sum = *sum + *data;
	MPI_Barrier(MCW);
	return;
}

// MPI_Send and Recv in hypercube
void cube(int *data, int m){
	int rank, size;
	int dest;
	int *data2;
	unsigned int mask=1;
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);

	dest = rank^(mask<<m);

	MPI_Send(data,1,MPI_INT,dest,0,MCW);
	MPI_Recv(data2,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

	*data = *data + *data2;
	MPI_Barrier(MCW);
	return;
}


int main(int argc, char **argv){
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MCW, &rank);

//	All Reduce Variation
	allReduce();
	MPI_Barrier(MCW);

//	Gather and BCast Variation
	gatherBcast();
	MPI_Barrier(MCW);

//	Send and Recv send to rank 0
	sendRecv();
	MPI_Barrier(MCW);

//	Send and Recv ring topology
	MPI_Comm_rank(MCW, &rank);
	int data3 = rank;
	int sum = 0;
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	ring(&data3,&sum,1);
	cout<<"Ring: Process "<<rank<<" : The sum of all processes is: "<<sum<<endl;
	MPI_Barrier(MCW);

//	Send and Recv hypercube topology
	MPI_Comm_rank(MCW, &rank);
	int data4 = rank;
	cube(&data4,0);
	cube(&data4,1);
	cube(&data4,2);
	cout<<"Hypercube: Process "<<rank<<" : The sum of all processes is: "<<data4<<endl;

//	cout<<"Hypercube: Process "<<rank<<"sum of all processes is: "<<data<<endl;
	MPI_Finalize();
	return 0;
}
