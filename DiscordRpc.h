#pragma once
#include "discord-rpc/win32-static/include/discord_rpc.h"
#include "discord-rpc/win32-static/include/discord_register.h"
#include <Windows.h>

class Discord {
public:
    void Initialize();
    void Update();
};