#ifndef GRAPHICSCOMPONENT_H_
#define GRAPHICSCOMPONENT_H_

#include "Entity.h"

class GraphicsComponent {
   public:
    void update(Entity** entities, uint8_t num_entities, uint8_t, uint8_t alien_id);
    void init();
    void shiftLedsLeft();
    void shiftLedsRight();

   private:
    class Led16Driver {
        uint16_t pattern = 0xFF;
       public:
        inline void shiftLeft();
        inline void shiftRight();
        void update();
    };


    void init_lcd_display();
    void welcome();
    Led16Driver leds;
};


#endif