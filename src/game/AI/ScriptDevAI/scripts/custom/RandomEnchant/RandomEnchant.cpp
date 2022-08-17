#include "RandomEnchant.h"
#include "Database/Database.h"
#include "Config/Config.h"
#include "Chat/Chat.h"

#include <algorithm>

RandomEnchant::RandomEnchant()
	: ScriptPlayer("random_enchant")
	, _enchants{}
	, _chances{}
	, _quality_min{}
	,_quality_max{}
{
	_isActive = sConfig.GetBoolDefault("RandomEnchant.Enabled", false);

	if (_isActive)
	{
		_quality_min = static_cast<ItemQualities>(sConfig.GetIntDefault("RandomEnchant.MinQuality", ItemQualities::ITEM_QUALITY_NORMAL));
		_quality_max = static_cast<ItemQualities>(sConfig.GetIntDefault("RandomEnchant.MaxQuality", ItemQualities::ITEM_QUALITY_LEGENDARY));

		const std::string& random_enchant_chances = sConfig.GetStringDefault("RandomEnchant.RandomEnchantChances", "55, 75, 90");
		uint32 random_enchant_count = static_cast<uint32>(sConfig.GetIntDefault("RandomEnchant.RandomEnchantCount", 3));
		if (random_enchant_count > 3) random_enchant_count = 3;

		const auto str = StrSplit(random_enchant_chances, ",");
		if (str.size() != random_enchant_count) random_enchant_count = str.size();

		for (size_t i = 0; i < str.size(); ++i)
		{
			_chances.push_back(std::atoll(str[i].c_str()));
		}

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
		sLog.outString("RandomEnchant module loaded.\nMinimum quality: {%d} Maximum quality: {%d} Chances: {%s} Maximum enchant count: {%d}.", _quality_min, _quality_max, random_enchant_chances, random_enchant_count);
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

	uint32 Quality = pItem->GetProto()->Quality;
	uint32 Class = pItem->GetProto()->Class;
	if ((Quality > (uint32)_quality_max || Quality < (uint32)_quality_min) ||
		(Class != ItemClass::ITEM_CLASS_WEAPON && Class != ItemClass::ITEM_CLASS_ARMOR))
	{
		return;
	}

	rollEnchant(pPlayer, pItem);
}

void RandomEnchant::rollEnchant(Player* player, Item* item)
{
	uint32 slotEnch[3] = { 0, 1, 5 };
	uint32 randEnchantCount = 0;
	uint32 cur_slot = 0;

	for (const auto& slot_chance : _chances)
	{
		if (rand_chance() >= slot_chance)
		{
			const auto& enchant_result = getRandomEnchantment(item);
			if (enchant_result != -1)
			{
				//Make sure enchantment id exists
				if (sSpellItemEnchantmentStore.LookupEntry(enchant_result))
				{
					player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[cur_slot]), false);
					item->SetEnchantment(EnchantmentSlot(slotEnch[cur_slot]), enchant_result, 0, 0);
					player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[cur_slot]), true);
					++randEnchantCount;
					++cur_slot;
				}
			}
			else break;
		}
	}

	ChatHandler chathandle = ChatHandler(player->GetSession());
	if (randEnchantCount > 0)
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