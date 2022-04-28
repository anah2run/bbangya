// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 aspect_matrix;
uniform mat4 orthogonal_projection_matrix;

out vec4 epos;	// eye-space position
out vec3 norm;
out vec2 tc;

uniform int	UI_mode;

void main()
{
	if(UI_mode == 0)
	{
		vec4 wpos = model_matrix * vec4(position,1);
		epos = view_matrix * wpos;
		gl_Position = aspect_matrix * projection_matrix * epos;

		// pass eye-coordinate normal to fragment shader
		norm = normalize(mat3(view_matrix*model_matrix)*normal);
		tc = texcoord;
	}
	else
	{
		epos = model_matrix * vec4(position,1);
		gl_Position = orthogonal_projection_matrix * epos;

		norm = normal;
		tc = texcoord;
	}
}
