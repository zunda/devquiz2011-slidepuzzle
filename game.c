#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <game.h>

#ifdef COUNT_MALLOC
#include <count-malloc.h>
extern int malloc_count;
#endif

struct game_s *
game_new(const char width, const char height, const char *panels, BOARD_INDEX_T tablesize, int vf)
{
	char buf[BOARD_LEN_MAX];
	struct game_s *result;

	result = (struct game_s *) malloc(sizeof(struct game_s));
	strncpy(buf, panels, sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
	result->ngoals = 0;
	result->start = board_new(width, height, buf);
	result->start->steps = 0;
	result->goal = board_new(width, height, buf);
	board_make_goal(result->goal);
	result->open = boards_queue_new();
	boards_enque(result->open, result->start);
	result->closed = boards_new(tablesize);
	boards_get(result->closed, result->start->panels)->board = result->start;
	return result;
}

void
game_delete(struct game_s *game)
{
	boards_delete(game->closed);
	boards_queue_delete(game->open);
	board_delete(game->goal);
	free(game);
}

unsigned long
game_step(struct game_s *game, int vf)
{
	int min_to_goal;
	int direction;
	struct board_datum_s *datum;
	struct board_s *current, *cur_board;
	char next_panels[BOARD_LEN_MAX];
	int i, len;
	char *solution;

	current = boards_deque(game->open);
	if (!current)
		{
			fputs("no more boards to visit\n", stderr);
			return 0;
		}

	if (vf > 3) board_show(current, stderr);
	if (vf > 2)
		{
			len = current->steps;
			solution = (char *) malloc(len + 1);
			i = len;
			solution[i] = '\0';
			i--;
			cur_board = current;
			while(i >= 0)
				{
					solution[i] = BOARD_MOVES[cur_board->direction];
					cur_board = cur_board->prev;
					i--;
				}
			fprintf(stderr, "%s\n", solution);
			free(solution);
		}

	/* are we there yet? */
	min_to_goal = board_difference(game->goal, current);
	if (min_to_goal == 0)
		{
			/* goal !! */
			if (vf > 2) fprintf(stderr, "goal!\n");
			game->ngoals++;
			if (game->max_step > current->steps) game->max_step = current->steps;
			len = current->steps;
			solution = (char *) malloc(len + 1);
			i = len;
			solution[i] = '\0';
			i--;
			cur_board = current;
			while(i >= 0)
				{
					solution[i] = BOARD_MOVES[cur_board->direction];
					cur_board = cur_board->prev;
					i--;
				}
			fprintf(stdout, "%s\n", solution);
			free(solution);
			return game->open->n;
		}

	/* can we go further away? */
	if (game->max_step < current->steps + min_to_goal)
		{
			/* already used up the steps */
			if (vf > 3)
				fprintf(stderr, "steps limit: max:%d < current:%d + minimum to goal:%d\n", game->max_step, current->steps, min_to_goal);
			return game->open->n;
		}

	/* ok, look around */
	for(direction = 0; direction < 4; direction++)
		{
			if (board_move(current, direction, next_panels))
				{
					/* can move to this direction */
					if (vf > 3)
						fprintf(stderr, "looking into %c\n", BOARD_MOVES[direction]);
					datum = boards_get(game->closed, next_panels);
					if (!datum->board)
						{
							if (vf > 3)
								fprintf(stderr, "new visit\n");
							datum->board = board_new(current->width, current->height, next_panels);
							datum->board->prev = current;
							datum->board->direction = direction;
							datum->board->steps = current->steps + 1;
							boards_enque(game->open, datum->board);
						}
					/* todo: should move there if the step is shorter */
				}
		}

	return game->open->n;
}
