#pragma once
#include <vector>
#include "PositioningSystem.hpp"
#include "Food.hpp"

namespace BodySystems {
	/*!
	* IBrain is an interface for the system that will be responsible for taking desidions
	* where to go.
	*/
	class ICognitiveSystem {
	public:
		/*! This method is responsible for learning and correcting the behaviour of the worm after death. */
		virtual void learn() = 0;

		/*! This method makes desitions on where to move next */
		virtual Positioning::Object2D& desideWhereToGo(const std::vector<Positioning::Object2D*>& objects) = 0;
		virtual ~ICognitiveSystem() = default;
	};

	/*!
	* IDegestiveSystem is like a converter food to energy.
	* Energy is gonna be used for movements, regeneration and all the rest.
	*/
	class IDigestiveSystem {
	public:
		/*! Destroys food and produces energy */
		virtual void consume(Objects::Food& food) noexcept = 0;

		/*! Get current number of energy in the system */
		virtual int getEnergy() const noexcept = 0;

		/*! gets amount of energy needed to perform some action. If there is enough energy for this, true is returned, else - false.*/
		virtual bool useEnergy(int required) noexcept = 0;
		virtual ~IDigestiveSystem() = default;
	};

	/*!
	* ISensor is an interface for the system that will be responsible for the mechanism
	* of how something (Worm, for example) is going to feel the surrounding him world.
	*/
	class ISensorSystem {
	public:
		/*! Walks through all objects on the map and returns only some of them that are considered as visible. */
		virtual const std::vector<Positioning::Object2D*> analyze(const Positioning::Coordinates& currentPosition, std::vector<Positioning::Object2D>& objects) = 0;
		virtual ~ISensorSystem() = default;
	};

	/*!
	* Interface for all damagable objects.
	*/
	class IDamagable {
	public:
		enum class AliveOrNot {
			Dead, Alive
		};

	public:
		/*!
		* getDamage method reflects the situation when the objects is damaged by something.
		* Returns the heals that remained after damaging.
		*/
		virtual AliveOrNot getDamage(int damageToGet) noexcept = 0;
		virtual AliveOrNot getState() const noexcept = 0;
		virtual ~IDamagable() = default;
	};

	/*!
	* Interface that defines a basic set of phisical characteristics of an object.
	* Such characteristics as movement speed, health, damage resistance, regeneration.
	*/
	class IBody : public IDamagable { //TODO: find more suitable name for this interface.
	public:
		/*! Returns the number that should be represetned as heals */
		virtual int getHealthValue() const noexcept = 0;

		/*! Returns the speed of current body */
		virtual int getMovementSpeedValue() const noexcept = 0;

		/*! Returns the value of damage that can cause this body */
		virtual int getDamageValue() const noexcept = 0;

		/*! Damages another body. If it's completely destroyed - it will be invalidated */
		virtual void damage(IDamagable& object) const noexcept = 0;

		/*! Regenerates some health at the expense of energy */
		virtual void regenerate(IDigestiveSystem& digestiveSystem) noexcept = 0;
		virtual ~IBody() = default;
	};
}
