#define STB_IMAGE_IMPLEMENTATION
#define BASE_CAM_Y 7.0f
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "item.h"
#include "map.h"
#include "character.h"
#include "gun.h"
#include "sprite.h"
#include "light.h"
#include "skybox.h"
#include "sound.h"
#include "particle.h"
#include<string>
#include<iostream>
//*************************************
// global constants

bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);
float	a = 0.0f;

static const char*	window_name = "bbangya";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 5, BASE_CAM_Y, 20 );
	vec3	at = vec3( 5, BASE_CAM_Y, 0 );
	vec3	up = vec3( 0, 1, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/4; // must be in radian
	float	dnear = 1.0f;
	float	dfar = 1000.0f;

	mat4	projection_matrix;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size
float	aspect = window_size.x / float(window_size.y);
mat4	aspect_matrix() {

	return {
	   std::min(aspect / 1.5f,1.0f), 0, 0, 0,
	   0, std::min(aspect / 1.5f,1.0f), 0, 0,
	   0, 0, 1, 0,
	   0, 0, 0, 1
	};
}
mat4	ortho(float l, float r, float t, float b, float n, float f) { 
	return mat4::scale(2 / (r - l), 2 / (t - b), 2 / (n - f)) * mat4::translate(-(l + r) / 2, -(b + t) / 2, (n + f) / 2);
};

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	sprite_vertex_array = 0;
GLuint	cube_vertex_array = 0;
GLuint	circle_vertex_array = 0;
GLuint	skybox_vertex_array = 0;
// Texture & sprite
GLuint	SPRITE_CRT_RUN = 0;
GLuint	SPRITE_CRT_IDLE = 0;
GLuint	SPRITE_CRT_JUMP = 0;
GLuint	SPRITE_ENEMY_RUN = 0;
GLuint	SPRITE_ENEMY_IDLE = 0;
GLuint	SPRITE_ENEMY_JUMP = 0;
GLuint	SPRITE_HEART = 0;
GLuint	SPRITE_COIN = 0;
GLuint	SPRITE_BULLET = 0;
GLuint	SPRITE_PISTOL= 0;
GLuint	SPRITE_SHOTGUN = 0;
GLuint	SPRITE_MACHINEGUN = 0;
GLuint	TEX_SKYBOX = 0;
GLuint	TEX_BLOCKS = 0;
GLuint	TEX_BLOCKS_OP = 0;
GLuint	MENU_START = 0;
GLuint	MENU_EASY = 0;
GLuint	MENU_NORMAL = 0;
GLuint	MENU_HARD = 0;
GLuint	MENU_IMPOSSIBLE = 0;
GLuint	MENU_HELP = 0;
GLuint	MENU_END = 0;
//*************************************
// game state
enum GAME_STATE {
	EMPTY,
	PLAYING,
	PAUSE,
	WIN,
	FAIL,
	WIN_WAIT,
	FAIL_WAIT,
	MOVE_NEXT_STAGE,
	RESTART
};
GAME_STATE		game_state = EMPTY;
static const std::vector<vec3> difficulty_prop = { //0:easy, 1:normal, 2: hard, 3: impossible
	{5, 4, 1}, // crt_maxhp, enemy_maxhp, enemy_mass
	{3, 6, 2},
	{2, 10, 4},
	{1, 99, 1}
};
int		difficulty = 0;
int		max_difficulty = 3;
//*************************************
// global variables
int		frame = 0;				// index of rendering frames
int		score = 0;
int		total_score = 0;
float	playtime = 0;
float	total_playtime = 0;
int		menu_state = 0;
// holder of vertices and indices of a unit circle
Map map;
std::list<Enemy> enemy_list;
Character	crt;
Gun			gun(&crt, 0);
bool	b_key_fire = false;
bool	b_key_right = false;
bool	b_key_left = false;
bool	b_key_jump = false;
//*************************************
// scene objects
camera		cam;
material_t	material;
//*************************************
void main_menu(ISoundSource* bgm_src) {
	engine->stopAllSounds();
	engine->play2D(bgm_src, true);
	menu_state = 0;
	game_state = EMPTY;
	glEnable(GL_CULL_FACE);
	glBindVertexArray(sprite_vertex_array);
	int index;
	while (!glfwWindowShouldClose(window) && menu_state >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MENU_START);
		glUniform1i(glGetUniformLocation(program, "MENU_START"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MENU_EASY);
		glUniform1i(glGetUniformLocation(program, "MENU_EASY"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, MENU_NORMAL);
		glUniform1i(glGetUniformLocation(program, "MENU_NORMAL"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, MENU_HARD);
		glUniform1i(glGetUniformLocation(program, "MENU_HARD"),3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, MENU_IMPOSSIBLE);
		glUniform1i(glGetUniformLocation(program, "MENU_IMPOSSIBLE"), 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, MENU_HELP);
		glUniform1i(glGetUniformLocation(program, "MENU_HELP"), 5);

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		mat4 orthogonal_projection_matrix = ortho(-200 * aspect, 200 * aspect, 200, -200, cam.dnear, cam.dfar);
		GLint uloc = glGetUniformLocation(program, "orthogonal_projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, orthogonal_projection_matrix);
		mat4 model_matrix = mat4::translate(0, 0, -1) * aspect_matrix() * mat4::scale(600, 400, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		uloc = glGetUniformLocation(program, "UI_mode");		if (uloc > -1) glUniform1i(uloc, 1);

		if (menu_state == 0) {
			index = 0;
		}
		else if (menu_state == 1) {
			index = difficulty + 1;
		}

		uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, index);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
	}
}
void ending(ISoundSource* bgm_src) {
	engine->stopAllSounds();
	engine->play2D(bgm_src, true);
	menu_state = 2;
	glEnable(GL_CULL_FACE);
	glBindVertexArray(sprite_vertex_array);

	printf("\nCongratulation! Game Cleared!!\n");
	switch (difficulty) {
	case 0:
		printf("difficulty : EASY\n");
		break;
	case 1:
		printf("difficulty : NORMAL\n");
		break;
	case 2:
		printf("difficulty : HARD\n");
		max_difficulty = 4;
		difficulty = 3;
		break;
	default:
		printf("difficulty : IMPOSSIBLE\n");
		printf("YOU ARE THE BEST!\n");
		break;
	}
	printf("total score : %d\n", total_score);
	printf("play time : %3f\n\n", total_playtime);
	while (!glfwWindowShouldClose(window) && menu_state == 2) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MENU_END);
		glUniform1i(glGetUniformLocation(program, "MENU_END"), 0);

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		mat4 orthogonal_projection_matrix = ortho(-200 * aspect, 200 * aspect, 200, -200, cam.dnear, cam.dfar);
		GLint uloc = glGetUniformLocation(program, "orthogonal_projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, orthogonal_projection_matrix);
		mat4 model_matrix = mat4::translate(0, 0, -1) * aspect_matrix() * mat4::scale(600, 400, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		uloc = glGetUniformLocation(program, "UI_mode");		if (uloc > -1) glUniform1i(uloc, 1);
		uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, 6);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);



		std::string s = std::to_string(total_score);
		std::string t = std::to_string(total_playtime);
		float dpi_scale = cg_get_dpi_scale();

		a = abs(sin(float(glfwGetTime()) * 2.5f));

		render_text("CONGRATULATION!", window_size.x / 2 - 120, 100, 0.8f, vec4(1.0f, 0.0f, 0.0f, a), dpi_scale);
		render_text("Mady by Duwon Kim, Kiryun Jang, Hyunwoo Jo", window_size.x / 2 - 260, 150, 0.5f, vec4(1.0f, 0.0f, 0.0f, a), dpi_scale);
		switch (difficulty) {
		case 0:
			render_text("difficulty : EASY", window_size.x / 2 - 100, 250, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
			break;
		case 1:
			render_text("difficulty : NORMAL", window_size.x / 2 - 100, 250, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
			break;
		case 2:
			render_text("difficulty : HARD", window_size.x / 2 - 100, 250, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
			break;
		default:
			render_text("difficulty : IMPOSSIBLE", window_size.x / 2 - 100, 250, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
			render_text("YOU ARE THE BEST!", window_size.x / 2 - 100, 600, 0.5f, vec4(1.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
			break;
		}

		render_text("score:", window_size.x / 2 - 50, 300, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text(s, window_size.x / 2 + 40, 300, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text("time:", window_size.x / 2 - 80, 350, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text(t, window_size.x / 2, 350, 0.5f, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);

		glfwSwapBuffers(window);
	}



}
void update(float t)
{
	if (game_state == PAUSE || game_state == FAIL_WAIT) t = 0;
	// update projection matrix
	cam.projection_matrix = mat4::perspective(cam.fovy, aspect, cam.dnear, cam.dfar);

	

	// setup sunlight properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, sunlight.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, sunlight.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, sunlight.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, sunlight.specular);

	//light source effect
	glUniform1i(glGetUniformLocation(program, "NUM_LIGHT"), NUM_LIGHT);
	int index = 0;
	char string[20] = "light_position2[0]";
	char string2[20] = "Ia2[0]";
	char string3[20] = "Id2[0]";
	char string4[20] = "Is2[0]";
	for (auto& l : lights) {

		glUniform4fv(glGetUniformLocation(program, string), 1, l.position);
		glUniform4fv(glGetUniformLocation(program, string2), 1, l.ambient);
		glUniform4fv(glGetUniformLocation(program, string3), 1, l.diffuse);
		glUniform4fv(glGetUniformLocation(program, string4), 1, l.specular);
		string[16] ++;
		string2[4]++;
		string3[4]++;
		string4[4]++;
	}

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	// player move
	if (game_state == PLAYING) {
		if (b_key_jump) {
			if (crt.jump()) 	engine->play2D(mp3_src_jump);
			b_key_jump = false;
		}
		if (b_key_right) {
			crt.move_right();
		}
		else if (b_key_left) {
			crt.move_left();
		}
		gun.trigger(&b_key_fire, t);
	}
	crt.update(float(t), game_state == PLAYING &&(b_key_left || b_key_right));

	// build bullets
	for (std::list<Bullet>::iterator it = bullet_instances.begin(); it != bullet_instances.end(); it++) {
		bool destroy = false;
		it->update(t);
		Block* bp = map.block(it->position);
		if (bp != 0) // bp is not null;
		{
			destroy = bp->prop->destroy_bullet;
			if (bp->prop->max_hp > 0) {
				bp->hit(it->prop->block_dmg);
			}
		}
		for (std::list<Enemy>::iterator eit = enemy_list.begin(); eit != enemy_list.end(); eit++) {
			if (eit->hitbox.collid(it->position - eit->position)) {
				if (eit->hit(it->prop->dmg, it->direction))
				{
					score += 100;
					destroy = true; //적들이 총알에 맞았을 때
					engine->play2D(mp3_src_enmhit); //sfx
					break;
				}
			}
		}
		if (it->life <= 0 || destroy) {
			it = bullet_instances.erase(it);
		}
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		if (it->hitbox.collid(it->position - crt.position)) {
			if(crt.hit(it->damage, it->position - crt.position)) engine->play2D(mp3_src_scream); // 플레이어 비명
		}
		it->update(t, game_state == PLAYING);
		if (!(it->alive)) {
			engine->play2D(mp3_src_enmboom); //sfx
			particles_list.push_back(Particle(1, it->position));
			it=enemy_list.erase(it);
			score += 300;
		}
	}

	a = abs(sin(float(glfwGetTime()) * 2.5f));

	// items
	for (std::list<Item>::iterator it = item_list.begin(); it != item_list.end(); it++) {
		it->theta += t * 2;
		if (it->theta > PI/2) it->theta -= PI;
		if (length(it->position - crt.position) <= .8f) {
			score += 300;
			switch (it->item_code) {
			case 1:
				crt.heal(2);
				break;
			case 2:
				gun.swap_gun(1);
				engine->play2D(mp3_src_machinegun);
				break;
			case 3:
				gun.swap_gun(2);
				engine->play2D(mp3_src_shotgun);
				break;
			case 4:
				gun.swap_gun(3);
				break;
			default: //5
				score += 700;
				engine->play2D(mp3_src_coin);
				break;
			}
			it = item_list.erase(it);
		}
	}

	//particles
	for (std::list<Particle>::iterator it = particles_list.begin(); it != particles_list.end(); it++) {
		it->updates(t);
		if(it->particles.size()== 0) it = particles_list.erase(it);
	}

	// camera & skybox
	float temp_x = crt.position.x;
	temp_x = std::min(std::max(temp_x, 10.0f), map.map_width -5.0f);
	float temp_y = std::max(crt.position.y, BASE_CAM_Y);
	temp_y = cam.eye.y - (cam.eye.y - temp_y) * 0.1f;

	cam.eye = vec3(temp_x, temp_y, 13);
	cam.at = vec3(temp_x,temp_y,0);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	// setup texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX_BLOCKS);
	glUniform1i(glGetUniformLocation(program, "TEX_BLOCKS"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEX_BLOCKS_OP);
	glUniform1i(glGetUniformLocation(program, "TEX_BLOCKS_OP"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SPRITE_CRT_IDLE);
	glUniform1i(glGetUniformLocation(program, "SPRITE_CRT_IDLE"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, SPRITE_CRT_RUN);
	glUniform1i(glGetUniformLocation(program, "SPRITE_CRT_RUN"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, SPRITE_CRT_JUMP);
	glUniform1i(glGetUniformLocation(program, "SPRITE_CRT_JUMP"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SPRITE_ENEMY_IDLE);
	glUniform1i(glGetUniformLocation(program, "SPRITE_ENEMY_IDLE"), 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, SPRITE_ENEMY_RUN);
	glUniform1i(glGetUniformLocation(program, "SPRITE_ENEMY_RUN"), 6);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, SPRITE_ENEMY_JUMP);
	glUniform1i(glGetUniformLocation(program, "SPRITE_ENEMY_JUMP"), 7);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, MENU_HELP);
	glUniform1i(glGetUniformLocation(program, "MENU_HELP"), 8);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, TEX_SKYBOX);
	glUniform1i(glGetUniformLocation(program, "TEX_SKYBOX"), 9);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, SPRITE_HEART);
	glUniform1i(glGetUniformLocation(program, "SPRITE_HEART"), 10);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, SPRITE_COIN);
	glUniform1i(glGetUniformLocation(program, "SPRITE_COIN"), 11);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, SPRITE_SHOTGUN);
	glUniform1i(glGetUniformLocation(program, "SPRITE_SHOTGUN"), 12);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, SPRITE_MACHINEGUN);
	glUniform1i(glGetUniformLocation(program, "SPRITE_MACHINEGUN"), 13);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, SPRITE_BULLET);
	glUniform1i(glGetUniformLocation(program, "SPRITE_BULLET"), 14);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, SPRITE_PISTOL);
	glUniform1i(glGetUniformLocation(program, "SPRITE_PISTOL"), 15);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(program, "aspect_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix());
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );

	if (game_state == PLAYING)
	{
		if (crt.stand_blockp != 0 && crt.stand_blockp->prop->block_id == 7) {
			game_state = WIN;
			crt.invincible = true;
		}
		else if (!crt.alive) {
			game_state = FAIL;
		}
	}
}
void render()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program);

	GLint uloc;
	mat4 model_matrix;

	// GAME_PART
	uloc = glGetUniformLocation(program, "UI_mode");		if (uloc > -1) glUniform1i(uloc, 0);

	

	// SKYBOX
	glBindVertexArray(skybox_vertex_array);
	uloc = glGetUniformLocation(program, "mode");
	if (uloc > -1) glUniform1i(uloc, 9);
	mat4 model_matrix_sky = mat4::translate(cam.eye.x, cam.eye.y - std::max(crt.position.y, BASE_CAM_Y) / 10, 0) *
		mat4::rotate(vec3(1, 0, 0), -PI / 2) *
		mat4::rotate(vec3(0, 0, 1), cam.eye.x / 20) *
		mat4::scale(vec3(20.0f));
	uloc = glGetUniformLocation(program, "model_matrix");
	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_sky);
	glDrawElements(GL_TRIANGLES, NUM_TESS * (NUM_TESS + 1) * 6, GL_UNSIGNED_INT, nullptr);

	// 맵
	glBindVertexArray(cube_vertex_array);
	uloc = glGetUniformLocation(program, "mode");
	if (uloc > -1) glUniform1i(uloc, 1);

	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_HEIGHT; j++) {
			int block_id = map.map[i][j].prop->block_id;
			if (block_id > 0) {
				Block* bp = &map.map[i][j];
				mat4 translate_matrix = mat4::translate(float(i), float(j), 0);
				mat4 scale_matrix = mat4::scale(1);
				switch (block_id) {
				case 6:
				case 2:
				case 5://wood & spike
					scale_matrix = mat4::scale(1, 1, float(bp->hp) / bp->prop->max_hp);
					break;
				default://wall
					break;
				}
				model_matrix = translate_matrix * scale_matrix;
				uloc = glGetUniformLocation(program, "index");
				if (uloc > -1) glUniform1i(uloc, block_id);
				uloc = glGetUniformLocation(program, "model_matrix");
				if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
				glDrawElements(GL_TRIANGLES, 3 * 4 * 6, GL_UNSIGNED_INT, nullptr);

			}
		}
	}
	


	// 그림자 
	glBindVertexArray(circle_vertex_array);
	uloc = glGetUniformLocation(program, "mode");
	if (uloc > -1) glUniform1i(uloc, -1);

	int temp_y = map.shadow_pos(crt.position);
	if (crt.alive && temp_y >= 0) {
		model_matrix = mat4::translate(crt.position.x, temp_y + 0.01f, 0) * mat4::scale(crt.hitbox.width() / 2, 1, crt.hitbox.width() / 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 3 * 32, GL_UNSIGNED_INT, nullptr);
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		temp_y = map.shadow_pos(it->position);
		model_matrix = mat4::translate(it->position.x, temp_y + 0.01f, 0) * mat4::scale(it->hitbox.width() / 2, 1, it->hitbox.width() / 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 3 * 32, GL_UNSIGNED_INT, nullptr);
	}

	// render sprites
	glDisable(GL_CULL_FACE);
	glBindVertexArray(sprite_vertex_array);

	// sprite animation part
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 3);
	// render enemies
	int temp_z = 0;
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		model_matrix = mat4::translate(it->position.x, it->position.y, ++temp_z * .001f) * mat4::scale(it->direction.x, 1, 1);
		uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		uloc = glGetUniformLocation(program, "animation");		if (uloc > -1) glUniform4i(uloc, 1, it->status, max_frame[it->status], it->frame); // sprite_id, status, max_frame, frame;	
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

	// build character model
	if (crt.alive && (crt.invinc_t <= 0 || int(crt.invinc_t * 10) % 2 == 0)) {
		model_matrix = mat4::translate(crt.position.x, crt.position.y, ++temp_z * .001f) * mat4::scale(crt.direction.x, 1, 1);
		uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		uloc = glGetUniformLocation(program, "animation");		if (uloc > -1) glUniform4i(uloc, 0, crt.status, max_frame[crt.status], crt.frame); // sprite_id, status, max_frame, frame;	
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

	// build bullets
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, 6);
	std::list<Bullet>::iterator it;
	for (it = bullet_instances.begin(); it != bullet_instances.end(); it++) {
		model_matrix = mat4::translate(it->position.x, it->position.y, ++temp_z * .001f) * mat4::scale(0.4f, 0.4f, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

	glDisable(GL_DEPTH_TEST);
	// render items
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 2);
	for (std::list<Item>::iterator it = item_list.begin(); it != item_list.end(); it++) {
		uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, it->item_code);
		model_matrix = mat4::translate(it->position.x, it->position.y, 0) * mat4::scale(0.9f, 0.9f, 1) * mat4::rotate(vec3(0, 1, 0), it->theta);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	// render particles
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 4);
	for (std::list<Particle>::iterator it = particles_list.begin(); it != particles_list.end(); it++) {
		for (std::list<particle_t>::iterator pit = it->particles.begin(); pit != it->particles.end(); pit++) {
			mat4 translate_matrix = mat4::translate(vec3(pit->position.x, pit->position.y, 0));
			mat4 scale_matrix = mat4::scale(pit->scale);
			model_matrix = translate_matrix * scale_matrix;

			uloc = glGetUniformLocation(program, "particle_color");			if (uloc > -1) glUniform4fv(uloc, 1, pit->color);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	// UI
	uloc = glGetUniformLocation(program, "UI_mode");		if (uloc > -1) glUniform1i(uloc, 1);
	uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, 7);
	mat4 orthogonal_projection_matrix = ortho(0, 400 * aspect, 0, -400, cam.dnear, cam.dfar);
	uloc = glGetUniformLocation(program, "orthogonal_projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, orthogonal_projection_matrix);
	vec2 offset = vec2(40, 40);
	for (int i = 0; i < crt.hp; i++) { //HP HEART
		model_matrix = mat4::translate(offset.x + 45 * i, -offset.y, -1) * mat4::scale(35, 35, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	// GUN
	offset = vec2(40, 80);
	uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, gun.prop->gun_id + 8);
	model_matrix = mat4::translate(offset.x , -offset.y, -1 ) * mat4::scale(35, 35, 1);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

	

	

	//HELP
	if (game_state == PAUSE) { 
		uloc = glGetUniformLocation(program, "index");		if (uloc > -1) glUniform1i(uloc, 5);
		mat4 orthogonal_projection_matrix = ortho(-200 * aspect, 200 * aspect, 200, -200, cam.dnear, cam.dfar);
		uloc = glGetUniformLocation(program, "orthogonal_projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, orthogonal_projection_matrix);
		model_matrix = mat4::translate(0,0,-1) * aspect_matrix() * mat4::scale(600, 400, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}
void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
	aspect = window_size.x / float(window_size.y);
}
void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "\n" );
}
void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
		{
			if (menu_state == 0) {
				glfwSetWindowShouldClose(window, GL_TRUE);
			}
			else {
				menu_state = 0;
			}
		}
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1) {
			if (game_state == PLAYING) {
				print_help();
				printf("score:%d, total:%d\n", score, total_score);
				printf("play_time:%f, total:%f\n", playtime, total_playtime);
				printf("position: vec2(%3f, %3f)\n", crt.position.x, crt.position.y);
				engine->play2D(mp3_src_select);
				game_state = PAUSE;
			}
			else if (game_state == PAUSE) {
				engine->play2D(mp3_src_select);
				game_state = PLAYING;
			}
		}
		else if (key == GLFW_KEY_R)	game_state = RESTART;
		else if (key == GLFW_KEY_W || key == GLFW_KEY_SPACE || key == GLFW_KEY_UP || key == GLFW_KEY_LEFT_ALT)
		{
			b_key_jump = true;
		}
		else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
		{
			if (game_state == PLAYING) b_key_left = true;
			else if (menu_state == 1)
			{
				difficulty = (max_difficulty + --difficulty) % max_difficulty;
				engine->play2D(mp3_src_select);
			}
		}
		else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
		{
			if(game_state == PLAYING) b_key_right = true;
			else if (menu_state == 1)
			{
				difficulty = (++difficulty) % max_difficulty;
				engine->play2D(mp3_src_select);
			}
		}
		else if (key == GLFW_KEY_ENTER) {
			if (menu_state == 0) { //start to difficulty select
				menu_state = 1;
				engine->play2D(mp3_src_select);
			}
			else if (menu_state >= 1) { // difficulty select to game play
				menu_state = -1;
			}
		}
		if (key == GLFW_KEY_J || key == GLFW_KEY_LEFT_CONTROL) {
			b_key_fire = true;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) b_key_left = false;
		if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) b_key_right = false;
		if (key == GLFW_KEY_J || key == GLFW_KEY_LEFT_CONTROL) b_key_fire = false;
	}
}
void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		//if (action == GLFW_PRESS);
		//else if (action == GLFW_RELEASE);
	}
}
void motion( GLFWwindow* window, double x, double y )
{
}
void update_vertex_buffer(const std::vector<vertex>& vertices, std::vector<uint>& indices, GLuint* v_array)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }
	
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (*v_array) glDeleteVertexArrays(1, v_array);
	*v_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!*v_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}
bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests	
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0

	// create vertex buffer
	update_vertex_buffer(create_block_vertices(), block_indices, &cube_vertex_array);
	update_vertex_buffer(sprite_vertices, sprite_indices, &sprite_vertex_array);
	update_vertex_buffer(create_circle_vertices(32), circle_indices, &circle_vertex_array);
	update_vertex_buffer(create_skybox_vertices(NUM_TESS), skybox_indices, &skybox_vertex_array);
	
	// sound
	init_sound();

	// texture
	TEX_SKYBOX = cg_create_texture(skybox_image_path, true);  if (!TEX_SKYBOX) return false;
	TEX_BLOCKS = cg_create_texture(blocks_image_path, true, GL_REPEAT, GL_NEAREST);
	TEX_BLOCKS_OP = cg_create_texture(blocks_opacity_image_path, true);

	// sprite
	SPRITE_CRT_RUN = cg_create_texture(sprite_crt_run_image_path, true); if (!SPRITE_CRT_RUN) return false;
	SPRITE_CRT_IDLE = cg_create_texture(sprite_crt_idle_image_path, true); if (!SPRITE_CRT_IDLE) return false;
	SPRITE_CRT_JUMP = cg_create_texture(sprite_crt_jump_image_path, true); if (!SPRITE_CRT_JUMP) return false;
	SPRITE_ENEMY_RUN = cg_create_texture(sprite_enemy_run_image_path, true); if (!SPRITE_ENEMY_RUN) return false;
	SPRITE_ENEMY_IDLE = cg_create_texture(sprite_enemy_idle_image_path, true); if (!SPRITE_ENEMY_IDLE) return false;
	SPRITE_ENEMY_JUMP = cg_create_texture(sprite_enemy_jump_image_path, true); if (!SPRITE_ENEMY_JUMP) return false;
	SPRITE_HEART = cg_create_texture(sprite_heart_image_path, true); if (!SPRITE_HEART) return false;
	SPRITE_COIN = cg_create_texture(sprite_coin_image_path, true); if (!SPRITE_COIN) return false;
	SPRITE_SHOTGUN = cg_create_texture(sprite_shotgun_image_path, true); if (!SPRITE_SHOTGUN) return false;
	SPRITE_MACHINEGUN = cg_create_texture(sprite_machinegun_image_path, true); if (!SPRITE_MACHINEGUN) return false;
	SPRITE_BULLET = cg_create_texture(sprite_bullet_image_path, true); if (!SPRITE_BULLET) return false;
	SPRITE_PISTOL = cg_create_texture(sprite_pistol_image_path, true); if (!SPRITE_PISTOL) return false;
	// menu
	MENU_START = cg_create_texture(menu_start_image_path, true); if (!MENU_START) return false;
	MENU_EASY = cg_create_texture(menu_easy_image_path, true); if (!MENU_EASY) return false;
	MENU_NORMAL = cg_create_texture(menu_normal_image_path, true); if (!MENU_NORMAL) return false;
	MENU_HARD = cg_create_texture(menu_hard_image_path, true); if (!MENU_HARD) return false;
	MENU_IMPOSSIBLE = cg_create_texture(menu_impossible_image_path, true); if (!MENU_IMPOSSIBLE) return false;
	MENU_HELP = cg_create_texture(menu_help_image_path, true); if (!MENU_HELP) return false;
	MENU_END = cg_create_texture(menu_end_image_path, true); if (!MENU_END) return false;

	if (!init_text()) return false;

	

	return true;
}
void user_finalize()
{
}
void load_map(Map* m) {
	vec3 diff = difficulty_prop[difficulty];
	game_state = PLAYING;
	map = *m;
	crt = Character(&map, map.crt_start_pos, int(diff.x));
	gun.swap_gun(0);
	particles_list.clear();
	bullet_instances.clear();
	enemy_list.clear();
	for (auto pos : map.enemy_pos_list) {
		enemy_list.push_back(Enemy(&map, &crt, pos, int(diff.y), diff.z ));
	}
	item_list.clear();
	for (auto item : map.items){
		item.position += 0.5f;
		item_list.push_back(item);
	}
}
int run_stage(Map* map, ISoundSource* bgm_src) {
	if (glfwWindowShouldClose(window) || menu_state == 0) return -1;
	game_state = PLAYING;
	float tp, t, timer;
	cam.eye.y = BASE_CAM_Y;
	while (game_state != MOVE_NEXT_STAGE && !glfwWindowShouldClose(window) && menu_state != 0) {
		load_map(map);
		t = float(glfwGetTime());
		score = 0;
		engine->stopAllSounds();
		engine->play2D(bgm_src, true);
		b_key_left = false;
		b_key_right = false;
		b_key_fire = false;
		b_key_jump = false;
		// enters rendering/event loop
		for (frame = 0; !glfwWindowShouldClose(window) && menu_state != 0; frame++)
		{
			tp = t;
			t = float(glfwGetTime());
			glfwPollEvents();	// polling and processing of events
			update(t - tp);		// per-frame update
			render();			// per-frame render
			if (game_state == WIN) {//win
				score += 1000;
				timer = t;
				engine->stopAllSounds();
				engine->play2D(mp3_src_win);
				game_state = WIN_WAIT;
			}
			else if (game_state == FAIL) {//fail
				timer = t;
				engine->stopAllSounds();
				engine->play2D(mp3_src_fail);
				particles_list.push_back(Particle(4, crt.position));
				game_state = FAIL_WAIT;
			}
			else if (game_state == WIN_WAIT) {//win_wait
				if (t - timer > 4) {
					game_state = MOVE_NEXT_STAGE;//move_next_stage
					break;
				}
			}
			else if (game_state == FAIL_WAIT) {//fail_wait
				if (t - timer > 3) {
					game_state = PLAYING;
					break;
				}
			}
			else if (game_state == RESTART) { //restart
				break;
			}
			else if(game_state == PLAYING) {
				playtime += t - tp;
			}
		}
	}
	if (game_state == MOVE_NEXT_STAGE) {
		total_playtime += playtime;
		total_score += score;
		return 0;
	}
	else {
		return -1;
	}
}
int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	while (!glfwWindowShouldClose(window)) {
		main_menu(mp3_src_bgm0);
		total_score = 0;
		total_playtime = 0;
		if(run_stage(&Map(new_map2, 100, vec2(4, 4), new_map2_enemies, new_map2_items), mp3_src_bgm2) == -1) continue;
		if(run_stage(&Map(new_map1, 100, vec2(4, 4), new_map1_enemies, new_map1_items), mp3_src_bgm1) == -1) continue;
		if (run_stage(&Map(new_map3, 100, vec2(4, 4), new_map3_enemies, new_map3_items), mp3_src_bgm1) == -1) continue;
		if(game_state == MOVE_NEXT_STAGE) {
			ending(mp3_src_bgm4);
		}
	}
	// normal termination
	user_finalize();
	cg_destroy_window(window);
	return 0;
}
