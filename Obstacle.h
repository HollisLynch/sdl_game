#ifndef GAME_OBSTACLE_H
#define GAME_OBSTACLE_H
#import "Entity.h"

std::vector<Entity*> entities;
float gravity = 10000;

int screenWidth = 1080;
int screenHeight = 720;

// Generate a random float between 0 and 1
float randomFloat() {
    return rand() / float(RAND_MAX);
}

void createParticles(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity, float speed, int count, int maxLife) {
    for (int i = 0; i < count; i++) {
        int lifetime = roundf((randomFloat() + randomFloat() + randomFloat()) / float(3) * maxLife);
        Particle* particle = new Particle(renderer, pos, lifetime);

        Vector2 direction;

        do {
            direction = Vector2(randomFloat() * 2.0 - 1, randomFloat() * 2.0 - 1);

        } while (direction * direction > 1);

        particle->velocity = velocity + direction * speed;

        entities.push_back(particle);
    }
}

class Obstacle : public Entity {
public:
    float mass;
    int maxHitpoints;
    int hitpoints;

    Obstacle(SDL_Renderer* renderer, std::string texture, float mass, int hitpoints, int width, int height, int scale, Vector2 pos, Vector2 velocity) : Entity(renderer, texture, pos, width, height, scale) {
        this->mass = mass;
        this->velocity = velocity;
        this->maxHitpoints = hitpoints;
        this->hitpoints = hitpoints;
    }

    void takeDamage(Vector2 splitDirection, Vector2 collisionNormal, float collisionEnergy) {
        float splitSpeed = sqrt(2 * collisionEnergy / this->getMass()) * 0.35;

        this->hitpoints = this->hitpoints - collisionEnergy / 2000;
        if (this->hitpoints > 0) {
            return;
        }

        float newScale = this->scale - 1;
        if (scale < 1) {
            createParticles(this->getRenderer(), this->pos, this->velocity, 100, 6, 300);
            return;
        }

        Vector2 dir = (collisionNormal + splitDirection) * (1 / 1.414);
        Obstacle* child1 = new Obstacle(
                this->getRenderer(), this->textureFile,
                this->mass / 2, this->maxHitpoints / 2,
                this->width, this->height, newScale,
                this->pos + dir * this->width * newScale * 2,
                this->velocity + dir * splitSpeed
        );
        entities.push_back(child1);

        dir = (collisionNormal - splitDirection) * (1 / 1.414);
        Obstacle* child2 = new Obstacle(
                this->getRenderer(), this->textureFile,
                this->mass / 2, this->maxHitpoints / 2,
                this->width, this->height, newScale,
                this->pos + dir * this->width * newScale * 2,
                this->velocity + dir * splitSpeed
        );

        entities.push_back(child2);

        createParticles(this->getRenderer(), this->pos, this->velocity, 50 * this->scale, 5 + this->scale * 3, 300);
    }

    virtual void update(int deltaTicks, Vector2 mousePosition) {
        Vector2 accel = Vector2(0, 0);

        SDL_Rect boundBox = this->getBoundBox();

        std::vector<Entity*>::size_type entitiesSize = entities.size();
        for (std::vector<Entity*>::size_type i = 0; i < entitiesSize; ++i) {
            Entity* entity = entities[i];
            if (entity == this || !entity->hasMass()) continue;

            Vector2 distance = entity->pos - this->pos;
            float magSq = distance * distance;

            if (magSq < 100 * 100) {
                float mag = sqrt(magSq);

                if (mag * 2 > this->width) {
                    accel += distance * ((gravity * entity->getMass()) / (magSq * mag));
                }
            }

            SDL_Rect entityBB = entity->getBoundBox();

            if (entity->canCollide() && SDL_HasIntersection(&boundBox, &entityBB)) {
                Vector2 normal = (entity->pos - this->pos).normalize();
                Vector2 relativeVelocity = entity->velocity - this->velocity;

                float combinedMass = this->getMass() + entity->getMass();
                float collisionEnergy = 0.1 * (relativeVelocity * relativeVelocity) * combinedMass;
                Vector2 splitDir = Vector2(normal.y, -normal.x);

                this->takeDamage(splitDir, normal, collisionEnergy);
                ((Obstacle*)entity)->takeDamage(splitDir, normal, collisionEnergy);
            }
        }

        this->velocity += accel * this->getDeltaTime(deltaTicks);

        Entity::update(deltaTicks, mousePosition);

        bool wrapped = false;

        if (this->pos.x < -this->width) {
            this->pos.x = this->pos.x + screenWidth + this->width * 2;
            wrapped = true;
        }

        if (this->pos.x > screenWidth + this->width && this->velocity.x >= 0) {
            this->pos.x = this->pos.x - (screenWidth + this->width * 2);
            wrapped = true;
        }

        if (this->pos.y < -this->height && this->velocity.y <= 0) {
            this->pos.y = this->pos.y + screenHeight + this->height * 2;
            wrapped = true;
        }

        if (this->pos.y > screenHeight + this->height && this->velocity.y >= 0) {
            this->pos.y = this->pos.y - (screenHeight + this->height * 2);
            wrapped = true;
        }

        if (wrapped) {
            this->prevPos.x = this->pos.x;
            this->prevPos.y = this->pos.y;
        }
    }

    virtual bool canCollide() {
        return true;
    }

    virtual bool hasMass() {
        return true;
    }

    virtual float getMass() {
        return this->mass;
    }

    virtual bool shouldRemove() {
        return this->hitpoints <= 0;
    }
};

#endif
