#include <math.h>
#include "Vector2.h"
#include "Entity.h"
#include "Particle.h"
#include "Obstacle.h"
#include "Spaceship.h"

class Cow : public Obstacle {
public:
  Cow(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity) : Obstacle(renderer, "cow.bmp", 10, 300, 28, 17, 2, pos, velocity) {};
};

class Asteroid : public Obstacle {
public:
  Asteroid(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity) : Obstacle(renderer, "asteroid.bmp", 50, 600, 32, 32, 2, pos, velocity) {};
};

class Planet : public Obstacle {
public:
  Planet(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity) : Obstacle(renderer, "planet.bmp", 75, 1000, 32, 32, 3, pos, velocity) {};
};

void addObstacle(SDL_Renderer* renderer) {
  int obstacleType = rand() % 3;

  Entity* obstacle;

  Vector2 obstaclePos = Vector2(randomFloat() * screenWidth, randomFloat() * screenHeight);
  Vector2 obstacleVelocity = Vector2(randomFloat() * 40 - 20, randomFloat() * 40 - 20);

  switch (obstacleType) {
  case 0:
    obstacle = new Cow(renderer, obstaclePos, obstacleVelocity);
    break;
  case 1:
    obstacle = new Asteroid(renderer, obstaclePos, obstacleVelocity);
    break;
  case 2:
    obstacle = new Planet(renderer, obstaclePos, obstacleVelocity);
    break;
  }

  entities.push_back(obstacle);
}

int main(int args, char* argv[]) {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window* window = SDL_CreateWindow("Gravity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

  if (window == NULL) {
    std::cout << "Failed to create window: " << SDL_GetError();
    exit(-1);
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == NULL) {
    std::cout << "Failed to create renderer: " << SDL_GetError();
    exit(-1);
  }

  bool running = true;

  int lastUpdateTicks = 0;
  int beginTime = 0;
  int elapsedTicks = 0;
  
  int updatePeriod = 33;

  std::vector<Entity*>::iterator entitiesIter;

  for (int x = 0; x < 2; x++) {
    addObstacle(renderer);
  }

  while (running) {
    int mX, mY;
    SDL_GetMouseState(&mX, &mY);

    Vector2 mousePosition = Vector2(mX, mY);

    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        std::cout << "Closed by user\n";
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (!placedSpaceship) {
          entities.push_back(new Spaceship(renderer, mousePosition));
          placedSpaceship = true;
        }
        break;
      }
    }

    int ticks = SDL_GetTicks();
    int deltaTicks = ticks - lastUpdateTicks;
    int updateCount = 0;

    while (deltaTicks > updatePeriod) {
      entitiesIter = entities.begin();
      while (entitiesIter < entities.end()) {
        Entity* entity = *entitiesIter;
        if (entity->shouldRemove()) {
          entitiesIter = entities.erase(entitiesIter);
          continue;
        }

        ++entitiesIter;
      }

      std::vector<Entity*>::size_type entitiesSize = entities.size();
      for (std::vector<Entity*>::size_type i = 0; i < entitiesSize; ++i) {
        Entity* entity = entities[i];
        entity->update(deltaTicks, mousePosition);
      }

      if (elapsedTicks % 300 == 0) {
        addObstacle(renderer);
      }

      deltaTicks = deltaTicks - updatePeriod;
      lastUpdateTicks = lastUpdateTicks + updatePeriod;
      updateCount = updateCount + 1;
      elapsedTicks++;

      if (updateCount >= 3) {
        lastUpdateTicks = ticks;
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (entitiesIter = entities.begin(); entitiesIter != entities.end(); ++entitiesIter) {
      Entity* entity = *entitiesIter;
      entity->render(deltaTicks / float(updatePeriod));
    }

    if (!placedSpaceship) {
      SDL_Rect rect = { mousePosition.x - 16, mousePosition.y - 16, 32, 32 };
      SDL_RenderCopy(renderer, loadTexture(renderer, "Assets/cursor.bmp"), NULL, &rect);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}