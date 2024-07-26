#ifndef GAME_FRONTEND
#define GAME_FRONTEND

#include "../../game/lib.h"

#define USERINPUT_COOLDOWN_MS 20

void init_cli(void);
void frontend_draw_game_scene(const GameInfo_t *, bool game_over, bool pause);
bool get_user_input(UserAction_t *);
void free_cli(void);
void frontend_interframe_delay(void);

#endif
