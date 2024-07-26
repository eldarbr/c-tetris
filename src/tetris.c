#include "game/tetris/lib.h"
#include "gui/cli/front.h"

void game_loop(void);

int main(void) {
  game_loop();
  return 0;
}

void game_loop(void) {
  init_cli();
  initGame();
  GameInfo_t frame = updateCurrentState();
  while (!getGameHasFinished()) {
    frontend_draw_game_scene(&frame, getGameOver(), getPause());
    UserAction_t user_input = 0;
    if (get_user_input(&user_input)) {
      userInput(user_input, true);
    }
    frontend_interframe_delay();
    frame = updateCurrentState();
  }
  free_cli();
}
