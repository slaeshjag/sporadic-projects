/* tree.c - Steven Arnow <s@rdw.se>,  2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define	HUFFMAN_BITS	3

int freq[(1 << HUFFMAN_BITS)];
int bits;
int bits_left;

void analyze(char data) {
	int i;

	freq[data & (0xFF >> (8 - HUFFMAN_BITS))]++;
	return;
}


char extract(FILE *fp) {
	char ret = 0;
	int data = 0;

	if (bits_left < HUFFMAN_BITS) {
		fread(&data, 1, 1, fp);
		bits |= (data << bits_left);
		bits_left += 8;
	}
	
	if (feof(fp))
		return bits_left;
	
	ret = bits & (0xFF >> (8 - HUFFMAN_BITS)), bits_left -= HUFFMAN_BITS, bits >>= HUFFMAN_BITS;
	return ret;
}


int main(int argc, char **argv) {
	int i, total;
	FILE *fp;

	fp = fopen(argv[1], "r");
	while (!feof(fp))
		analyze(extract(fp));
	for (i = 0; i < (1 << HUFFMAN_BITS); i++) {
		fprintf(stderr, "0x%X: %i\n", i, freq[i]);
		total += freq[i];
	}

	fprintf(stderr, "Total chunks: %i, average would be %i\n", total, total >> HUFFMAN_BITS);
	
	return 0;
}
