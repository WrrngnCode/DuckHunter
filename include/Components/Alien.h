
#ifndef ALIEN_H_
#define ALIEN_H_

#include "Entity.h"
#include "GameStates.h"
#include "GraphicsComponent.h"
#define INIT_SPEED (50U)

typedef enum {
     PAUSED = 0,
     MOVING_LEFT = 1,
     MOVING_RIGHT = 2
 } move_dir_t;

class Alien : public Entity {
   public:
    uint8_t spd_ticks_per_step = INIT_SPEED;  // ticks needed to make one step

    Alien();
    void update() override;
    void setState(move_dir_t move_direction);
    move_dir_t getState();

   private:
    move_dir_t st_move_direction{PAUSED};

};

#endif