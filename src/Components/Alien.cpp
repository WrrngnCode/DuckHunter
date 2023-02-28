#include "Alien.h"
#include "GameStates.h"

extern volatile uint32_t sysTick;

Alien::Alien() : Entity() {}

void Alien::update() {
    static volatile uint32_t lastUpdate = 0;
    if (sysTick - lastUpdate >= spd_ticks_per_step) {
        if (st_move_direction == MOVING_RIGHT) {
            this->incrementPosition();
        } else if (st_move_direction == MOVING_LEFT) {
            this->decrementPosition();
        }
        lastUpdate = sysTick;
    }
}

void Alien::setState(move_dir_t move_direction) {
    this->st_move_direction = move_direction;
    this->render_needed = true;
}

move_dir_t Alien::getState() {
    return this->st_move_direction;
}
