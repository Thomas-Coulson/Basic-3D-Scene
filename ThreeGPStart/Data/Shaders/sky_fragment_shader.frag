#version 330

//uniform sampler2D sampler_tex;

uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;

in vec3 varying_normals;
in vec2 varying_texCoord;

out vec4 fragment_colour;

void main(void)
{
	vec3 normals = normalize(varying_normals);

	vec3 tex_colour = texture(sampler_tex, varying_texCoord).rgb;

	fragment_colour = vec4(tex_colour * 0.7 ,1.0);
}