#include <darnit/darnit.h>
#include <GL/gl.h>

#define	GRAYSCALE_BITS	6
#define	RLE_MAX		4
#define	WIDTH		640
#define	HEIGHT		480


int rle_encode(int *luma, int w, int h) {
	int rle_cnt, rle[w*h], i, old, old_cnt, rle_bits;

	for (i = old = rle_cnt = rle_bits = old_cnt = 0; i < w * h; i++) {
		if (luma[i] == old && old_cnt < (1 << RLE_MAX));
		else {
			if (old_cnt <3) {
				rle[rle_cnt++] = old;
				rle_bits++;
				if (old_cnt == 2)
					rle[rle_cnt++] = old, rle_bits++;
			} else {
				rle[rle_cnt++] = 0;
				rle[rle_cnt++] = old;
				rle[rle_cnt++] = old_cnt;
				rle_bits += GRAYSCALE_BITS + 4 + RLE_MAX;
			}

			old = luma[i];
			old_cnt = 0;
		}
		old_cnt++;
	}

	fprintf(stderr, "Raw: %i bytes, RLE: %i bytes\n", w * h * GRAYSCALE_BITS / 8, rle_bits / 8);
}


int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA imgdat;
	DARNIT_TILESHEET *ts;
	int *luma;
	int i, old, cnt;

	d_init_custom("Image compression test", WIDTH, HEIGHT, 0, "imgcompr", NULL);

	imgdat = d_img_load_raw(argv[1]);
	ts = d_render_tilesheet_new(1, 1, imgdat.w, imgdat.h, DARNIT_PFORMAT_RGBA8);
	luma = malloc(sizeof(int) * imgdat.w * imgdat.h);

	for (i = 0; i < imgdat.w*imgdat.h; i++) {
		luma[i] = ((imgdat.data[i] & 0xFF) * 64 / 256 + ((imgdat.data[i] & 0xFF00) >> 8) * 128 / 256 + ((imgdat.data[i] & 0xFF0000) >> 16) * 24 / 256 + 16) & ~(0xFF >> GRAYSCALE_BITS);
	}

	rle_encode(luma, imgdat.w, imgdat.h);
	
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
