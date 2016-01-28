#include "scene.h"
#include <iostream>

scene::scene()
{
	obj_num = 0;
}

scene::scene(int TestScene)
{
	obj_num = 0;
	load_scene(TestScene);
}

scene::~scene()
{
	
}

void scene::load_scene(int test_scene)
{
	ifstream 		ifs;
	string			token;

	if (test_scene == 1)
		ifs.open("Scene1.scene");
	else if (test_scene == 2)
		ifs.open("Scene2.scene");
	else if (test_scene == 3)
		ifs.open("Scene3.scene");
	else
		cout << "File not found";


	while (ifs >> token)
	{
		if (token == "model")
		{
			ifs >> token;
			object[obj_num] = new mesh(token.c_str());

			ifs >> scale_value[obj_num].x >> scale_value[obj_num].y >> scale_value[obj_num].z;
			ifs >> angle[obj_num] >> rotation_axis_vector[obj_num].x >> rotation_axis_vector[obj_num].y >> rotation_axis_vector[obj_num].z;
			ifs >> transfer_vector[obj_num].x >> transfer_vector[obj_num].y >> transfer_vector[obj_num].z;
			
			obj_num++;
		}

		else if (token == "shader_textures")
		{
			for (int i = 0; i < 3; i++)
				ifs >> texture_name[i];
		}
	}

	ifs.close();
	
}

unsigned int scene::load_texture(const char* pszFile)
{
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pszFile, 0), pszFile);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	unsigned int iTextureId;
	glGenTextures(1, &iTextureId);
	glBindTexture(GL_TEXTURE_2D, iTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return iTextureId;
}
