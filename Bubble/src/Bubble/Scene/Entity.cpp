#include "bgpch.h"
#include "Entity.h"

namespace Bubble {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	Entity::Entity(uint32_t handle, Scene* scene)
		: m_EntityHandle((entt::entity)handle), m_Scene(scene)
	{
	}

}
