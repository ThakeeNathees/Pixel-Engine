#include "pch.h"
#include "Application.h"
#include "api/utils/AssetsReader.h"
#include "api/utils/AssetsWriter.h"

#include "entities/Sprite.h"
#include "entities/Object.h"

#include "utils/math_utils.h"
#include "misc/Event.h"

#include "utils/AssetsReader.h"

#include <pybind11/stl.h>
#include <pybind11/embed.h>
namespace py = pybind11;
// test
#include "bindings/python/pybind.h"
#include "bindings/python/PythonObject.h"

namespace pe
{
	void Application::test( Application& app ) {
		return;
		
		try
		{
			pe::Event eve;
			Object* o = new PythonObject("py_test");
			while (true) {
				try
				{
					while (app.getWindow().pollEvent(eve)){
						o->handleEvent(eve);
					}
					o->process(1/30);
					app.getWindow().clear(sf::Color(30,40,50));
					app.getWindow().draw(*o);
					app.getWindow().display();
					o->scriptReload();
					
					/* interpriter
					py::exec("print('>>> ', end='')");
					py::exec("_pe_cmd = input()");
					py::exec("if _pe_cmd[:6]!= 'print(':\n\ttry:\n\t\tprint(eval(_pe_cmd))\n\texcept:\n\t\tpass");
					py::exec("exec(_pe_cmd)");//*/
				}
				catch (const std::exception& e) { PE_PRINT(e.what()); }
			}
		}
		catch (const std::exception& e ) { PE_PRINT(e.what()); }

		__debugbreak();
	}

	sf::Color Application::s_background_color = sf::Color(80, 80, 80, 255);
	sf::Color Application::s_default_color = sf::Color(50, 75, 100, 255);

	void Application::mainLoop(const _peproj& proj) {
		py::scoped_interpreter intp;

		pe::Application app(proj);

		// for testing
		pe::AssetsWriter w;
		w.addAssets();
		w.save("test.xml");
		pe::Application::test(app); 

		app.update();
	}

	Application::Application( const sf::Vector2i& window_size, const std::string& title )
		: m_scene_changed_signal( Signal("scene_changed") )
	{
		m_window = new sf::RenderWindow(sf::VideoMode(window_size.x, window_size.y), title);
		m_window->setFramerateLimit(1/30);
	}
	Application::Application(const struct _peproj& proj)
		: m_scene_changed_signal(Signal("scene_changed"))
	{
		m_peproj = proj;
		m_window = new sf::RenderWindow(sf::VideoMode(proj.window_size.x, proj.window_size.y), proj.title);
		setDebugMode(proj.is_debug_mode);
		setFrameRate( proj.frame_rate );
		m_window->setFramerateLimit(1/proj.frame_rate);
#ifdef PE_PLATFORM_WINDOWS
		if (m_peproj.no_console_window) FreeConsole();
		else std::cout << "[pe] set no console window in preference to run without console window" << std::endl;
#endif

		AssetsReader reader;
		for (auto path : proj.assets_paths) {
			reader.loadFile( path.c_str());
			reader.readAssets(this);
		}
		
		int texture_id = proj.window_icon_texture_id;
		if (texture_id >= 0) {
			assert( Assets::hasAsset(texture_id) );
			auto tex = Assets::getAsset<Texture>(texture_id);
			m_window->setIcon( tex->getSize().x,tex->getSize().y, tex->copyToImage().getPixelsPtr() );
		}

		if (proj.assets_paths.size() == 0) {
			auto scene = Assets::newAsset<Scene>();
			setCurrentScene(scene);
		}
		else setCurrentScene(proj.begin_scene_id);

		setBgColor( proj.default_bg_color );
	}

	Application::~Application() {
		if ( m_window ) delete m_window;
	}

	void Application::addPersistenceObject(Object* obj) {
		for (auto _obj : m_persistent_objects) {
			if (_obj == obj) return;
		} m_persistent_objects.push_back(obj);
	}

	void Application::addScene(Scene* scene) {
		assert( scene != nullptr );
		m_scenes.push_back(scene);
		scene->setSceneWindowSize(sf::Vector2i(m_window->getSize().x, m_window->getSize().y) );
		for (auto obj : m_persistent_objects) scene->addObject(obj);
	}

	void Application::setCurrentScene(Scene* scene) {
		m_current_scene = scene;
		m_current_scene->clear();

		for (Object* obj : m_persistent_objects) obj->m_scene = m_current_scene;

		m_scene_changed_signal.clear();
		for (Object* obj : m_current_scene->getObjects()) m_scene_changed_signal.addReciever(obj);
		m_scene_changed_signal.setData((void*)m_current_scene->getId());
		m_current_scene->addSignal(&m_scene_changed_signal);
		
		for (auto obj : m_current_scene->getObjects()) {
			obj->m_applicaton = this;
			obj->sceneEntered(m_current_scene);
			for (Timer* timer : obj->m_timers) m_current_scene->addTimer(timer);
		}
	}
	void Application::setCurrentScene(int id) {
		for (auto scene : m_scenes) {
			if (scene->getId() == id) {
				setCurrentScene(scene);
				return;
			}
		} assert(false && "invalid scene id to set");
	}
	void Application::setCurrentScene(const std::string& name) {
		for (auto scene : m_scenes) {
			if (scene->getName() == name) {
				setCurrentScene(scene);
				return;
			} assert( false && "invalid scene name to set" );
		}
	}

	/// main loop
	void Application::update()
	{
		sf::Clock clock;
		sf::Int64 last_time = clock.getElapsedTime().asMicroseconds();
		double dt = 0;

		while (m_window->isOpen()) {

			Event event;
			while (m_window->pollEvent(event)) {
				assert( m_current_scene );
				for (Object* object : m_current_scene->getObjects()) {
					try{ object->handleEvent(event); }
					catch (const std::exception& err) { PE_PRINT(err.what()); }
					if (event.isHandled()) continue;
				}
			}
			// process
			dt += clock.getElapsedTime().asMicroseconds() / 1000000.0;
			if (dt >= 1 / m_frame_rate) {
				if (m_is_debug_mode) m_window->setTitle(m_peproj.title + std::string(" | DebugMode | fps : ").append(std::to_string(1 / dt)));
				for (Timer* timer : m_current_scene->m_timers) {
					timer->update();
				}

				for (Signal* signal : m_current_scene->m_signals) {
					for (Object* object : signal->getRecievers()) {
						if (object != nullptr) { 
							try{ object->recieveSignal(*signal); }
							catch (const std::exception& err) { PE_PRINT(err.what()); }
						}
					}
				}
				m_current_scene->m_signals.clear();

				for (Object* object : m_current_scene->getObjects()) {
					dt += clock.getElapsedTime().asMicroseconds() / 1000000.0;
					try { object->process(dt); }
					catch (const std::exception& err) { PE_PRINT(err.what()); }
				}

				if (m_current_scene->getBackground()) { m_current_scene->getBackground()->move(dt); }
				//dt -=  (1 / m_frame_rate);
				dt =  0;
			}
			clock.restart();
			double interpolation = dt / (1 / m_frame_rate);

			// draw
			m_window->clear(s_background_color);
			if (m_current_scene->getBackground()){
				Background* bg = m_current_scene->getBackground();
				if (bg->isVisible()) m_window->draw( *bg );
			}

			for (pe::Drawable* drawable : m_current_scene->getDrawables()) {
				if (drawable->isVisible()) {
					try { m_window->draw(*drawable); }
					catch (const std::exception& err) { PE_PRINT(err.what()); }
				}
			}
			m_window->display();

			if (m_is_debug_mode && sf::Keyboard::isKeyPressed(sf::Keyboard::F4)) {
				for (auto obj : m_current_scene->getObjects()) obj->scriptReload();
			}
		}
	}
}