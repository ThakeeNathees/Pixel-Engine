#pragma once

#include "Core.h"
#include "Window.h"

namespace PE
{
	class PIXEL_ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();	
		void run();

	private:
		std::shared_ptr<Window> m_window;
		double m_frame_rate = 30.0;
	};

	// To be defined in CLIENT
	Application* createApplication();
}


