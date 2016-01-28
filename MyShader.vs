varying 	vec3 normal, light_dir, eye_dir, out_tangent, out_bitengent;
attribute 	vec3 in_tangent;

void main()
{	
	//**************************
	// Normal Map
	//**************************
	normal 			= gl_NormalMatrix * gl_Normal;
	out_tangent		= gl_NormalMatrix * in_tangent;
	out_bitengent 	= cross(normal , out_tangent);
	
	vec3 vVertex 	= vec3(gl_ModelViewMatrix * gl_Vertex);
	light_dir 		= vec3(gl_LightSource[0].position.xyz - vVertex);
	eye_dir			= -vVertex;
	
	//**************************
	// Texture
	//**************************
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
	gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
	gl_TexCoord[2].xy = gl_MultiTexCoord2.xy;
	
	gl_Position = ftransform();	
	
}