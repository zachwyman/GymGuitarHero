#include <vector>
#include <SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "sound.h"
#include "viewport.h"

class Engine {
public:
  Engine ();
  ~Engine ();
  void play();
  void switchSprite();

private:
  const RenderContext* rc;
  const IOmod& io;
  SDLSound sound;
  Clock& clock;

  SDL_Renderer * const renderer;
  World nightsky;
  Viewport& viewport;

  std::vector<Drawable*> sprites;
  std::vector<int> times;

  bool playRecording;
  bool recording;

  int currentSprite;
  int counter;
  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  void printScales() const;
  void checkForCollisions();
};
