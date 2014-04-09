#include <darnit/darnit.h>


int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA imgdat;
	DARNIT_TILESHEET *ts;
	int i;

	d_init_custom("Image compression test", 320, 240, 0, "imgcompr", NULL);

	imgdat = d_img_load_raw(argv[1]);
	ts = d_render_tilesheet_new(1, 1, 320, 240, DARNIT_PFORMAT_RGBA8);

	/* Convert image to R2GB3 for reference */
	for (i = 0; i < 320 * 240; i++)
		imgdat.data[i] &= 0xE0F0C0;
	d_render_tilesheet_update(ts, 0, 0, 320, 240, imgdat.data);

	for (;;) {
		d_render_begin();
	
		d_render_tile_blit(ts, 0, 0, 0);
		
		d_loop();
		d_render_end();
	}
}
