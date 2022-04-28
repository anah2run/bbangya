#pragma once
#include "sound.h"
#include "particle.h"
#include <cmath>
static const std::vector<uint> max_frame = { 4,6,4 }; // 0:idle, 1:run ,2:jump

class Map;
class Hitbox {
public:
	vec4	box_rec = { -0.3f, -0.5f, 0.3f, 0.5f };	// rectangle margin
	Hitbox() {
	}
	Hitbox(float l, float b, float r, float t) {
		box_rec = vec4(l, b, r, t);
	}
	bool collid(vec2 pos) {
		bool left_check = box_rec.x < pos.x&& box_rec.y < pos.y;
		bool right_check = pos.x < box_rec.z&& pos.y < box_rec.a;
		return  left_check && right_check;
	}
	float width() {
		return box_rec.z - box_rec.x;
	}
	float height() {
		return box_rec.a - box_rec.y;
	}
};
class Character {
public:
	Character() {
	}
	Character(Map* p, vec2 pos, int m_hp = 3) {
		mapp = p;
		position = pos;
		max_hp = m_hp;
		hp = max_hp;
	}
	// 애니메이션 관련
	int		sprite_index = 0;
	int		status = 0;
	int		frame = 0;
	float	frame_interval = 0.1f;
	float	frame_t = 0;
	//

	bool	invincible = false; // 무적
	bool	alive = true;		// 생존
	float	invinc_time = 1.5f;	// 피격 시 무적 시간
	float	max_speed = 5.0f;	// 최대 이동속도
	float	speed = 1.0f;		// 이동 가속도
	float	max_jump = 3.5f;	// 최대 점프
	float	mass = 1; 			// 반동 감소
	int	max_hp = 3;			// 최대 체력
	Map* mapp;					// 맵 포인터

	vec2	direction = vec2(1,0);	// 가고자 하는 방향
	Hitbox	hitbox = Hitbox();	// 히트 박스
	vec2	position;			// 위치	
	vec2	velocity = vec2(0);
	vec2	acceleration = vec2(0);
	int		hp = max_hp;		// 현재 체력
	float	invinc_t = 0;		// 무적 타이머
	bool	is_jump = true;		// 점프 상태
	Block*	stand_blockp;		// 바로 아래 블록 포인터

	// function
	bool	hit(int damage, vec2 hit_pos);
	bool	heal(int amount);
	void	move_right();
	void	move_left();
	bool	jump();
	bool	checkJump();
	bool	checkDeath();
	void	physics(float t, bool moving);
	void	update(float t, bool moving);
	void	update_status(float t, bool moving);
};

inline bool Character::hit(int damage, vec2 hit_pos) {
	if (invinc_t <= 0 && !invincible) {
		hp -= damage;
		particles_list.push_back(Particle(0, position));
		if (length(hit_pos) > 0)
		{
			velocity = hit_pos.normalize() * 10 / mass;
		}
		if (velocity.y > 0) {
			is_jump = true;
		}
		invinc_t = invinc_time;
		alive = !checkDeath();
		return true;
	}
	return false;
}
inline bool Character::heal(int amount) {
	bool flag = hp <= max_hp;
	hp = std::min(hp + amount, max_hp);
	engine->play2D(mp3_src_heart); //sfx
	return flag;
}
inline bool Character::checkDeath() {
	return hp <= 0 || position.y < -5;
}

inline bool Character::checkJump() {
	vec2 stand_block_pos = position - vec2(0, 1);
	//printf("check jump : %f %f \n",stand_block_pos.x,position.y);
	stand_blockp = mapp->block(stand_block_pos);
	if (stand_blockp == 0) return 1;
	return stand_blockp->prop->block_id == 0;
}
inline void Character::physics(float t, bool moving)
{
	bool burned = false;
	vec2 temp_pos = position;
	if (is_jump) {
		acceleration.x *= 0.8f;
		acceleration.y = -mapp->gravity;
		stand_blockp = 0;
		alive = !checkDeath();
	}
	else {
		is_jump = checkJump();
		acceleration.y = 0;
		velocity.y = 0;
		if (stand_blockp != 0 && stand_blockp->prop->collid_dmg > 0) {
			burned = hit(stand_blockp->prop->collid_dmg, vec2(0, 1));
			is_jump = burned;
		}
		if (!moving) acceleration.x = -velocity.x * 10;
		else acceleration.x = 0;
	}
	velocity += acceleration * t;
	position += velocity * t + acceleration * powf(t, 2);
	if (position.x - temp_pos.x >= 0) { //캐릭터가 오른쪽으로 움직임
		int temp_pos_x = int(position.x + hitbox.box_rec.z);
		for (int i = int(temp_pos.x + hitbox.box_rec.z); i <= temp_pos_x; i++)
		{
			Block* bp = mapp->block(vec2(float(i), temp_pos.y));
			if (bp == 0) continue;
			if (bp->prop->block_id > 0) {
				position.x = i - hitbox.box_rec.z;
				if (bp->prop->collid_dmg > 0) {
					burned = burned || hit(bp->prop->collid_dmg, vec2(-1, 0));
				}
				break;
			}
		}
	}
	if (position.x - temp_pos.x <= 0) { //캐릭터가 왼쪽으로 움직임
		int temp_pos_x = int(position.x + hitbox.box_rec.x);
		for (int i = int(temp_pos.x + hitbox.box_rec.x); i >= temp_pos_x; i--)
		{
			Block* bp = mapp->block(vec2(float(i), temp_pos.y));
			if (bp == 0) continue;
			if (bp->prop->block_id > 0) {
				position.x = i + 1 - hitbox.box_rec.x;
				if (bp->prop->collid_dmg > 0) {
					burned = burned || hit(bp->prop->collid_dmg, vec2(1, 0));
				}
				break;
			}
		}
	}
	if(is_jump)
	{
		if (position.y - temp_pos.y > 0) { //캐릭터가 위쪽으로 움직임
			int temp_pos_y = int(position.y + hitbox.box_rec.a);
			for (int i = int(temp_pos.y + hitbox.box_rec.a); i <= temp_pos_y; i++)
			{
				Block* bp = mapp->block(vec2(position.x, float(i)));
				if (bp == 0) continue;
				if (bp->prop->max_hp > 0) {
					bp->hit(1);
				}
				if (bp->prop->block_id > 0) {
					position.y = i - hitbox.box_rec.a;
					velocity.y = 0; // 천장 부딫힘
					if (bp->prop->collid_dmg > 0) {
						burned = burned || hit(bp->prop->collid_dmg, vec2(0, -1));
					}
					else {
						engine->play2D(mp3_src_blockhit); //sfx
					}
					break;
				}
			}
		}
		else {
			int temp_pos_y = int(position.y + hitbox.box_rec.y);
			for (int i = int(temp_pos.y + hitbox.box_rec.y); i >= temp_pos_y; i--)
			{
				Block* bp = mapp->block(vec2(position.x, float(i)));
				if (bp == 0) continue;
				if (bp->prop->block_id > 0) {
					position.y = i + 1 - hitbox.box_rec.y;
					if (bp->prop->collid_dmg > 0) {
						burned = burned || hit(bp->prop->collid_dmg, vec2(0, -1));
						is_jump = burned;
					}
					else {
						is_jump = false;
					}
					break;
				}
			}
		}
	}
	if(burned) engine->play2D(mp3_src_burned);
}
inline void Character::update_status(float t, bool moving) {
	int temp_status = 0;
	if (is_jump) temp_status = 2;
	else if (moving) temp_status = 1;

	if (temp_status != status) {
		frame = 0;
		frame_t = 0;
		status = temp_status;
	}
	else
	{
		frame_t += t;
		int temp_frame = int(frame_t / frame_interval);
		frame_t -= frame_interval * temp_frame;
		if (temp_status == 2) {
			frame += temp_frame;
			frame = uint(frame) >= max_frame[status] ? max_frame[status] - 1 : frame;
		}
		else {
			frame = (frame + temp_frame) % max_frame[status];
		}
	}
}
inline void Character::update(float t, bool moving)
{
	invinc_t = std::max(invinc_t - t, 0.0f);
	physics(t, moving);
	update_status(t, moving);
}


inline void Character::move_right()
{
	direction.x = 1;
	if (is_jump) {
		if (velocity.x < max_speed/2)
		{
			acceleration.x = max_speed*5;
		}
	}
	else
	{
		velocity.x = std::min(max_speed, velocity.x + speed);
	}
}

inline void Character::move_left()
{
	direction.x = -1;
	if (is_jump ) {
		if (velocity.x > -max_speed/2)
		{
			acceleration.x = -max_speed*5;
		}
	}
	else
		velocity.x = std::max(-max_speed, velocity.x - speed);
}

inline bool Character::jump()
{
	if (!is_jump) {
		float jv = sqrt(2 * max_jump * mapp->gravity);
		velocity.y = jv;
		is_jump = true;
		particles_list.push_back(Particle(2, position + vec2(0,hitbox.box_rec.y)));
		return true;
	}
	return false;
	
}

class Enemy : public Character
{
public:
	Character* crt;
	float	active_range = 8;	// 인식 반경 (x축)
	float	action_timer = 0;	// 행동 대기 시간
	int		damage = 1;
	bool	active = false;
	Enemy(Map* mp, Character* cp, vec2 pos, int m_hp = 5, float m = 2) {
		mass = m;
		invinc_time = 0.05f;	// 피격 시 무적 시간
		direction = vec2(-1, 0);
		max_speed = 4.5f;	// 최대 이동속도
		speed = 0.3f;
		max_jump = 1.5f;	// 최대 점프
		mapp = mp;
		max_hp = m_hp;
		hp = max_hp;
		position = pos;
		crt = cp;
		if (max_hp >= 99) {
			invincible = true;
		}
	}
	void update(float t, bool moving);
	void move();
	bool check_active();
};
inline bool Enemy::check_active() {
	return active || hp < max_hp || length(crt->position - position) < active_range;
}
inline void Enemy::update(float t, bool moving) {
	invinc_t = std::max(invinc_t - t, 0.0f);
	action_timer = std::max(action_timer - t, 0.0f);
	physics(t, active);

	update_status(t, active);
	if (active && moving) {
		move();
	}
	else {
		active = check_active();
	}
}
inline void Enemy::move() {
	if (crt->position.x > position.x) {
		move_right();
	}
	else {
		move_left();
	}
	if (action_timer <= 0 && (random_range(0, 1)<= 0.04f || crt->position.y > position.y)) {
		jump();
		action_timer = random_range(0.1f,.7f);
	}
}
