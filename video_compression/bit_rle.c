/* bit_rle.c - Steven Arnow <s@rdw.se>,  2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int bits = 0;
int bits_remain = 0;


char fetch_bit(FILE *fp) {
	int n;

	if (!bits_remain) {
		bits = 0;
		fread(&bits, 1, 1, fp);
		if (feof(fp))
			return 0;
		bits_remain = 8;
	
	}
	n = bits & 1;
	bits >>= 1, bits_remain--;
	return n;
}


int main(int argc, char **argv) {
	int last_bit, last_bit_cnt, new_bit, bad_cnt;
	FILE *fp = fopen(argv[1], "r");
	int size = 0;

	for (last_bit = bad_cnt = 0, last_bit_cnt = 0; !feof(fp);) {
		if ((new_bit = fetch_bit(fp)) == last_bit && last_bit_cnt < 16)
			last_bit_cnt++;
		else {
			if (last_bit_cnt >= 5) {
				fprintf(stdout, "%i\n", last_bit_cnt);
				size += 5;
			} else {
				bad_cnt++;
				size += 5;
			}
			last_bit = new_bit;
			last_bit_cnt = 1;
		}
	}

	fprintf(stdout, "Uncompressable bit strings: %i, %i bytes\n", bad_cnt, size / 8);

	return 0;
}
