
#ifndef DUCK_H_
#define DUCK_H_

#include <stdint.h>

#include "Entity.h"

class Duck : public Entity {
   public:
    Duck();
    void update() override;
};


#endif