#include "World.h"

World::World()
	: m_nextId(1)
	, m_nextIdx(0)
{}

Entity World::make()
{
	const uint32_t entityId = m_nextId++;

	uint32_t entityIdx = 0;
	if (!m_freeIndices.empty())
	{
		entityIdx = m_freeIndices[m_freeIndices.size() - 1];
		m_freeIndices.pop_back();
	}
	else
	{
		entityIdx = m_nextIdx++;
	}

	const Entity entity(entityId, entityIdx);
	return entity;
}

void World::removeEntity(const Entity& entity)
{
	if (contains(entity))
	{
		m_freeIndices.push_back(entity.idx);
		for (std::pair<const uint32_t, Storage*>& store : m_componentStorages)
		{
			store.second->remove(entity);
		}
	}
}

bool World::contains(const Entity& entity) const
{
	return std::find(m_freeIndices.begin(), m_freeIndices.end(), entity.idx) == m_freeIndices.end();
}
