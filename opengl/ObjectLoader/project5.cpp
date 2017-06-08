/** Joseph Vietti 
  * Loading ateneam statues from wavefront object file
  * In the init I call loadObject with the file and vectors address then
  * Using stringstreams and vectors I read the file and find the 
  * corresponding prefix for each vector to hold the corresponding values:
  * vertex, normals, faces(indices). Each value is stored in a vector of structure 
  * VertexXYZ - vertex and normals, indicesVTN for the faces(indices)
  * vertex and normals : 3 values for the vertex of the trinagle
  * Faces formatted to: vertex/textures/normals in sets of 3
  * Using this I parse for the prefix and take each value to the structure from white space in vertex and normals
  * as well as faces checking for '/' and storing values accordingly, form vvvv.../ttt.../nn..., making sure I capture 
  * it all and check for empty strings, such as vv..//z..
  
  * In this I commented out the trace values after the loadObject is called
  * The solution to this lab was an exntension of the last one in which if vt is ever in
  * the file then I account for the vt to be present for the indices f  = v/t/n.
  
  * The texture vertices from vt are captured and a check is used to dentoe that the face will account
  * for the vt in the format v/t/n, as well as capture them as x y or x y z when parsing the string.
  
  * Then I produce the model by creating it by the for loop using GL_TRIANGLES
  * In this case the mode is very large so I scaled it to a 1000th of the model size to fit in the skybox and viewport.
      
  To Compile: g++ -std=c++11 -o prog project5.cpp -lGL -lGLU -lglut -lSOIL
  To Run: ./prog

**/

#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>
#include <fstream>
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

GLuint tex[6]; // Texture pointer
 
float RotateX =0;
float RotateY =0;
float RotateZ =0;
float translate_z =-1;
float objX, objY, objZ =-1;
float spin;

float x_cam, y_cam, z_cam;
GLdouble upX, upY, upZ; //camera, GL_LOOKAT

float position[] = {0.0,0.0,1.5}; //fr lighting posiiton

typedef struct {
    float x; //coordinates
    float y;
    float z;
} vertXYZ;

typedef struct {
    float v;
    float t;
    float n;
} indicesVN;

//Vectors from Structures
    vector<vertXYZ> vert;
    vector <vertXYZ> norm;
    vector <vertXYZ> vtext;
    vector <indicesVN> indices;


 
static void text(int i, const char* filePath){
unsigned char* image;
    int width, height;
     glBindTexture(GL_TEXTURE_2D, tex[i]); // images are 2D arrays of pixels, bound to the GL_TEXTURE_2D target.
      
     image = SOIL_load_image(filePath, &width, &height, 0, SOIL_LOAD_RGB);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
     
    // binding image data
     SOIL_free_image_data(image);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 
}
 

//Object Loader
void loadObject (const char *filename, vector<vertXYZ> &vert, vector<vertXYZ> &norm, vector<vertXYZ> &text, vector<indicesVN> &indices)
{  int textC = 0; //texture checker for if 
    //stringstream stream, prestream;
    ifstream in(filename, ios::in);
    if(!in){
        cerr << "File doesn't exist or cannot be opened'"<< filename << endl; exit(1);
    }
    //string vert_delimiter = "v";
    //string face_delimiter = "f";
    //string norm_delimiter = "vn";
    string line;
    //Begin Loop
    int count = 0;
    while(getline(in, line)){
    stringstream stream;
        string prefix;
        string x_str, y_str, z_str, res; //VertexXYZ vectors
        string v, t, n; //Indices vector
        stream.str(line);
        stream >> prefix;
        //cout<< prefix<<endl;
      
        if(prefix == "v"){ //vertex case
            float x, y, z;
            
            stream >> x_str; 
            stream >> y_str;
            stream >> z_str;
            //cout<< x_str;
            vertXYZ vertCoord;
            vertCoord.x = stof(x_str);
            vertCoord.y = stof(y_str);
            vertCoord.z = stof(z_str);
            vert.push_back(vertCoord);               
        }
        else if(prefix == "vn"){ //vertex normal
            stream >> x_str; 
            stream >> y_str;
            stream >> z_str;
            //cout<< x_str;
            vertXYZ normCoord;
            normCoord.x = stof(x_str);
            normCoord.y = stof(y_str);
            normCoord.z = stof(z_str);
            norm.push_back(normCoord);         
        }
        else if (prefix == "vt"){ //vertex texture check
            string tempText[3] = {" ", " ", " "};
            int i = 0;
            while( i !=3 && stream >> tempText[i]){ //while there is values to get and room for it (x y z)
                i++;
            }
            //cout<< x_str;
            vertXYZ textCoord;
            if(tempText[2] != " "){ // x y z case
                textCoord.x = stof(tempText[0]);
                textCoord.y = stof(tempText[1]);
                textCoord.z = stof(tempText[2]);
                text.push_back(textCoord);  
            }
            else{ // x y case
                textCoord.x = stof(tempText[0]);
                textCoord.y = stof(tempText[1]);
                text.push_back(textCoord);
            }
                 
            textC = 1; //texture to be expected in faces
        }
        else if(prefix == "f"){
        //split the stream after each '/'
        string delim = "/";
        string s;
        string coord[2];
       
        stringstream stream1;
        while(stream){
            if(!getline(stream, s, '/')){
                s = " ";
            }
           // cout<<s<<endl;
            res.append(" ");
            res.append(s);
        }
          //cout<<res<<endl;
          
        if(textC == 0){ //No vt present
            indicesVN indCord;
             int i = 0; 
             string in;
                stream1.str(res);
                //cout<<res<<endl;
                while(stream1){
                    stream1 >> in;
                     //cout<<in << "   ";
                    if(i!=0 && i%2 == 0){
                        // cout<<endl;
                         //cout<< indCord.v << indCord.n <<endl;
                        indices.push_back(indCord);
                    }
                    if(i%2 == 0){
                        indCord.v = stoi(in);
                    }
                    else if(i%2 == 1){
                         indCord.n = stoi(in);
                    }
                    i++;
                }
                
        
        }
        else{ //Vertex Texture exists case
        //cout<<textC;
            indicesVN indCord;
             int i = 0; 
             string in;
                stream1.str(res);
                //cout<<res<<endl;
                while(stream1){
                    stream1 >> in;
                     //cout<<in << "   ";
                    if(i!=0 && i%3 == 0){
                        // cout<<endl;
                         //cout<< indCord.v << indCord.n <<endl;
                        indices.push_back(indCord);
                    }
                    if(i%3 == 0){
                        indCord.v = stoi(in);
                    }
                    else if(i%3 == 1){
                        indCord.t = stoi(in);
                    }
                    else if(i%3 == 2){
                         indCord.n = stoi(in);
                    }
                    i++;
                }
                
            }
        }
        else{
        //ignore #, and any other prefix besided wavefront specific prefixes
        }
        count++;
     
    }//end of while
    
    //cout<<vertices.size()<<endl;
   // cout<<count<<endl;
   
} //end of Object Loader

/* GLUT callback Handlers */

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
    
    //Texture Box:
     glEnable(GL_TEXTURE_2D);
     
    glColor3f(1.0,1.0,1.0);
    glTranslated(0,0,translate_z);
   // glRotated(RotateX,1,0,0);
    //glRotated(RotateY,0,1,0);
    //glRotated(RotateZ,0,0,1);
     
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
 
    glDisable(GL_TEXTURE_2D);
     
    glRotated(RotateX,1,0,0); //Rotation Initialization
    glRotated(RotateY,0,1,0);
    glRotated(RotateZ,0,0,1);
    glColor3f(1.0f, 0.0f, 0.0f);
    //glTranslated(objX,objY, objZ);
    glScalef(0.001f, 0.001f, 0.001f);
    glPushMatrix();
    glBegin(GL_TRIANGLES);

//Loop through 36 times as a cube has 6 sides * 2 triangles * 3 points each and 3 connections each vertice
for(int i=0; i< indices.size() ; i+=3){
	//Use the Vertices and Normals with the Corresponding Indices, -1 due to incorrect data points should be 0-7
	//glColor3f(vert[indices[i].v-1].x, vert[indices[i].v-1].y, vert[indices[i].v-1].z); //use vertices indices
	glNormal3f(norm[indices[i].n-1].x, norm[indices[i].n-1].y, norm[indices[i].n-1].z); //use normals indices
	glVertex3f(vert[indices[i].v-1].x, vert[indices[i].v-1].y, vert[indices[i].v-1].z); //use vertices indices
   
	//glColor3f(vert[indices[i+1].v-1].x, vert[indices[i+1].v-1].y, vert[indices[i+1].v-1].z);
	glNormal3f(norm[indices[i+1].n-1].x, norm[indices[i+1].n-1].y, norm[indices[i+1].n-1].z);
	glVertex3f(vert[indices[i+1].v-1].x, vert[indices[i+1].v-1].y, vert[indices[i+1].v-1].z);

	//glColor3f(vert[indices[i+2].v-1].x, vert[indices[i+2].v-1].y, vert[indices[i+2].v-1].z);
	glNormal3f(norm[indices[i+2].n-1].x, norm[indices[i+2].n-1].y, norm[indices[i+2].n-1].z);
	glVertex3f(vert[indices[i+2].v-1].x, vert[indices[i+2].v-1].y, vert[indices[i+2].v-1].z);
	
}

	glEnd();
    glPopMatrix();

    glutSwapBuffers();
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
 
 
void resetCord(){
   x_cam = 0.0;
    y_cam = 0.0;
    z_cam = 30.0;
     
    upX = 0.0;
    upY = 1.0;
    upZ = 0.0;
 
}
 
 
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
      case 'a':
        objZ += (float) 1.0f;
         break;
      case 'z':
      objZ -= (float) 1.0f;
        break;
    case 'o':
        objX += (float) 1.0f;
         break;
      case 'p':
      objX -= (float) 1.0f;
        break;
        case 27 :
        case 'q':
            exit(0);
            break;
 
 
      case 'w':
        WireFrame =!WireFrame;
           break;   
     
     case '1':
        x_cam += 0.5;
        break;
    case '2':
        x_cam += -0.5; 
        break;
    case '3':
        y_cam += 0.5;
        break;
    case '4':
        y_cam += -0.5;
        break;
    case '5':
        z_cam += 0.5;
        break;
    case '6':
        z_cam += -0.5;
        break;
    case '0':
        resetCord();
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
    case GLUT_KEY_UP:                   /* Clockwise rotation over X */
        RotateX = ((int)RotateX +5)%360;
        break;
    case GLUT_KEY_DOWN:                 /* Counter Clockwise rotation over X */
        RotateX = ((int)RotateX -5)%360;
          break;
    case GLUT_KEY_LEFT:                 /* Clockwise rotation over Y */
        RotateY =((int)RotateY +5)%360;
        break;
    case GLUT_KEY_RIGHT:
        RotateY = ((int)RotateY -5)%360;/* Counter Clockwise rotation over Y */
         break;
   }
  glutPostRedisplay();
}
static void idle(void)
{
    glutPostRedisplay();
}

static void init(void)
{
    //Load the Object
    loadObject("ateneam.obj", vert, norm, vtext,indices); //Load Object file into vectors
    /*cout<< "Vertices"<<endl;
    for(int i=0; i<vert.size(); i++){ //test vectors
        cout<< vert[i].x <<" "<< vert[i].y << " " << vert[i].z<<endl;
    }
    cout<<"Normals"<<endl;
     for(int i=0; i<norm.size(); i++){ //test vectors
        cout<< norm[i].x <<" "<< norm[i].y << " " << norm[i].z<<endl;
    }
    cout<< "Faces" <<endl;
  for(int i=0; i<indices.size(); i++){ //test vectors
        cout<< indices[i].v <<" "<<indices[i].t << " "<< indices[i].n <<endl;
    }
    */
     glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
 
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
 
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
 
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
 
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
 
 
   
     glGenTextures(6, tex);
     glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      
     //glBindTexture(GL_TEXTURE_2D, tex[0]); // images are 2D arrays of pixels, bound to the GL_TEXTURE_2D target.
     
     int width, height; // width & height for the image reader
     text(0,"./images/wall.jpeg");
     text(1,"./images/wall.jpeg"); //back
     text(3,"./images/roof.jpeg"); //top
     text(2,"./images/floor.jpeg"); //bottom
     text(4,"./images/wall.jpeg"); //right
     text(5,"./images/wall.jpeg"); //left
     //unsigned char* image2;
      
     //image = SOIL_load_image("./images/crate.jpeg", &width, &height, 0, SOIL_LOAD_RGB);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
     
    x_cam = 0.0;
    y_cam = 0.0;
    z_cam = 30.0;
     
    upX = 0.0;
    upY = 1.0;
    upZ = 0.0;
    
     glClearColor(1.0f, 1.0f, 1.0f,0.5f);
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
