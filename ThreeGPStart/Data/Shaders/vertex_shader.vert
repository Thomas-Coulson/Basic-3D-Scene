#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;


layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_normals;
layout (location=2) in vec2 texCoords;

out vec3 varying_normals;
out vec3 varying_position;
out vec2 varying_texCoord;

void main(void)
{	
	varying_position = vertex_position;

	varying_normals = vertex_normals;

	varying_texCoord = texCoords;

	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}