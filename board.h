#ifndef _BOARD_H
#define _BOARD_H

#include <stdio.h>

#define BOARD_MOVES "UDLR^"
#define BOARD_SPACE '0'
#define BOARD_WALL '='
#define BOARD_LEN_MAX (6*6 + 1)	/* including \0 */

struct board_s {
	char width;
	char height;
	char panels[BOARD_LEN_MAX];
	int direction;	/* 0-3:checking ways to goal 4:retired */
	struct board_s *prev;
	int steps;	/* steps from start */
};

struct board_s *board_new(const char width, const char height, const char *panels);
void board_delete(struct board_s *board);

int board_move(const struct board_s *src, const char direction, char *dst);
	/* returns 1 and set to_panels if movable */
	/* BOARD_NMOVES has to be allocated to to_panels */

void board_make_goal(struct board_s *src);
	/* sorts src->panels */

int board_difference(const struct board_s *a, const struct board_s *b);
	/* returns number of panels that are at different locations */

void board_show(const struct board_s *board, FILE *stream);

#endif /* board.h */
