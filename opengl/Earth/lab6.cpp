
#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string>
#include "SOIL.h"

using namespace std;

bool WireFrame= false;

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


GLuint fogMode[] = {GL_EXP, GL_EXP2, GL_LINEAR};
GLuint fogfilter = 3;
GLfloat fogColor[] = {0.5f, 0.5f, 0.5f,1.0f};

GLuint tex[7]; // Texture pointer
GLuint Map;
GLuint moon_map;

float RotateX =0;
float RotateY =0;
float RotateZ =0;
float translate_z =-1;
  bool status = true;
float spin, espin;

float x_cam, y_cam, z_cam;
GLdouble upX, upY, upZ; //camera, GL_LOOKAT

GLUquadricObj *sphere = NULL;
GLUquadricObj *moon = NULL;
bool togglefog;

float position[] = {0.0,5.0,10.5,1.0}; //fr lighting posiiton
/* GLUT callback Handlers */



static void text(GLuint tex, const char* filePath){
unsigned char* image;
    int width, height;
     glBindTexture(GL_TEXTURE_2D, tex); // images are 2D arrays of pixels, bound to the GL_TEXTURE_2D target.
     
     image = SOIL_load_image(filePath, &width, &height, 0, SOIL_LOAD_RGB);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    // binding image data
     SOIL_free_image_data(image);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

}


static void resize(int width, int height)
{
    double Ratio = (double)width / double(height);
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective (45.0f,Ratio,0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(x_cam,y_cam,z_cam,0.0,0.0,0.0,upX,upY,upZ);

    if(WireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		//Draw Our Mesh In Wireframe Mesh
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		//Toggle WIRE FRAME

    // your code here
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHT0);
    glColor3f(1.0,1.0,1.0);
    glTranslated(0,0,translate_z);
    glRotated(RotateX,1,0,0);
    glRotated(RotateY,0,1,0);
    glRotated(RotateZ,0,0,1);
    glDisable(GL_LIGHTING);    
  glPushMatrix();
   

    glScalef(20.5,10.0,20.0);
  glBindTexture(GL_TEXTURE_2D, tex[0]); 
  glBegin(GL_QUADS);
  //glNormalize3f(1.0,1.0,1.0);
    // Front Face
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  -1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  -1.0f,  1.0f);  // Top Left Of The Texture and Quad
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glBegin(GL_QUADS);
    //glNormalize3f(1.0,1.0,1.0);
// Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glBegin(GL_QUADS);
    //glNormalize3f(1.0,1.0,1.0);
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  -1.0f, -1.0f);  // Top Right Of The Texture and Quad
     glEnd();
     
    glBindTexture(GL_TEXTURE_2D, tex[3]);
    glBegin(GL_QUADS);
    //glNormalize3f(1.0,1.0,1.0);
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
     glEnd();
     
    glBindTexture(GL_TEXTURE_2D, tex[4]);
    glBegin(GL_QUADS);
    //glNormalize3f(1.0,1.0,1.0);
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  -1.0f,  1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, tex[5]);
    glBegin(GL_QUADS);
    //glNormalize3f(1.0,1.0,1.0);
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  -1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glEnd();

    glPopMatrix();
    glDisable(GL_LIGHT0);
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D); 
    glPushMatrix();
    //glColor3f(0.8,0.0,0.0);
    //glTranslated(0,0,translate_z);
    
    glBindTexture(GL_TEXTURE_2D,Map);
    glRotated(86.5,1,0,0);
    glRotated(-espin, 0, 0, 1);
    gluSphere(sphere,3.0,40,40);
    glPopMatrix(); 
       glDisable(GL_TEXTURE_2D);
    //glRotated(spin,0,1,0);
    //glLightfv(GL_LIGHT1, GL_POSITION, position);
       
         //glColor3f(1.0,1.0,1.0);
         //glRotated(spin,0,1,0);
       glEnable(GL_TEXTURE_2D); 
       glPushMatrix();
       glBindTexture(GL_TEXTURE_2D,moon_map);
          glTranslated(5.0,1.0,0.0);
         gluSphere(moon,0.8,20,20);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        //glLightfv()
        //glutSolidSphere(5)
        //glTranslated(20,15,20);    
        glRotated((GLdouble)spin,0.0,1.0,0.0);
            glLightfv(GL_LIGHT0,GL_POSITION,position);
            glTranslated(0.0,5.0,10.5);
            glDisable(GL_LIGHTING);
            glutWireCube(0.1);
            glEnable(GL_LIGHTING);
    glPopMatrix();

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
      case 'a':
	    translate_z += (float) 1.0f;
	     break;
	  case 'z':
	  translate_z -= (float) 1.0f;
		break;
        case 27 :
        case 'q':
            exit(0);
            break;

	  case 'w':
		WireFrame =!WireFrame;
	       break;	
    
        case 'f':
      
            if(status){
                glDisable(GL_FOG);
                status = false;
            }else {
                glEnable(GL_FOG);
                status = true;
            }
            break;
    }
    
}



void Specialkeys(int key, int x, int y)
{
     switch(key)
    {
     case GLUT_KEY_END:
	  translate_z += (float) 1.0f;
		break;
	case GLUT_KEY_HOME:
	  translate_z -= (float) 1.0f;
		break;
    case GLUT_KEY_UP:					/* Clockwise rotation over X */
	 	RotateX = ((int)RotateX +5)%360;
		break;
	case GLUT_KEY_DOWN:					/* Counter Clockwise rotation over X */
		RotateX = ((int)RotateX -5)%360;
		  break;
	case GLUT_KEY_LEFT:					/* Clockwise rotation over Y */
    	RotateY =((int)RotateY +5)%360;
		break;
	case GLUT_KEY_RIGHT:
		RotateY = ((int)RotateY -5)%360;/* Counter Clockwise rotation over Y */
		 break;

   }
  glutPostRedisplay();
}


void mouseMove(int x, int y){
    static float prevX = 0.0;
    static float prevY = 0.0;    
    
    prevX = (float)x - prevX;
    prevY = (float)y - prevY;

    if((abs((int)prevX) > 15)||abs((int)prevY)>15){
        prevX = (float)x;
        prevY = (float)y;
        return;
    }
    
    RotateX = ((int)RotateX + (int)prevY)%360;
    RotateY = ((int)RotateY + (int)prevX)%360;


}

static void idle(void)
{
      
    spin = ((int)spin + 2)%360;
    espin = ((int)espin + 1)%360;
    glutPostRedisplay();
}


static void init(void)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);


    glFogi(GL_FOG_MODE, fogMode[fogfilter]);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.05f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, -1.0f);
    glFogf(GL_FOG_END, -3.0f);

    glEnable(GL_FOG);    

     glEnable(GL_TEXTURE_2D);
     glGenTextures(6, tex);
     //glGenTextures(1,MAP);
     glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
     
     //glBindTexture(GL_TEXTURE_2D, tex[0]); // images are 2D arrays of pixels, bound to the GL_TEXTURE_2D target.
    
      int width, height; // width & height for the image reader
     text(tex[0],"./images/night_sky.jpeg");
     text(tex[1],"./images/night_sky.jpeg"); //back
     text(tex[3],"./images/night_sky.jpeg"); //top
     text(tex[2],"./images/night_sky.jpeg"); //bottom
     text(tex[4],"./images/night_sky.jpeg"); //right
     text(tex[5],"./images/night_sky.jpeg"); //left
     
     glGenTextures(1, &Map);
     text(Map,"./images/BigEarth.jpg");
     
     glGenTextures(1, &moon_map);
     text(moon_map, "./images/Moon.jpg");
     //unsigned char* image2;
     
     
     //image = SOIL_load_image("./images/crate.jpeg", &width, &height, 0, SOIL_LOAD_RGB);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    
    moon = gluNewQuadric();
    gluQuadricDrawStyle(moon, GLU_FILL);
    gluQuadricTexture(moon, GL_TRUE);
    gluQuadricNormals(moon, GLU_SMOOTH);
    
     x_cam = 0.0;
    y_cam = 0.0;
    z_cam = 30.0;
    
    upX = 0.0;
    upY = 1.0;
    upZ = 0.0;
    
    
}


/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(800,600);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);
    glutMotionFunc(mouseMove);
    
    glutIdleFunc(idle);
    glutMainLoop();

    return EXIT_SUCCESS;
}
