#ifndef _BOARDS_H
#define _BOARDS_H

#include <board.h>

typedef unsigned long long BOARD_INDEX_T;

struct board_datum_s {
	struct board_s *board;
	struct board_datum_s *next;
};

struct boards_s {
	BOARD_INDEX_T tablesize;
	struct board_datum_s **table;
};

struct boards_s *boards_new(BOARD_INDEX_T tablesize);
void boards_delete(struct boards_s *boards);

struct board_datum_s *boards_get(struct boards_s *boards, const char *panels);

struct boards_queue_s {
	struct board_datum_s *first;
	struct board_datum_s *last;
	unsigned long n;
};

struct boards_queue_s *boards_queue_new(void);
void boards_queue_delete(struct boards_queue_s *queue);
unsigned long boards_enque(struct boards_queue_s *queue, struct board_s *board);
struct board_s *boards_deque(struct boards_queue_s *queue);

#endif	/* boards */
