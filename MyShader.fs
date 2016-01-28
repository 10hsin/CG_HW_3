varying vec3 normal, light_dir, eye_dir ,out_tangent, out_bitengent;
uniform sampler2D normal_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_exture;

void main (void)
{
	//**************************
	// Texture
	//**************************
	vec3 normal_map 	= texture2D(normal_texture, gl_TexCoord[0].xy).rgb;
	vec4 diffuse_map 	= texture2D(diffuse_texture, gl_TexCoord[1].xy).rgba;
	vec4 specular_map 	= texture2D(specular_exture, gl_TexCoord[2].xy).rgba;
	
	//**************************
	// Normal Map
	//**************************
	
	vec3 normalized_normal_map = normalize(normal_map*2.0 - 1.0);
	vec3 N = normalize(normal);
	vec3 T = normalize(out_tangent);
	vec3 B = normalize(out_bitengent);
	
	vec3 final_normal = (normalized_normal_map.r) *T + 
						(normalized_normal_map.g) *B + 
						(normalized_normal_map.b) *N;
						
	//**************************
	// Phong shading
	// ambient + diffuse + specular
	//**************************

	vec4 final_color = 
	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
	(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
	vec3 L = normalize(light_dir);
	float lambertTerm = dot(final_normal,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += 	gl_LightSource[0].diffuse * 
						diffuse_map * 
						lambertTerm;	
		
		vec3 R = reflect(-L, final_normal);
		vec3 E = normalize(eye_dir);
		
		float specular = pow(	max(dot(R, E), 0.0), 
								gl_FrontMaterial.shininess );
							
		final_color += 	gl_LightSource[0].specular * 
						specular_map * 
						specular;	
	}
	
	gl_FragColor = final_color;	
}