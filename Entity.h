#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <SDL2/SDL.h>

std::map<std::string, SDL_Texture*> textures;

SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path) {
    if (textures.count(path) != 0) {
        return textures.at(path);
    }

    SDL_Surface* surface = SDL_LoadBMP(path.c_str());

    if (surface == NULL) {
        std::cout << "Failed to create surface from file '" << path << "': " << SDL_GetError();
        exit(-1);
    }

    SDL_SetColorKey(surface, SDL_TRUE, 0);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        std::cout << "Failed to create texture from file '" << path << "': " << SDL_GetError();
        exit(-1);
    }

    SDL_FreeSurface(surface);

    textures.insert(std::pair<std::string, SDL_Texture*>(path, texture));
    return texture;
}

class Entity {
private:
    SDL_Texture* texture;
    SDL_Renderer* renderer;

public:
    std::string textureFile;
    Vector2 pos, prevPos, velocity;
    int width, height, scale;

    Entity(SDL_Renderer* renderer, std::string textureFile, Vector2 pos, int width, int height, int scale = 1) {
        this->textureFile = textureFile;
        this->texture = loadTexture(renderer, "Assets/" + textureFile);
        this->renderer = renderer;

        this->pos = pos;
        this->prevPos = Vector2(pos);
        this->velocity = Vector2(0, 0);

        this->width = width;
        this->height = height;
        this->scale = scale;
    }

    float getDeltaTime(int deltaTicks) {
        return deltaTicks / 1000.0;
    }

    virtual void update(int deltaTicks, Vector2 mousePosition) {
        float deltaTime = this->getDeltaTime(deltaTicks);

        Vector2 delta = this->velocity * deltaTime;

        this->prevPos.x = this->pos.x;
        this->prevPos.y = this->pos.y;

        this->pos += delta;
    }

    SDL_Rect getBoundBox(Vector2 pos, bool noScale = false) {
        int scale = noScale ? 1 : this->scale;

        float scaledWidth = this->width * scale;
        float scaledHeight = this->height * scale;
        SDL_Rect rect = { roundf(pos.x - scaledWidth / 2.0), roundf(pos.y - scaledHeight / 2.0), scaledWidth, scaledHeight };
        return rect;
    }

    SDL_Rect getBoundBox() {
        return this->getBoundBox(this->pos);
    }

    virtual void render(float partialTicks) {
        Vector2 pos = this->pos.lerp(this->prevPos, partialTicks);
        SDL_Rect bb = this->getBoundBox(pos);
        SDL_RenderCopy(this->renderer, this->texture, NULL, &bb);
    }

    virtual bool shouldRemove() {
        return false;
    }

    virtual bool canCollide() {
        return false;
    }

    SDL_Renderer* getRenderer() {
        return this->renderer;
    }

    SDL_Texture* getTexture() {
        return this->texture;
    }

    virtual bool hasMass() {
        return false;
    }

    virtual float getMass() {
        return 0;
    }
};

#endif
