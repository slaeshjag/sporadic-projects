#include <darnit/darnit.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "mesh.h"
#include "heightmap.h"

int main(int argc, char **argv) {
	struct mesh *m;
	float z = 3.0f, y = 2;
	d_init("GL-test!", "gltest", NULL);
	
	m = mesh_alloc(50, 50, 5);

	for (;;) {
		if (d_keys_get().up)
			z -= 0.2f;
		if (d_keys_get().down)
			z += 0.2f;
		glLoadIdentity();
		gluPerspective(45.0, 800.0f/480.0f, 0.1f, 500.0f);
		gluLookAt(0.0f, y + 1.7f, z, 0.0f, y + 1.7f, z-6.0f, 0.0f, 1.0f, 0.0f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m->tri);
		glDrawArrays(GL_POINTS, 0, m->tri_width * m->tri_depth * 3);
		glDisableClientState(GL_VERTEX_ARRAY);
		d_loop();
		if (mesh_collide_test_small(m, 0.0f, y - 0.1f, z, 1.0f, 0.5f) < 0)
			y -= 0.1f;
	}

	d_quit();
}
