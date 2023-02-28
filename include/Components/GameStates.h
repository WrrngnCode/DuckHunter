
// #ifndef GAMESTATES_H_
// #define GAMESTATES_H_
// #include <stdint.h>

// #include "Entity.h"

// class State {  // Abstract State
//    public:
//     State();
//     virtual ~State();
//     virtual void update(Entity* const game) const = 0;
// };

// class MoveLeftState : public State {
//    public:
//     MoveLeftState();
//     ~MoveLeftState();
//     void update(Entity* const game) const override;

// };

// class MoveRightState : public State {
//    public:
//     MoveRightState();
//     ~MoveRightState();
//     void update(Entity* const game) const override;

// };

// class PausedState : public State {
//    public:
//     PausedState();
//     ~PausedState();
//     void update(Entity* const game) const override;

// };

// #endif