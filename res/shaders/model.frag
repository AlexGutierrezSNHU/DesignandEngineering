version 330 core\
	
in vec3 oColor;
in vec2 oTexCoord;
in vec3 oNormal;
in vec3 FragPos;
	
out vec4 fragColor;

uniform sampler2D myTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	//Ambient scene lighting Strength
	float ambientStrength = 5.0f;
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse Lighting
	vec3 norm =  normalize(oNormal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.1);
	vec3 diffuse = diff * lightColor;
		
	//Specular Strength
	float specularStrength = 3.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.1), 128);
	vec3 specular = specularStrength * spec * lightColor;
	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = texture(myTexture, oTexCoord)* vec4(result, 1.0f);

};