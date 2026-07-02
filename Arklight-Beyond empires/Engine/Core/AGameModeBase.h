/* ArkLight Beyond - AGameModeBase
 * Developer: Stephen
 * Game rules, mode configuration, and player management.
 */
#pragma once
#include "AActor.h"
#include <vector>
#include <string>

namespace ogb {
class APlayerController;
class AGameStateBase;

enum class EGameMode { Sandbox, Campaign, Multiplayer, MMO };

struct GameModeConfig {
    EGameMode mode = EGameMode::Sandbox;
    int maxPlayers = 100;
    bool allowAI = true;
    bool friendlyFire = false;
    float startingCredits = 10000;
    float startingMetal = 5000;
    float startingCrystal = 3000;
    int startingFleetSize = 5;
    bool permadeath = false;
    bool saveEnabled = true;
    float gameSpeed = 1.0f;
    bool pauseOnStart = false;
};

class AGameStateBase : public UObject {
public:
    const char* GetClassName() const override { return "GameState"; }
    int totalPlayers = 0;
    int totalFleets = 0;
    int totalSystems = 0;
    bool isGameActive = true;
    float elapsedTime = 0;
};

class AGameModeBase : public UObject {
public:
    AGameModeBase() { SetName("GameMode"); }
    const char* GetClassName() const override { return "GameMode"; }

    virtual void InitGame() {
        gameState = std::make_unique<AGameStateBase>();
        gameState->AddToRoot();
    }

    virtual void PostLogin(APlayerController* controller) {
        activeControllers.push_back(controller);
        gameState->totalPlayers++;
    }

    virtual void Logout(APlayerController* controller) {
        activeControllers.erase(
            std::remove(activeControllers.begin(), activeControllers.end(), controller),
            activeControllers.end());
        gameState->totalPlayers--;
    }

    virtual void StartMatch() {
        gameState->isGameActive = true;
        for (auto* ctrl : activeControllers) {
            if (onMatchStart) onMatchStart(ctrl);
        }
    }

    virtual void EndMatch() {
        gameState->isGameActive = false;
        if (onMatchEnd) onMatchEnd(gameState.get());
    }

    virtual void Tick(float dt) {
        if (gameState && gameState->isGameActive) {
            gameState->elapsedTime += dt;
        }
    }

    GameModeConfig config;
    AGameStateBase* GetGameState() const { return gameState.get(); }

    std::function<void(APlayerController*)> onMatchStart;
    std::function<void(AGameStateBase*)> onMatchEnd;

protected:
    std::unique_ptr<AGameStateBase> gameState;
    std::vector<APlayerController*> activeControllers;
};
} // namespace ogb
