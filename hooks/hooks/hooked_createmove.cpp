﻿// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\ragebot\antiaim.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\fakelag.h"
#include "..\..\cheats\misc\prediction_system.h"
#include "..\..\cheats\ragebot\aim.h"
#include "..\..\cheats\legitbot\legitbot.h"
#include "..\..\cheats\misc\bunnyhop.h"
#include "..\..\cheats\misc\airstrafe.h"
#include "..\..\cheats\misc\spammers.h"
#include "..\..\cheats\fakewalk\slowwalk.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\GrenadePrediction.h"
#include "..\..\cheats\ragebot\knifebot.h"
#include "..\..\cheats\ragebot\zeusbot.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\lagcompensation\animation_system.h"

using CreateMove_t = void(__stdcall*)(int, float, bool);

void __stdcall hooks::hooked_createmove(int sequence_number, float input_sample_frametime, bool active, bool& send_packet)
{
	static auto original_fn = client_hook->get_func_address <CreateMove_t>(22);
	original_fn(sequence_number, input_sample_frametime, active);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	auto m_pcmd = m_input()->GetUserCmd(sequence_number);
	auto verified = m_input()->GetVerifiedUserCmd(sequence_number);

	g_ctx.globals.in_createmove = false;
	Vector wish_yaw = m_pcmd->m_viewangles;

	if (!m_pcmd)
		return;

	if (!m_pcmd->m_command_number)
		return;

	if (original_fn)
	{
		m_prediction()->SetLocalViewAngles(m_pcmd->m_viewangles);
		m_engine()->SetViewAngles(m_pcmd->m_viewangles);
	}

	if (!g_ctx.available())
		return;

	misc::get().rank_reveal();
	spammers::get().clan_tag();

	if (!g_ctx.local()->is_alive())
		return;

	g_ctx.globals.weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!g_ctx.globals.weapon)
		return;

	g_ctx.globals.in_createmove = true;
	g_ctx.set_command(m_pcmd);

	if (menu_open && g_ctx.globals.focused_on_input)
	{
		m_pcmd->m_buttons = 0;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
	}

	if (g_ctx.globals.ticks_allowed < 14 && (misc::get().double_tap_enabled && misc::get().double_tap_key || misc::get().hide_shots_enabled && misc::get().hide_shots_key))
		g_ctx.globals.should_recharge = true;

	g_ctx.globals.backup_tickbase = g_ctx.local()->m_nTickBase();

	if (g_ctx.globals.next_tickbase_shift)
		g_ctx.globals.fixed_tickbase = g_ctx.local()->m_nTickBase() - g_ctx.globals.next_tickbase_shift;
	else
		g_ctx.globals.fixed_tickbase = g_ctx.globals.backup_tickbase;

	if (menu_open)
	{
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;
	}

	if (m_pcmd->m_buttons & IN_ATTACK2 && g_cfg.ragebot.enable && g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		m_pcmd->m_buttons &= ~IN_ATTACK2;

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->can_fire(true))
	{
		if (m_pcmd->m_buttons & IN_ATTACK && !g_ctx.globals.weapon->is_non_aim() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			m_pcmd->m_buttons &= ~IN_ATTACK;
		else if ((m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2) && (g_ctx.globals.weapon->is_knife() || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_HEALTHSHOT)
		{
			if (m_pcmd->m_buttons & IN_ATTACK)
				m_pcmd->m_buttons &= ~IN_ATTACK;

			if (m_pcmd->m_buttons & IN_ATTACK2)
				m_pcmd->m_buttons &= ~IN_ATTACK2;
		}
	}

	if (m_pcmd->m_buttons & IN_FORWARD && m_pcmd->m_buttons & IN_BACK)
	{
		m_pcmd->m_buttons &= ~IN_FORWARD;
		m_pcmd->m_buttons &= ~IN_BACK;
	}

	if (m_pcmd->m_buttons & IN_MOVELEFT && m_pcmd->m_buttons & IN_MOVERIGHT)
	{
		m_pcmd->m_buttons &= ~IN_MOVELEFT;
		m_pcmd->m_buttons &= ~IN_MOVERIGHT;
	}

	g_ctx.send_packet = true;
	g_ctx.globals.tickbase_shift = 0;
	g_ctx.globals.double_tap_fire = false;
	g_ctx.globals.force_send_packet = false;
	g_ctx.globals.exploits = misc::get().double_tap_key || misc::get().hide_shots_key;
	g_ctx.globals.current_weapon = g_ctx.globals.weapon->get_weapon_group(g_cfg.ragebot.enable);
	g_ctx.globals.slowwalking = false;
	g_ctx.globals.original_forwardmove = m_pcmd->m_forwardmove;
	g_ctx.globals.original_sidemove = m_pcmd->m_sidemove;

	antiaim::get().breaking_lby = false;

	misc::get().fast_stop(m_pcmd);

	if (g_cfg.misc.hidencvars)
		misc::get().EnableHiddenCVars();

	if (g_cfg.misc.bunnyhop)
		bunnyhop::get().create_move();

	misc::get().SlideWalk(m_pcmd);

	misc::get().NoDuck(m_pcmd);

	misc::get().AutoCrouch(m_pcmd);

	GrenadePrediction::get().Tick(m_pcmd->m_buttons);

	if (g_cfg.misc.crouch_in_air && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_DUCK;

	engine_prediction::m_misc.m_bOverrideModifier = true;
	engine_prediction::update();
	engine_prediction::predict(m_pcmd, g_ctx.local());
	local_animations::get().update_prediction_animations();

	g_ctx.globals.eye_pos = g_ctx.local()->get_shoot_position();

	if (g_cfg.misc.airstrafe)
		airstrafe::get().create_move(m_pcmd);

	if (key_binds::get().get_key_bind_state(19) && engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_JUMP;

	if (key_binds::get().get_key_bind_state(21))
		slowwalk::get().create_move(m_pcmd);

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->is_non_aim() && engineprediction::get().backup_data.flags & FL_ONGROUND && g_ctx.local()->m_fFlags() & FL_ONGROUND)
		slowwalk::get().create_move(m_pcmd, 0.95f + 0.003125f * (16 - m_clientstate()->iChokedCommands));

	if (!g_ctx.globals.should_recharge)
		fakelag::get().Createmove();

	g_ctx.globals.aimbot_working = false;
	g_ctx.globals.revolver_working = false;

	auto backup_velocity = g_ctx.local()->m_vecVelocity();
	auto backup_abs_velocity = g_ctx.local()->m_vecAbsVelocity();

	g_ctx.local()->m_vecVelocity() = engineprediction::get().backup_data.velocity;
	g_ctx.local()->m_vecAbsVelocity() = engineprediction::get().backup_data.velocity;

	g_ctx.globals.weapon->update_accuracy_penality();

	g_ctx.local()->m_vecVelocity() = backup_velocity;
	g_ctx.local()->m_vecAbsVelocity() = backup_abs_velocity;

	g_ctx.globals.inaccuracy = g_ctx.globals.weapon->get_inaccuracy();
	g_ctx.globals.spread = g_ctx.globals.weapon->get_spread();

	aim::get().run(m_pcmd);
	legit_bot::get().createmove(m_pcmd);

	zeusbot::get().run(m_pcmd);
	knifebot::get().run(m_pcmd);

	misc::get().automatic_peek(m_pcmd, wish_yaw.y);

	antiaim::get().desync_angle = 0.0f;
	antiaim::get().create_move(m_pcmd);

	if (m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 6) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}
	else if (!m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 16) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_send_packet)
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_choke_packet)
	{
		g_ctx.globals.should_choke_packet = false;
		g_ctx.globals.should_send_packet = true;
		g_ctx.send_packet = false;
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
		{
			static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));

			if (g_cfg.ragebot.enable)
				m_pcmd->m_viewangles -= g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();

			if (!g_ctx.globals.fakeducking)
			{
				g_ctx.globals.force_send_packet = true;
				g_ctx.globals.should_choke_packet = true;
				g_ctx.send_packet = true;
				fakelag::get().started_peeking = false;
			}

			aim::get().last_shoot_position = g_ctx.globals.eye_pos;

			if (!double_tap_aim_check)
				g_ctx.globals.double_tap_aim = false;
		}
	}
	else if (!g_ctx.globals.fakeducking && g_ctx.globals.weapon->is_knife() && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2))
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.globals.should_choke_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.fakeducking)
		g_ctx.globals.force_send_packet = g_ctx.send_packet;

	for (auto& backup : aim::get().backup)
		backup.adjust_player();

	auto backup_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (misc::get().double_tap(m_pcmd))
		misc::get().hide_shots(m_pcmd, false);
	else
	{
		g_ctx.globals.ticks_allowed = backup_ticks_allowed;
		misc::get().hide_shots(m_pcmd, true);
	}

	//misc::get().createmove(m_pcmd);

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (!double_tap_aim_check && m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
			g_ctx.globals.double_tap_aim = false;
	}

	if (m_globals()->m_tickcount - g_ctx.globals.last_aimbot_shot > 16) //-V807
	{
		g_ctx.globals.double_tap_aim = false;
		g_ctx.globals.double_tap_aim_check = false;
	}

	engine_prediction::restore(g_ctx.local());
	engine_prediction::m_misc.m_bOverrideModifier = false;

	if (g_ctx.globals.loaded_script)
		for (auto current : c_lua::get().hooks.getHooks(crypt_str("on_createmove")))
			current.func();

	if (g_cfg.misc.anti_untrusted)
		math::normalize_angles(m_pcmd->m_viewangles);
	else
	{
		m_pcmd->m_viewangles.y = math::normalize_yaw(m_pcmd->m_viewangles.y);
		m_pcmd->m_viewangles.z = 0.0f;
	}

	if (g_ctx.send_packet && !g_ctx.globals.should_send_packet && (!g_ctx.globals.should_choke_packet || (!misc::get().hide_shots_enabled && !g_ctx.globals.double_tap_fire)))
	{
		local_animations::get().local_data.fake_angles = m_pcmd->m_viewangles; //-V807
		local_animations::get().local_data.real_angles = local_animations::get().local_data.stored_real_angles;
	}

	if (!antiaim::get().breaking_lby)
		local_animations::get().local_data.stored_real_angles = m_pcmd->m_viewangles;

	util::movement_fix(wish_yaw, m_pcmd);

	if (g_ctx.globals.should_recharge)
		g_ctx.send_packet = true;

	static auto previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.send_packet && m_clientstate()->pNetChannel)
	{
		auto choked_packets = m_clientstate()->pNetChannel->m_nChokedPackets;

		if (choked_packets >= 0)
		{
			auto ticks_allowed = g_ctx.globals.ticks_allowed;
			auto command_number = m_pcmd->m_command_number - choked_packets;

			do
			{
				auto command = &m_input()->m_pCommands[m_pcmd->m_command_number - 150 * (command_number / 150) - choked_packets];

				if (!command || command->m_tickcount > m_globals()->m_tickcount * 2)
				{
					if (--ticks_allowed < 0)
						ticks_allowed = 0;

					g_ctx.globals.ticks_allowed = ticks_allowed;
				}

				++command_number;
				--choked_packets;
			} while (choked_packets >= 0);
		}
	}

	if (g_ctx.globals.ticks_allowed > 17)
		g_ctx.globals.ticks_allowed = math::clamp(g_ctx.globals.ticks_allowed - 1, 0, 17);

	if (previous_ticks_allowed && !g_ctx.globals.ticks_allowed)
		g_ctx.globals.ticks_choke = 16;

	previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.globals.ticks_choke)
	{
		g_ctx.send_packet = g_ctx.globals.force_send_packet;
		--g_ctx.globals.ticks_choke;
	}

	auto& correct = g_ctx.globals.data.emplace_front();

	correct.command_number = m_pcmd->m_command_number;
	correct.choked_commands = m_clientstate()->iChokedCommands + 1;
	correct.tickcount = m_globals()->m_tickcount;

	if (g_ctx.send_packet)
		g_ctx.globals.choked_number.clear();
	else
		g_ctx.globals.choked_number.emplace_back(correct.command_number);

	while (g_ctx.globals.data.size() > (int)(2.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.data.pop_back();

	auto& out = g_ctx.globals.commands.emplace_back();

	out.is_outgoing = g_ctx.send_packet;
	out.is_used = false;
	out.command_number = m_pcmd->m_command_number;
	out.previous_command_number = 0;

	while (g_ctx.globals.commands.size() > (int)(1.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.commands.pop_front();

	if (!g_ctx.send_packet && !m_gamerules()->m_bIsValveDS())
	{
		auto net_channel = m_clientstate()->pNetChannel;

		if (net_channel->m_nChokedPackets > 0 && !(net_channel->m_nChokedPackets % 4))
		{
			auto backup_choke = net_channel->m_nChokedPackets;
			net_channel->m_nChokedPackets = 0;

			net_channel->send_datagram();
			--net_channel->m_nOutSequenceNr;

			net_channel->m_nChokedPackets = backup_choke;
		}
	}

	if (g_ctx.send_packet && g_ctx.globals.should_send_packet)
		g_ctx.globals.should_send_packet = false;

	if (g_cfg.misc.buybot_enable && g_ctx.globals.should_buy)
	{
		--g_ctx.globals.should_buy;

		if (!g_ctx.globals.should_buy)
		{
			std::string buy;

			switch (g_cfg.misc.buybot1)
			{
			case 1:
				buy += crypt_str("buy g3sg1; ");
				break;
			case 2:
				buy += crypt_str("buy awp; ");
				break;
			case 3:
				buy += crypt_str("buy ssg08; ");
				break;
			}

			switch (g_cfg.misc.buybot2)
			{
			case 1:
				buy += crypt_str("buy elite; ");
				break;
			case 2:
				buy += crypt_str("buy deagle; buy revolver; ");
				break;
			}

			if (g_cfg.misc.buybot3[BUY_ARMOR])
				buy += crypt_str("buy vesthelm; buy vest; ");

			if (g_cfg.misc.buybot3[BUY_TASER])
				buy += crypt_str("buy taser; ");

			if (g_cfg.misc.buybot3[BUY_GRENADES])
				buy += crypt_str("buy molotov; buy hegrenade; buy smokegrenade; ");

			if (g_cfg.misc.buybot3[BUY_DEFUSER])
				buy += crypt_str("buy defuser; ");

			m_engine()->ExecuteClientCmd(buy.c_str());
		}
	}

	g_ctx.globals.in_createmove = false;
	send_packet = g_ctx.send_packet;

	verified->m_cmd = *m_pcmd;
	verified->m_crc = m_pcmd->GetChecksum();
}

__declspec(naked) void __stdcall hooks::hooked_createmove_proxy(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[esp + 20]
		push dword ptr[esp + 0Ch + 8]
		push dword ptr[esp + 10h + 4]
		call hooked_createmove
		pop ebx
		retn 0Ch
	}
}