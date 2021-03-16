#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv){
  int rank, size;
  int order;
  int quit;
  int orders = 0;
  int flag = 0;
  MPI_Init(&argc, &argv);
  MPI_Status mystatus;
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);
  srand(rank);

// master - the cook
// the cook takes a 1 sec smoke if no orders in buffer
// the cook cooks each order in the buffer (one second each)
// the cook quits if over 20 orders in the buffer

	if(rank==0){
		int happy = 1;
		while(happy){
			flag = 0;
			MPI_Iprobe(MPI_ANY_SOURCE,0,MCW,&flag,&mystatus);
			if(!flag){
				cout<<"Cook: No orders, going for a smoke break."<<endl;
				sleep(1);
			}
			else{
				while(flag){
					MPI_Recv(&order,21,MPI_INT,MPI_ANY_SOURCE,0,MCW,&mystatus);
					MPI_Iprobe(MPI_ANY_SOURCE,0,MCW,&flag,&mystatus);
					orders++;
				}
				if(orders<21){
					cout<<"Cook: Looks like I had : "<<orders<<" waiting for me."<<endl;
					int ordersLeft = orders;
					for(int i=0;i<orders;++i){
					cout<<"Cook: Cooking order "<<ordersLeft<<"..."<<endl;
					ordersLeft--;
					sleep(1);
					}
					orders = 0;
					cout<<"Cook: Finished all the orders, lets see if there are more."<<endl;
				}else{
					cout<<"Cook: This is too much work, screw this I quit."<<endl;
					for(int i=1;i<size;++i){
						quit = 404;
						MPI_Send(&quit,1,MPI_INT,i,0,MCW);
						happy = 0;
					}
				}
			}
		}
	}
// slave - chefs
// chefs make sure cook hasn't quit
// if not, they take 1-5 seconds deciding what to order
// if the chef did quit, they leave
	else{
		while(1){
			flag = 0;
			MPI_Iprobe(0,0,MCW,&flag,&mystatus);
			if(flag){
				MPI_Recv(&quit,1,MPI_INT,0,0,MCW,&mystatus);
				if(quit==404){
					cout<<"Chef "<<rank<<": Why did the cook quit?"<<endl;
					break;
				}
			}else{
				MPI_Send(&order,1,MPI_INT,0,0,MCW);
				cout<<"Chef "<<rank<<": Sent in order."<<endl;
				int sleeptime = rand()%5+1;
				sleep(sleeptime);
			}
		}
	}
	MPI_Finalize();
	return 0;
}
