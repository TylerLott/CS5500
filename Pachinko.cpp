#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

#define MCW MPI_COMM_WORLD

using namespace std;

// MUST USE 24 PROCESSES
// There are 5 catagories at the bottom
// There are 4 levels in this hypothetical board
// balls are not numbered in order, instead ball is a value keeping track of the level

int main(int argc, char **argv){
  int rank, size;
  int ball;
  int bin0=0;
  int bin1=0;
  int bin2=0;
  int bin3=0;
  int bin4=0;
  int numballs=50;
  MPI_Status mystatus;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);
  srand(rank);

// master
  if(rank==0){
    for(int i=0; i<numballs; ++i){
      ball = 0;
      MPI_Send(&ball,1,MPI_INT,3,0,MCW);
      //cout<<"Ball "<<ball<<" sent to process "<<3<<endl;
    }
    for(int i=0;i<numballs; ++i){
      MPI_Recv(&ball,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
      //cout<<"Ball recieved in bin "<<ball<<endl;
      if(ball==0){bin0++;}
      if(ball==1){bin1++;}
      if(ball==2){bin2++;}
      if(ball==3){bin3++;}
      if(ball==4){bin4++;}
    }
    for(int i=0; i<size; ++i){
      ball=404;
      // cout<<"breaking"<<endl;
      MPI_Send(&ball,1,MPI_INT,i,0,MCW);
    }
    cout<<"Simulation Complete"<<endl;
    cout<<"bin 0: "<<bin0<<endl;
    cout<<"bin 1: "<<bin1<<endl;
    cout<<"bin 2: "<<bin2<<endl;
    cout<<"bin 3: "<<bin3<<endl;
    cout<<"bin 4: "<<bin4<<endl;
  }
// slave
  else{
   while(1){
    MPI_Recv(&ball,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
    if(ball==404){
      break;
    }else{
      if(ball>3){
        ball=rank-19;
        MPI_Send(&ball,1,MPI_INT,0,0,MCW);
      }else{
        ball++;
        int direction=rand()%2;
        //cout<<"Level "<<ball<<" ball sent from process "<<rank<<" direction "<<direction<<endl;
        if(direction==0){
          MPI_Send(&ball,1,MPI_INT,rank+4,0,MCW);
        }else{
          MPI_Send(&ball,1,MPI_INT,rank+5,0,MCW);
        }
      }
    }
   }
  }
  MPI_Finalize();
  return 0;
}
