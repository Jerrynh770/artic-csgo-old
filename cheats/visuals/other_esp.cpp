// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f) //-V550
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat());
}
/*
void otheresp::holopanel(player_t* WhoUseThisBone, int hitbox_id, bool autodir)
{

	std::string ping = std::to_string(g_ctx.globals.ping);
	char const* pincc = ping.c_str();

	std::string fps = std::to_string(g_ctx.globals.framerate);
	char const* fpsc = fps.c_str();

	if (g_cfg.misc.holo_panel)
	{
		auto bone_pos = WhoUseThisBone->hitbox_position(hitbox_id);
		Vector angle;
		if (key_binds::get().get_key_bind_state(17))
		{
			if (math::world_to_screen(bone_pos, angle))
			{
				render::get().line(angle.x, angle.y, angle.x + 100, angle.y - 150, Color(255, 255, 255));

				render::get().rect_filled(angle.x + 100, angle.y - 155, 150, 5, Color(g_cfg.menu.menu_theme.r(), g_cfg.menu.menu_theme.g(), g_cfg.menu.menu_theme.b(), 255));
				if (g_ctx.globals.framerate < 60)
				{
					//render::get().rect_filled(angle.x + 100, angle.y - 150, 150, 105 + render::get().text_heigth(fonts[NAME], "WARNING! LOW FPS") + 5, Color(0, 0, 0, 150));
					//render::get().text(fonts[NAME], angle.x + 110, angle.y - 45, Color(219, 179, 15, 255), 0, "WARNING! LOW FPS");
				}
				else
				{
					render::get().rect_filled(angle.x + 100, angle.y - 150, 150, 105, Color(0, 0, 0, 150));
				}

				if (g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key)
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(117, 219, 15, 255), 0, "Doubletap");
				}
				else if (g_cfg.antiaim.hide_shots && g_cfg.antiaim.hide_shots_key.key > KEY_NONE && g_cfg.antiaim.hide_shots_key.key < KEY_MAX && misc::get().hide_shots_key)
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(117, 219, 15, 255), 0, "HideShots");
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(219, 15, 15, 255), 0, "Not active");
				}

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 125, Color(255, 255, 255, 255), 0, "DoubleTap Type : ");
				if (g_cfg.ragebot.double_tap or g_cfg.ragebot.peej)
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Doubletap Type : "), angle.y - 125, Color(117, 219, 15, 255), 0, "Ofensive");
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Doubletap Type : "), angle.y - 125, Color(219, 15, 15, 255), 0, "Defensive");
				}

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 105, Color(255, 255, 255, 255), 0, "Fps : ");
				if (g_ctx.globals.framerate >= 60)
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Fps : "), angle.y - 105, Color(117, 219, 15, 255), 0, fpsc);
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Fps : "), angle.y - 105, Color(219, 15, 15, 255), 0, fpsc);
				}


				render::get().text(fonts[NAME], angle.x + 110, angle.y - 85, Color(255, 255, 255, 255), 0, "Ping : ");
				render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Ping : "), angle.y - 85, Color(255, 255, 255, 255), 0, pincc);

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 65, Color(255, 255, 255, 255), 0, "artic - 1.2.2");

			}
		}
	}
}
*/
void otheresp::penetration_reticle()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.penetration_reticle)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = Color::Red;

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color::Green;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	render::get().rect_filled(width / 2, height / 2 - 1, 1, 3, color);
	render::get().rect_filled(width / 2 - 1, height / 2, 3, 1, color);
}


void otheresp::custom_scopes_lines()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (!g_cfg.esp.removals[REMOVALS_SCOPE])
		return;

	if (!g_cfg.esp.custom_scopes_line)
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto is_scoped = g_ctx.globals.scoped && weapon->is_sniper() && weapon->m_zoomLevel();

	if (!is_scoped)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto offset = g_cfg.esp.scopes_line_offset;
	auto leng = g_cfg.esp.scopes_line_width;
	auto accent = g_cfg.esp.scopes_line_color;
	auto accent2 = Color(g_cfg.esp.scopes_line_color.r(), g_cfg.esp.scopes_line_color.g(), g_cfg.esp.scopes_line_color.b(), 0);

	render::get().gradient(width / 2 + offset, height / 2, leng, 1, accent, accent2, GradientType::GRADIENT_HORIZONTAL);
	render::get().gradient(width / 2 - leng - offset, height / 2, leng, 1, accent2, accent, GradientType::GRADIENT_HORIZONTAL);
	render::get().gradient(width / 2, height / 2 + offset, 1, leng, accent, accent2, GradientType::GRADIENT_VERTICAL);
	render::get().gradient(width / 2, height / 2 - leng - offset, 1, leng, accent2, accent, GradientType::GRADIENT_VERTICAL);
}
void otheresp::indicators()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (g_cfg.esp.indicators[INDICATOR_LC] && (antiaim::get().type == ANTIAIM_LEGIT || g_cfg.antiaim.type[antiaim::get().type].desync))
	{
		auto color = Color(130, 20, 20);
		auto animstate = g_ctx.local()->get_animation_state();

		if (animstate && local_animations::get().local_data.animstate)
		{
			auto delta = fabs(math::normalize_yaw(animstate->m_flGoalFeetYaw - local_animations::get().local_data.animstate->m_flGoalFeetYaw));
			auto desync_delta = max(g_ctx.local()->get_max_desync_delta(), 58.0f);

			color = Color(130, 20 + (int)(min(delta / desync_delta, 1.0f) * 150.0f), 20);
		}


		m_indicators.push_back(m_indicator("LC", color));
	}

	if (g_cfg.esp.indicators[INDICATOR_DAMAGE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("DMG", Color(130, 170, 20)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(3) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("SAFE", Color(130, 170, 20)));

	if (g_cfg.esp.indicators[INDICATOR_BODY_AIM] && key_binds::get().get_key_bind_state(22) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("BAIM", Color(130, 170, 20)));

	if (g_cfg.esp.indicators[INDICATOR_DT] && g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key)
		m_indicators.push_back(m_indicator("DT", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().double_tap_enabled && !weapon->is_grenade() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && weapon->can_fire(false) ? Color(255, 255, 255) : Color(255, 0, 0)));

	if (g_cfg.esp.indicators[INDICATOR_HS] && g_cfg.antiaim.hide_shots && g_cfg.antiaim.hide_shots_key.key > KEY_NONE && g_cfg.antiaim.hide_shots_key.key < KEY_MAX && misc::get().hide_shots_key)
		m_indicators.push_back(m_indicator("HS", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().hide_shots_enabled ? Color(255, 255, 255) : Color(255, 0, 0)));
}

void otheresp::draw_indicators()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto h = height - 325;

	for (auto& indicator : m_indicators)
	{
		render::get().text(fonts[INDICATORFONT], 27, h, indicator.m_color, HFONT_CENTERED_Y, indicator.m_text.c_str());
		h -= 25;
	}

	m_indicators.clear();
}

void otheresp::hitmarker_paint()
{
	if (!g_cfg.esp.hitmarker[0] && !g_cfg.esp.hitmarker[1])
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (g_cfg.esp.hitmarker[0])
		{
			static int width, height;
			m_engine()->GetScreenSize(width, height);

			auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
			hitmarker.hurt_color.SetAlpha(alpha);

			auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

			render::get().line(width / 2 + 5 + offset, height / 2 - 5 - offset, width / 2 + 12 + offset, height / 2 - 12 - offset, hitmarker.hurt_color);
			render::get().line(width / 2 + 5 + offset, height / 2 + 5 + offset, width / 2 + 12 + offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 + 5 + offset, width / 2 - 12 - offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 - 5 - offset, width / 2 - 12 - offset, height / 2 - 12 - offset, hitmarker.hurt_color);
		}

		if (g_cfg.esp.hitmarker[1])
		{
			Vector world;

			if (math::world_to_screen(hitmarker.point, world))
			{
				auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
				hitmarker.hurt_color.SetAlpha(alpha);

				auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

				render::get().line(world.x + 5 + offset, world.y - 5 - offset, world.x + 12 + offset, world.y - 12 - offset, hitmarker.hurt_color);
				render::get().line(world.x + 5 + offset, world.y + 5 + offset, world.x + 12 + offset, world.y + 12 + offset, hitmarker.hurt_color);
				render::get().line(world.x - 5 - offset, world.y + 5 + offset, world.x - 12 - offset, world.y + 12 + offset, hitmarker.hurt_color);
				render::get().line(world.x - 5 - offset, world.y - 5 - offset, world.x - 12 - offset, world.y - 12 - offset, hitmarker.hurt_color);
			}
		}
	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y, damage_marker[i].hurt_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);
		}
	}
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	draw_circe((float)w * 0.5f, (float)h * 0.5f, g_ctx.globals.inaccuracy * 500.0f, 50, D3DCOLOR_RGBA(g_cfg.esp.show_spread_color.r(), g_cfg.esp.show_spread_color.g(), g_cfg.esp.show_spread_color.b(), g_cfg.esp.show_spread_color.a()), D3DCOLOR_RGBA(0, 0, 0, 0), device);
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr); //-V107

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0); //-V107
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void Beam(Vector src, Vector end, Color color)
{

	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS; //TE_BEAMPOINTS
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	beamInfo.m_pszModelName = "sprites/glow01.vmt";
	beamInfo.m_pszHaloName = "sprites/glow01.vmt";
	beamInfo.m_flHaloScale = 1.5;
	beamInfo.m_flWidth = 1.5f;
	beamInfo.m_flEndWidth = 1.5f;
	beamInfo.m_flFadeLength = 0.5f;
	beamInfo.m_flAmplitude = 0;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.1f;
	beamInfo.m_nStartFrame = 0.0;
	beamInfo.m_flFrameRate = 0.0;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_flLife = 0.1;
	beamInfo.m_nFlags = FBEAM_SHADEIN | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE;
	Beam_t* myBeam = m_viewrenderbeams()->CreateBeamPoints(beamInfo);
	if (myBeam)
		m_viewrenderbeams()->DrawBeam(myBeam);
}
//просто кружок зеленый
/*void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 4.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 9.0f * m_globals()->m_frametime; //-V807
		else
			alpha -= 9.0f * m_globals()->m_frametime;

		alpha = math::clamp(alpha, 0.0f, 1.0f);
		//render::get().Draw3DFilledCircle(position, 25.0f, g_cfg.esp.molotov_timer ? Color(183, 206, 232, (int)(alpha * 55.0f)) : Color(183, 206, 232, (int)(alpha * 55.0f)));
		render::get().Draw3DFilledCircle(position, alpha * 20.f, g_cfg.esp.molotov_timer ? Color(40, 220, 5, (int)(alpha * 135.0f)) : Color(200, 200, 200, (int)(alpha * 125.0f)));
		Vector screen;

		if (math::world_to_screen(position, screen))
		{
			static auto offset = 30.0f;

			if (!g_ctx.globals.fired_shot)
			{
				static auto switch_offset = false;

				if (offset <= 30.0f || offset >= 55.0f)
					switch_offset = !switch_offset;

				offset += switch_offset ? 22.0f * m_globals()->m_frametime : -22.0f * m_globals()->m_frametime;
				offset = math::clamp(offset, 30.0f, 55.0f);
			}
		}
	}
}*/
//def stock ap
void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 2.3f * m_globals()->m_frametime; //-V807
		else
			alpha -= 2.3f * m_globals()->m_frametime;
		alpha = math::clamp(alpha, 0.0f, 1.0f);
		auto color_main = Color(0, 139, 254, (int)((alpha * 0)));
		auto color_outline = Color(0, 139, 254, (int)(alpha * 255));
		auto beam_circle = [](Vector position, float points, float radius, Color color, float rotate = 0, bool start_rotate = false) {
			float step = start_rotate ? rotate : 1 * M_PI * 2.0f / points;
			for (float a = 0; a < (M_PI * 2.0f); a += step)
			{

				const Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
				const Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);
				Vector start2d, end2d;
				Beam(start, end, color);
			}
		};
		static float rotate = 0.f;
		if (rotate <= 0.7f)
			rotate += 0.01f;
		else
			rotate = 0.1;

		beam_circle(position, 12, alpha * 20.f, color_outline, rotate, alpha == 1);

	}
}
bool mouse_pointer(Vector2D position, Vector2D size)
{
	return hooks::mouse_pos.x > position.x && hooks::mouse_pos.y > position.y && hooks::mouse_pos.x < position.x + size.x && hooks::mouse_pos.y < position.y + size.y;
}
void otheresp::keybinds() // да много щит кода но это мои первые кейбинды
{
	auto size = Vector2D(150.f, 20.f);

	bool _click = false;

	static bool _dragging = false;

	static Vector2D tl(0, 0);

	static int drag_x = 50;
	static int width, height;
	m_engine()->GetScreenSize(width, height);
	static int drag_y = 50;

	if (hooks::menu_open && GetAsyncKeyState(VK_LBUTTON))
		_click = true;

	Vector2D _mouse_pos = hooks::mouse_pos;

	if (hooks::menu_open)
	{
		if (_dragging && !_click)
			_dragging = false;

		if (_dragging && _click)
		{
			tl.x = _mouse_pos.x - drag_x;
			tl.y = _mouse_pos.y - drag_y;
		}



		if (mouse_pointer(Vector2D(tl.x + 10, tl.y + height / 2 + 10), Vector2D(160, 20)))
		{
			_dragging = true;
			drag_x = _mouse_pos.x - tl.x;
			drag_y = _mouse_pos.y - tl.y;
		}
	}

	auto pos = Vector2D(tl.x, tl.y);

	int x = pos.x;
	int y = pos.y;
	int offset = 1;


	if (!g_cfg.menu.bin)
		return;

	if (!g_ctx.local()->is_alive()) //-V807
		return;


	render::get().rect_filled(x + 10, y + height / 2 + 10, 160, 5 + 15, { 15, 15, 15, 155 });

	render::get().gradient(x + 10, y + height / 2 + 10, 80, 2, Color(48, 206, 230, 255), Color(220, 57, 218, 255), GRADIENT_HORIZONTAL);
	render::get().gradient(x + 90, y + height / 2 + 10, 80, 2, Color(220, 57, 218, 255), Color(220, 232, 47, 255), GRADIENT_HORIZONTAL);

	// название
	render::get().text(fonts[WATERMARK], x + 10 + 60, y + height / 2 + 14, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "keybinds");

	// дт
	if (misc::get().double_tap_key)
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Double tap");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// хайд шоты
	if (misc::get().hide_shots_key)
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Hide shots");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// pitch exploit
	if (key_binds::get().get_key_bind_state(23))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Pitch Exploit");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// мин дамаг
	if (key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Min damage");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// баим
	if (key_binds::get().get_key_bind_state(22))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Body aim");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// резольвер овервайд ( для моей пасты,можете его удалить )
	if (key_binds::get().get_key_bind_state(24))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Resolver override");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// 3 лицо
	if (key_binds::get().get_key_bind_state(17))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Thirdperson");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// слоуволк (
	if (key_binds::get().get_key_bind_state(21))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Slow walk");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// фейк дак
	if (key_binds::get().get_key_bind_state(20))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Fake duck");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// эдже джамп
	if (key_binds::get().get_key_bind_state(19))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Edge jump");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// авто пик
	if (key_binds::get().get_key_bind_state(18))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Auto peek");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}

	// инвентор аа
	if (key_binds::get().get_key_bind_state(16))
	{
		render::get().text(fonts[WATERMARK], x + 10 + 6, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Desync invert");
		render::get().text(fonts[WATERMARK], x + 10 + 111, y + height / 2 + 15 + 15 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		offset = offset + 1;
	}
}





