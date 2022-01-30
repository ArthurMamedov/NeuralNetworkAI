#pragma once
#include "PositioningSystem.hpp"

namespace Objects {
	/*!
		* Food is just food :)
		*/
	struct Food : public Positioning::Object2D {
	public:
		/*! There are several types of food. Some jaws can handle only soft food, some - hard */
		enum class FoodType : unsigned char {
			HardFood,
			SoftFood
		};

	public:
		explicit Food(int nutrition) noexcept : nutrition(nutrition), foodType(FoodType::SoftFood) {}
		Food(int nutrition, FoodType foodType) noexcept : nutrition(nutrition), foodType(foodType) {}

		virtual FoodType getFoodType() const noexcept final {
			return foodType;
		}

		/*! Returns the number of nutritions that will be remade to evergy after consumming */
		virtual int getNutrition() noexcept final {
			auto result = nutrition;
			nutrition = 0;
			_invalidate();
			return result;
		}

		virtual ~Food() = default;

	private:
		int nutrition;
		FoodType foodType;
	};
}
