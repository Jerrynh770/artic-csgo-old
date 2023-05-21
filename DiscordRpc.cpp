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
    discord.details = "Playing with artic maan";
    discord.state = "With artic.su";
    discord.largeImageKey = "iconrpc";
    discord.smallImageKey = "iconrpc";
    Discord_UpdatePresence(&discord);
}