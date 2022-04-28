#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

//texture
uniform sampler2D	SPRITE_CRT_IDLE;
uniform sampler2D	SPRITE_CRT_RUN;
uniform sampler2D	SPRITE_CRT_JUMP;
uniform sampler2D	SPRITE_ENEMY_IDLE;
uniform sampler2D	SPRITE_ENEMY_RUN;
uniform sampler2D	SPRITE_ENEMY_JUMP;
uniform sampler2D	SPRITE_HEART;
uniform sampler2D	SPRITE_COIN;
uniform sampler2D	SPRITE_SHOTGUN;
uniform sampler2D	SPRITE_MACHINEGUN;
uniform sampler2D	SPRITE_BULLET;
uniform sampler2D	SPRITE_PISTOL;

uniform sampler2D	TEX_SKYBOX;
uniform sampler2D	TEX_BLOCKS;
uniform sampler2D	TEX_BLOCKS_OP;

uniform sampler2D	MENU_START;
uniform sampler2D	MENU_EASY;
uniform sampler2D	MENU_NORMAL;
uniform sampler2D	MENU_HARD;
uniform sampler2D	MENU_IMPOSSIBLE;
uniform sampler2D	MENU_HELP;
uniform sampler2D	MENU_END;
//
uniform int			UI_mode;
uniform int			mode;
uniform int			index;
uniform ivec4		animation; // sprite_id, status, max_frame, frame;
uniform vec4		particle_color;

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

//light
uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	//sunlight
uniform int		NUM_LIGHT;
uniform vec4	light_position2[10], Ia2[10], Id2[10], Is2[10];	//lights
uniform vec4	Ka, Kd, Ks;

vec4 phong(vec3 l, vec3 n, vec3 h, vec4 Kd, vec4 ila, vec4 ild, vec4 ils)
{
	
	vec4 Ira,Ird,Irs;
	if (index == 4) {
		Ira = 0.5f*Ka * ila;											// ambient reflection
		Ird = 0.7f*max(Kd * dot(l, n) * ild, 0.0);					// diffuse reflection
		Irs = max(Ks * pow(dot(h, n), shininess*5.f) * ils, 0.0);	// specular reflection
	}
	else {
		Ira = Ka * ila;
		Ird = max(Kd * dot(l, n) * ild, 0.0);
		Irs = max(Ks * pow(dot(h, n), shininess) * ils, 0.0);
	}
		
	return Ira + Ird + Irs;
}

void main()
{
	if(UI_mode == 0)
	{
		vec4 sprite;
		vec4 lpos = view_matrix * light_position;
		vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
		vec3 p = epos.xyz;			// 3D position of this fragment
		vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
		vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
		vec3 h = normalize(l + v);	// the halfway vector
		//sunlight effect
		fragColor = phong(l, n, h, Kd,Ia,Id,Is) * 1.7f;
		
		//lights effect
		float len = 10.f;
		for (int i = 0; i < NUM_LIGHT; i++) {
			lpos = view_matrix * light_position2[i];
			len = 0.4f*abs(length(lpos - epos));
			len = len < 0.3f ? 0.3f : len;
			l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));
			h = normalize(l + v);
			fragColor = min(fragColor + phong(l, n, h, Kd, Ia2[i], Id2[i], Is2[i]) / (len *len),1.5f);
		}

		switch (mode) {
		case 9: // skybox
			fragColor = texture( TEX_SKYBOX, tc);
			break;
		case 1: // block texture
			fragColor *= texture(TEX_BLOCKS, tc+vec2(0,0.1f* (index-1)));
			fragColor.a = texture(TEX_BLOCKS_OP, tc + vec2(0, 0.1f * (index - 1))).x;
			break;
		case 2: // sprite
			switch(index)
			{
			default: //heart
				sprite = texture( SPRITE_HEART, tc);
				break;
			case 1:
				sprite = texture( SPRITE_HEART, tc);
				break;
			case 2:
				sprite = texture( SPRITE_MACHINEGUN, tc);
				break;
			case 3:
				sprite = texture( SPRITE_SHOTGUN, tc);
				break;
			case 4:
				sprite = texture( SPRITE_HEART, tc);
				break;
			case 5:
				sprite = texture( SPRITE_COIN, tc);
				break;
			case 6: //bullet
				sprite = texture( SPRITE_BULLET, tc);
				break;
			}
			fragColor *= sprite;
			break;
		case 3: // sprite animation
			if(animation.x == 0){ //crt
				if(animation.y == 0){
					sprite = texture( SPRITE_CRT_IDLE, vec2((animation.a + tc.x) / animation.z ,tc.y * 0.9f + 0.1f));
				}
				else if(animation.y == 1){
					sprite = texture( SPRITE_CRT_RUN, vec2((animation.a + tc.x) / animation.z ,tc.y * 0.9f + 0.1f));
				}
				else{
					sprite = texture( SPRITE_CRT_JUMP, vec2((animation.a + tc.x)/ animation.z ,tc.y * 0.9f + 0.1f));
				}	
			}
			else{//enemy
				if(animation.y == 0){ 
					sprite = texture( SPRITE_ENEMY_IDLE, vec2((animation.a + tc.x) /  animation.z ,tc.y * 0.9f + 0.1f));
				}
				else if(animation.y == 1){
					sprite = texture( SPRITE_ENEMY_RUN, vec2((animation.a + tc.x) / animation.z ,tc.y * 0.9f + 0.1f));
				}
				else{
					sprite = texture( SPRITE_ENEMY_JUMP, vec2((animation.a + tc.x) / animation.z ,tc.y * 0.9f + 0.1f));
				}	
			}
			fragColor *= sprite;
			break;
		case 4: // particle
			fragColor *= particle_color;
			break;
		case -1: //shadow
			fragColor = vec4(0,0,0,fragColor.x/3 );
			break;
		default:
			fragColor = vec4(normalize(norm), 1.0f);
			break;
		}
	}
	else
	{
		switch(index)
		{
			case 6 : // end
				fragColor = texture( MENU_END, tc);
				break;
			case 5 : // help
				fragColor = texture( MENU_HELP, tc);
				break;
			case 4 : // impossible
				fragColor = texture( MENU_IMPOSSIBLE, tc);
				break;
			case 3 : // hard
				fragColor = texture( MENU_HARD, tc);
				break;
			case 2 : // normal
				fragColor = texture( MENU_NORMAL, tc);
				break;
			case 1 : // easy
				fragColor = texture( MENU_EASY, tc);
				break;
			case 0 : // start
				fragColor = texture( MENU_START, tc);
				break;
			case 7 : // heart
				fragColor = texture( SPRITE_HEART, tc);
				break;
			case 8: // pistol
				fragColor = texture( SPRITE_PISTOL, tc);
				break;
			case 9: // machinegun
				fragColor = texture( SPRITE_MACHINEGUN, tc);
				break;
			case 10: // shotgun
				fragColor = texture( SPRITE_SHOTGUN, tc);
				break;
			default:
				fragColor = vec4(normalize(norm), 1.0f);
				break;
		}
		
	}
}
