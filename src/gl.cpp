#include "gl.hpp"
#include <OpenGL/gl.h>
#include <SDL3/SDL_video.h>

void hwInit(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

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
	if (mesh.colors && (tex == 0 || !mesh.tcs)) {
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

void hwTexFromFb(unsigned int texId, const FrameBuffer &fb) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb.w, fb.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, fb.cb);
}

void hwBegin2D(int w, int h) {
	glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void hwEnd2D(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
