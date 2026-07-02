/* ArcLight Engine - Crafting System
 * Developer: Stephen
 * Handles item crafting, recipes, and crafting queues.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

struct CraftingIngredient {
	std::string itemID;
	int quantity = 1;
};

struct CraftingRecipe {
	std::string id;
	std::string name;
	std::string description;
	std::string category; // e.g., "Weapons", "Armor", "Buildings"
	std::vector<CraftingIngredient> ingredients;
	std::string resultItemID;
	int resultQuantity = 1;
	float craftTime = 5.0f; // seconds
	int requiredLevel = 1;
	bool isKnownByDefault = false;
};

struct CraftingQueueItem {
	std::string recipeID;
	float timeRemaining = 0;
	int quantity = 1;
};

class CraftingSystem {
public:
	void RegisterRecipe(const CraftingRecipe& recipe) {
		recipes[recipe.id] = recipe;
		if (recipe.isKnownByDefault) knownRecipes.insert(recipe.id);
	}

	bool LearnRecipe(const std::string& recipeID) {
		if (recipes.find(recipeID) == recipes.end()) return false;
		knownRecipes.insert(recipeID);
		return true;
	}

	bool IsRecipeKnown(const std::string& recipeID) const {
		return knownRecipes.find(recipeID) != knownRecipes.end();
	}

	bool CanCraft(const std::string& recipeID, int playerLevel,
		const std::function<bool(const std::string&, int)>& hasIngredients) const
	{
		auto it = recipes.find(recipeID);
		if (it == recipes.end()) return false;
		if (it->second.requiredLevel > playerLevel) return false;
		for (auto& ing : it->second.ingredients) {
			if (!hasIngredients(ing.itemID, ing.quantity)) return false;
		}
		return true;
	}

	void StartCrafting(const std::string& recipeID) {
		auto it = recipes.find(recipeID);
		if (it == recipes.end()) return;

		CraftingQueueItem item;
		item.recipeID = recipeID;
		item.timeRemaining = it->second.craftTime;
		queue.push_back(item);
	}

	void Update(float dt) {
		for (auto& item : queue) {
			item.timeRemaining -= dt;
			if (item.timeRemaining <= 0) {
				auto it = recipes.find(item.recipeID);
				if (it != recipes.end() && onCraftComplete) {
					onCraftComplete(it->second, item.quantity);
				}
				item.timeRemaining = -1; // mark for removal
			}
		}
		queue.erase(
			std::remove_if(queue.begin(), queue.end(),
				[](const CraftingQueueItem& i) { return i.timeRemaining < 0; }),
			queue.end()
		);
	}

	std::vector<CraftingRecipe> GetRecipesByCategory(const std::string& category) const {
		std::vector<CraftingRecipe> result;
		for (auto& [id, recipe] : recipes) {
			if (recipe.category == category && IsRecipeKnown(id)) {
				result.push_back(recipe);
			}
		}
		return result;
	}

	std::function<void(const CraftingRecipe&, int)> onCraftComplete;

private:
	std::unordered_map<std::string, CraftingRecipe> recipes;
	std::unordered_set<std::string> knownRecipes;
	std::vector<CraftingQueueItem> queue;
};

} // namespace arclight
