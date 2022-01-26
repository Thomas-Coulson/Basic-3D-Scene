#version 330

//uniform sampler2D sampler_tex;

uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;

in vec3 varying_position;
in vec3 varying_normals;
in vec2 varying_texCoord;

out vec4 fragment_colour;

void main(void)
{
	vec3 normals = normalize(varying_normals);

	vec3 tex_colour = texture(sampler_tex, varying_texCoord).rgb;

	vec3 point_light_pos = vec3(100, 20, -400);

	vec3 light_direction = vec3(0, -0.5, -5);
	vec3 point_light_direction = point_light_pos - varying_position;

	vec3 dir_light = normalize(-light_direction);
	vec3 point_light = normalize(point_light_direction);

	float dir_intensity = max(0, dot(dir_light, normals));
	float point_intesnity = max(0, dot(point_light, normals));

	vec3 ambient_light = vec3(0.05);



	vec3 result = ambient_light + tex_colour * (dir_intensity + point_intesnity);

	fragment_colour = vec4(result, 1.0);
}