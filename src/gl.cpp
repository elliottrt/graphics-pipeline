#include "gl.hpp"

void hwClear(const V3 &color) {
	glClearColor(color[0], color[1], color[2], 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void hwDrawMesh(const Mesh &mesh, bool fill, unsigned int tex) {
	if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// load our pointers
	if (mesh.vertices) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, mesh.vertices);
	}
	if (mesh.normals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, mesh.normals);
	}
	if (mesh.colors) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, mesh.colors);
	}
	if (mesh.tcs && tex != 0) {
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, mesh.tcs);
	}

	// draw stuff
	glDrawElements(GL_TRIANGLES, 3 * mesh.triangleCount, GL_UNSIGNED_INT, mesh.triangles);

	// disable stuff
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
