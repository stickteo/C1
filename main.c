
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void printMapEntry (uint16_t *map, int mw, char *s) {
	char *t;

	t = strtok(s, " ,");

	// skip comment
	if (t[0] == ';') {
		return;
	}

	// skip newline
	if (t[0] == '\n') {
		return;
	}
	if (t[0] == '\r') {
		return;
	}

	// label
	printf("%s:\n", t);

	int x, y, w, h;
	t = strtok(NULL, " ,");
	x = atoi(t);

	t = strtok(NULL, " ,");
	y = atoi(t);

	t = strtok(NULL, " ,");
	w = atoi(t);

	t = strtok(NULL, " ,");
	h = atoi(t);

	//printf("%d %d %d %d\n",x,y,w,h);

	if (w <= 0 || h <= 0) {
		printf(".dh 0xFFFF\n");
		return;
	}

	for (int i=0; i<h; i++) {
		printf(".dh ");
		for (int j=0; j<w; j++) {
			printf("0x%04x, ", map[mw*(y+i) + (x+j)]);
		}
		if (i<h-1) {
			printf("0xFFFE\n");
		} else {
			printf("0xFFFF\n");
		}
	}
}

int main(int argc, char** argv) {
	// open files
	if (argc < 3) {
		printf(
			"Not enough arguments.\n"
			"Usage: C1.exe desc.txt map.bin (out.asm)\n"
		);
		return 0;
	}

	if (argc >= 4) {
		freopen(argv[3],"wb",stdout);
	}

	FILE *fd, *fm, *fa;
	fd = fopen(argv[1],"rb");
	fm = fopen(argv[2],"rb");
	

	// read in descriptor
	// ------------------

	// get map width
	char *s;
	s = malloc(256);

	int w;
	w = 0;

	while (!feof(fd)) {
		if (fgets(s,256,fd) == NULL) {
			continue;
		}

		char *t;
		t = strtok(s, " ,");
		
		// skip comment
		if (t[0] == ';') {
			//printf("comment: %s\n", t);
			continue;
		}

		// skip newline
		if (t[0] == '\n') {
			continue;
		}
		if (t[0] == '\r') {
			continue;
		}

		int w2;

		//printf("token 0: %s (%d)\n", t, t[0]);
		//printf("%s ", t);
		
		t = strtok(NULL, " ,");
		w2 = atoi(t);

		t = strtok(NULL, " ,");
		t = strtok(NULL, " ,");
		w2 = w2 + atoi(t);
		//printf("%d\n", w2);

		if (w2 > w) {
			w = w2;
		}
	}

	//printf("width is %d\n", w);
	
	// read in map
	// -----------
	
	int size;
	fseek(fm, 0, SEEK_END);
	size = ftell(fm);

	uint16_t *map;
	map = malloc(size);

	fseek(fm, 0, SEEK_SET);
	fread(map, 2, size/2, fm);

	//for(int i=0; i<size/2; i++) {
	//	printf("%04x ", map[i]);
	//}

	// print out
	// ---------
	printf(".autoregion\n");

	fseek(fd, 0, SEEK_SET);
	while (!feof(fd)) {
		if (fgets(s,256,fd) == NULL) {
			continue;
		}

		//char *t;
		//t = strtok(s, " ,");
		
		

		printMapEntry(map, w, s);
	}

	printf(".endautoregion\n");

	free(s);
	return 0;
}
