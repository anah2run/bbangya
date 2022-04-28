#pragma once
#include "cgmath.h"
#include "cgut.h"

float random_range( float min, float max ){
	return mix( min, max, rand()/float(RAND_MAX)); 
}
static const float life_factor = 0.001f;
static const float alpha_factor = 0.001f;

struct particle_t
{
	vec2 position;
	vec4 color;
	vec2 velocity;
	float scale;
	float life;
	bool alive;

	//optional
	float elapsed_time;
	float time_interval;

	void dust(vec2 pos);
	void blood(vec2 pos);
	void bomb(vec2 pos);
	void box(vec2 pos);
	void fail(vec2 pos);
	void update(float t);
};

inline void particle_t::dust(vec2 pos)
{
	position = pos;
	color = vec4(0.5f, 0.5f, 0.5f,1);
	scale = random_range(0.01f, 0.1f);
	life = random_range(0.01f, 1.0f);
	velocity = vec2(random_range(-3.0f, 3.0f), random_range(0.0f, 4.0f)) * 0.003f;
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	alive = true;
}

inline void particle_t::blood(vec2 pos)
{
	position = pos;
	color = vec4(1.0f, 0.0f, 0.0f, 1);
	scale = random_range(0.05f, 0.1f);
	life = random_range(0.01f, 1.0f);
	velocity = vec2(random_range(-5.0f, 5.0f), random_range(-5.0f, 5.0f)) * 0.003f;
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	alive = true;
}

inline void particle_t::bomb(vec2 pos)
{
	position = pos;
	color = vec4(1.0f, 0.0f, 0.0f, 1);
	scale = random_range(0.07f, 0.1f);
	life = random_range(0.01f, 1.0f);
	velocity = vec2(random_range(-20.0f, 20.0f), random_range(-20.0f, 20.0f)) * 0.003f;
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	alive = true;
}
inline void particle_t::box(vec2 pos)
{
	position = pos;
	color = vec4(1.0f, 1.0f, 1.0f, 1);
	scale = random_range(0.05f, 0.2f);
	life = random_range(0.01f, 1.0f);
	velocity = vec2(random_range(-10.0f, 10.0f), random_range(-10.0f, 10.0f)) * 0.003f;
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	alive = true;
}

inline void particle_t::fail(vec2 pos)
{
	position = pos;
	color = vec4(1.0f, 0.7f, 0.0f, 1);
	scale = random_range(0.1f, 0.5f);
	life = random_range(2, 3);
	velocity = vec2(random_range(-15.0f, 10.0f), random_range(-15.0f, 10.0f)) * 0.003f;
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	alive = true;
}
inline void particle_t::update(float t)
{
	if (elapsed_time > time_interval)
	{
		const float theta = random_range(0, 1.0f) * PI * 2.0f;
		constexpr float velocity_factor = 0.003f;
		velocity = vec2(cos(theta), sin(theta)) * velocity_factor;

		elapsed_time = 0.0f;
	}

	position += velocity;

	life -= life_factor * 30.0f;
		
	// disappear
	if (life < 0.0f) 
	{ 
		constexpr float alpha_factor = 0.001f;
		color.a -= alpha_factor * 30.0f;
	}

	// dead
	if (color.a < 0.0f) alive = false;
}


class Particle {
public:
	std::list<particle_t> particles;
	Particle() {

	}
	Particle(int particle_mode, vec2 pos) {
		int num = 0;
		switch (particle_mode) {
		case 1: // bomb
			num = 100;
			break;
		default:
			num = 50;
			break;
		}
		particle_t temp_particle = {};
	
		for (int i = 0; i < num; i++) {
			switch (particle_mode) {
			case 4: // fail
				temp_particle.fail(pos);
				break;
			case 3: // break box
				temp_particle.box(pos);
				break;
			case 2: // dust
				temp_particle.dust(pos);
				break;
			case 1: // bomb
				temp_particle.bomb(pos);
				break;
			default: // blood
				temp_particle.blood(pos);
				break;
			}
			particles.push_back(temp_particle);
		}
	}
	void updates(float t) {
		for (std::list<particle_t>::iterator it = particles.begin(); it != particles.end(); it++) {
			it->update(t);
			if(! it->alive)it = particles.erase(it);
		}
	}
};
std::list<Particle> particles_list;