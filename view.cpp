#include "view.h"
#include <iostream>
#include <fstream>
#include <string>

view::view()
{

}

view::view(int TestScene)
{
	load_view(TestScene);
}

view::~view()
{
	
}

void view::load_view(int test_scene)
{
	ifstream ifs;
	string	token;

	if (test_scene == 1)
		ifs.open("Scene1.view");
	else if (test_scene == 2)
		ifs.open("Scene2.view");
	else if (test_scene == 3)
		ifs.open("Scene3.view");
	else
		cout << "File not found";

	ifs >> token >> eye.x >> eye.y >> eye.z;
	ifs >> token >> vat.x >> vat.y >> vat.z;
	ifs >> token >> vup.x >> vup.y >> vup.z;
	ifs >> token >> fovy;
	ifs >> token >> dnear;
	ifs >> token >> dfar;
	ifs >> token >> viewport_x >> viewport_y >> viewport_width >> viewport_height;

	ifs.close();
	
}