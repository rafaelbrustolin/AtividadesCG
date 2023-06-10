#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 texc;

out vec4 finalColor;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	//...pode ter mais linhas de c�digo aqui!
	gl_Position = projection * model * vec4(position, 1.0);
	finalColor = vec4(vertex_color, 1.0);

	fragPos = vec3(model * vec4(position, 1.0));
	texCoord = vec2(texc.x,1-texc.y);
}