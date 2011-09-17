#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <boards.h>

#ifdef COUNT_MALLOC
#include <count-malloc.h>
extern int malloc_count;
#endif

#define NHASH 128
int ascii_to_hash[NHASH];
int ascii_to_hash_max;

static inline BOARD_INDEX_T boards_hash_inline(const char *string, const BOARD_INDEX_T tablesize);

static inline BOARD_INDEX_T
boards_hash_inline(const char *string, const BOARD_INDEX_T tablesize)
{
	volatile const char *ptr;
	volatile BOARD_INDEX_T r;

	r = 0;
	ptr = string;
	while(*ptr)
		{
			r *= ascii_to_hash_max;
			r += ascii_to_hash[(int) *ptr];
			ptr++;
		}

	return r % tablesize;
}

struct boards_s *
boards_new(BOARD_INDEX_T tablesize)
{
	int i, j;
	for(i = 0; i < NHASH; i++) ascii_to_hash[i] = 0;
	for(i = '0'; i <= '9'; i++) ascii_to_hash[i] = i;
	j = i;
	for(i = 'A'; i <= 'Z'; i++, j++) ascii_to_hash[i] = j;
	ascii_to_hash_max = j;

	struct boards_s *result;
	result = (struct boards_s *) malloc(sizeof(struct boards_s));
	result->tablesize = tablesize;
	result->table = (struct board_datum_s **) malloc(sizeof(struct board_datum_s *) * tablesize);
	memset(result->table, 0, sizeof(struct board_datum_s *) * tablesize);
	return result;
}

void
boards_delete(struct boards_s *boards)
{
	BOARD_INDEX_T i;
	struct board_datum_s *datum, *next;

	for(i = 0; i < boards->tablesize; i++)
		{
			datum = boards->table[i];
			while(datum)
				{
					next = datum->next;
					board_delete(datum->board);
					free(datum);
					datum = next;
				}
		}
	free(boards->table);
	free(boards);
}

struct board_datum_s *
boards_get(struct boards_s *boards, const char *panels)
{
	struct board_datum_s *prev, *cur;
	BOARD_INDEX_T i;

	i = boards_hash_inline(panels, boards->tablesize);
	prev = NULL;
	cur = boards->table[i];
	while(cur)
		{
			if (cur->board && !strcmp(panels, cur->board->panels)) break;
			prev = cur;
			cur = cur->next;
		}

	if (!cur)
		{
			cur = (struct board_datum_s *) malloc(sizeof(struct board_datum_s));
			cur->board = NULL;
			cur->next = NULL;
			if (prev)
				prev->next = cur;
			else
				boards->table[i] = cur;
		}

	return cur;
}

struct boards_queue_s *
boards_queue_new(void)
{
	struct boards_queue_s *result;

	result = (struct boards_queue_s *) malloc(sizeof(struct boards_queue_s));
	result->first = NULL;
	result->last = NULL;
	result->n = 0;
	return result;
}

void
boards_queue_delete(struct boards_queue_s *queue)
{
	struct board_datum_s *cur, *next;
	cur = queue->first;
	while(cur)
		{
			next = cur->next;
			board_delete(cur->board);
			free(cur);
			cur = next;
		}
	free(queue);
}

unsigned long
boards_enque(struct boards_queue_s *queue, struct board_s *board)
{
	struct board_datum_s *datum;
	datum = (struct board_datum_s *) malloc(sizeof(struct board_datum_s));
	datum->board = board;
	if (queue->last)
		{
			datum->next = NULL;
			queue->last->next = datum;
		}
	else
		{
			datum->next = queue->first;
			queue->first = datum;
		}
	queue->last = datum;
	queue->n++;
	return queue->n;
}

struct board_s *
boards_deque(struct boards_queue_s *queue)
{
	struct board_datum_s *datum;
	struct board_s *result;
	datum = queue->first;
	if (!datum) return NULL;

	result = datum->board;
	queue->first = datum->next;
	if (!datum->next) queue->last = NULL;
	free(datum);
	queue->n--;
	return result;
}
