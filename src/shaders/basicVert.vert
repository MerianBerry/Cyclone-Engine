//we will be using glsl version 4.5 syntax
#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

layout( push_constant ) uniform constants
{
	vec4 data;
	mat4 renderMatrix;
} PushConstants;

void main()
{
	//output the position of each vertex
	gl_Position = PushConstants.renderMatrix * vec4(vPosition, 1.f);
	outColor = vColor;
}