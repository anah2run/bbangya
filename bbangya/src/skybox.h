#pragma once
static const char* skybox_image_path = "../bin/textures/at-the-river.jpg";
uint				NUM_TESS = 40;
std::vector<uint> skybox_indices;
std::vector<vertex> create_skybox_vertices(uint N)
{
	std::vector<vertex> v;
	for (uint h = 0; h <= N; h++) {
		float t1 = PI * h / float(N), ch = cos(t1), sh = sin(t1);
		for (uint w = 0; w <= N; w++) {
			float t2 = 2.f * PI * w / float(N), cw = cos(t2), sw = sin(t2);
			v.push_back({ vec3(sh * cw,sh * sw,ch),vec3(-sh * cw,-sh * sw,-ch) ,vec2(t2 / (PI * 2.0f),1 - t1 / PI) });
		}
	}
	for (uint k = 0; k < N * (N + 1); k++)
	{
		skybox_indices.push_back(k);
		skybox_indices.push_back(k + N + 1);
		skybox_indices.push_back(k + N);

		skybox_indices.push_back(k);
		skybox_indices.push_back(k + 1);
		skybox_indices.push_back(k + N + 1);
	}
	return v;
}