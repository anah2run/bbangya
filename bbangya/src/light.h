#pragma once
uint		NUM_LIGHT = 9;
vec4		lights_position[10] = { vec4(5.0f,5.f,2.f,1.f),vec4(15.f,5.f,2.f,1.f) , vec4(25.f,5.f,2.f,1.f), vec4(35.f,5.f,2.f,1.f), vec4(45.f,5.f,2.f,1.f),
								vec4(55.0f,5.f,2.f,1.f),vec4(65.f,5.f,2.f,1.f), vec4(70.f,5.f,2.f,1.f), vec4(85.f,5.f,2.f,1.f), vec4(95.f,5.f,2.f,1.f) };

struct light_t
{
	vec4	position = vec4(25.0f, 25.0f, 1000.0f, 1.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.7f, 0.7f, 0.5f, 1.0f);
	vec4	specular = vec4(0.3f, 0.3f, 0.1f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(0.6f, 0.6f, 0.3f, 1.0f);
	float	shininess = 1000.0f;
};

std::vector<light_t> create_lights(uint N) {
	std::vector<light_t> v;
	light_t light_m;
	light_m.ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	light_m.diffuse = vec4(0.8f, 0.6f, 0.4f, 1.0f);
	light_m.specular = vec4(1.0f, 0.80f, 0.5f, 1.0f);
	for (uint i = 0; i < N; i++) {
		light_m.position = lights_position[i];
		v.push_back(light_m);
	}
	return v;
}
light_t		sunlight = { {.4f,1,1,0},{0.5f,0.5f,0.5f,1},{0.7f,0.7f,0.5f,1},{0.3f,0.3f,0.1f,1} };
std::vector<light_t> lights = create_lights(NUM_LIGHT);
