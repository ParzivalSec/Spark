#pragma once

#include <vector>
#include <unordered_map>
#include "ComponentStorage.h"

class World
{
public:
	World();

	Entity make();

	template<typename C>
	void addComponent(const Entity& entity, C component);
	
	template<typename C>
	void removeComponent(const Entity& entity);

	void removeEntity(const Entity& entity);

	template<typename C>
	bool hasComponent(const Entity& entity) const;

	template<typename C>
	const std::vector<Entity>* entitiesWithComponent(void) const;

	template<typename C>
	C* getComponent(const Entity& entity);

	template<typename C>
	const C* getComponent(const Entity& entity) const;

	bool contains(const Entity& entity) const;

private:
	uint32_t m_nextId;
	uint32_t m_nextIdx;
	std::vector<uint32_t> m_freeIndices;
	std::unordered_map<uint32_t, bool> m_activeEntities;
	std::unordered_map<uint32_t, Storage*> m_componentStorages;
};

template <typename C>
bool World::hasComponent(const Entity& entity) const
{
	const uint32_t componentId = C::UID();
	if (m_componentStorages.count(componentId) == 1)
	{
		ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_componentStorages.at(componentId));
		return storage->contains(entity);
	}

	return false;
}

template <typename C>
const std::vector<Entity>* World::entitiesWithComponent() const
{
	const uint32_t componentId = C::UID();
	if (m_componentStorages.count(componentId) == 1)
	{
		ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_componentStorages.at(componentId));
		return &storage->entities();
	}

	return nullptr;
}

template <typename C>
C* World::getComponent(const Entity& entity)
{
	const uint32_t componentId = C::UID();
	if (m_componentStorages.count(componentId) == 1)
	{
		ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_componentStorages.at(componentId));
		return storage->get(entity);
	}

	return nullptr;
}

template <typename C>
const C* World::getComponent(const Entity& entity) const
{
	return getComponent<C>(entity);
}

template <typename C>
void World::addComponent(const Entity& entity, C component)
{
	const uint32_t componentId = C::UID();
	if (m_componentStorages.count(componentId) == 0)
	{
		m_componentStorages.insert({ componentId, new ComponentStorage<C>() });
	}

	ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_componentStorages.at(componentId));
	if (contains(entity))
	{
		storage->insert(entity, component);
	}
}

template <typename C>
void World::removeComponent(const Entity& entity)
{
	const uint32_t componentId = C::UID();
	if (m_componentStorages.count(componentId) == 1)
	{
		ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_componentStorages.at(componentId));
		storage->remove(entity);
	}
}
