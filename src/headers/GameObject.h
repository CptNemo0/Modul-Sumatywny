#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "glad/glad.h"

#include "glm/glm.hpp"
#include <memory>
#include <unordered_map>
#include <typeindex>

#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "Component.h"

class GameObject
{
private:
	std::unordered_map<std::string, std::shared_ptr<Component>> components_;
public:
	std::shared_ptr<Components::Transform> transform_;

	GameObject();

	void Update();
	void PropagateUpdate();

	template <typename T>
	void AddComponent(std::shared_ptr<T> component)
	{
		components_[typeid(T).name()] = component;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		return std::dynamic_pointer_cast<T>(components_[typeid(T).name()]);
	}
};


#endif // !GAMEOBJECT_H



