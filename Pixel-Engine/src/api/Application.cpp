#include "pch.h"
#include "Application.h"

#include "entities/Sprite.h"
#include "entities/Object.h"

#include "..//utils/math_utils.h"

//#include "..//utils/XmlFile.h"

namespace pe
{
	Application::Application()
	{
		m_window = new sf::RenderWindow(sf::VideoMode(640,480), "pixel-engine");
		m_current_scene = nullptr;
	}
	Application::~Application() {
		for (auto scene : m_scenes) {
			delete scene.second;
		}
	}

	void Application::addScene( Scene* scene) {
		m_scenes[scene->getName()] = scene;
		scene->setSceneWindowSize({ m_window->getSize().x, m_window->getSize().y });
	}
	void Application::setCurrentScene(std::string scene_name) {
		assert( m_scenes.find(scene_name) != m_scenes.end() && "invalid scene name to set" );
		m_current_scene = m_scenes[scene_name];
		for (auto obj : m_current_scene->getObjects()) {
			obj->m_applicaton = this;
			obj->init();
		}
	}
	
	/// main loop
	void Application::update()
	{
		sf::Clock clock;
		sf::Int64 last_time =  clock.getElapsedTime().asMicroseconds();
		double dt = 0;

		while (m_window->isOpen()) {
			
			sf::Event event;
			while (m_window->pollEvent(event)) {
				for (Object* object : m_current_scene->getObjects()) {
					if (object->input(event)) break;
				}
			}
			// process
			dt += clock.restart().asMicroseconds() / 1000000.0;
			if (dt >= 1 / m_frame_rate) {
				for (Object* object : m_current_scene->getObjects()) {
					object->process(dt);
				}
				for (Signal* signal : m_current_scene->m_signals) {
					for (Object* object : signal->getRecievers()) object->recieveSignal(*signal);
				} m_current_scene->m_signals.clear();
				dt -= (1 / m_frame_rate);
			}
			double interpolation = dt / (1 / m_frame_rate);

			// draw
			m_window->clear(m_background_color);
			if (m_current_scene->hasBackground())
				m_window->draw(  m_current_scene->getBackground() );

			for (Object* object : m_current_scene->getObjects()) {
				 if (object->getVisible()) m_window->draw(*object);
			}
			m_window->display();
		}
	}
}