#include "ShaderLoader.h"
#include "mesh.h"
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "view.h"
#include "scene.h"
#include "light.h"
#include "Vector3D.h"

#define TESTSCENE 3

using namespace std;
using namespace MathTool;

GLhandleARB MyShader;
GLint		location;

void	display();
void	reshape(GLsizei, GLsizei);
void	keyboard(unsigned char, int, int);
void	LoadShaders();

int		obj_id = 0;
int		x_pos = -1, y_pos = -1;
int		windowSize[2];
float	flag = 0;


scene	s(TESTSCENE);
view	v(TESTSCENE);
light	l(TESTSCENE);

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitWindowSize(v.viewport_width, v.viewport_height);
	glutInitWindowPosition(v.viewport_x, v.viewport_y);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("HW3");

	GLenum glew_error;
	if ((glew_error = glewInit()) != GLEW_OK)
		return -1;

	FreeImage_Initialise();
	for (int i = 0; i<3; i++)
		s.texture_obj[i] = s.load_texture(s.texture_name[i].c_str());
	FreeImage_DeInitialise();
	
	glewInit();
	LoadShaders();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}


void display()
{
	//**************************
	// initialize
	//**************************
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      
	glClearDepth(1.0f);                        
	glEnable(GL_DEPTH_TEST);                   
	glDepthFunc(GL_LEQUAL);                    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(v.viewport_x, v.viewport_y, windowSize[0], windowSize[1]);

	//**************************
	// projection transformation
	//**************************
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(v.fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v.dnear, v.dfar);

	//**************************
	// view & model transformation
	//**************************
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	v.eye.x,	v.eye.y,	v.eye.z,	// eye
				v.vat.x,	v.vat.y,	v.vat.z,	// center
				v.vup.x,	v.vup.y,	v.vup.z);	// up	

	l.load_normal();
	glUseProgram(MyShader);

	//**************************
	// load texture
	// texture 0 : Normal map
	// texture 1 : diffuse map
	// texture 2 : specular map
	//**************************

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s.texture_obj[0]);
	location = glGetUniformLocation(MyShader, "normal_texture");
	if (location == -1)
		cout << "normal_texture not found" << endl;
	else 
		glUniform1i(location, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, s.texture_obj[1]);
	location = glGetUniformLocation(MyShader, "diffuse_texture");
	if (location == -1)
		cout << "diffuse_texture not found" << endl;
	else 
		glUniform1i(location, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, s.texture_obj[2]);
	location = glGetUniformLocation(MyShader, "specular_exture");
	if (location == -1) 
		cout << "specular_exture not found" << endl;
	else 
		glUniform1i(location, 2);

	//**************************
	// draw object
	//**************************
	for (int t = 0, k = 0; k < s.obj_num; k++)
	{
		glPushMatrix();//儲存現在的矩陣 (目前是modelview)
		glTranslatef(s.transfer_vector[k].x, s.transfer_vector[k].y, s.transfer_vector[k].z);
		glRotatef(s.angle[k], s.rotation_axis_vector[k].x, s.rotation_axis_vector[k].y, s.rotation_axis_vector[k].z);
		glScalef(s.scale_value[k].x, s.scale_value[k].y, s.scale_value[k].z);

		int lastMaterial = -1;

		for (int i = 0; i < s.object[k]->fTotal; i++)
		{
			// set material property if this face used different material
			if (lastMaterial != s.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s.object[k]->mList[lastMaterial].Ns);

				//you can obtain the texture name by object[k]->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}

			//**************************
			// compute tangent
			//**************************
			Vector3D P[3], UV[3], P_edge[2], UV_edge[2], tangent;
			float	 cp;

			for (int j = 0; j < 3; j++)
			{
				P[j] = s.object[k]->vList[s.object[k]->faceList[i][j].v].ptr;
				UV[j] = s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr;
			}

			P_edge[0] = P[1] - P[0];
			P_edge[1] = P[2] - P[1];
			UV_edge[0] = UV[1] - UV[0];
			UV_edge[1] = UV[2] - UV[1];
			cp = (UV_edge[0].x * UV_edge[1].y) - (UV_edge[1].x * UV_edge[0].y);

			if (cp != 0.0f)
			{
				float mul = 1.0f / cp;
				tangent = (P_edge[0] * UV_edge[1].y + P_edge[1] * -UV_edge[0].y) * mul;
			}

			location = glGetAttribLocation(MyShader, "in_tangent");
			if (location == -1)
				cout << "in_tangent not found" << endl;
			else 
				glVertexAttrib3f(location, tangent.x, tangent.y, tangent.z);


			glBegin(GL_POLYGON);
			for (int j = 0; j < 3; j++)
			{
				glMultiTexCoord2fv(GL_TEXTURE0,	s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
				glMultiTexCoord2fv(GL_TEXTURE1, s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
				glMultiTexCoord2fv(GL_TEXTURE2, s.object[k]->tList[s.object[k]->faceList[i][j].t].ptr);
				glNormal3fv(s.object[k]->nList[s.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s.object[k]->vList[s.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}

		glPopMatrix();//拿stack中的矩陣到指定矩陣(modelview),會回到 I * ModelView
	}

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y)
{
	float x_rotate, y_rotate , z_rotate;
	switch (key)
	{
	case 'w':
		v.fovy += 1;
		break;

	case 's':
		v.fovy -= 1;
		break;

	case 'd':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(-0.1) + z_rotate*sin(-0.1);
		v.eye.z = z_rotate*cos(-0.1) - x_rotate*sin(-0.1);
		break;

	case 'a':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(0.1) + z_rotate*sin(0.1);
		v.eye.z = z_rotate*cos(0.1) - x_rotate*sin(0.1);
		break;

	case 'q':
		x_rotate = l.light_position[0];
		z_rotate = l.light_position[2];
		l.light_position[0] = x_rotate*cos(-0.1) + z_rotate*sin(-0.1);
		l.light_position[2] = z_rotate*cos(-0.1) - x_rotate*sin(-0.1);
		break;

	case 'e':
		x_rotate = l.light_position[0];
		z_rotate = l.light_position[2];
		l.light_position[0] = x_rotate*cos(0.1) + z_rotate*sin(0.1);
		l.light_position[2] = z_rotate*cos(0.1) - x_rotate*sin(0.1);
		break;

	case '1':
		y_rotate = v.eye.y - v.vat.y;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.y = y_rotate*cos(0.5) - z_rotate*sin(0.5);
		v.eye.z = z_rotate*cos(0.5) + y_rotate*sin(0.5);
		break;

	case '2':
		y_rotate = v.eye.y - v.vat.y;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.y = y_rotate*cos(-0.5) - z_rotate*sin(-0.5);
		v.eye.z = z_rotate*cos(-0.5) + y_rotate*sin(-0.5);
		break;
	
	default:
		break;
	}

	glutPostRedisplay();
}

void LoadShaders()
{
	MyShader = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "MyShader.vs", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "MyShader.fs", GL_FRAGMENT_SHADER);
	}
}
