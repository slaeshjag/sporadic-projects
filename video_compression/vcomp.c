#include <darnit/darnit.h>
#include <GL/gl.h>


static int residual_bits = 0;
static int residual_bits_cnt = 0;
/* Defaults */
static int grayscale_bits = 6;
static int rle_max = 5;
static int luma_smudge_diff = 10;

#define	GRAYSCALE_BITS	grayscale_bits
#define	RLE_MAX		rle_max
#define	WIDTH		640
#define	HEIGHT		480
#define	LUMA_SMUDGE_DIFF	luma_smudge_diff
#define	abs(x)		(((x) < 0) ? (-(x)) : (x))


void push_bits(FILE *fp, int bits, int bits_cnt) {

	bits &= (0xFFFF >> (16 - bits_cnt));
	residual_bits |= (bits << residual_bits_cnt);
	residual_bits_cnt += bits_cnt;
	while (residual_bits_cnt >= 8) {
		fwrite(&residual_bits, 1, 1, fp);
		residual_bits >>= 8;
		residual_bits_cnt -= 8;
	}
}


void flush_bits(FILE *fp) {
	while (residual_bits_cnt > 0) {
		fwrite(&residual_bits, 1, 1, fp);
		residual_bits >>= 8;
		residual_bits_cnt -= 8;
	}
}
	

int rle_encode(int *luma, int w, int h, FILE *fp) {
	int i, old, old_cnt, rle_bits, j;

	for (i = old = rle_bits = old_cnt = 0; i < w * h; i++) {
		if (luma[i] == old && old_cnt < (1 << RLE_MAX));
		else {
			if (old_cnt * GRAYSCALE_BITS < GRAYSCALE_BITS + 3 + RLE_MAX) {
				for (j = 0; j < old_cnt; j++) {
					push_bits(fp, old, GRAYSCALE_BITS);
				}
			} else {
				push_bits(fp, 0xFF, 3);
				push_bits(fp, old, GRAYSCALE_BITS);
				push_bits(fp, old_cnt, RLE_MAX);
			}

			old = luma[i];
			old_cnt = 0;
		}
		old_cnt++;
	}

	fprintf(stderr, "Raw: %i bytes, RLE: %i bytes\n", w * h * GRAYSCALE_BITS / 8, rle_bits / 8);
}


void smudge_compress(int *luma, int w, int h) {
	int i, j, p, r;

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if (j < 2 && !i)
				continue;
			p = luma[w * i + j - 2];
			r = luma[w * i + j];
			if (abs(p - r) < LUMA_SMUDGE_DIFF)
				luma[w * i + j - 1] = luma[w * i + j - 2];
		}
	}

	return;
}


/* NOTE: Add 16 on the client side, removed it for more efficient RLE */
int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA imgdat;
	DARNIT_TILESHEET *ts;
	int *luma;
	int i;
	FILE *out;

	d_init_custom("Image compression test", WIDTH, HEIGHT, 0, "imgcompr", NULL);
	if (argc >= 3)
		grayscale_bits = atoi(argv[2]);
	if (argc >= 4)
		rle_max = atoi(argv[3]);
	if (argc >= 5)
		luma_smudge_diff = atoi(argv[4]);
		

	imgdat = d_img_load_raw(argv[1]);
	ts = d_render_tilesheet_new(1, 1, imgdat.w, imgdat.h, DARNIT_PFORMAT_RGBA8);
	luma = malloc(sizeof(int) * imgdat.w * imgdat.h);
	out = fopen("rle_encode.dat", "w");

	for (i = 0; i < imgdat.w*imgdat.h; i++) {
		luma[i] = ((imgdat.data[i] & 0xFF) * 64 / 256 + ((imgdat.data[i] & 0xFF00) >> 8) * 128 / 256 + ((imgdat.data[i] & 0xFF0000) >> 16) * 16 / 256) & ~(0xFF >> GRAYSCALE_BITS);
	}

	smudge_compress(luma, imgdat.w, imgdat.h);
	rle_encode(luma, imgdat.w, imgdat.h, out);
	flush_bits(out);
	fclose(out);
	
	/* Convert image to R2GB3 for reference */
	for (i = 0; i < imgdat.w * imgdat.h; i++)
		imgdat.data[i] = luma[i] + (luma[i] << 8) + (luma[i] << 16);
	d_render_tilesheet_update(ts, 0, 0, imgdat.w, imgdat.h, imgdat.data);

	for (;;) {
		d_render_begin();
		
		d_render_tile_blit(ts, 0, 0, 0);
		
		d_loop();
		d_render_end();
	}
}
