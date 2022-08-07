/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "Common.h"
#include "Server/DBCStructure.h"
#include "Server/SQLStorages.h"
#include "Spells/SpellMgr.h"

#include <functional>

class Player;
class Creature;
class UnitAI;
class InstanceData;
class Quest;
class Item;
class GameObject;
class SpellCastTargets;
class Map;
class Unit;
class WorldObject;
class Aura;
class Object;
class ObjectGuid;
class GameObjectAI;

// *********************************************************
// ************** Some defines used globally ***************

// Basic defines
#define VISIBLE_RANGE       (166.0f)                        // MAX visible range (size of grid)
#define DEFAULT_TEXT        "<ScriptDev2 Text Entry Missing!>"

/* Escort Factions
 * TODO: find better naming and definitions.
 * N=Neutral, A=Alliance, H=Horde.
 * NEUTRAL or FRIEND = Hostility to player surroundings (not a good definition)
 * ACTIVE or PASSIVE = Hostility to environment surroundings.
 */
enum EscortFaction
{
    FACTION_ESCORT_A_NEUTRAL_PASSIVE    = 10,
    FACTION_ESCORT_H_NEUTRAL_PASSIVE    = 33,
    FACTION_ESCORT_N_NEUTRAL_PASSIVE    = 113,

    FACTION_ESCORT_A_NEUTRAL_ACTIVE     = 231,
    FACTION_ESCORT_H_NEUTRAL_ACTIVE     = 232,
    FACTION_ESCORT_N_NEUTRAL_ACTIVE     = 250,

    FACTION_ESCORT_N_FRIEND_PASSIVE     = 290,
    FACTION_ESCORT_N_FRIEND_ACTIVE      = 495,

    FACTION_ESCORT_A_PASSIVE            = 774,
    FACTION_ESCORT_H_PASSIVE            = 775,

    FACTION_ESCORT_N_ACTIVE             = 1986,
    FACTION_ESCORT_H_ACTIVE             = 2046
};

// *********************************************************
// ************* Some structures used globally *************

template <typename T>
UnitAI* GetNewAIInstance(Creature* creature)
{
    return new T(creature);
}

template <typename T>
GameObjectAI* GetNewAIInstance(GameObject* gameobject)
{
    return new T(gameobject);
}

template <typename T>
InstanceData* GetNewInstanceScript(Map* map)
{
    return new T(map);
}

class Script
{
public:
    Script() :
        pGossipHello(nullptr), pGossipHelloGO(nullptr), pGossipSelect(nullptr), pGossipSelectGO(nullptr),
        pGossipSelectWithCode(nullptr), pGossipSelectGOWithCode(nullptr),
        pDialogStatusNPC(nullptr), pDialogStatusGO(nullptr),
        pQuestAcceptNPC(nullptr), pQuestAcceptGO(nullptr), pQuestAcceptItem(nullptr),
        pQuestRewardedNPC(nullptr), pQuestRewardedGO(nullptr),
        pGOUse(nullptr), pItemUse(nullptr), pItemLoot(nullptr), pAreaTrigger(nullptr), pProcessEventId(nullptr),
        pEffectDummyNPC(nullptr), pEffectDummyGO(nullptr), pEffectDummyItem(nullptr), pEffectScriptEffectNPC(nullptr),
        pEffectAuraDummy(nullptr), pTrapSearching(nullptr), GetGameObjectAI(nullptr), GetAI(nullptr), GetInstanceData(nullptr)
    {}

    

    bool (*pGossipHello)(Player*, Creature*);
    bool (*pGossipHelloGO)(Player*, GameObject*);
    bool (*pGossipSelect)(Player*, Creature*, uint32, uint32);
    bool (*pGossipSelectGO)(Player*, GameObject*, uint32, uint32);
    bool (*pGossipSelectWithCode)(Player*, Creature*, uint32, uint32, const char*);
    bool (*pGossipSelectGOWithCode)(Player*, GameObject*, uint32, uint32, const char*);
    uint32(*pDialogStatusNPC)(const Player*, const Creature*);
    uint32(*pDialogStatusGO)(const Player*, const GameObject*);
    bool (*pQuestAcceptNPC)(Player*, Creature*, Quest const*);
    bool (*pQuestAcceptGO)(Player*, GameObject*, Quest const*);
    bool (*pQuestAcceptItem)(Player*, Item*, Quest const*);
    bool (*pQuestRewardedNPC)(Player*, Creature*, Quest const*);
    bool (*pQuestRewardedGO)(Player*, GameObject*, Quest const*);
    bool (*pGOUse)(Player*, GameObject*);
    bool (*pItemUse)(Player*, Item*, SpellCastTargets const&);
    bool (*pItemLoot)(Player*, Item*, bool);
    bool (*pAreaTrigger)(Player*, AreaTriggerEntry const*);
    bool (*pProcessEventId)(uint32, Object*, Object*, bool);
    bool (*pEffectDummyNPC)(Unit*, uint32, SpellEffectIndex, Creature*, ObjectGuid);
    bool (*pEffectDummyGO)(Unit*, uint32, SpellEffectIndex, GameObject*, ObjectGuid);
    bool (*pEffectDummyItem)(Unit*, uint32, SpellEffectIndex, Item*, ObjectGuid);
    bool (*pEffectScriptEffectNPC)(Unit*, uint32, SpellEffectIndex, Creature*, ObjectGuid);
    bool (*pEffectAuraDummy)(const Aura*, bool);

    std::function<bool(Unit*)>* pTrapSearching;
    GameObjectAI* (*GetGameObjectAI)(GameObject*);
    UnitAI* (*GetAI)(Creature*);
    InstanceData* (*GetInstanceData)(Map*);

    void RegisterSelf(bool bReportError = true);

    std::string_view GetName() const { return Name; }
    std::string Name;

protected:
    virtual void do_not_edit(){}
};

class ScriptDevAIMgr
{
public:
    ScriptDevAIMgr() : m_scriptCount(0) {}
    ~ScriptDevAIMgr();

    void Initialize();
    void LoadScriptNames();
    void CheckScriptNames();
    void LoadAreaTriggerScripts();
    void LoadEventIdScripts();

    
    
public: /* Backport functionnality */
    std::function<bool(Unit*)>* OnTrapSearch(GameObject* go);
    bool OnProcessEvent(uint32 uiEventId, Object* pSource, Object* pTarget, bool bIsStart);
    bool OnAuraDummy(Aura const* pAura, bool bApply);
    void AddScript(uint32 id, Script* script);
    Script* GetScript(uint32 id) const;
    const char* GetScriptName(uint32 id) const { return id < m_scriptNames.size() ? m_scriptNames[id].c_str() : ""; }
    uint32 GetScriptId(const char* name) const;
    uint32 GetScriptIdsCount() const { return m_scriptNames.size(); }
    InstanceData* CreateInstanceData(Map* pMap);
    uint32 GetAreaTriggerScriptId(uint32 triggerId) const;
    uint32 GetEventIdScriptId(uint32 eventId) const;

public: /* CreatureScript */
    bool OnGossipHello(Player* player, Creature* creature);
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestRewarded(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
    uint32 GetDialogStatus(const Player* pPlayer, const Creature* pCreature) const;
    UnitAI* GetCreatureAI(Creature* pCreature) const;

public: /* GameObjectScript */
    bool OnGossipHello(Player* player, GameObject* go);
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest);
    bool OnQuestRewarded(Player* pPlayer, GameObject* pGo, Quest const* pQuest);
    uint32 GetDialogStatus(const Player* pPlayer, const GameObject* pGo) const;
    bool OnGameObjectUse(Player* pPlayer, GameObject* pGo);
    GameObjectAI* GetGameObjectAI(GameObject* gameobject) const;

public: /* ItemScript */
    bool OnQuestAccept(Player* player, Item* item, Quest const* quest);
    bool OnItemUse(Player* player, Item* item, SpellCastTargets const& targets);
    bool OnItemLoot(Player* pPlayer, Item* pItem, bool apply);

public: /* PlayerScript*/
    void OnPlayerLogin(Player* pPlayer);
    void OnPlayerLogout(Player* pPlayer);
    bool OnPlayerItemLoot(Player* pPlayer, Item* pItem);

public: /* AreaTriggerScript */
    bool OnAreaTrigger(Player* pPlayer, AreaTriggerEntry const* atEntry);

public: /* SpellSC */
    bool OnEffectDummy(Unit* pCaster, uint32 spellId, SpellEffectIndex effIndex, Creature* pTarget, ObjectGuid originalCasterGuid);
    bool OnEffectDummy(Unit* pCaster, uint32 spellId, SpellEffectIndex effIndex, GameObject* pTarget, ObjectGuid originalCasterGuid);
    bool OnEffectDummy(Unit* pCaster, uint32 spellId, SpellEffectIndex effIndex, Item* pTarget, ObjectGuid originalCasterGuid);
    bool OnEffectScriptEffect(Unit* pCaster, uint32 spellId, SpellEffectIndex effIndex, Creature* pTarget, ObjectGuid originalCasterGuid);


private:
    typedef std::vector<Script*> SDScriptVec;
    typedef std::vector<std::string> ScriptNameMap;
    typedef std::unordered_map<uint32, uint32> AreaTriggerScriptMap;
    typedef std::unordered_map<uint32, uint32> EventIdScriptMap;

    int                     m_scriptCount;
    SDScriptVec             m_scripts;
    AreaTriggerScriptMap    m_AreaTriggerScripts;
    EventIdScriptMap        m_EventIdScripts;
    ScriptNameMap           m_scriptNames;
};

// *********************************************************
// ************* Some functions used globally **************

// Generic scripting text function
void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget = nullptr, uint32 chatTypeOverride = 0);
void DoBroadcastText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget = nullptr, uint32 chatTypeOverride = 0);
void DoOrSimulateScriptTextForMap(int32 iTextEntry, uint32 uiCreatureEntry, Map* pMap, Creature* pCreatureSource = nullptr, Unit* pTarget = nullptr);


#define sScriptDevAIMgr MaNGOS::Singleton<ScriptDevAIMgr>::Instance()

#endif
