#ifndef ENTITY_H_
#define ENTITY_H_

#include <stdint.h>

typedef struct _Position {
    uint8_t row;     // 0 or 1
    uint8_t column;  // 0...15
} Position;

bool operator==(Position lhs, Position rhs);

class Entity {
   public:
    bool render_needed = true;
    uint8_t figure_id = 0x20;  // empty figure

    Entity();
    virtual ~Entity() {}
    virtual void update() = 0;
    void setPosition(uint8_t _row, uint8_t _column);
    void setPosition(Position _position);
    void incrementPosition();
    void decrementPosition();
    Position getPosition() { return position; };
    Position getPrevPosition() { return prev_position; };

   private:
    Position position{0, 5};
    Position prev_position{0, 5};
};

#endif
