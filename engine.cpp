#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include "sprite.h"
#include "multisprite.h"
#include "twoway.h"
#include "gamedata.h"
#include "engine.h"
#include "frameGenerator.h"

Engine::~Engine() {
  // delete star;
  // delete spinningStar;
  for (unsigned int i = 0; i < sprites.size(); i++) {
    delete sprites[i];
  }
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IOmod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  nightsky("blackBackground", Gamedata::getInstance().getXmlInt("blackBackground/factor") ),
  viewport( Viewport::getInstance() ),
  sprites(),
  currentSprite(0),
  counter(0),
  makeVideo( false )
{
//  sprites.push_back(new MultiSprite("Football"));
  sprites.push_back(new MultiSprite("Red", 0));
  Viewport::getInstance().setObjectToTrack(sprites[currentSprite]);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  nightsky.draw();

  for (unsigned int i = 0; i < sprites.size(); i++) {
    sprites[i]->draw();
  }
  // star->draw();
  // spinningStar->draw();

  // std::stringstream strm;
  // strm << "fps: " << clock.getFps();
  // io.writeText(strm.str(), {255, 0, 255, 255}, 30, 60);
  // io.writeText("Zach Wyman", {0, 0, 0, 255}, 30, 425);


  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  for (unsigned int i = 0; i < sprites.size(); i++) {
    sprites[i]->update(ticks);
  }

  // counter++;
  //
  // if (counter % 140 == 0 && counter > 900) {
  //   sprites.push_back(new MultiSprite("Football", 150));
  //   std::cout << clock.getTicks() << std::endl;
  // }
  // else if (counter % 61 && counter && counter < 10000) {
  //   sprites.push_back(new MultiSprite("Football", 300));
  // }
  // } else if (counter > 10000) {
  //   counter = counter % 1000;
  // }

  nightsky.update();
  viewport.update(); // always update viewport last
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % sprites.size();
  Viewport::getInstance().setObjectToTrack(sprites[currentSprite]);
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if ( keystate[SDL_SCANCODE_T] ) {
          switchSprite();
        }
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        if (keystate[SDL_SCANCODE_R]) {
          sprites.push_back(new MultiSprite("Blue", 0));
        }
        if (keystate[SDL_SCANCODE_W]) {
          sprites.push_back(new MultiSprite("Red", 0));
        }
        if (keystate[SDL_SCANCODE_E]) {
          sprites.push_back(new MultiSprite("Green", 0));
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
