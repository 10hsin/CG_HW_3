#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "mesh.h"
using namespace std;

class scene
{
	public:
		// variable
		struct vertex
		{
			float x, y, z;
		};
		
		float	angle[100];
		vertex	rotation_axis_vector[100];
		vertex	scale_value[100];
		vertex	transfer_vector[100];
		
		int		obj_num;
		string	texture_name[100];
		GLuint	texture_obj[3];
		mesh	*object[100];
		
		// function
		scene();
		scene(int);
		~scene();
		void load_scene(int test_scene);
		unsigned int load_texture(const char* pszFile);
};