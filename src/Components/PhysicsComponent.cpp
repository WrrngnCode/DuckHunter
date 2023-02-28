
#include <avr/io.h>
#include "PhysicsComponent.h"
#include "Alien.h"
#include "Duck.h"
#include "Entity.h"
#include "Game.h"

extern volatile uint32_t sysTick;

void PhysicsComponent::update(Game* game, Alien* alien, Duck* duck) {
    // if alien has reached the duck:
    if (checkCollision(alien, duck) == true) {
        game->increment_score();
        alien->setState(PAUSED);

        Position new_duck_pos;
        new_duck_pos.row = duck->getPosition().row == 0 ? 1 : 0;
        new_duck_pos.column = PhysicsComponent::getRandomNum();
        duck->setPosition(new_duck_pos);
    }
}

uint8_t PhysicsComponent::getRandomNum() {
    return (uint8_t)(sysTick * TCNT1) & 0x0F;
}

bool PhysicsComponent::checkCollision(Alien* alien, Duck* duck) {
    return alien->getPosition() == duck->getPosition();
}
