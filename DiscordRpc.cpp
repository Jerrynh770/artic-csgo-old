#include "DiscordRpc.h"
#include "discord-rpc/win32-static/include/discord_rpc.h"

void Discord::Initialize()
{
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));
    Discord_Initialize("1109844249173819434", &Handle, 1, NULL);
}

void Discord::Update()
{
    DiscordRichPresence discord;
    memset(&discord, 0, sizeof(discord));
    discord.details = u8"Artic v1.2.2";
    discord.state = u8"t.me/arsnwise";
    discord.largeImageKey = "iconrpc";
    discord.smallImageKey = "iconrpc";
    Discord_UpdatePresence(&discord);
}