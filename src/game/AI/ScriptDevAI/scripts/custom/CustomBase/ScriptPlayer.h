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

    // After looting item
    virtual void OnLootItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/) { }

    // Before Player::Update
    virtual void OnPreUpdate(Player* /*player*/, const uint32 /*diff*/) { }

    // Inside Player::Update
    virtual void OnUpdate(Player* /*player*/, const uint32 /*diff*/) { }

    // After Player::Update
    virtual void OnPostUpdate(Player* /*player*/, const uint32 /*diff*/) { }

    // On unit killed (monster - pvp - anything)
    virtual void OnKillUnit(Player* /*player*/, const Unit* /*unit*/) { }

    // Whenever the player as killed an other, is called on grey kill too
    virtual void OnPvPKill(Player* /*player*/, Player* /*pVictim*/) { }

    // On xp is gain xp_gain is the total xp the player is gonna get with bonus
    virtual void OnXpGain(Player* /*player*/, const uint32& /*xp_gain*/) { }

    // Whenever the player levelup
    virtual void OnLevelUp(Player* /*player*/) { }
};