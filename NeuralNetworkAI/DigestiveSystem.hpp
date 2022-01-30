#pragma once
#include "Systems.hpp"
#include "Food.hpp"

namespace DigestiveSystems {
	/*!
	* Simple digestive system allows to digest all types of food and get 100% of energy from consumed food;
	*/
	class SimpleDigestiveSystem final : public BodySystems::IDigestiveSystem {
	public:
		SimpleDigestiveSystem(int startAmountOfEnergy): energy(startAmountOfEnergy) {}

		void consume(Objects::Food& food) noexcept override {
			energy += food.getNutrition();
		}

		int getEnergy() const noexcept override {
			return energy;
		}

		bool useEnergy(int required) noexcept override {
			if (required > energy) {
				return false;
			}
			energy -= required;
			return true;
		}

	private:
		int energy; 
	};
}