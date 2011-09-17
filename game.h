#ifndef _GAME_H
#define _GAME_H

#include <boards.h>
#include <board.h>

struct game_s
{
	int ngoals;
	int max_step;

	struct board_s *start;
	struct board_s *goal;
	struct boards_queue_s *open;	/* nodes to be visited */
	struct boards_s *closed;	/* nodes that have been visited */
};

struct game_s *game_new(const char width, const char height, const char *panels,
BOARD_INDEX_T tablesize, int vf);
void game_delete(struct game_s *game);

unsigned long game_step(struct game_s *game, int vf);

#endif
