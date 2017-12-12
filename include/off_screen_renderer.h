#pragma once

#ifndef OFF_SCREEN_RENDERER_H
#define OFF_SCREEN_RENDERER_H

#include <gl.h>

#include <iostream>
#include <string>
#include <vector>

#include <cgal.h>
#include <part_base.h>

#define WIDTH 224
#define HEIGHT 224

using std::vector;

enum View {FRONT, SIDE, TOP, SIDE_RIGHT};

namespace OffScreenRenderer {
	inline void mGLRender(PartBase *part, Vector cam, int total, bool inverse, double maxD);

	void write_ppm_2(const char *filename, unsigned char *buffer, int width, int height);

	unsigned char *getPixels (float *depth_z);

	inline unsigned char *render(PartBase *part, Vector cam, int total, bool inverse, double maxD);

	unsigned char *getProjection (PartBase *part, View view);
}

#endif