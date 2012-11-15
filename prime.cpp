#include <iostream>
#include <stdlib.h>
using namespace std;



int z[99];
int q[99];

int set=0;
int n=0;

int newset=0;

int d1(int x, int y);
int d2(int x, int y);
int d3(int x, int y);
int d4(int x, int y);

void printd (int z[]);




void setsubstraction(int z[]);


int main(){


   for(int i=1;i<=5;i++){
 
	for(int j=1;j<=5;j++){


	z[n]=d1(i,j);
	n = n+1;
	z[n]=d2(i,j);
	n = n+1;
	z[n]=d3(i,j);
	n = n+1;
	z[n]=d4(i,j);
	n = n+1;




	}
   }

	
	cout<<"now the final result"<<endl;
	setsubstraction(z);
	printd(q);

return 0;

}




void printd (int q[]){

	for(int p=0;p<100;p++){

		cout<<"("<<6*q[p]-1<<","<<6*q[p]+1<<")"<<endl;

	}

	
}

void setsubstraction(int z[]){

	int x =0;	

	for(int n=1;n<100;n++){
	
		for(int m=0;m<100;m++){
			x=m;
			if(n == z[m]){break;}
			
			
			

		}
		
		if(x==99){q[newset]=n;newset=newset+1;}


	}



}


int d1(int x, int y){

return ((6*x*y)+x+y);

}

int d2(int x, int y){

return ((6*x*y)+x-y);

}
int d3(int x, int y){

return ((6*x*y)-x+y);

}
int d4(int x, int y){

return ((6*x*y)-x-y);

}
