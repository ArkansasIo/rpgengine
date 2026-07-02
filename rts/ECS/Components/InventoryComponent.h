/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Inventory Component for MMORPG/RTS hybrid gameplay.
 * Supports equipment slots, item stacks, and loot tables.
 */

#pragma once

#include "../Entity.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace arclight {

enum class ItemRarity {
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
};

enum class EquipmentSlot {
	None,
	Head,
	Chest,
	Legs,
	Feet,
	Hands,
	MainHand,
	OffHand,
	Ring1,
	Ring2,
	Neck,
	Back,
};

struct ItemDef {
	int itemID = 0;
	std::string name;
	std::string description;
	ItemRarity rarity = ItemRarity::Common;
	int maxStack = 1;
	float weight = 1.0f;
	bool isEquippable = false;
	EquipmentSlot equipSlot = EquipmentSlot::None;
	bool isConsumable = false;
	bool isQuestItem = false;

	// Stat bonuses when equipped
	float bonusHealth = 0.0f;
	float bonusDamage = 0.0f;
	float bonusArmor = 0.0f;
	float bonusSpeed = 0.0f;
	float bonusRange = 0.0f;
};

struct InventorySlot {
	ItemDef item;
	int count = 0;
	bool isEmpty() const { return count <= 0; }
};

struct InventoryComponent : public IComponent {
	static const int MAX_SLOTS = 40;
	static const int MAX_EQUIPMENT_SLOTS = 12;

	std::vector<InventorySlot> slots;
	std::unordered_map<EquipmentSlot, ItemDef> equipment;
	int capacity = MAX_SLOTS;
	float maxWeight = 200.0f;
	float currentWeight = 0.0f;
	bool isLootable = true;

	// Callbacks
	std::function<void(const ItemDef&, int)> onItemAdded;
	std::function<void(const ItemDef&, int)> onItemRemoved;
	std::function<void(const ItemDef&, EquipmentSlot)> onItemEquipped;
	std::function<void(const ItemDef&, EquipmentSlot)> onItemUnequipped;

	void Initialize() {
		slots.resize(capacity);
		equipment.clear();
	}

	// Add item to inventory, returns amount that couldn't fit
	int AddItem(const ItemDef& item, int count = 1) {
		int remaining = count;

		// Try to stack with existing
		if (item.maxStack > 1) {
			for (auto& slot : slots) {
				if (remaining <= 0) break;
				if (slot.item.itemID == item.itemID && slot.count < item.maxStack) {
					int canAdd = std::min(remaining, item.maxStack - slot.count);
					slot.count += canAdd;
					remaining -= canAdd;
					currentWeight += item.weight * canAdd;
					if (onItemAdded) onItemAdded(item, canAdd);
				}
			}
		}

		// Fill empty slots
		for (auto& slot : slots) {
			if (remaining <= 0) break;
			if (slot.isEmpty()) {
				slot.item = item;
				int canAdd = std::min(remaining, item.maxStack);
				slot.count = canAdd;
				remaining -= canAdd;
				currentWeight += item.weight * canAdd;
				if (onItemAdded) onItemAdded(item, canAdd);
			}
		}

		return remaining;
	}

	// Remove item by ID, returns amount removed
	int RemoveItem(int itemID, int count = 1) {
		int removed = 0;
		for (auto& slot : slots) {
			if (removed >= count) break;
			if (slot.item.itemID == itemID && !slot.isEmpty()) {
				int canRemove = std::min(count - removed, slot.count);
				slot.count -= canRemove;
				removed += canRemove;
				currentWeight -= slot.item.weight * canRemove;
				if (onItemRemoved) onItemRemoved(slot.item, canRemove);
				if (slot.count <= 0) {
					slot.item = ItemDef();
					slot.count = 0;
				}
			}
		}
		return removed;
	}

	bool HasItem(int itemID, int count = 1) const {
		int total = 0;
		for (auto& slot : slots) {
			if (slot.item.itemID == itemID) {
				total += slot.count;
				if (total >= count) return true;
			}
		}
		return false;
	}

	int GetItemCount(int itemID) const {
		int total = 0;
		for (auto& slot : slots) {
			if (slot.item.itemID == itemID) total += slot.count;
		}
		return total;
	}

	// Equipment
	bool EquipItem(const ItemDef& item) {
		if (!item.isEquippable) return false;
		if (item.equipSlot == EquipmentSlot::None) return false;

		// Unequip existing
		if (equipment.find(item.equipSlot) != equipment.end()) {
			UnequipItem(item.equipSlot);
		}

		equipment[item.equipSlot] = item;
		RemoveItem(item.itemID, 1);
		if (onItemEquipped) onItemEquipped(item, item.equipSlot);
		return true;
	}

	bool UnequipItem(EquipmentSlot slot) {
		auto it = equipment.find(slot);
		if (it == equipment.end()) return false;

		ItemDef item = it->second;
		int leftover = AddItem(item, 1);
		if (leftover == 0) {
			equipment.erase(it);
			if (onItemUnequipped) onItemUnequipped(item, slot);
			return true;
		}
		return false; // inventory full
	}

	// Get total stat bonuses from equipment
	float GetTotalBonusHealth() const {
		float total = 0.0f;
		for (auto& [slot, item] : equipment) total += item.bonusHealth;
		return total;
	}

	float GetTotalBonusDamage() const {
		float total = 0.0f;
		for (auto& [slot, item] : equipment) total += item.bonusDamage;
		return total;
	}

	float GetTotalBonusArmor() const {
		float total = 0.0f;
		for (auto& [slot, item] : equipment) total += item.bonusArmor;
		return total;
	}

	bool IsOverweight() const { return currentWeight > maxWeight; }
	float GetWeightPercent() const { return maxWeight > 0 ? currentWeight / maxWeight : 0.0f; }

	void Clear() {
		slots.clear();
		slots.resize(capacity);
		equipment.clear();
		currentWeight = 0.0f;
	}
};

} // namespace arclight
