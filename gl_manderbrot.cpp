#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

void setXYpos(int px, int py)
{	
	xco = left+(right-left)*px/dividecubesby;
	yco = top-(top-bottom)*py/dividecubesby;
	//xco = ((px-0)/(dividecubesby))*(right-left)+left;
	//yco = ((py-0)/(dividecubesby))*(top-bottom)+bottom;


}

void MouseButton(int button,int state,int x,int y){
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
        
        int xx= x;
            int yy= y;
        setXYpos(xx,yy);
        Size = 0.5*(pow(2.0,(-zoomlevel)));	

        printf("Size = %f \n",Size);
        left = xco - Size;
        right = xco + Size;
        top = yco + Size;
        bottom = yco - Size;
        dividecubesby = dividecubesby + 100;
        maxiteration = maxiteration + 100;
        zoomlevel = zoomlevel+1;
        
        glutPostRedisplay();
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{

        
        int xx= x;
            int yy= y;
        setXYpos(xx,yy);
        zoomlevel = zoomlevel-1;
        Size = 0.5*(pow(2.0,(-zoomlevel)));	

        printf("Size = %f \n",Size);
        left = xco - Size;
        right = xco + Size;
        top = yco + Size;
        bottom = yco - Size;
        //dividecubesby = dividecubesby - 100;
        //maxiteration = maxiteration - 100;
        //zoomlevel = zoomlevel-1;
            
        glutPostRedisplay();

	}
}


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


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f,1.0f,1.0f);
  	double deltax = ((right - left)/(dividecubesby-1));
	double deltay = ((top- bottom)/(dividecubesby-1));
	
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	
        gluOrtho2D(left,right,bottom,top);
	

	glBegin(GL_POINTS);
       	
	for(double x= left;x<=right;x += deltax ){
            for(double y= bottom; y<=top;y +=  deltay ){
      		    if((mandtest(x,y))==maxiteration){
    			    glColor3f(1.0f,1.0f,1.0f); 
      			    glVertex2f(x,y);
			
		        }   
		        else {
			        glColor3f((float)mandtest(x,y)/10,0.0f,(float)mandtest(x,y)/30);
                    glVertex2f(x,y);
		        }   
						
	        }
    }
    glEnd();
	glFlush();
}

void init(void)
{
	
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //gluOrtho2D(-2,2,-2,2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   
}


int main(int argc, char ** argv)
{	
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(dividecubesby,dividecubesby);
	glutCreateWindow("A Simple OpenGL Windows Application with GLUT");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(MouseButton);
	glutMainLoop();
return 0;
}
