#pragma once
#include <vector>
#include "Systems.hpp"

namespace SensorSystems {
	/*!
	* Simple sensor system allows its owner to see/feel objects in some range.
	*/
	class SimpleSensorSystem final : public BodySystems::ISensorSystem {
	public:
		const std::vector<Positioning::Object2D*> analyze(const Positioning::Coordinates& currentPosition, std::vector<Positioning::Object2D>& objects) override {
			std::vector<Positioning::Object2D*> visibleObjects;
			visibleObjects.reserve(objects.size());

			for (auto& object : objects) {
				if (currentPosition.getDistance(object) <= visibleRange) {
					visibleObjects.push_back(&object);
				}
			}

			return std::move(visibleObjects);
		}
		
	private:
		const double visibleRange = 250.0;
	};
}