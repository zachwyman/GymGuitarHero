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
  sound(),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  nightsky("blackBackground", Gamedata::getInstance().getXmlInt("blackBackground/factor") ),
  viewport( Viewport::getInstance() ),
  sprites(),
  timesRed(),
  timesBlue(),
  timesGreen(),
  playRecording(false),
  recording(false),
  currentSprite(0),
  counter(0),
  makeVideo( false )
{
//  sprites.push_back(new MultiSprite("Football"));
  sprites.push_back(new Sprite("Red"));

  Viewport::getInstance().setObjectToTrack(sprites[currentSprite]);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  nightsky.draw();

  for (unsigned int i = 0; i < sprites.size(); i++) {
    sprites[i]->draw();
  }
  //
  // if (recording) {
  //   std::stringstream strm;
  //   strm << "Recording time: " << (clock.getTicks() - times[0]) / 1000 << "." << (clock.getTicks()-times[0])%1000;
  //   io.writeText(strm.str(), {255, 0, 255, 255}, 30, 440);
  // }
  //
  // if (playRecording) {
  //   std::stringstream strm;
  //   strm << "Playing time: " << (clock.getTicks() - times[times.size()-1]) / 1000 << "." << (clock.getTicks()-times[times.size()-1 ])%1000;
  //   io.writeText(strm.str(), {255, 0, 255, 255}, 30, 440);
  // }



  SDL_RenderPresent(renderer);
}

void  Engine::checkForCollisions() {
  for (unsigned int i = 0; i < sprites.size(); i++) {
    if(sprites[i]->getX() > 700) {
      static_cast<Sprite*>(sprites[i])->explode();
    }
  }
}

void Engine::update(Uint32 ticks) {
  checkForCollisions();
  for (unsigned int i = 0; i < sprites.size(); i++) {
    sprites[i]->update(ticks);
  }
  if (playRecording) {
    int weight = -timesRed[0]+timesRed[timesRed.size()-1]-3500;

    for (unsigned int i = 1; i < timesRed.size()-1; i++) {
      float dist = timesRed[i]-timesRed[0];
      float start = dist / 3350 * 854;
      int st = start;
      if (dist < 3350 && dist > 0) {
        std::cout << st << std::endl;
        sprites.push_back(new Sprite("Red", st));
        timesRed[i] -= 4000;
      }
      else if (timesRed[i]+weight > (clock.getTicks()) && (timesRed[i]+weight < (clock.getTicks()+100))) {
        sprites.push_back(new Sprite("Red"));
        timesRed[i] = 0 - timesRed[i];
      }
    }

    for (unsigned int i = 0; i < timesGreen.size(); i++) {
      float dist = timesGreen[i]-timesRed[0];
      std::cout << dist << std::endl;
      float start =  (1 - (dist / 3500)) * 854;
      int st = start;
      if (dist < 3500 && dist > 0) {
        std::cout << st << std::endl;
        sprites.push_back(new Sprite("Green", st));
        timesGreen[i] -= 4000;
      }
      else if (timesGreen[i]+weight > (clock.getTicks()) && (timesGreen[i]+weight < (clock.getTicks()+100))) {
        sprites.push_back(new Sprite("Green"));
        timesGreen[i] = 0 - timesGreen[i];
      }
    }

    for (unsigned int i = 0; i < timesBlue.size(); i++) {
      float dist = timesBlue[i]-timesRed[0];
      float start = dist / 3350 * 750;
      int st = start;
      if (dist < 3350 && dist > 0) {
        std::cout << st << std::endl;
        sprites.push_back(new Sprite("Blue", st));
        timesBlue[i] -= 4000;
      }
      else if (timesBlue[i]+weight > (clock.getTicks()) && (timesBlue[i]+weight < (clock.getTicks()+100))) {
        sprites.push_back(new Sprite("Blue"));
        timesBlue[i] = 0 - timesBlue[i];
      }
    }

  }
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
          sound.startMusic();
          timesRed.clear();
          timesBlue.clear();
          timesGreen.clear();
          recording = true;
          playRecording = false;
          timesRed.push_back(clock.getTicks());
        }
        if (keystate[SDL_SCANCODE_J] && recording) {
          timesRed.push_back(clock.getTicks());
          sprites.push_back(new Sprite("Red"));
        }
        if (keystate[SDL_SCANCODE_K] && recording) {
          timesGreen.push_back(clock.getTicks());
          sprites.push_back(new Sprite("Green"));
        }
        if (keystate[SDL_SCANCODE_L] && recording) {
          timesBlue.push_back(clock.getTicks());
          sprites.push_back(new Sprite("Blue"));
        }
        if (keystate[SDL_SCANCODE_G]) {
          sound.stopMusic();
          sound.startMusic();
        //  makeVideo = true;
          recording = false;
          playRecording = true;
          timesRed.push_back(clock.getTicks());
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
