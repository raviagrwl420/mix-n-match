#include <off_screen_renderer.h>

namespace OffScreenRenderer {
	inline void mGLRender(Mesh mesh, Vector cam, int total = -1, bool inverse = false, double maxD = -1) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		if (!inverse)
			gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1e-10, 1);
		else
			gluLookAt(cam[0], cam[1], cam[2], 0, 0, 0, 0, 1, 0);
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBegin(GL_TRIANGLES);

		// Render Mesh
		auto boundingBox = CGAL::Polygon_mesh_processing::bbox(mesh,
			CGAL::Polygon_mesh_processing::parameters::vertex_point_map(mesh.points()).
			geom_traits(K()));		

		double scale = 0;
		if (maxD == -1) {
			scale = getScale(boundingBox);
		} else {
			scale = maxD;
		}

		for (FaceIndex f: faces(mesh)) {
			for (VertexIndex v: vertices_around_face(mesh.halfedge(f), mesh)) {
				Vector point(mesh.point(v).x(), mesh.point(v).y(), mesh.point(v).z());
				point -= getCenter(boundingBox);
				point *= 1.2f / scale;
				glVertex3f(point.x(), point.y(), point.z());
			}
		}

		glEnd();
		glFlush(); // remember to flush GL output!
		glFinish();					
	}

	void write_ppm_2(const char *filename, unsigned char *buffer, int width, int height) {
		const int binary = 0;
		FILE *f = fopen( filename, "w" );
		if (f) {
			int i, x, y;
			unsigned char *ptr = buffer;
			int counter = 0;
			fprintf(f,"P3\n");
			fprintf(f,"# ascii ppm file created by osdemo.c\n");
			fprintf(f,"%i %i\n", width, height);
			fprintf(f,"255\n");
			for (y=height-1; y>=0; y--) {
				for (x=0; x<width; x++) {
					i = (y*width + x);
					int val = ptr[i];
					fprintf(f, " %3d %3d %3d", val, val, val);
					counter++;
					if (counter % 5 == 0)
						fprintf(f, "\n");
				}
			}
			fclose(f);
		}
	}

	unsigned char *getPixels (float *depth_z) {
		unsigned char *Pixels = (unsigned char *) malloc(WIDTH * HEIGHT * sizeof(unsigned char));

		float max_depth = 0.0f;
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			if (depth_z[i] == 1.0f)
				continue;
			if (depth_z[i] > max_depth)
				max_depth = depth_z[i];
		}

		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			unsigned int depth = 255 * depth_z[i];
			if (depth < 255)
				depth *= 0.5 / max_depth;
			else
				depth = 255;
			Pixels[i] = (unsigned char)depth;
		}

		return Pixels;
	}

	inline unsigned char *render(Mesh mesh, Vector cam, int total = -1, bool inverse = false, double maxD = -1) {
		int Width = 224;
		int Height = 224;

		// Create OSMesa context
		OSMesaContext ctx = OSMesaCreateContext(OSMESA_RGBA, NULL);
		if (!ctx) {
			std::cout << "OSMesaCreateContext failed!" << std::endl;
		}

		// Allocate the image buffer
		void *buffer = malloc( WIDTH * HEIGHT * 4 * sizeof(GLubyte) );
		if (!buffer) {
			std::cout << "Alloc image buffer failed!" << std::endl;
		}

		// Bind the buffer to the context and make it current
		if (!OSMesaMakeCurrent( ctx, buffer, GL_UNSIGNED_BYTE, WIDTH, HEIGHT )) {
			std::cout << "OSMesaMakeCurrent failed!" << std::endl;
		}

		int z, s, a;
		glGetIntegerv(GL_DEPTH_BITS, &z);
		glGetIntegerv(GL_STENCIL_BITS, &s);
		glGetIntegerv(GL_ACCUM_RED_BITS, &a);

		// Rendering Stuff
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, 0.0, 2.0);
		glViewport(0, 0, (GLsizei) WIDTH, (GLsizei) HEIGHT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(1, 0, 0, 0, 0, 0, 0, 1, 0);
		mGLRender(mesh, cam, total, inverse, maxD);
		
		void **depthBuffer = (void**) malloc( WIDTH * HEIGHT * z * sizeof(GLubyte) );
		if (!depthBuffer) {
			std::cout << "Alloc depth buffer failed!" << std::endl;
		}

		float *depth_z = (float *) malloc(WIDTH * HEIGHT * sizeof(float));

		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, depth_z);

		unsigned char *Pixels = getPixels(depth_z);

		OSMesaDestroyContext(ctx);

		write_ppm_2("Test", Pixels, WIDTH, HEIGHT);

		return Pixels;
	}

	unsigned char *getView (Mesh mesh, View view) {
		Vector cam;
		bool inverse = false; 
		switch(view) {
			case FRONT:
				cam = Vector(0, -1, 0);
				break;
			case SIDE:
				cam = Vector(-1, 0, 0);
				break;
			case TOP:
				cam = Vector(0, 0, 1);
				inverse = true;
				break;
			case SIDE_RIGHT:
				cam = Vector(1, 0, 0);
				break;
		}

		return render(mesh, cam, NULL, inverse);
	}
}