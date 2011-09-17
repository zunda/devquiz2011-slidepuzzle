#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <game.h>

#define DEFAULT_TTL 256
#define INFLATE_TTL 1.2

#ifdef COUNT_MALLOC
#include <count-malloc.h>
extern int malloc_count;
#endif

void usage(FILE *stream);

void
usage(FILE *stream)
{
	fputs("usage: solve [options]\n", stream);
	fputs("trys to solve `slidingpuzzle' from Google Developer Day 2011 Japan DevQuiz\n", stream); 
	fputs("and prints out solutions to stdout\n", stream);
	fputs("\nREQUIRED ARGUMENTS\n", stream);
	fputs("-p [problem] : problem string, e.g. 3,3,40=215=86\n", stream);
	fputs("\nOPTIONS\n", stream);
	fputs("-v           : increase verbosity to stderr\n", stream);
	fprintf(stream, "-n STEPS     : max number of steps. Default: %d\n", DEFAULT_TTL);
}

int
main(int argc, char **argv)
{
	int ngoals;
	struct game_s *game;
	unsigned char h = 0, w = 0;
	char *panels = NULL;
	int vf = 0;
	int cur_ttl, new_ttl, ttl = DEFAULT_TTL;;
	unsigned long nq;

	int o;
	while((o = getopt(argc, argv, "hvp:n:")) != -1)
		{
			switch(o)
				{
				case 'n':
					ttl = atoi(optarg);
					break;
				case 'v':
					vf++;
					break;
				case 'p':
					w = (unsigned char) strtol(optarg, &panels, 10);
					if (!panels || *panels != ',' || w < 1)
						{
							fprintf(stderr, "format error on -p option: %s\n", optarg);
							return EXIT_FAILURE;
						}
					h = (unsigned char) strtol(panels + 1, &panels, 10);
					if (!panels || *panels != ',' || w < 1)
						{
							fprintf(stderr, "format error on -p option: %s\n", optarg);
							return EXIT_FAILURE;
						}
					panels++;
					break;
				case 'h':
					usage(stdout);
					return EXIT_SUCCESS;
				default:
					usage(stderr);
					return EXIT_FAILURE;
				}
		}
	if (!panels)
		{
			fprintf(stderr, "-p option is not supplied\n");
			return EXIT_FAILURE;
		}

	cur_ttl = (w + h) * 2;
	do {
		game = game_new(w, h, panels, (w*h)*(w*h)*(w*h)*(w*h)*(w*h), vf);

		if (vf > 1) board_show(game->start, stderr);
		if (vf > 0)
			fprintf(stderr, "Looking for solutions within %d steps\n", cur_ttl);

		game->max_step = cur_ttl;
		do {
			nq = game_step(game, vf);
		} while(nq > 0);

		ngoals = game->ngoals;
		new_ttl = cur_ttl * INFLATE_TTL;
		if (cur_ttl < ttl && ttl < new_ttl) new_ttl = ttl;
		if (new_ttl == cur_ttl) new_ttl++;
		cur_ttl = new_ttl;

		game_delete(game);
	} while(ngoals == 0 && cur_ttl <= ttl);

	return ngoals > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
