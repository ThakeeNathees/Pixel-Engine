#pragma once
#include "..//core.h"


namespace pe
{
	class Object;


	class PIXEL_ENGINE_API Signal
	{
	public:
		inline Signal(std::string name) : m_name(name) {}
		inline Signal() : m_name("UNKNOWN") {}

		// setters
		inline void addReciever( Object* reciever) { m_recievers.push_back(reciever); }
		inline void setName(std::string name) { m_name; }

		// getters
		inline std::vector<Object*>& getRecievers() { return m_recievers; }
		inline Object& getSender() const { assert( m_sender != nullptr );  return *m_sender; }
		inline const std::string& getName() const { return m_name; }


	private:
		friend class Object; // for access m_sender;
		std::string m_name;
		Object* m_sender = nullptr;
		std::vector<Object*> m_recievers;
	};
}
