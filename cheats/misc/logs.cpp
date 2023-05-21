// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "logs.h"

void eventlogs::paint_traverse()
{
	if (logs.empty())
		return;

	auto last_y = -19;

	const auto font = fonts[WATERMARK];
	const auto name_font = fonts[WATERMARK];

	for (unsigned int i = 0; i < logs.size(); i++) {
		auto& log = logs.at(i);

		if (util::epoch_time() - log.log_time > 5500) {
			float factor = (log.log_time + 5900) - util::epoch_time();
			factor /= 1000;

			auto opacity = int(255 * factor);

			if (opacity < 2) {
				logs.erase(logs.begin() + i);
				continue;
			}
			log.x -= 20 * (factor * 1.25);
			log.y -= 0 * (factor * 1.25);
		}

		const auto text = log.message.c_str();

		const auto color = log.color;
		auto name_size = render::get().text_width(name_font, text);
		float logs[4] = { g_cfg.misc.log_color.r() / 255, g_cfg.misc.log_color.g() / 255, g_cfg.misc.log_color.b() / 255, 1.f };

		render::get().rect_filled(log.x + 5, last_y + log.y + 20, name_size + 18, 18, Color(15, 15, 15, 155));

		render::get().gradient(log.x + 5, last_y + log.y + 20, (name_size + 18)/2, 2, Color(48, 206, 230, 255), Color(220, 57, 218, 255), GRADIENT_HORIZONTAL);
		render::get().gradient(log.x + 5 + (name_size + 18) / 2, last_y + log.y + 20, (name_size + 18)/2, 2, Color(220, 57, 218, 255), Color(220, 232, 47, 255), GRADIENT_HORIZONTAL);

		//render::get().circle_filled(log.x + 12, last_y + log.y + 30, 5, 3, color); // âàòåðìàðê åáëàíû ÍÈÊÎÃÄÀ ÍÅ ÞÇÀÒÜ ËÎÌÀÅÒ ÂÑ¨ ÏÐÎÑÒÎ Â ÏÈÇÄÓ

		render::get().text(font, log.x + 20, last_y + log.y + 23, Color(255, 255, 255, 255), HFONT_CENTERED_NONE, text);


		last_y += 25;
	}
}

void eventlogs::events(IGameEvent* event) 
{
	static auto get_hitgroup_name = [](int hitgroup) -> std::string
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			return crypt_str("head");
		case HITGROUP_CHEST:
			return crypt_str("chest");
		case HITGROUP_STOMACH:
			return crypt_str("stomach");
		case HITGROUP_LEFTARM:
			return crypt_str("left arm");
		case HITGROUP_RIGHTARM:
			return crypt_str("right arm");
		case HITGROUP_LEFTLEG:
			return crypt_str("left leg");
		case HITGROUP_RIGHTLEG:
			return crypt_str("right leg");
		default:
			return crypt_str("generic");
		}
	};

	if (g_cfg.misc.events_to_log[EVENTLOG_HIT] && !strcmp(event->GetName(), crypt_str("player_hurt")))
	{
		auto userid = event->GetInt(crypt_str("userid")), attacker = event->GetInt(crypt_str("attacker"));

		if (!userid || !attacker) 
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid), attacker_id = m_engine()->GetPlayerForUserID(attacker); //-V807

		player_info_t userid_info, attacker_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info)) 
			return;

		if (!m_engine()->GetPlayerInfo(attacker_id, &attacker_info)) 
			return;

		auto m_victim = static_cast<player_t *>(m_entitylist()->GetClientEntity(userid_id));

		std::stringstream ss;

		if (attacker_id == m_engine()->GetLocalPlayer() && userid_id != m_engine()->GetLocalPlayer()) 
		{
			ss << crypt_str("Hit ") << userid_info.szName << crypt_str(" in the ") << get_hitgroup_name(event->GetInt(crypt_str("hitgroup"))) << crypt_str(" for ") << event->GetInt(crypt_str("dmg_health"));
			ss << crypt_str(" damage (") << event->GetInt(crypt_str("health")) << crypt_str(" health remaining)");

			add(ss.str());
		}
		else if (userid_id == m_engine()->GetLocalPlayer() && attacker_id != m_engine()->GetLocalPlayer()) 
		{
			ss << crypt_str("Take ") << event->GetInt(crypt_str("dmg_health")) << crypt_str(" damage from ") << attacker_info.szName << crypt_str(" in the ") << get_hitgroup_name(event->GetInt(crypt_str("hitgroup")));
			ss << crypt_str(" (") << event->GetInt(crypt_str("health")) << crypt_str(" health remaining)");

			add(ss.str());
		}
	}

	if (g_cfg.misc.events_to_log[EVENTLOG_ITEM_PURCHASES] && !strcmp(event->GetName(), crypt_str("item_purchase")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid) 
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info)) 
			return;

		auto m_player = static_cast<player_t *>(m_entitylist()->GetClientEntity(userid_id));

		if (!g_ctx.local() || !m_player) 
			return;

		if (g_ctx.local() == m_player)
			g_ctx.globals.should_buy = 0;

		if (m_player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) 
			return;

		std::string weapon = event->GetString(crypt_str("weapon"));
		
		std::stringstream ss;
		ss << userid_info.szName << crypt_str(" bought ") << weapon;

		add(ss.str(), true, 3);
	}

	if (g_cfg.misc.events_to_log[EVENTLOG_BOMB] && !strcmp(event->GetName(), crypt_str("bomb_beginplant")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid) 
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info)) 
			return;

		auto m_player = static_cast<player_t *>(m_entitylist()->GetClientEntity(userid_id));

		if (!m_player) 
			return;

		std::stringstream ss;
		ss << userid_info.szName << crypt_str(" has began planting the bomb");

		add(ss.str());
	}

	if (g_cfg.misc.events_to_log[EVENTLOG_BOMB] && !strcmp(event->GetName(), crypt_str("bomb_begindefuse")))
	{
		auto userid = event->GetInt(crypt_str("userid"));

		if (!userid) 
			return;

		auto userid_id = m_engine()->GetPlayerForUserID(userid);

		player_info_t userid_info;

		if (!m_engine()->GetPlayerInfo(userid_id, &userid_info)) 
			return;

		auto m_player = static_cast<player_t *>(m_entitylist()->GetClientEntity(userid_id));

		if (!m_player) 
			return;

		std::stringstream ss;
		ss << userid_info.szName << crypt_str(" has began defusing the bomb ") << (event->GetBool(crypt_str("haskit")) ? crypt_str("with defuse kit") : crypt_str("without defuse kit"));

		add(ss.str());
	}

}

void eventlogs::add(std::string text, bool full_display, int colorz)
{
	if (colorz > 0) {
		switch (colorz)
		{
		case 1:
			logs.emplace_front(loginfo_t(util::epoch_time(), text, Color(10, 119, 255, 255)));
			break;
		case 2:
			logs.emplace_front(loginfo_t(util::epoch_time(), text, Color(255, 61, 61, 255)));
			break;
		case 3:
			logs.emplace_front(loginfo_t(util::epoch_time(), text, Color(19, 255, 10, 255)));
			break;
		}
	}
	if(colorz == 0)
		logs.emplace_front(loginfo_t(util::epoch_time(), text, g_cfg.misc.log_color));

	if (!full_display)
		return;

	if (g_cfg.misc.log_output[EVENTLOG_OUTPUT_CONSOLE])
	{
		last_log = true;

#if RELEASE
#if BETA
		m_cvar()->ConsoleColorPrintf(g_cfg.misc.log_color, crypt_str("[ artic beta ] ")); //-V807
#else
		m_cvar()->ConsoleColorPrintf(g_cfg.misc.log_color, crypt_str("[ artic ] "));
#endif
#else
		m_cvar()->ConsoleColorPrintf(g_cfg.misc.log_color, crypt_str("[ artic.su ] ")); //-V807
#endif

		m_cvar()->ConsoleColorPrintf(Color::White, text.c_str());
		m_cvar()->ConsolePrintf(crypt_str("\n"));
	}

	if (g_cfg.misc.log_output[EVENTLOG_OUTPUT_CHAT])
	{
		static CHudChat* chat = nullptr;

		if (!chat)
			chat = util::FindHudElement <CHudChat> (crypt_str("CHudChat"));

#if RELEASE
#if BETA
		auto log = crypt_str("[ \x0C artic beta \x01] ") + text;
		chat->chat_print(log.c_str());
#else
		auto log = crypt_str("[ \x0C artic \x01] ") + text;
		chat->chat_print(log.c_str());
#endif
#else
		auto log = crypt_str("[\x0C artic.su \x01 ] ") + text;
		chat->chat_print(log.c_str());
#endif
	}
}