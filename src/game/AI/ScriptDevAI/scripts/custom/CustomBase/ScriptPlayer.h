#pragma once

#include "AI/ScriptDevAI/scripts/custom/custom_script.h"

class ScriptPlayer : public CustomScript
{
public:
    ScriptPlayer(const char* name);

public:
    // Called when a player logs in.
    virtual void OnLogin(Player* /*player*/) { }

    // Called when a player logs out.
    virtual void OnLogout(Player* /*player*/) { }

    //After looting item
    virtual void OnLootItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/) { }
};