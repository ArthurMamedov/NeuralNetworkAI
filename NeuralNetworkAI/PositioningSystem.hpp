#pragma once

namespace Positioning {
	/*!
	* Coordinates is a class-wrapper over the most common operations with coordinates,
	* for example +, -, *, /.
	*/
	class Object2D;

	struct Coordinates {
		int x;
		int y;

		Coordinates& operator += (const Coordinates& coords) noexcept;

		Coordinates operator + (const Coordinates& coords) const noexcept;

		Coordinates& operator -= (const Coordinates& coords) noexcept;

		Coordinates operator - (const Coordinates& coords) const noexcept;

		Coordinates& operator *= (const int num) noexcept;

		Coordinates operator * (const int num) const noexcept;

		Coordinates operator /= (const int num);

		Coordinates operator / (const int num) const;

		double getDistance(const Object2D& object) const noexcept;

		double getDistance(const Coordinates& objectCoordinates) const noexcept;
	};


	/*!
	* Object2D is a set of methods for moving on the map.
	*/
	class Object2D {
	public:
		virtual ~Object2D() = default;

		virtual bool shallDestruct() const noexcept final {
			return shallBeDestructed;
		}

		virtual double getDistance(const Object2D& object) const noexcept final {
			return coords.getDistance(object);
		}

		virtual double getDistance(const Coordinates& objectCoordinates) const noexcept final {
			return coords.getDistance(objectCoordinates);
		}

		virtual void move(int offsetX, int offsetY) noexcept final {
			coords.x += offsetX;
			coords.y += offsetY;
		}

		virtual void move(Coordinates offsets) noexcept final {
			coords += offsets;
		}

		virtual void setLocation(int newX, int newY) noexcept final {
			coords.x = newX;
			coords.y = newY;
		}

		virtual void setLocation(Coordinates newCoords) noexcept final {
			coords = newCoords;
		}

		virtual int getX() const noexcept final {
			return coords.x;
		}

		virtual int getY() const noexcept final {
			return coords.x;
		}

		virtual Coordinates getCoordinates() const noexcept final {
			return coords;
		}

	protected:
		Object2D() noexcept : coords{ 0, 0 }, shallBeDestructed(false) {}
		Object2D(const int defX, const int defY) noexcept : coords{ defX, defY }, shallBeDestructed(false) {}

	protected:
		virtual void _invalidate() noexcept final {
			shallBeDestructed = true;
		}

	private:
		Coordinates coords;
		bool shallBeDestructed;
	};
}
