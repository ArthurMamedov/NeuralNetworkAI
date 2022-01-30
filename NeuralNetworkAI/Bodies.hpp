#pragma once
#include <stdexcept>
#include "Systems.hpp"

namespace Bodies {
	class SimpleBody : public BodySystems::IBody {
	public:
		SimpleBody(const int health, const int speed, const int damage, int regenerationSpeed):
			maxHealth(health), movementSpeed(speed), damageValue(damage), energyRequiredForRegeneration(14), health(health), aliveOrNot(AliveOrNot::Alive), regenerationSpeed(regenerationSpeed) {
				if (maxHealth <= 0) {
					throw std::runtime_error("SimpleBody: health can't be 0.");
				}
				if (regenerationSpeed <= 0) {
					throw std::runtime_error("SimpleBody: regeneration speed cannot be 0");
				}
			}

		int getHealthValue() const noexcept override {
			return health;
		}

		int getMovementSpeedValue() const noexcept override {
			return movementSpeed;
		}

		int getDamageValue() const noexcept override {
			return damageValue;
		}

		void damage(IDamagable& object) const noexcept override {
			object.getDamage(damageValue);
		}

		AliveOrNot getDamage(int damageToGet) noexcept override {
			if (health <= damageToGet) {
				health = 0;
				return AliveOrNot::Dead;
			}
			health -= damageToGet;
			return AliveOrNot::Alive;
		}

		AliveOrNot getState() const noexcept override {
			return aliveOrNot;
		}

		void regenerate(BodySystems::IDigestiveSystem& digestiveSystem) noexcept override {
			if (health == maxHealth) {
				return;
			}
			if (digestiveSystem.useEnergy(energyRequiredForRegeneration)){
				health += regenerationSpeed;
				if (health > maxHealth) {
					health = maxHealth;
				}
			}
		}

	private:
		const int maxHealth;
		const int movementSpeed;
		const int damageValue;
		const int energyRequiredForRegeneration;
		const int regenerationSpeed;
		int health;
		AliveOrNot aliveOrNot;
	};
}