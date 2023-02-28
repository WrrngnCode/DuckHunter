#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

#include "Entity.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"

class Game {
   public:
    static const uint8_t ALIEN_ID;
    static const uint8_t DUCK_ID;

    Game(InputComponent* _input,
         GraphicsComponent* _graphics,
         PhysicsComponent* _physics);
    void init();
    void update();
    void increment_score();

   private:
    InputComponent* input;
    GraphicsComponent* graphics;
    PhysicsComponent* physics;
    uint8_t score{0};
    static const uint8_t NUM_ENTITIES = 2;
    Entity* entities[NUM_ENTITIES];
};


#endif