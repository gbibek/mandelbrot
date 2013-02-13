
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


int l = 800;
int b = 800;

double dividecubesby  = 800;
double left = -2.0;
double right = 2.0;
double bottom = -2.0;
double top = 2.0;
int maxiteration = 400;
int zoomlevel = 1;
double baseSize = 4.0;
double Size=0.0;
double xco=0.0;
double yco=0.0;
int n =0.0;

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


int hpx_main()
{

    BMP draw_fractal;
    draw_fractal.SetBitDepth(24);
    draw_fractal.SetSize(l,b);
    int counter = 0;
    action_mandtest fn;
    std::vector<hpx::future<int>> vector_future;
    RGBApixel NewColor;
    double deltax = ((right - left)/(dividecubesby-1));
    double deltay = ((top- bottom)/(dividecubesby-1));
    int i = -1 ;
    int j = -1 ;
	
    hpx::util::high_resolution_timer t;
    {
        for(double x= left;x<=right;x += deltax ){
            for(double y= bottom; y<=top;y +=  deltay ){
	        vector_future.push_back(hpx::async(fn,hpx::find_here(),x,y));	
	    }
	}
	
        for(double x= left;x<=right;x += deltax ){
	    
            i++;
            for(double y= bottom; y<=top;y +=  deltay ){
		
                j++;
      	        if((vector_future[counter].get())==maxiteration){
    		    NewColor.Red = 0;
                    NewColor.Green = 0;
                    NewColor.Blue = 0;
                    NewColor.Alpha = 0;
                    draw_fractal.SetPixel(i,j,NewColor);
				
		}
		else {
		    NewColor.Red = 160;
                    NewColor.Green = vector_future[counter].get();
                    NewColor.Blue = 10;
                    NewColor.Alpha = 10;
                    draw_fractal.SetPixel(i,j,NewColor);
				
		}
		counter++;				
            }
            j=-1;
        }
    }
    	draw_fractal.WriteToFile("fractel.bmp");
	std::cout << "time it took to do hpx  " << t.elapsed()<<" sec"<<std::endl;
    

    return hpx::finalize();

}


int main(int argc, char ** argv)
{	
	
    return hpx::init();

	
}
