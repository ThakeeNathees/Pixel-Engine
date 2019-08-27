#include "pch.h"
#include "Scene.h"

namespace pe
{
	int Signal::s_signal_count = 0;

	Scene::Scene(std::string name) : m_name(name) {}

	Scene::~Scene() {
		for (Object* object : m_objects) {
			delete object;
		}
	}
	bool Scene::sortCompare(pe::Drawable* obj1, pe::Drawable* obj2) {
		return obj1->getZIndex() < obj2->getZIndex();
	}

	void Scene::clear()
	{
		m_timers.clear();
		m_signals.clear();
	}

	// setters
	void Scene::addObject(Object* object) {
		m_objects.push_back(object);
		m_drawables.push_back(object);
		object->setScene(this);
		sortZIndex();
	}
	void Scene::sortZIndex() {
		std::sort(m_drawables.begin(), m_drawables.end(), Scene::sortCompare);
	}
	void Scene::setBackground(Background* background) {
		m_background = background;
		m_background->setVisible(true);
		m_background->setTextureRectSize(m_window_size);
	}

	void Scene::setSceneWindowSize(glm::ivec2 window_size) {
		m_window_size = window_size;
		if (m_background != nullptr) m_background->setTextureRectSize(m_window_size);
	}

	void Scene::addTimer(Timer* timer) {
		m_timers.push_back(timer);
		timer->m_scene = this;
	}

	// getters
	Object& Scene::getObject(const std::string& name) {
		for ( Object* object : m_objects ) {
			if (object->getName() == name) return *object;
		}
		assert( false && "can't get object which the scene doesn't contain");
	}

	bool Scene::hasObject(const std::string& name) {
		for (Object* object : m_objects) {
			if (object->getName() == name) return true;
		}
		return false;
	}

}