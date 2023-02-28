#include "Game.h"
#include "Alien.h"
#include "Duck.h"


Game::Game(InputComponent* _input, GraphicsComponent* _graphics, PhysicsComponent* _physics)
    : input(_input), graphics(_graphics), physics(_physics) {
    entities[Game::ALIEN_ID] = new Alien();
    entities[Game::ALIEN_ID]->figure_id = Game::ALIEN_ID;
    entities[Game::ALIEN_ID]->setPosition(0, 5);

    entities[Game::DUCK_ID] = new Duck();
    entities[Game::DUCK_ID]->figure_id = Game::DUCK_ID;
    entities[Game::DUCK_ID]->setPosition(1, 10);
}

void Game::update() {
    input->update((Alien*)entities[Game::ALIEN_ID]);
    physics->update(this,
                    (Alien*)entities[Game::ALIEN_ID],
                    (Duck*)entities[Game::DUCK_ID]);

    entities[Game::ALIEN_ID]->update();
    graphics->update(entities, NUM_ENTITIES, score, Game::ALIEN_ID);

}

void Game::init() {
    this->graphics->init();
}

void Game::increment_score() {
    score++;
}

const uint8_t Game::ALIEN_ID{0};
const uint8_t Game::DUCK_ID{1};
