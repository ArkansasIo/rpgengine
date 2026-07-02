/* OGame Beyond - Galaxy
 * Developer: Stephen
 * Galaxy generation, star systems, and spatial management.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace ogb {
enum class StarType { Yellow, Red, Blue, White, Orange, Giant, Neutron, BlackHole };
enum class BodyType { Planet, Moon, AsteroidBelt, Nebula, Comet, Station };

struct StarSystem {
    int id = -1;
    std::string name;
    Vec3 position;
    StarType starType = StarType::Yellow;
    float starSize = 1.0f;
    Color starColor;
    int ownerEmpireID = -1;
    bool explored = false;
    bool hasWormhole = false;
    int wormholeTarget = -1;
    std::vector<int> planetIDs;
    std::vector<int> stationIDs;
    float securityLevel = 1.0f; // 0=lawless, 1=secure
};

struct CelestialBody {
    int id = -1;
    std::string name;
    BodyType type = BodyType::Planet;
    int parentSystemID = -1;
    float orbitalDistance = 0;
    float orbitalPeriod = 0;
    float rotationPeriod = 0;
    float radius = 1.0f;
    float gravity = 1.0f;
    int ownerEmpireID = -1;
    int colonyID = -1; // -1 = uncolonized
    bool hasRing = false;
    Color atmosphereColor;
};

struct Wormhole {
    int id = -1;
    int fromSystemID = -1;
    int toSystemID = -1;
    bool isActive = true;
    float stability = 1.0f;
};

class Galaxy {
public:
    std::string name;
    std::vector<StarSystem> systems;
    std::vector<CelestialBody> bodies;
    std::vector<Wormhole> wormholes;

    int Generate(int numSystems, float galaxyRadius) {
        systems.clear();
        bodies.clear();
        for (int i = 0; i < numSystems; i++) {
            StarSystem sys;
            sys.id = i;
            sys.name = GenerateSystemName(i);
            float angle = (float)i / numSystems * 6.28318f + (float)(rand()%100)/100.0f * 0.5f;
            float dist = (float)(rand()%1000)/1000.0f * galaxyRadius;
            sys.position = Vec3(cosf(angle)*dist, (float)(rand()%200-100)/10.0f, sinf(angle)*dist);
            sys.starType = static_cast<StarType>(rand()%6);
            sys.starColor = GetStarColor(sys.starType);

            // Generate planets
            int numPlanets = 2 + rand()%6;
            for (int j = 0; j < numPlanets; j++) {
                CelestialBody body;
                body.id = static_cast<int>(bodies.size());
                body.parentSystemID = i;
                body.orbitalDistance = 50.0f + j * 30.0f + (float)(rand()%100)/10.0f;
                body.radius = 0.5f + (float)(rand()%100)/100.0f * 2.0f;
                body.gravity = body.radius * 0.8f;
                body.type = (rand()%10 == 0) ? BodyType::Moon : BodyType::Planet;
                sys.planetIDs.push_back(body.id);
                bodies.push_back(body);
            }
            systems.push_back(sys);
        }
        return numSystems;
    }

    StarSystem* GetSystem(int id) { return (id >= 0 && id < (int)systems.size()) ? &systems[id] : nullptr; }
    CelestialBody* GetBody(int id) { return (id >= 0 && id < (int)bodies.size()) ? &bodies[id] : nullptr; }
    int GetSystemCount() const { return static_cast<int>(systems.size()); }

private:
    std::string GenerateSystemName(int id) {
        const char* prefixes[] = {"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota","Kappa"};
        const char* suffixes[] = {"Centauri","Draconis","Eridani","Cygni","Pavonis","Tucanae","Gruis","Indi","Volantis","Achernar"};
        return std::string(prefixes[id%10]) + " " + suffixes[(id/10)%10] + "-" + std::to_string(id);
    }

    Color GetStarColor(StarType t) {
        switch(t) {
            case StarType::Red: return {1.0f, 0.3f, 0.1f};
            case StarType::Orange: return {1.0f, 0.6f, 0.1f};
            case StarType::Yellow: return {1.0f, 1.0f, 0.6f};
            case StarType::White: return {0.9f, 0.9f, 1.0f};
            case StarType::Blue: return {0.5f, 0.5f, 1.0f};
            case StarType::Giant: return {1.0f, 0.4f, 0.2f};
            case StarType::Neutron: return {0.3f, 0.3f, 1.0f};
            case StarType::BlackHole: return {0.0f, 0.0f, 0.0f};
            default: return {1.0f, 1.0f, 0.8f};
        }
    }
};
} // namespace ogb
