#include <stdio.h>
#include <mpi.h>

#define SIZE 100000

struct Complex {
double r;
double i;
};


Complex operator + (Complex s, Complex t){
Complex v;
v.r = s.r + t.r;
v.i = s.i + t.i;
return v;
}

Complex operator * (Complex s, Complex t){
Complex v;
v.r = s.r*t.r - s.i*t.i;
v.i = s.r*t.i + s.i*t.r;
return v;
}

int mandel(Complex c, int maxIter){
	int i=0;
	Complex z;
	z=c;
	while(i<maxIter && z.r*z.r + z.i*z.i < 4){
		z=z*z + c;
		i++;
	}
	return i;
}


int main(){
  double xmin,xmax,ymin,ymax;
  int i,j,rows,columns,rank,nproc;
  Complex z;
  int row[SIZE],hdr;
  unsigned char line[3*SIZE];
  FILE *img;

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  img=fopen("mandel.ppm","w");
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank==0) hdr=fprintf(img,"P6\n%d %d 255\n",SIZE,SIZE);

  MPI_Bcast(&hdr,1,MPI_INT,0,MPI_COMM_WORLD);

  Complex c1,c2;
  c1.r=-2;
  c1.i=-1;
  c2.r=0;
  c2.i=1;

  for(i=(rank*SIZE)/nproc;i<((rank+1)*SIZE)/nproc;i++){
    for(j=0;j<SIZE;j++){
      z.r = (j*(c1.r-c2.r)/SIZE)+c2.r;
      z.i = (i*(c1.i-c2.i)/SIZE)+c2.i;
      row[SIZE-j]=mandel(z,255);
    }

    for(j=0;j<SIZE;j++){
      if (row[j]<=63){
        line[3*j]=100;
        line[3*j+1]=line[3*j+2]=200-3*row[j];
      }
      else{
        line[3*j]=125;
        line[3*j+1]=row[j]-63;
        line[3*j+2]=60;
      }
      if (row[j]==320) line[3*j]=line[3*j+1]=line[3*j+2]=255;
    }

    fseek(img,hdr+3*SIZE*i,SEEK_SET);
    fwrite(line,1,3*SIZE,img);
  }

  MPI_Finalize();
  return 0;
}
