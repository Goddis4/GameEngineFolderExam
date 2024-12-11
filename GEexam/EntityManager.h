#pragma once
#include <cassert>
#include <unordered_set>

struct EntityManager {
    static constexpr size_t MAX_ENTITIES = 10000;
    uint32_t entityCount = 0;
    std::vector<uint32_t> freeEntityIDs;

    uint32_t createEntity() {
        if (!freeEntityIDs.empty()) {
            uint32_t id = freeEntityIDs.back();
            freeEntityIDs.pop_back();
            return id;
        }
        assert(entityCount < MAX_ENTITIES);
        return entityCount++;
    }

    void destroyEntity(uint32_t entityID) {
        freeEntityIDs.push_back(entityID);
    }
};
