/* OGame Beyond - UObject (Engine Root)
 * Developer: Stephen
 * Base class for all engine objects with reflection, serialization, and GC support.
 */
#pragma once
#include <string>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <functional>

namespace ogb {
class UObject {
public:
    UObject() : objectID(nextID++) {}
    virtual ~UObject() = default;

    uint64_t GetObjectID() const { return objectID; }
    const std::string& GetName() const { return name; }
    void SetName(const std::string& n) { name = n; }
    std::type_index GetType() const { return std::type_index(typeid(*this)); }
    bool IsA(std::type_index type) const { return GetType() == type; }
    bool IsPendingKill() const { return pendingKill; }
    void MarkPendingKill() { pendingKill = true; }
    bool IsRooted() const { return isRooted; }
    void AddToRoot() { isRooted = true; }
    void RemoveFromRoot() { isRooted = false; }

    // Reflection
    virtual const char* GetClassName() const { return "UObject"; }
    virtual void Serialize(class Serializer& s) {}
    virtual void PostInitProperties() {}
    virtual void BeginDestroy() {}

    // Event handling
    virtual void OnTick(float dt) {}
    virtual void OnReplicated() {}

    // Tagging
    void Tag(const std::string& t) { tags.insert(t); }
    bool HasTag(const std::string& t) const { return tags.count(t); }
    void Untag(const std::string& t) { tags.erase(t); }

protected:
    std::string name;
    uint64_t objectID;
    bool pendingKill = false;
    bool isRooted = false;
    std::unordered_set<std::string> tags;

private:
    static inline uint64_t nextID = 1;
};
} // namespace ogb
