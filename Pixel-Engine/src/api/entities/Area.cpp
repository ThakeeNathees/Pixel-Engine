#include "pch.h"
#include "Area.h"

#include "..//utils/math_utils.h"

namespace pe
{
	int Area::s_area_count = 0;
	int Area::s_next_id = static_cast<int>(Asset::Type::Area);
	sf::Color Area::s_debug_shape_color = sf::Color(100, 0, 0, 255);

	Area::~Area() {
		std::cout << "area distruct" << std::endl;
		s_area_count--;
	}

	// getters
	bool Area::isContains(float x, float y) {
		assert( m_shape != nullptr );
		return isContainPoint(*m_shape, sf::Vector2f(x, y));
	}
	/*
	std::size_t Area::getPointCount() const {
		assert(m_shape != nullptr);
		return m_shape->getPointCount();
	}
	sf::Vector2f Area::getPoint(std::size_t index) const {
		assert(m_shape != nullptr);
		return m_shape->getPoint(index);
	}
	*/

	// setters
	void Area::setPosition(float x, float y) {
		sf::Transformable::setPosition(x, y);
		if (m_shape) m_shape->setPosition(x, y);
	}
	void Area::setRotation(float angle) {
		sf::Transformable::setRotation(angle);
		if (m_shape) m_shape->setRotation(getRotation());
	}
	void Area::setScale(float x, float y) {
		sf::Transformable::setScale(x, y);
		if (m_shape) m_shape->setScale(getScale());
	}
	void Area::setOrigin(float x, float y) {
		sf::Transformable::setOrigin(x, y);
		if (m_shape) m_shape->setOrigin(x, y);
	}


	void Area::move(float x, float y) {
		setPosition(getPosition() + sf::Vector2f(x, y));
	}
	void Area::rotate(float angle) {
		setRotation(getRotation() + angle);
	}
	void Area::scale(float x, float y) {
		setScale(getScale().x * x, getScale().y * y);
	}

	void Area::setShape(sf::Shape* shape) {
		assert(pe::isShapeConvex(*shape) && "shape of an area has to be convex");
		m_shape = shape;
		m_centroid = ::pe::getCentroid(shape);
		m_is_convex = isShapeConvex(*shape);
		m_shape->setPosition(getPosition());
		m_shape->setRotation(getRotation());
		m_shape->setScale(getScale());
		m_shape->setOutlineThickness(1);
		m_shape->setOutlineColor(s_debug_shape_color);
		m_shape->setFillColor(sf::Color(0,0,0,0));
	}




}