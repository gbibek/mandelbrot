#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/util.hpp>

#include <hpx/hpx.hpp>
#include <hpx/include/iostreams.hpp>

#include <iostream>

//Boost includes
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include <boost/thread/locks.hpp>
#include <boost/foreach.hpp>
#include <boost/ref.hpp>

#include <algorithm>
#include <vector>




#include "EasyBMP.h"

int l = 800;
int b = 800;

int n=1;
double left = -2.0;
double right = 2.0;
double bottom = -2.0;
double top = 2.0;
int maxiteration = 400;
double number_of_time =0;//400




//calculate thes value of the each pixel that is required
//by the BMP to draw it latter in the function do_the_pixel
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

//this function takes co-ordinate after hpx-thread has been created 
//the co-ordinates are from where each of pieces of mandelbrot start
//forming one of the square of the whole mandelbrot   

std::vector<int> do_the_pixel(double x, double y){

    std::vector<int> each_pixel_vector;
    
    int i=0;
    int j=0;
    int value;
    double dummy_x  = x;
    double dummy_y  = y;
    double unit_value = 4.000/800.000;

    while(i<number_of_time){
         
        
        while (j<number_of_time) {

	    value = mandtest(dummy_x,dummy_y);
            dummy_x = dummy_x + unit_value;
            each_pixel_vector.push_back(value);
            j++;
      
        }

        dummy_y = dummy_y - unit_value;
        dummy_x = x;
        j=0;
        i++;

    }
    return each_pixel_vector;
    

}

HPX_PLAIN_ACTION(do_the_pixel,action_do_the_pixel);



//calls the do_the_pixel in each hpx-thread individually

int hpx_main(boost::program_options::variables_map& vm)

{
    
    int counter = 0;

    action_do_the_pixel fn;

    std::vector<hpx::future<std::vector<int>>> vector_future;  //vector which holds futures that will later call do_the_pixel

    BMP draw_fractal;
    draw_fractal.SetBitDepth(24);         
    draw_fractal.SetSize(l,b);                  // set image size
    std::vector<int> future_return;
    RGBApixel NewColor;
    int i_=0;
    int j_=0;
    int x_=0;
    int y_=0;
    int final_x = x_;
    int final_y = y_;
 
 
    
    std::cout<<"before "<<std::endl;
    n = vm["n-value"].as<int>();
    std::cout<<"n="<<n<<std::endl;
    number_of_time = 800.00/n;    

    hpx::util::high_resolution_timer t;
    {
        for (double y=2.0; y>-1.99998; y=y-(4.0/n)) {
            for (double x = -2.0; x<1.99998; x=x+(4.0/n)) {
                
                vector_future.push_back(hpx::async(fn,hpx::find_here(),x,y));
                //pushing function object into the vector
                counter++;
            }

        }
        std::cout<<"counter = "<<counter<<std::endl;
        for(int i=0; i < counter;i++) {
                                                 
            future_return=vector_future[i].get();
            for( y_ = final_y ; y_ < (final_y + number_of_time);y_++){
      
                for( x_ = final_x; x_ < (final_x+ number_of_time);x_++){    
           
                 
                    if(future_return[i_] ==maxiteration){
      
                        NewColor.Red = 0;
                        NewColor.Green = 0;
                        NewColor.Blue = 0;
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(x_,y_,NewColor);

                    }
                    else {
                      
                        NewColor.Red = 10;
                        NewColor.Green = 2;
                        NewColor.Blue = 160;
                        NewColor.Alpha = 10;
                        draw_fractal.SetPixel(x_,y_,NewColor);
                       
                    }                   
                    i_++;
                    
                }
                
            }
            j_++;
            i_ = 0; 
            final_x = final_x + number_of_time;
           
            if(j_ == n ){
                j_ = 0;
                final_y = final_y + number_of_time; 
                final_x = 0;
              
            }               
       
                       
                         
        }
        std::cout << "-------time it took to do hpx  " << t.elapsed()<<" sec"<<std::endl;
        draw_fractal.WriteToFile("fractel.bmp");  //BMP writes into the file fractel.bmp

    }
    
    
   
    return hpx::finalize();

}

int main(int argc, char * argv[])
{

    boost::program_options::options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ( "n-value",
          boost::program_options::value<int>()->default_value(1),
          "n value for number of thread")
        ;

    
    return hpx::init(desc_commandline,argc,argv);

	
}
