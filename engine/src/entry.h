#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

// Externally-defined function to create a game, to be filled out on the client side.
extern b8 create_game(game* out_game);

/**
 * The main entry point of the application.
 */
int main(void){

    // Request the game instance from the application.
    game game_inst;
    if (!create_game(&game_inst)) {
        RQ_FATAL("Could not create game.");
        return -1;
    }

    // Ensure the function pointers exist.
    if (!game_inst.render || !game_inst.initialize || !game_inst.update || !game_inst.on_resize) {
        RQ_FATAL("The game's function pointers must be assigned.");
        return -2;
    }

    if (!application_create(&game_inst)) {
        RQ_INFO("Applicaation failed to be created.");
        return 1;
    }

    // Begin game loop
    if (!application_run()) {
        RQ_INFO("Application did not shutdown smoothly and gracefully.");
        return 2;
    }

    return 0;
}