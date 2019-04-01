#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out vec3 oColor;
out vec2 oTexCoord;
out vec3 oNormal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
	oColor = aColor;
	oTexCoord = texCoord;
	oNormal = mat3(transpose(inverse(model)))* normal;
	FragPos = vec3(model * vec4 (vPosition, 1.0f));

};