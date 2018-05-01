#pragma once

#include <cassert>
#include <vector>

#include "Entity.h"

///
template<typename Comp>
struct DenseStorage
{
	std::vector<Comp> data;
	std::vector<Entity> entities;
};

class Storage
{
public:
	virtual void remove(const Entity& entity) = 0;
	virtual bool contains(const Entity& entity) = 0;
	virtual ~Storage() = default;
};

///
template<typename Comp>
class ComponentStorage : public Storage
{
public:
	void insert(const Entity& entity, Comp& comp);
	void remove(const Entity& entity) override;
	bool contains(const Entity& entity) override;
	const std::vector<Entity>& entities(void) const;
	Comp* get(const Entity& entity);
	const Comp* get(const Entity& entity) const;

private:
	DenseStorage<Comp> m_innerStorage;
	std::vector<Index> m_indexIntoInnerData;
};

template <typename Comp>
void ComponentStorage<Comp>::insert(const Entity& entity, Comp& comp)
{
	if (contains(entity))
	{
		m_innerStorage.data[m_indexIntoInnerData[entity.idx].idx] = comp;
	}
	else
	{
		if (entity.idx >= m_indexIntoInnerData.size())
		{
			m_indexIntoInnerData.resize(entity.idx + 1u);
		}

		const uint32_t newIdx = static_cast<uint32_t>(m_innerStorage.data.size());
		m_innerStorage.data.push_back(comp);
		m_innerStorage.entities.push_back(entity);
		m_indexIntoInnerData[entity.idx] = Index(entity.id, newIdx);
	}
}

template <typename Comp>
void ComponentStorage<Comp>::remove(const Entity& entity)
{
	if (contains(entity))
	{
		const Index removed = m_indexIntoInnerData[entity.idx];
		m_indexIntoInnerData[entity.idx] = Index(0, 0);

		const uint32_t entityArraySize = static_cast<uint32_t>(m_innerStorage.entities.size());
		const Entity lastEntity = m_innerStorage.entities[entityArraySize - 1];
		if (removed.idx != entityArraySize - 1)
		{
			m_innerStorage.entities[removed.idx] = m_innerStorage.entities[entityArraySize - 1u];
			m_innerStorage.entities.erase(m_innerStorage.entities.begin() + lastEntity.idx);
			m_indexIntoInnerData[lastEntity.idx] = Index(lastEntity.id, removed.idx);
		}
		else
		{
			m_innerStorage.entities[removed.idx] = m_innerStorage.entities[entityArraySize - 1u];
			m_innerStorage.entities.erase(m_innerStorage.entities.begin() + lastEntity.idx);
		}

		m_innerStorage.data[removed.idx] = m_innerStorage.data[entityArraySize - 1u];
		m_innerStorage.data.erase(m_innerStorage.data.begin() + removed.idx);
	}
}

template <typename Comp>
bool ComponentStorage<Comp>::contains(const Entity& entity)
{
	{
		const bool id_not_zero = entity.id != 0;
		assert(id_not_zero && "Id shall not be zero");
	}

	const bool containsComponent = entity.idx < m_indexIntoInnerData.size() 
		&& m_indexIntoInnerData[entity.idx].id == entity.id;

	return containsComponent;
}

template <typename Comp>
const std::vector<Entity>& ComponentStorage<Comp>::entities() const
{
	return m_innerStorage.entities;
}

template <typename Comp>
Comp* ComponentStorage<Comp>::get(const Entity& entity)
{
	if (contains(entity))
	{
		return &m_innerStorage.data[m_indexIntoInnerData[entity.idx].idx];
	}
	
	return nullptr;
}

template <typename Comp>
const Comp* ComponentStorage<Comp>::get(const Entity& entity) const
{
	return get(entity);
}
