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
	result->boards = boards_new(tablesize);
	boards_get(result->boards, result->start->panels)->board = result->start;
	boards_get(result->boards, result->goal->panels)->board = result->goal;
	/* result->start and result->goal will be free()ed through boards_delete */

	return result;
}

void
game_delete(struct game_s *game)
{
	boards_delete(game->boards);
	free(game);
}

struct board_s *
game_step(struct game_s *game, struct board_s *current, int vf)
{
	int min_to_goal;
	struct board_s *cur_board;
	struct board_datum_s *datum;
	char next_panels[BOARD_LEN_MAX];
	int i, len;
	char *solution;

	if (vf > 3) board_show(current, stderr);
	if (vf > 2)
		{
			len = current->steps;
			solution = (char *) malloc(len + 1);
			i = len;
			solution[i] = '\0';
			i--;
			cur_board = current->prev;
			while(cur_board)
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
			cur_board = current->prev;
			while(cur_board)
				{
					solution[i] = BOARD_MOVES[cur_board->direction];
					cur_board = cur_board->prev;
					i--;
				}
			fprintf(stdout, "%s\n", solution);
			free(solution);
			return current->prev;
		}

	/* can we go further away? */
	if (game->max_step < current->steps + min_to_goal)
		{
			/* already used up the steps */
			if (vf > 3)
				fprintf(stderr, "steps limit: max:%d < current:%d + minimum to goal:%d\n", game->max_step, current->steps, min_to_goal);
			current->direction = 4;
			return current->prev;
		}

	/* ok, look around */
	while(current->direction < 4)
		{
			if (board_move(current, current->direction, next_panels))
				{
					/* can move to this direction */
					if (vf > 3)
						fprintf(stderr, "looking into %c\n", BOARD_MOVES[current->direction]);
					datum = boards_get(game->boards, next_panels);
					if (!datum->board)
						{
							/* new visit */
							if (vf > 3)
								fprintf(stderr, "new visit\n");
							datum->board = board_new(current->width, current->height, next_panels);
						}
					if (datum->board->steps > current->steps + 1)
						{
							/* shorter path than previous visit */
							datum->board->prev = current;
							datum->board->steps = current->steps + 1;
							datum->board->direction = -1;
							/* visit there */
							if (vf > 3)
								fprintf(stderr, "moving to %c\n", BOARD_MOVES[current->direction]);
							return datum->board;
						}
					if (vf > 3)
						fprintf(stderr, "steps limit: previous %d > current %d + 1\n", datum->board->steps, current->steps + 1);
				}
			current->direction++;
		}

	/* move back */
	if (vf > 3) fprintf(stderr, "moving back to direction:%d\n", current->direction);
	return current->prev;
}
