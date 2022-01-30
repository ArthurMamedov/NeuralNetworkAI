#pragma once
#include <memory>
#include "Systems.hpp"

namespace Objects {
	/*! Just a wall object that cannot is just for limitation of the active map */
	class Wall : public Positioning::Object2D {
	public:
	private:
	};

	/*!
	* The worm consists of
	* - sensors (allows to see food, enemies, walls, ...)
	* - brain (takes decisions based on the data obtained by sensors)
	* - digestive system (gives enegry for movements, worm dies if has no food)
	* - body (health of the worm, speed of movement, how many food requires, ...)
	*/
	class Worm final : public Positioning::Object2D {
	public:
		Worm() = default;
		Worm(std::unique_ptr<BodySystems::ICognitiveSystem>&& brain,
			std::unique_ptr<BodySystems::ISensorSystem>&& sensor,
			std::unique_ptr<BodySystems::IDigestiveSystem>&& degSystem,
			std::unique_ptr<BodySystems::IBody>&& body) : brain(std::move(brain)),
			sensorSystem(std::move(sensor)),
			digestiveSystem(std::move(degSystem)),
			body(std::move(body)) {};

		/*!
		* This method is like moving and acting on stimulus if one is here.
		* First we look for all visible objects (it's defined in the implementation of sensor system).
		* Then we decide where to go. When the target is selected, we start moving towards it.
		* If it's food, we try to eat it. If it's wall, then we're doing nothing because we cannot do anything
		* with walls. If it's another warm - we try to damage/kill it.
		*/
		void makeNextMove(std::vector<Positioning::Object2D>& objects) {
			auto visibleObjects = sensorSystem->analyze(getCoordinates(), objects);
			auto selected = brain->desideWhereToGo(visibleObjects);
			auto distance = getDistance(selected);
			auto direction = ((selected.getCoordinates() - getCoordinates()) / distance) * body->getMovementSpeedValue();
			move(direction);
			if (getDistance(selected) <= 1) {
				_handleObject(selected);
			}
			if (Positioning::Object2D::shallDestruct()) {
				brain->learn();
			}
		}

	private:
		void _handleObject(Positioning::Object2D& object) { /*! Defines action what to do with the object we just met */
			const auto& objectType = typeid(object);

			if (objectType == typeid(Food)) {
				Objects::Food& food = *dynamic_cast<Objects::Food*> (&object);
				digestiveSystem->consume(food);
				body->regenerate(*digestiveSystem);
			}
			else if (objectType == typeid(Wall)) {
				body->regenerate(*digestiveSystem);
			}
			else if (objectType == typeid(Worm)) {
				auto& anotherWorm = dynamic_cast<Worm&>(object);
				body->damage(*anotherWorm.body);
			}
		}

	private:
		std::unique_ptr<BodySystems::ICognitiveSystem> brain;
		std::unique_ptr<BodySystems::ISensorSystem> sensorSystem;
		std::unique_ptr<BodySystems::IDigestiveSystem> digestiveSystem;
		std::unique_ptr<BodySystems::IBody> body;
	};

}