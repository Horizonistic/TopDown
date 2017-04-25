#include <cmath>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <limits>
#include <cfloat>
#include <memory>
#include <ctime>

#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "Utils.h"
#include "Engine.h"
#include "BackgroundEntity.h"
#include "Powerup.h"
#include "Entity.h"
#include "Enemy.h"
#include "Timer.h"

int main()
{
    Engine engine;

    // Timers
    Timer fpsTimer;
    int countedFrames = 0;
    float averageFPS = 0.0f;
    fpsTimer.start();

    Timer capTimer;

    Timer deltaTimer;
    deltaTimer.start();

    // Stats
    std::stringstream fpsText;
    auto fps = std::make_shared< Text >(engine.camera, 10, 10);
    engine.initText(fps, phraktal::levels::DEFAULT_FONT_SIZE);

    std::stringstream statsText;
    auto stats = std::make_shared< Text >(engine.camera, 10, 50);
    engine.initText(stats, phraktal::levels::DEFAULT_FONT_SIZE);

    // Textures
    auto background = std::make_shared< BackgroundEntity >(engine.camera, 0, 0);
    engine.initEntity(background, phraktal::assets::BG_PNG);

    // todo: HUD items

    // Shot power bar
    SDL_Rect* filled = new SDL_Rect{phraktal::levels::SCREEN_WIDTH - 311, 11, 0, 35};
    SDL_Rect* barOutline = new SDL_Rect{phraktal::levels::SCREEN_WIDTH - 312, 10, 301, 37};

    // Keystates
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Entities
    auto player = std::make_shared< Player >(engine.camera, 0, 0);
    engine.initEntity(player, phraktal::assets::PLAYER_PNG);

    // Bullets
    std::vector< Bullet > bullets;

    // todo: temp
    bool leftMouseButtonState = false;
    int shotPower = 0;

    // Main loop
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        capTimer.start();
        /*
        if (fpsTimer->getTicks() - secondCounter > 3000)
        {
            secondCounter = fpsTimer->getTicks();
            std::srand((uint) std::time(0));
            int x = (std::rand() % phraktal::levels::LEVEL_WIDTH);
            int y = (std::rand() % phraktal::levels::LEVEL_HEIGHT);
            auto testEnemy = std::make_shared< Enemy >(camera);
            testEnemy->setRenderer(renderer);
            testEnemy->loadTexture(phraktal::assets::ENEMY_PNG);
            testEnemy->setPos(x, y);
            testEnemy->setCurrentTarget(player);
            testEnemy->toggleActive();
            grid->addEntity(testEnemy);
        }
         */
        player->handleKeystates(keystates);

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else
            {
                engine.handleEvents(e);
            }
        }

        // FPS
        averageFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
        if (averageFPS > 20000)
        {
            averageFPS = 0;
        }
        fpsText.str("");
        fpsText << "FPS: " << averageFPS;
        engine.updateText(fps, fpsText.str());

        // Stats
        statsText.str("");
        statsText << engine.getStats();
        engine.updateText(stats, statsText.str());

        // Delta time
        float dTime = deltaTimer.getTicks() / 1000.f;

        if (player->getShotCooldown() != 0)
        {
            filled->w = (int) (((float) player->getShotCooldown() / (float) player->getMaxShotCooldownTime()) * 100.f) * 3;
        }

        // Update all entities
        engine.updateEntities(dTime);
        engine.resetGrid();

        // Camera
        engine.camera.update();

        deltaTimer.start();

        // Render textures
        engine.setDrawColor(0x00, 0x00, 0x00, 0xFF);
        engine.rendererClear();
        engine.renderEntity(background);
        engine.renderEntities();

        // Player shot cooldown bar
        {
            int percentage = (int) (((float) player->getShotCooldown() / (float) player->getMaxShotCooldownTime()) * 100.f);
            SDL_Color color{0, 0, 0, 255};

            if (percentage == 100)
            {
                color.r = 0;
                color.g = 255;
            }
            else if (percentage > 50)
            {
                color.r = (Uint8) ((100 - (((float) player->getShotCooldown() / (float) player->getMaxShotCooldownTime()) * 100.f)) / phraktal::levels::HALF_PERCENT_TO_COLOR_CONVERSION);
                color.g = 255;
            }
            else
            {
                color.r = 255;
                color.g = (Uint8) (((float) player->getShotCooldown() / (float) player->getMaxShotCooldownTime()) * 100.f / phraktal::levels::HALF_PERCENT_TO_COLOR_CONVERSION);
            }

            engine.setDrawColor(color.r, color.g, color.b, color.a);
        }
        engine.renderRectangleFilled(*filled);
        engine.setDrawColor(255, 255, 255, 255);

        engine.renderRectangleOutline(*barOutline);

        engine.rendererPresent();

        // FPS
        countedFrames++;
        Uint32 frameTicks = capTimer.getTicks();
        if (frameTicks < phraktal::levels::SCREEN_TICKS_PER_FRAME)
        {
            SDL_Delay(phraktal::levels::SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }
}