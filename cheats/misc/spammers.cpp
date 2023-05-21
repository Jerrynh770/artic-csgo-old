#include "spammers.h"

void spammers::clan_tag()
{
    auto apply = [](const char* tag) -> void
    {
        using Fn = int(__fastcall*)(const char*, const char*);
        static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

        fn(tag, tag);
    };

    static auto removed = false;

    if (!g_cfg.misc.clantag_spammer && !removed)
    {
        removed = true;
        apply(crypt_str(""));
        return;
    }

    auto nci = m_engine()->GetNetChannelInfo();

    if (!nci)
        return;

    static auto time = -1;

    auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)m_globals()->m_tickcount; //-V807
    auto intervals = 0.3f / m_globals()->m_intervalpertick;

    if (g_cfg.misc.clantag_spammer && g_cfg.misc.clantags_mode == 0)
    {
        auto main_time = (int)(ticks / intervals) % 13;

        if (main_time != time && !m_clientstate()->iChokedCommands)
        {
            auto tag = crypt_str("");

            switch (main_time)
            {
            case 0:
                tag = crypt_str("artic");
                break;
            case 1:
                tag = crypt_str("|artic");
                break;
            case 2:
                tag = crypt_str("a|rtic"); //ARTIC CLANTAG
                break;
            case 3:
                tag = crypt_str("ar|tic");
                break;
            case 4:
                tag = crypt_str("art|ic");
                break;
            case 5:
                tag = crypt_str("arti|c");
                break;
            case 6:
                tag = crypt_str("artic|");
                break;
            case 7:
                tag = crypt_str("arti|c");
                break;
            case 8:
                tag = crypt_str("art|ic");
                break;
            case 9:
                tag = crypt_str("ar|tic");
                break;
            case 10:
                tag = crypt_str("a|rtic");
                break;
            case 11:
                tag = crypt_str("|artic");
                break;
            case 12:
                tag = crypt_str("artic");
                break;
            }

            apply(tag);
            time = main_time;
        }
        removed = false;
    }
    else if (g_cfg.misc.clantag_spammer && g_cfg.misc.clantags_mode == 1)
    {
        auto main_time = (int)(ticks / intervals) % 19;

        if (main_time != time && !m_clientstate()->iChokedCommands)
        {
            auto tag = crypt_str(u8"");

            switch (main_time)
            {
            case 0:
                tag = crypt_str(u8"quizhook");
                break;
            case 1:
                tag = crypt_str(u8"|quizhook"); // quizhook
                break;
            case 2:
                tag = crypt_str(u8"q|uizhook");
                break;
            case 3:
                tag = crypt_str(u8"qu|izhook");
                break;
            case 4:
                tag = crypt_str(u8"qui|zhook");
                break;
            case 5:
                tag = crypt_str(u8"quiz|hook");
                break;
            case 6:
                tag = crypt_str(u8"quizh|ook");
                break;
            case 7:
                tag = crypt_str(u8"quizho|ok");
                break;
            case 8:
                tag = crypt_str(u8"quizhoo|k");
                break;
            case 9:
                tag = crypt_str(u8"quizhook|");
                break;
            case 10:
                tag = crypt_str(u8"quizhoo|k");
                break;
            case 11:
                tag = crypt_str(u8"quizho|ok");
                break;
            case 12:
                tag = crypt_str(u8"quizh|ook");
                break;
            case 13:
                tag = crypt_str(u8"quiz|hook");
                break;
            case 14:
                tag = crypt_str(u8"qui|zhook");
                break;
            case 15:
                tag = crypt_str(u8"qu|izhook");
                break;
            case 16:
                tag = crypt_str(u8"q|uizhook");
                break;
            case 17:
                tag = crypt_str(u8"|quizhook");
                break;
            case 18:
                tag = crypt_str(u8"quizhook");
                break;
            }
            apply(tag);
            time = main_time;
        }
        removed = false;
    }
}