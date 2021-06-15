#ifndef GAME_PARTICLE_H
#define GAME_PARTICLE_H
#import "Entity.h"

class Particle : public Entity {
public:
    int lifetime;
    int age;

    Particle(SDL_Renderer* renderer, Vector2 pos, int lifetime = 500) : Entity(renderer, "particle.bmp", pos, 3, 3) {
        this->lifetime = lifetime;
        this->age = 0;
    }

    virtual void update(int deltaTicks, Vector2 mousePosition) {
        this->age += deltaTicks;

        Entity::update(deltaTicks, mousePosition);
    }

    virtual bool shouldRemove() {
        return this->age >= this->lifetime;
    }
};

#endif
