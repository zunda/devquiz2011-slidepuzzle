#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <board.h>

#ifdef COUNT_MALLOC
#include <count-malloc.h>
extern int malloc_count;
#endif

static int board_panel_cmp(const void *a, const void *b);

static int
board_panel_cmp(const void *a, const void *b)
{
  return (*(unsigned char *)a - *(unsigned char *)b);
}

struct board_s *
board_new(const char width, const char height, const char *panels)
{
	struct board_s *result;
	result = (struct board_s *) malloc(sizeof(struct board_s));

	result->width = width;
	result->height = height;
	memcpy(result->panels, panels, BOARD_LEN_MAX);
	result->direction = 0;
	result->prev = NULL;
	result->steps = INT_MAX;
	return result;
}

void
board_delete(struct board_s *board)
{
	free(board);
}

int
board_move(const struct board_s *src, const char direction, char *dst)
{
	int c2, r2;	/* new locations of SPACE */
	int off1, off2;

	/* starting location */
	off1 = strchr(src->panels, BOARD_SPACE) - src->panels;
	r2 = off1 / src->width;
	c2 = off1 - r2 * src->width;

	/* check and to new location */
	switch(direction)
		{
			case 0:	/* UP */
				if (r2 == 0) return 0;
				r2--;
				break;
			case 1:	/* DOWN */
				if (r2 == src->height - 1) return 0;
				r2++;
				break;
			case 2:	/* LEFT */
				if (c2 == 0) return 0;
				c2--;
				break;
			case 3:	/* RIGHT */
				if (c2 == src->width - 1) return 0;
				c2++;
				break;
		}
	off2 = r2 * src->width + c2;
	if (src->panels[off2] == BOARD_WALL) return 0;

	/* move the panels */
	memcpy(dst, src->panels, BOARD_LEN_MAX);
	dst[off2] = src->panels[off1];
	dst[off1] = src->panels[off2];
	return 1;
}

void
board_make_goal(struct board_s *src)
{
	unsigned char i, j, len;
	char movable[BOARD_LEN_MAX];

	len = src->width * src->height;
	for(i = 0, j = 0; i < len; i++)
		{
			if (src->panels[i] != BOARD_WALL)
				{
					movable[j] = src->panels[i] == BOARD_SPACE ? CHAR_MAX : src->panels[i];
					j++;
				}
		}	/* now j holds number of movable panels */
	qsort(movable, j, 1, board_panel_cmp);

	for(i = 0, j = 0; i < len; i++)
		{
			if (src->panels[i] != BOARD_WALL)
				{
					src->panels[i] = movable[j] == CHAR_MAX ? BOARD_SPACE : movable[j];
					j++;
				}
		}
}

int
board_difference(const struct board_s *a, const struct board_s *b)
{
	int i, r;
	for(i = 0, r = 0; i < a->width * a->height; i++)
		if (a->panels[i] != b->panels[i]) r++;
	return r;
}

void
board_show(const struct board_s *board, FILE *stream)
{
	int i;
	for(i = 0; i < board->width * board->height; i++)
		{
			if (i % board->width == 0 && i != 0) fputc('\n', stream);
			fputc(board->panels[i], stream);
		}
	fputc('\n', stream);
}
