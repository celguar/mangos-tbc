#include "RandomEnchant.h"
#include "Database/Database.h"
#include "Config/Config.h"
#include "Chat/Chat.h"

#include <algorithm>

RandomEnchant::RandomEnchant()
	: ScriptPlayer("random_enchant")
{
	_isActive = sConfig.GetBoolDefault("RandomCraft.Enabled", false);

	if (_isActive)
	{
		QueryResult* result = WorldDatabase.Query("SELECT enchantID, tier, class, exclusiveSubClass FROM item_enchantment_random_tiers");
		{
			do
			{
				Field* fields = result->Fetch();

				_enchants.emplace_back(item_enchantment_random_tiers{
					fields[0].GetUInt32(),
					fields[1].GetUInt8(),
					fields[2].GetCppString(),
					fields[3].GetUInt32()
					});
			} while (result && result->NextRow());
		}

		RegisterSelf();
		sLog.outString("RandomEnchant loaded.");
	}
}

void RandomEnchant::OnLogin(Player* player)
{
	if (!_isActive)
		return;

	ChatHandler chathandle = ChatHandler(player->GetSession());
	chathandle.PSendSysMessage("This server is running |cff4CFF00RandomEnchant |rmodule");
}

// catch by any loot function (loot / craft / additem / etc)
// not called but exchange etc where the item already exist in world
void RandomEnchant::OnLootItem(Player* pPlayer, Item* pItem, uint32 /*count*/)
{
	// -- do not enchant at all as we are on cleaning player
	if (!_isActive)
		return;
	if (!pPlayer || !pItem)
		return;

	rollEnchant(pPlayer, pItem);
}

void RandomEnchant::rollEnchant(Player* player, Item* item)
{
	uint32 Quality = item->GetProto()->Quality;
	uint32 Class = item->GetProto()->Class;

	if (
		(Quality > ItemQualities::ITEM_QUALITY_LEGENDARY || Quality < ItemQualities::ITEM_QUALITY_NORMAL) ||
		(Class != ItemClass::ITEM_CLASS_WEAPON && Class != ItemClass::ITEM_CLASS_ARMOR)
		)
	{
		return;
	}

	uint32 slotRand[3] = { -1, -1, -1 };
	uint32 slotEnch[3] = { 0, 1, 5 };
	uint32 randEnchantCount = 0;

	if (rand_chance() >= 60.0)
		slotRand[0] = getRandomEnchantment(item);
	if (slotRand[0] != -1)
	{
		++randEnchantCount;
		if (rand_chance() >= 70.0)
		{
			slotRand[1] = getRandomEnchantment(item);
			++randEnchantCount;
		}
		if (slotRand[1] != -1)
		{
			if (rand_chance() >= 75.0)
			{
				slotRand[2] = getRandomEnchantment(item);
				++randEnchantCount;
			}
		}
	}
	for (int i = 0; i < 2; i++)
	{
		if (slotRand[i] != -1)
		{
			//Make sure enchantment id exists
			if (sSpellItemEnchantmentStore.LookupEntry(slotRand[i]))
			{
				player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), false);
				item->SetEnchantment(EnchantmentSlot(slotEnch[i]), slotRand[i], 0, 0);
				player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), true);
			}
		}
	}
	ChatHandler chathandle = ChatHandler(player->GetSession());
	if (slotRand[0] != -1)
		chathandle.PSendSysMessage("Newly Acquired |cffFF0000 %s |rhas received|cffFF0000 %d |rrandom enchantment!", item->GetProto()->Name1, randEnchantCount);
}

uint32 RandomEnchant::getRandomEnchantment(Item* item)
{
	uint32 Class = item->GetProto()->Class;
	std::string ClassQueryString = "";
	switch (Class)
	{
	case 2:
		ClassQueryString = "WEAPON";
		break;
	case 4:
		ClassQueryString = "ARMOR";
		break;
	}
	if (ClassQueryString == "")
		return -1;
	uint32 Quality = item->GetProto()->Quality;
	int rarityRoll = -1;
	switch (Quality)
	{
	case 0://grey
		rarityRoll = rand_norm() * 25;
		break;
	case 1://white
		rarityRoll = rand_norm() * 50;
		break;
	case 2://green
		rarityRoll = 45 + (rand_norm() * 20);
		break;
	case 3://blue
		rarityRoll = 65 + (rand_norm() * 15);
		break;
	case 4://purple
		rarityRoll = 80 + (rand_norm() * 14);
		break;
	case 5://orange
		rarityRoll = 93;
		break;
	}
	if (rarityRoll < 0)
		return -1;
	uint8 tier = 0;
	if (rarityRoll <= 44)
		tier = 1;
	else if (rarityRoll <= 64)
		tier = 2;
	else if (rarityRoll <= 79)
		tier = 3;
	else if (rarityRoll <= 92)
		tier = 4;
	else
		tier = 5;

	std::vector<RandomEnchant::item_enchantment_random_tiers> _shuffle;
	for (const auto& enchant : _enchants)
	{
		if (
			(enchant.exlusive_sub_class == 0 || ClassQueryString == enchant._class) ||
			(enchant.exlusive_sub_class == item->GetProto()->SubClass || enchant._class == "ANY")
			)
		{
			if (tier == enchant.tier)
				_shuffle.emplace_back(enchant);
		}
	}

	if (_shuffle.empty())
	{
		sLog.outError("RandomEnchant::getRandEnchantment returned -1 as _shuffle list was empty");
		return -1;
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(_shuffle.begin(), _shuffle.end(), std::default_random_engine(seed));

	return _shuffle.front().enchant_id;
}

void AddSC_random_enchant()
{
    RandomEnchant* pNewScript = new RandomEnchant;
}