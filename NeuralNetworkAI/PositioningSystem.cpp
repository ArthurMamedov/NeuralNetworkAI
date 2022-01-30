#include <cmath>
#include <stdexcept>
#include "PositioningSystem.hpp"

namespace Positioning {
	Coordinates& Coordinates::operator+=(const Coordinates& coords) noexcept {
		x += coords.x;
		y += coords.y;
		return *this;
	}

	Coordinates Coordinates::operator+(const Coordinates& coords) const noexcept {
		auto res = *this;
		res += coords;
		return res;
	}

	Coordinates& Coordinates::operator-=(const Coordinates& coords) noexcept {
		x -= coords.x;
		y -= coords.y;
		return *this;
	}

	Coordinates Coordinates::operator-(const Coordinates& coords) const noexcept {
		auto res = *this;
		res -= coords;
		return res;
	}

	Coordinates& Coordinates::operator*=(const int num) noexcept {
		x *= num;
		y *= num;
		return *this;
	}

	Coordinates Coordinates::operator*(const int num) const noexcept {
		auto res = *this;
		res *= num;
		return res;
	}

	Coordinates Coordinates::operator/=(const int num) {
		if (num == 0) {
			throw std::runtime_error("Zero division error: you cannot divide coordinates by 0.");
		}
		x /= num;
		y /= num;
		return *this;
	}

	Coordinates Coordinates::operator/(const int num) const {
		auto res = *this;
		res /= num;
		return res;
	}
	
	double Coordinates::getDistance(const Object2D& object) const noexcept {
		return sqrt(pow(x - object.getX(), 2) + pow(y - object.getY(), 2));
	}
	
	double Coordinates::getDistance(const Coordinates& objectCoordinates) const noexcept {
		return sqrt(pow(x - objectCoordinates.x, 2) + pow(y - objectCoordinates.y, 2));
	}
}