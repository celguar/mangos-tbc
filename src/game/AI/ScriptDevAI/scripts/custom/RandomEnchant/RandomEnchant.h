#pragma once

#include "AI/ScriptDevAI/scripts/custom/CustomBase/ScriptPlayer.h"
#include "Entities/Player.h"

class RandomEnchant : public ScriptPlayer
{
public:
	RandomEnchant();

	// catch by any loot function (loot / craft / additem / etc)
	// not called but exchange etc where the item already exist in world
	virtual void OnLootItem(Player* player, Item* item, uint32 count) override;

	// Called when a player logs in.
	virtual void OnLogin(Player* player) override;

private:
	void rollEnchant(Player* player, Item* item);
	uint32 getRandomEnchantment(Item* item);

private:
	struct item_enchantment_random_tiers
	{
		uint32 enchant_id;
		uint8 tier;
		std::string _class;
		uint32 exlusive_sub_class;
	};
	std::vector<item_enchantment_random_tiers> _enchants;
	std::vector<uint32> _chances;
	ItemQualities _quality_min, _quality_max;
};