//we will be using glsl version 4.5 syntax
#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

void main()
{
	//const array of positions for the triangle
	const vec3 positions[3] = vec3[3](
		vec3(1.f, 1.f, 0.f),
		vec3(-1.f, 1.f, 0.f),
		vec3(0.f, -1.f, 0.f)
	);

	const vec3 colors[3] = vec3[3](
		vec3(1.f, 0.f, 0.f), //red
		vec3(0.f, 1.f, 0.f), //green
		vec3(0.f, 0.f, 1.f)  //blue
	);

	//output the position of each vertex
	gl_Position = vec4(vPosition, 1.f);
	outColor = vColor;
}