#ifndef PHYSICSCOMPONENT_H_
#define PHYSICSCOMPONENT_H_


#include "Alien.h"
#include "Duck.h"


class Game;

class PhysicsComponent {
   public:
    void update(Game* Game, Alien* alien, Duck* duck);
    bool checkCollision(Alien* alien, Duck* duck);

   private:
    static uint8_t getRandomNum();
};


#endif