#include "Entity.h"


Entity::Entity() {}

bool operator==(Position lhs, Position rhs) {
    return lhs.row == rhs.row && lhs.column == rhs.column;
}

void Entity::setPosition(uint8_t _row, uint8_t _column) {
    prev_position = position;
    this->position.row = _row;
    this->position.column = _column;
    render_needed = true;
}

void Entity::setPosition(Position _position) {
    prev_position = position;
    this->position = _position;
    render_needed = true;
}

void Entity::incrementPosition() {
    prev_position = position;

    if (position.column < 15) {
        position.column += 1;
    } else {
        position.row = position.row == 0 ? 1 : 0;
        position.column = 0;
    };
    render_needed = true;
}

void Entity::decrementPosition() {
    prev_position = position;

    if (position.column >= 1) {
        position.column -= 1;
    } else {
        position.row = position.row == 0 ? 1 : 0;
        position.column = 15;
    };
    render_needed = true;
}
