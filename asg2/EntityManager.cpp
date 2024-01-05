#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	// TODO: Add entities from m_entitiesToAdd to the proper locations
	/*- add them to vector of all entities
	- add them to vector inside the map, with tag as key*/
	for (auto& eToAdd : m_entitiesToAdd)
	{
		// Add to vector of all entities
		m_entities.push_back(eToAdd);
		// Add to vector inside map
		auto a = getEntities(eToAdd->tag());
		a.push_back(eToAdd);
		m_entityMap[eToAdd->tag()] = a;
		//m_entityMap.insert(std::make_pair( eToAdd->tag(), a));
	}

	m_entitiesToAdd.clear();

	//remove dead entities from vector with all entities
	removeDeadEntities(m_entities);

	//remove dead entities from each vector in the entity map
	// C++17 way of iterating through [key,value] pairs in map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	//TODO: Remove all dead entities from input vector
	// this is called by update() function
	auto asd = std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) { return !e->isActive(); });
	vec.erase(asd, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
	/*auto i = m_entityMap.find(tag);
	if (i != m_entityMap.end())
	{
		return i->second;
	}
	else
	{
		std::cerr << "No entity found for tag: " << tag;
		exit(-1);
	}*/
}