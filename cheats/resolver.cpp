// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}
int resolver::GetChokedPackets(player_t* entity)
{
	int last_ticks[65];

	auto ticks = TIME_TO_TICKS(entity->m_flSimulationTime() - entity->m_flOldSimulationTime());
	if (ticks == 0 && last_ticks[entity->EntIndex()] > 0)
	{
		return last_ticks[entity->EntIndex()] - 1;
	}
	else
	{
		last_ticks[entity->EntIndex()] = ticks;
		return ticks;
	}
}
bool resolver::IsLBYdesync(player_t* record) {

	if (GetChokedPackets(record) >= 1 && record->m_vecVelocity().Length2D() <= 0.15)
		return true;
	else //íàõóÿ ýòî ???
		return false;
}


void resolver::help() {
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) //-V807
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] >= 2 || g_ctx.globals.missed_shots[player->EntIndex()] && aim::get().last_target[player->EntIndex()].record.type != LBY)
	{
		switch (last_side)
		{
		case RESOLVER_ORIGINAL:
			g_ctx.globals.missed_shots[player->EntIndex()] = 0;
			fake = true;
			break;
		case RESOLVER_ZERO:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_FIRST;

			was_first_bruteforce = false;
			was_second_bruteforce = false;
			return;
		case RESOLVER_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

			was_first_bruteforce = true;
			return;
		case RESOLVER_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

			was_second_bruteforce = true;
			return;
		case RESOLVER_LOW_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_SECOND;
			return;
		case RESOLVER_LOW_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_FIRST;
			return;
		}
	}
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}



void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	auto animstate = player->get_animation_state();

	if (!animstate)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (fabs(original_pitch) > 70.0f)
		fake = true;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
	auto valid_lby = true;

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.23f;

	if (fabs(delta) > 30.0f && valid_lby)
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()])
			delta = -delta;

		if (delta > 30.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_FIRST;
		}
		else if (delta < -30.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_SECOND;
		}
	}
	else
	{
		auto trace = false;

		if (m_globals()->m_curtime - lock_side > 2.0f)
		{
			auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
			auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

			if (first_visible != second_visible)
			{
				trace = true;
				side = second_visible;
				lock_side = m_globals()->m_curtime;
			}
			else
			{
				auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
				auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

				if (fabs(first_position - second_position) > 1.0f)
					side = first_position > second_position;
			}
		}
		else
			trace = true;

		if (side)
		{
			player_record->type = trace ? TRACE : DIRECTIONAL;
			player_record->side = RESOLVER_FIRST;
		}
		else
		{
			player_record->type = trace ? TRACE : DIRECTIONAL;
			player_record->side = RESOLVER_SECOND;
		}
	}

	AnimationLayer m_server_anim_layers[13];
	AnimationLayer m_previous_anim_layers[13];
	AnimationLayer m_resolver_anim_layers[3][13];

	int m_side;
	int m_way;

	auto speed_2d = player->m_vecVelocity().Length2D();

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	player->update_clientside_animation();

	if (player->m_fFlags() & FL_ONGROUND)
	{

		if (speed_2d <= 0.1)
		{
			if (m_server_anim_layers[ANIMATION_LAYER_ADJUST].m_flWeight == 0.0
				&& m_server_anim_layers[ANIMATION_LAYER_ADJUST].m_flCycle == 0.0)
			{
				m_way = 1;
				auto fatal_delta = math::angle_diff(player->m_angEyeAngles().y, gfy_default);
				m_side = 2 * (fatal_delta <= 0.0) - 1;
			}
		}
		else if (!(m_server_anim_layers[ANIMATION_LAYER_LEAN].m_flWeight * 1000.0) && (m_server_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight * 1000.0) == (m_previous_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight * 1000.0))
		{
			auto m_first_delta = abs(m_server_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - m_resolver_anim_layers[ANIMATION_LAYER_AIMMATRIX][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate);
			auto m_second_delta = abs(m_server_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - m_resolver_anim_layers[ANIMATION_LAYER_WEAPON_ACTION_RECROUCH][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate);
			auto m_third_delta = abs(m_server_anim_layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - m_resolver_anim_layers[ANIMATION_LAYER_WEAPON_ACTION][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate);

			if (m_first_delta < m_second_delta || m_third_delta <= m_second_delta || (m_second_delta * 1000.0))
			{
				if (m_first_delta >= m_third_delta && m_second_delta > m_third_delta && !(m_third_delta * 1000.0))
				{
					m_way = 1;
					m_side = 1;
				}
			}
			else
			{
				m_way = 1;
				m_side = -1;
			}
		}
	}

	auto angle = player->get_max_desync_delta();

	auto big_angle = m_side == -1 ? -angle : angle;

	auto claculate_angle = speed_2d > 3.03 ? 30 * m_side : big_angle;


	float m_flGoalFeetYaw = animstate->m_flGoalFeetYaw;

	float m_flYawModifier = claculate_angle;

	float desync_delta; // xmm0_4
	float v4; // xmm1_4
	float v5; // xmm1_4
	float v6; // xmm1_4
	float v7; // xmm1_4
	float v8; // xmm1_4
	float v9; // xmm1_4

	desync_delta = m_flYawModifier;


	switch (g_ctx.globals.missed_shots[player->EntIndex()] >= 1)
	{
	case 1:
		v4 = m_flGoalFeetYaw + (desync_delta + desync_delta);
		if (v4 > 180.0 || v4 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v4 / 360.0));
		break;
	case 2:
		v5 = m_flGoalFeetYaw + (desync_delta * 0.5);
		if (v5 > 180.0 || v5 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v5 / 360.0));
		break;
	case 4:
		v6 = m_flGoalFeetYaw + (desync_delta * -0.5);
		if (v6 > 180.0 || v6 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v6 / 360.0));
		break;
	case 5:
		v8 = m_flGoalFeetYaw - (desync_delta + desync_delta);
		if (v8 > 180.0 || v8 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v8 / 360.0));
		break;
	case 7:
		v9 = m_flGoalFeetYaw + 120.0;
		if ((m_flGoalFeetYaw + 120.0) > 180.0 || v9 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v9 / 360.0));
		break;
	case 8:
		v7 = m_flGoalFeetYaw - 120.0;
		if ((m_flGoalFeetYaw - 120.0) > 180.0 || v7 < -180.0)
			m_flGoalFeetYaw = player->m_angEyeAngles().y + (math::normalize_yaw(v7 / 360.0));
		break;
	default:
		return;
	}

	//if (g_ctx.globals.missed_shots[player->EntIndex()] >= 3)
		//help();
}

float resolver::resolve_pitch()
{
	return original_pitch;
}