#pragma once

#include "Pixel-Engine.h"

class Npc : public pe::Object
{
public:
	Npc(int id) : pe::Object::Object(id) {}

	void sceneEntered(pe::Scene* scene) override {
	}

	void process(double dt) override {
		setZindex(getPosition().y);
		auto mouse = sf::Mouse::getPosition(getApplication().getWindow());
		setPosition(sf::Vector2f(mouse.x, mouse.y));
	}

private:
};
