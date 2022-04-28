#pragma once
#pragma comment(lib, "irrKlang.lib")
#include "irrKlang\irrKlang.h"
using namespace irrklang;
ISoundEngine* engine = createIrrKlangDevice();
ISoundSource* mp3_src_bgm0;
ISoundSource* mp3_src_bgm1;
ISoundSource* mp3_src_bgm2;
ISoundSource* mp3_src_bgm3;
ISoundSource* mp3_src_bgm4;
ISoundSource* mp3_src_gun;
ISoundSource* mp3_src_jump;
ISoundSource* mp3_src_step;
ISoundSource* mp3_src_enmhit;
ISoundSource* mp3_src_enmboom;
ISoundSource* mp3_src_blockhit;
ISoundSource* mp3_src_scream;
ISoundSource* mp3_src_heart;
ISoundSource* mp3_src_burned;
ISoundSource* mp3_src_glass;
ISoundSource* mp3_src_win;
ISoundSource* mp3_src_fail;
ISoundSource* mp3_src_select;
ISoundSource* mp3_src_coin;
ISoundSource* mp3_src_shotgun;
ISoundSource* mp3_src_machinegun;
ISoundSource* mp3_src_box;
//
static const char* mp3_path_bgm0 = "../bin/sounds/bgm0.wav";
static const char* mp3_path_bgm1 = "../bin/sounds/bgm1.wav";
static const char* mp3_path_bgm2 = "../bin/sounds/bgm2.wav";
static const char* mp3_path_bgm3 = "../bin/sounds/bgm3.wav";
static const char* mp3_path_bgm4 = "../bin/sounds/bgm4.wav";
static const char* mp3_path_gun = "../bin/sounds/gun.mp3";
static const char* mp3_path_jump = "../bin/sounds/jump.mp3";
static const char* mp3_path_step = "../bin/sounds/step.ogg";
static const char* mp3_path_enmhit = "../bin/sounds/hit.wav";
static const char* mp3_path_enmboom = "../bin/sounds/boom.wav";
static const char* mp3_path_blockhit = "../bin/sounds/solid.wav";
static const char* mp3_path_scream = "../bin/sounds/scream.mp3";
static const char* mp3_path_heart = "../bin/sounds/heart.wav";
static const char* mp3_path_burned = "../bin/sounds/burned.wav";
static const char* mp3_path_glass = "../bin/sounds/glass.mp3";
static const char* mp3_path_win = "../bin/sounds/win.wav";
static const char* mp3_path_fail = "../bin/sounds/fail.wav";
static const char* mp3_path_select = "../bin/sounds/select.wav";
static const char* mp3_path_coin = "../bin/sounds/coin.wav";
static const char* mp3_path_machinegun = "../bin/sounds/machinegun.mp3";
static const char* mp3_path_shotgun = "../bin/sounds/shotgun.mp3";
static const char* mp3_path_box = "../bin/sounds/box.mp3";
//
void init_sound() {
	mp3_src_bgm0 = engine->addSoundSourceFromFile(mp3_path_bgm0);
	mp3_src_bgm0->setDefaultVolume(0.3f);

	mp3_src_bgm1 = engine->addSoundSourceFromFile(mp3_path_bgm1);
	mp3_src_bgm1->setDefaultVolume(0.3f);

	mp3_src_bgm2 = engine->addSoundSourceFromFile(mp3_path_bgm2);
	mp3_src_bgm2->setDefaultVolume(0.3f);

	mp3_src_bgm3 = engine->addSoundSourceFromFile(mp3_path_bgm3);
	mp3_src_bgm3->setDefaultVolume(0.3f);

	mp3_src_bgm4 = engine->addSoundSourceFromFile(mp3_path_bgm4);
	mp3_src_bgm4->setDefaultVolume(0.3f);

	mp3_src_gun = engine->addSoundSourceFromFile(mp3_path_gun);
	mp3_src_gun->setDefaultVolume(0.5f);

	mp3_src_jump = engine->addSoundSourceFromFile(mp3_path_jump);
	mp3_src_jump->setDefaultVolume(0.7f);

	mp3_src_step = engine->addSoundSourceFromFile(mp3_path_step);
	mp3_src_step->setDefaultVolume(0.5f);

	mp3_src_enmhit = engine->addSoundSourceFromFile(mp3_path_enmhit);
	mp3_src_enmhit->setDefaultVolume(0.4f);

	mp3_src_enmboom = engine->addSoundSourceFromFile(mp3_path_enmboom);
	mp3_src_enmboom->setDefaultVolume(0.3f);

	mp3_src_blockhit = engine->addSoundSourceFromFile(mp3_path_blockhit);
	mp3_src_blockhit->setDefaultVolume(0.5f);

	mp3_src_scream = engine->addSoundSourceFromFile(mp3_path_scream);
	mp3_src_scream->setDefaultVolume(0.7f);

	mp3_src_heart = engine->addSoundSourceFromFile(mp3_path_heart);
	mp3_src_heart->setDefaultVolume(0.4f);

	mp3_src_burned = engine->addSoundSourceFromFile(mp3_path_burned);
	mp3_src_burned->setDefaultVolume(0.3f);

	mp3_src_glass = engine->addSoundSourceFromFile(mp3_path_glass);
	mp3_src_glass->setDefaultVolume(0.4f);

	mp3_src_win = engine->addSoundSourceFromFile(mp3_path_win);
	mp3_src_win->setDefaultVolume(0.5f);

	mp3_src_fail = engine->addSoundSourceFromFile(mp3_path_fail);
	mp3_src_fail->setDefaultVolume(0.5f);

	mp3_src_select = engine->addSoundSourceFromFile(mp3_path_select);
	mp3_src_select->setDefaultVolume(0.3f);

	mp3_src_coin = engine->addSoundSourceFromFile(mp3_path_coin);
	mp3_src_coin->setDefaultVolume(0.5f);

	mp3_src_machinegun = engine->addSoundSourceFromFile(mp3_path_machinegun);
	mp3_src_machinegun->setDefaultVolume(0.8f);

	mp3_src_shotgun = engine->addSoundSourceFromFile(mp3_path_shotgun);
	mp3_src_shotgun->setDefaultVolume(0.8f);

	mp3_src_box = engine->addSoundSourceFromFile(mp3_path_box);
	mp3_src_box->setDefaultVolume(1.0f);
}
