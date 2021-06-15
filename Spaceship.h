#ifndef GAME_SPACESHIP_H
#define GAME_SPACESHIP_H

#include "Entity.h"
#include "Vector2.h"
#include "Obstacle.h"

bool placedSpaceship = false;

class Spaceship : public Entity {
public:
    bool dead;

    Spaceship(SDL_Renderer* renderer, Vector2 pos) : Entity(renderer, "spaceship.bmp", pos, 17, 17, 2) {
        this->dead = false;
    }

    virtual void update(int deltaTicks, Vector2 mousePosition) {
        float deltaTime = this->getDeltaTime(deltaTicks);

        Vector2 delta = mousePosition - this->pos;

        Vector2 accel = delta * 50 - this->velocity * 10;

        this->velocity += accel * deltaTime;

        int particleCount = floor(sqrt(accel * accel) * 0.0015);

        // createParticles(SDL_Renderer * renderer, Vector2 pos, Vector2 velocity, float speed, int count, int maxLife)
        createParticles(this->getRenderer(), this->pos, accel * -0.1, 150, particleCount, 200);

        Entity::update(deltaTicks, mousePosition);

        SDL_Rect boundBox = this->getBoundBox();

        std::vector<Entity*>::size_type entitiesSize = entities.size();
        for (std::vector<Entity*>::size_type i = 0; i < entitiesSize; ++i) {
            Entity* entity = entities[i];
            if (!entity->canCollide()) continue;

            SDL_Rect entityBB = entity->getBoundBox();
            if (SDL_HasIntersection(&boundBox, &entityBB)) {
                placedSpaceship = false;
                this->dead = true;
                std::cout << "You collided with an obstacle!" << std::endl;
                break;
            }

        }
    }

    virtual bool shouldRemove() {
        return this->dead;
    }

    virtual bool hasMass() {
        return true;
    }

    virtual float getMass() {
        return 25;
    }
};

#endif //GAME_SPACESHIP_H
