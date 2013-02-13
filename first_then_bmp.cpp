
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <hpx/hpx_main.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/util.hpp>

#include <hpx/hpx.hpp>
#include <hpx/include/iostreams.hpp>

#include <iostream>
#include "EasyBMP.h"


int l = 10;
int b = 10;

double dividecubesby  = 5;
double left = -2.0;
double right = 2.0;
double bottom = -2.0;
double top = 2.0;
int maxiteration = 5;
int zoomlevel = 1;
double baseSize = 4.0;
double Size=0.0;
double xco=0.0;
double yco=0.0;
int n =0.0;

RGBApixel NewColor;
BMP draw_fractal;


double deltax = ((right - left)/(dividecubesby-1));
double deltay = ((top- bottom)/(dividecubesby-1));

int mandtest(double Cr, double Ci){

    double Zr = 0.0;
    double Zi = 0.0;
    int times = 0;
    double temp;
    Zr = Zr+Cr;
    Zi = Zi+Ci;
          
    while ((((Zr*Zr)+(Zi*Zi))<=4) && (times < maxiteration)){
        	
        temp = (Zr*Zr)-(Zi*Zi);
        Zi = 2*Zr*Zi;
        Zr = temp+Cr;
        Zi = Zi+Ci;		           
        times = times+1;  
           
    }
	

    return times;

}

HPX_PLAIN_ACTION(mandtest,action_mandtest);

int get_mandel(hpx::future<int> fut)
{
    int i = -1 ;
    int j = -1 ;

    for(double x= left;x<=right;x += deltax ){
        i++;
        for(double y= bottom; y<=top;y +=  deltay ){
	    if( fut.get() == maxiteration){
                NewColor.Red = 0;
                NewColor.Green = 0;
                NewColor.Blue = 0;
                NewColor.Alpha = 0;
                draw_fractal.SetPixel(i,j,NewColor);
				
	    }
 
	    else {
		 NewColor.Red = 10;
                 NewColor.Green =fut.get()*2;
                 NewColor.Blue = 160;
                 NewColor.Alpha = 10;
                 draw_fractal.SetPixel(i,j,NewColor);	
	    
            }
							
	}
        j=-1;
    }

return 0;
	
}

int hpx_main()
{


    action_mandtest fn;
    std::vector<hpx::future<int>> vector_future;
    draw_fractal.SetBitDepth(24);
    draw_fractal.SetSize(l,b);	

    int i = -1 ;
    int j = -1 ;
	
    hpx::util::high_resolution_timer t;
    {
        for(double x= left;x<=right;x += deltax ){
            for(double y= bottom; y<=top;y +=  deltay ){
	        vector_future.push_back(hpx::async(fn,hpx::find_here(),x,y).then(&get_mandel));	
	    }
	}
	
        hpx::wait(vector_future);
    }
    	draw_fractal.WriteToFile("fractel.bmp");
	std::cout << "time it took to do hpx  " << t.elapsed()<<" sec"<<std::endl;
    

    return hpx::finalize();

}


int main(int argc, char ** argv)
{

	
    return hpx::init();

	
}
