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

#endif	/* boards */
