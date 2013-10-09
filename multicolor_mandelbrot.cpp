
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

int l = 12800;
int b = 12800;

int n = 1;
double min_real  = -2.000;
double min_image = -2.000;

int maxiteration   = 400;
int number_of_time = 0;//400

struct each_pixel {
    double real_axis;
    double image_axis;
    int mantest_no;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & real_axis & image_axis & mantest_no;
    }

};


//calculate thes value of the each pixel that is required
//by the BMP to draw it latter in the function do_the_pixel
int mandtest(double Cr, double Ci){

    double Zr = 0.0;
    double Zi = 0.0;
    int times = 0;
    double temp;
           
    while ((((Zr*Zr)+(Zi*Zi))<=4) && (times < maxiteration)){
        	
        temp  = (Zr*Zr)-(Zi*Zi);
        Zi    = 2*Zr*Zi;
        Zr    = temp+Cr;
        Zi    = Zi+Ci;		           
        times = times+1;  
           
    }


    return times;

}

// collects all the pixels from all the hpx-thread from 
// particular node 

std::vector<each_pixel> thread_manage_pixel(int i, int j, int non)
{
    std::vector<each_pixel> each_pixel_vector;                                   
    int value;                                                                   
    double scale_real      = 4.000/((double)l);                                  
    double scale_image     = 4.000/((double)b);                                  
    int where_is_pixel     = 0;                                                  
    int hold_remainder     = 0;                                                  
    int mult       = 0; // rang from 0 - 800 for -2 to 2                               
    int mult_image = 0; // int becuase 799/800 ~ 0 
    // if we try to divide the pixel and it turns out its less                   
    // then one.      


    double number_of_pixel=((l*b)/(non*n));                                      
    int remainder         = (l*b)-(number_of_pixel*non*n);                               
    hold_remainder        = remainder;                                                  
    remainder             = remainder/(non);                                                 
    int remainder1        = hold_remainder - (remainder*non);                           
    if(j<remainder && i<remainder)                                              
    {                                                                            
        number_of_pixel   = number_of_pixel+1;                                       
        
        if(j<remainder1) number_of_pixel = number_of_pixel+1;                            
        if(j!=0)         where_is_pixel  = (number_of_pixel* ( i*n+j ));                      
    }                                                                        
    
    else                                                                         
    {                                                                            
        where_is_pixel  = (number_of_pixel* ( i*n+j ))+remainder;                
    }                                                               
                                                                              
     //                                                                           
    for(int k_= 0 ; k_ < number_of_pixel;k_++)                                  
    {                                                                            
        each_pixel pixel;                                                                                                                                  
        
        mult                   = where_is_pixel%(int)l; 
        pixel.real_axis        = min_real +  ((double)scale_real*mult);
        mult_image             = where_is_pixel/(int)b;                                     
        pixel.image_axis       = 
             min_image + (((double)scale_image)*mult_image);
        value                  =
            mandtest(pixel.real_axis,pixel.image_axis);          
        pixel.mantest_no = value;                                          
        each_pixel_vector.push_back(pixel);                                
                                                                                 
        where_is_pixel++;     

    }
    
    return each_pixel_vector;

}

HPX_PLAIN_ACTION(thread_manage_pixel,action_thread_manage_pixel);
// this function takes co-ordinate after hpx-thread has been created 
// the co-ordinates are from where each of pieces of mandelbrot start
// forming one of the square of the whole mandelbrot   
// i is id of node, j is id of hpx-thread and non is total number 
// of node used while running this program
std::vector<each_pixel> do_the_pixel(int i, int non)
{

    std::vector<hpx::future<std::vector<each_pixel>>> vector_future;
    std::vector<each_pixel> all_pixel;  
    std::vector<each_pixel> hold_pixel; 

    hpx::naming::id_type const here = hpx::find_here();
 
    for(int j =0;j<n;j++)                                                
    { 
    
        typedef action_thread_manage_pixel fn;                                  
        vector_future.push_back(hpx::async<fn>(here,i,j,non));
    
    }

    

    for(int k = 0; k <n; k++)
    {
        hold_pixel = vector_future[k].get();
        all_pixel.insert(all_pixel.end(),hold_pixel.begin(),hold_pixel.end());  
            
    }  
    return all_pixel;       
}

HPX_PLAIN_ACTION(do_the_pixel,action_do_the_pixel);



//calls the do_the_pixel in each hpx-thread individually
//how many of individual sqaure is needed is determined 
//by for loop
int hpx_main(boost::program_options::variables_map& vm)

{
 
    std::vector<hpx::future<std::vector<each_pixel>>> vector_future;   

    BMP draw_fractal;
    draw_fractal.SetBitDepth(24);         
    draw_fractal.SetSize(l,b);                  // set image size
    std::vector<each_pixel> future_return;
    RGBApixel NewColor;
    std::vector<each_pixel> iter;
    int i_=0;
    int j_=0;
    int i=0;
    
    double x = 0.00;
    double y = 0.00;
    

    std::vector<hpx::naming::id_type> localities =
            hpx::find_all_localities();
    
    std::cout<<"before "<<std::endl;
    n = vm["n-value"].as<int>();
    //std::cout<<"n="<<n<<std::endl;
    std::cout<<"Start time"<<std::endl;
    hpx::util::high_resolution_timer t;
    {
        BOOST_FOREACH(hpx::naming::id_type const& node,localities)
        {
          
            
           // x = min_real;  
           // y = min_image + i*(b/localities.size());
             
            typedef action_do_the_pixel fn;
            vector_future.push_back(hpx::async<fn>(node,i,localities.size()));
            i++;// i tells which node
               
            
            
        }        
        // loop through nodes
        std::cout<<"collecting all the pixels "<<std::endl;        
        
        for(unsigned int  i=0; i < localities.size();i++)
        {
                // loop through hpx-thread
                
                future_return = vector_future[i].get();
                // loop through all the pixel of each thread
                for(unsigned int k = 0;k<future_return.size();k++)
                { 
                    
                    if(future_return[k].mantest_no > 1)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*204));
                        NewColor.Green = (255 - (future_return[k].mantest_no*204));
                        NewColor.Blue = (255 - (future_return[k].mantest_no));
                        NewColor.Alpha = 0;
                    //    std::cout<<future_return[k].mantest_no<<std::endl;
                        draw_fractal.SetPixel(i_,j_,NewColor);
                
                    }
                    if(future_return[k].mantest_no > 2)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*10));
                        NewColor.Green = (255-future_return[k].mantest_no*100);
                        NewColor.Blue = (255-(future_return[k].mantest_no*109));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 3)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*10));
                        NewColor.Green = (255-future_return[k].mantest_no*45);
                        NewColor.Blue = (255-(future_return[k].mantest_no*45));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 5)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*1.5));
                        NewColor.Green = (255-future_return[k].mantest_no*15);
                        NewColor.Blue = (255-(future_return[k].mantest_no*15));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 6)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*3));
                        NewColor.Green = (255-future_return[k].mantest_no*25);
                        NewColor.Blue = (255-(future_return[k].mantest_no*15));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 25)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*15));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
                                                                   
                    }
                    if(future_return[k].mantest_no > 35)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*15);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 45)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*25);
                        NewColor.Blue = (255-(future_return[k].mantest_no*15));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                     }
                    if(future_return[k].mantest_no > 55)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*25);
                        NewColor.Blue = (255-(future_return[k].mantest_no*15));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 100)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*31));
                        NewColor.Green = (255-future_return[k].mantest_no*25);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 150)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*3));
                        NewColor.Green = (255-future_return[k].mantest_no*1.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 200)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*1.01));
                        NewColor.Green = (255-future_return[k].mantest_no);
                        NewColor.Blue = (255-(future_return[k].mantest_no));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 215)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*1.5));
                        NewColor.Green = (255-future_return[k].mantest_no);
                        NewColor.Blue = (255-(future_return[k].mantest_no));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 250)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*0.8));  
                        NewColor.Green = (255-future_return[k].mantest_no);  
                        NewColor.Blue = (255-(future_return[k].mantest_no)); 
                        NewColor.Alpha = 0;                                      
                        draw_fractal.SetPixel(i_,j_,NewColor);

                    }
                    
                    if(future_return[k].mantest_no > 255)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no*0.8));
                        NewColor.Green = (255-future_return[k].mantest_no);
                        NewColor.Blue = (255-(future_return[k].mantest_no));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 265)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 275)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 300)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 350)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 360)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no > 375)
                    {
                        NewColor.Red = (255 - (future_return[k].mantest_no/3));
                        NewColor.Green = (255-future_return[k].mantest_no*2.5);
                        NewColor.Blue = (255-(future_return[k].mantest_no*1.5));
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }
                    if(future_return[k].mantest_no == maxiteration)
                    {
                        NewColor.Red =  51;
                        NewColor.Green = 0;
                        NewColor.Blue = 0;
                        NewColor.Alpha = 0;
                        draw_fractal.SetPixel(i_,j_,NewColor);
   
                    }                                         
      
                    if(j_== (l-1)){ j_= 0; i_++;}
                    else (j_++);                                         
                    
                   
                }                                    
                                   
                                                           
        }                                   
                                          
        std::cout << "-------time it took to do hpx  "; 
        std::cout<< t.elapsed()<<" sec"<<std::endl;
        //BMP writes into the file fractel.bmp
        draw_fractal.WriteToFile("fractel.bmp"); 

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
