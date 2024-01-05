#pragma once

#include "Components.h"
#include <memory>
#include <string>

class Entity
{
	friend class EntityManager;

	bool m_active = true;
	size_t m_id = 0;
	std::string m_tag = "default";

	// constructor and destructor is private because they can only be used by EntityManager
	Entity(const size_t id, const std::string& tag);

public:

	//component ptrs, prefixed with C to mean component
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;

	//private member access functions
	bool isActive() const;
	const std::string& tag() const;
	const size_t id() const;
	void destroy();
};