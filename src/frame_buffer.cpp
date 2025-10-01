#include "frame_buffer.hpp"
#include "color.hpp"
#include "font.hpp"

#include <cassert>
#include <tiff.h>
#include <tiffio.h>

FrameBuffer::FrameBuffer(unsigned width, unsigned height): cb(nullptr), zb(nullptr) {
	Resize(width, height);
}

FrameBuffer::~FrameBuffer() {
	if (cb) delete[] cb;
	if (zb) delete[] zb;
}

void FrameBuffer::Resize(unsigned width, unsigned height) {
	w = (int) width;
	h = (int) height;

	// free the old ones if they existed
	if (cb) delete[] cb;
	if (zb) delete[] zb;

	// create the new framebuffer. undefined contents
	cb = new uint32_t [w*h];
	assert(cb != nullptr && "color buffer allocation failed");

	zb = new float [w * h];
	assert(zb != nullptr && "z buffer allocation failed");
}

// copied and modified from framebuffer.cpp example code
bool FrameBuffer::SaveToTiff(const char *path) const {
	TIFF* out = TIFFOpen(path, "w");

	if (out == NULL) {
		fprintf(stderr, "error: could not load tiff from %s\n", path);
		return false;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (int row = 0; row < h; row++) {
		TIFFWriteScanline(out, &cb[row * w], row);
	}

	TIFFClose(out);
	return true;
}

// copied and modified from framebuffer.cpp example code
bool FrameBuffer::LoadFromTiff(const char *path) {
	TIFF* in = TIFFOpen(path, "r");
	if (in == NULL) {
		fprintf(stderr, "error: could not read tiff from %s\n", path);
		return false;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);

	if (w != width || h != height) {
		Resize(width, height);
	}

	if (TIFFReadRGBAImageOriented(in, w, h, cb, ORIENTATION_TOPLEFT, 0) == 0) {
		TIFFClose(in);
		fprintf(stderr, "error: could not load tiff from %s\n", path);
		return false;
	}

	TIFFClose(in);
	return true;
}

void FrameBuffer::SetPixel(int u, int v, uint32_t color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h && u >= 0 && v >= 0) {
#endif

	cb[u + v * w] = color;

#ifdef WINDOW_SAFE
	} else {
		printf("warning: SetPixel %ux%u out of window %ux%u\n", u, v, w, h);
	}
#endif

}

void FrameBuffer::Clear(uint32_t color) {
	// clear color buffer
	size_t pixelCount = w * h;
	for (size_t uv = 0; uv < pixelCount; uv++) {
		cb[uv] = color;
	}

	// clear z buffer
	memset(zb, 0, pixelCount * sizeof(*zb));
}

void FrameBuffer::DrawRect(int u, int v, unsigned width, unsigned height, uint32_t color) {
	int uMax = u + width;
	int vMax = v + height;

	// clip offscreen parts
	if (u < 0) u = 0;
	if (uMax >= w) uMax = w - 1;
	if (v < 0) v = 0;
	if (vMax >= h) vMax = h - 1;

	for (int y = v; y < vMax; y++) {
		for (int x = u; x < uMax; x++) {
			SetPixel(x, y, color);
		}
	}
}

void FrameBuffer::DrawCircle(int u, int v, unsigned radius, uint32_t color) {
	int uMin = u - radius;
	int uMax = u + radius;
	int vMin = v - radius;
	int vMax = v + radius;

	// clip offscreen parts
	if (uMin < 0) uMin = 0;
	if (uMax >= w) uMax = w - 1;
	if (vMin < 0) vMin = 0;
	if (vMax >= h) vMax = h - 1;

	// very similar to DrawRect, but with a distance check against the radius
	int squareRadius = radius * radius;

	for (int y = vMin; y <= vMax; y++) {
		// we can precompute this for the inner loop
		// instead of repeatedly doing this for each value of x
		int dy = y - v;
		int squareDy = dy * dy;
		for (int x = uMin; x <= uMax; x++) {
			int dx = x - u;
			
			if (dx * dx + squareDy <= squareRadius) {
				SetPixel(x, y, color);
			}
		}
	}
}

// similar idea to some of those in class
// walking along a path based on a direction vector
void FrameBuffer::DrawLine(int u0, int v0, int u1, int v1, uint32_t color) {
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	float distanceU = (float) (u1 - u0);
	float distanceV = (float) (v1 - v0);

	float steps = std::max(std::abs(distanceU), std::abs(distanceV));
	float du = distanceU / steps;
	float dv = distanceV / steps;

	float u = (float) u0, v = (float) v0;

	for (unsigned step = 0; step <= steps; step++) {
		if (u >= 0 && v >= 0 && u < w && v < h) SetPixel((int) u, (int) v, color);

		u += du;
		v += dv;
	}
}

// same as draw line, but interpolates colors
void FrameBuffer::DrawLine(int u0, int v0, int u1, int v1, const V3 &c0, const V3 &c1) {
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	float distanceU = (float) (u1 - u0);
	float distanceV = (float) (v1 - v0);

	float steps = std::max(std::abs(distanceU), std::abs(distanceV));
	float du = distanceU / steps;
	float dv = distanceV / steps;

	float u = (float) u0, v = (float) v0;

	for (unsigned step = 0; step <= steps; step++) {
		if (u >= 0 && v >= 0 && u < w && v < h)
			SetPixel((int) u, (int) v, ColorFromV3(c0.Interpolate(c1, step / steps)));

		u += du;
		v += dv;
	}
}

// implementation from Lectures/TRast.pdf
// this took me a long time. I couldn't figure out where the variable i came from
void FrameBuffer::DrawTriangle(int u0, int v0, int u1, int v1, int u2, int v2, uint32_t color) {

	// line equation values
	V3 a = {
		(float) (v1 - v0), // edge 0,1
		(float) (v2 - v1), // edge 1,2
		(float) (v0 - v2), // edge 2,0
	};
	V3 b = {
		(float) (-u1 + u0), // edge 0,1
		(float) (-u2 + u1), // edge 1,2
		(float) (-u0 + u2), // edge 2,0
	};
	V3 c = {
		(float) (-u0*v1 + v0*u1), // edge 0,1
		(float) (-u1*v2 + v1*u2), // edge 1,2
		(float) (-u2*v0 + v2*u0), // edge 2,0
	};

	// sidedness calculations for each edge

	// edge 0,1
	if (a[0] * u2 + b[0] * v2 + c[0] < 0) {
		a[0] = -a[0];
		b[0] = -b[0];
		c[0] = -c[0];
	}

	// edge 1,2
	if (a[1] * u0 + b[1] * v0 + c[1] < 0) {
		a[1] = -a[1];
		b[1] = -b[1];
		c[1] = -c[1];
	}

	// edge 2,0
	if (a[2] * u1 + b[2] * v1 + c[2] < 0) {
		a[2] = -a[2];
		b[2] = -b[2];
		c[2] = -c[2];
	}

	// bounding box calculations
	float boundingBox[4] = {
		// left & right
		(float) std::min({u0, u1, u2}), (float) std::max({u0, u1, u2}),
		// top & bottom
		(float) std::min({v0, v1, v2}), (float) std::max({v0, v1, v2})
	};

	// clip box to window
	if (boundingBox[0] < 0.f) boundingBox[0] = 0.f;
	if (boundingBox[1] >= w) boundingBox[1] = (float) w;
	if (boundingBox[2] < 0.f) boundingBox[2] = 0.f;
	if (boundingBox[3] >= h) boundingBox[3] = (float) h;

	int left = (int) (boundingBox[0] + 0.5f);
	int right = (int) (boundingBox[1] - 0.5f);
	int top = (int) (boundingBox[2] + 0.5f);
	int bottom = (int) (boundingBox[3] - 0.5f);

	// edge expression values for line starts and within line
	V3 currEELS, currEE;

	for (int i = 0; i < 3; i++) {
		currEELS[i] = a[i] * (left+0.5f) + b[i] * (top+0.5f) + c[i];
	}

	for (int currPixY = top; currPixY <= bottom; currPixY++, currEELS += b) {
		currEE = currEELS;

		for (int currPixX = left; currPixX <= right; currPixX++, currEE += a) {
			if (currEE[0] >= 0 && currEE[1] >= 0 && currEE[2] >= 0) {
				SetPixel(currPixX, currPixY, color);
			}
		}
	}
}

void FrameBuffer::DrawChar(int u, int v, unsigned scale, char ch, uint32_t color) {
	const uint8_t *bits = FontGetChar(ch);

	// we're using a bitmap font, so we iterate through each row
	for (unsigned row = 0; row < FontSize(); row++) {
		uint8_t byte = bits[row];

		// for each bit in the row, we fill in the rectangle if that bit is a 1
		for (unsigned col = 0; col < FontSize(); col++) {
			int bitValue = byte & 1;
			byte >>= 1;

			if (bitValue) {
				DrawRect(u, v, scale, scale, color);
			}

			u += scale;
		}
		
		u -= scale * FontSize();
		v += scale;
	}
}

void FrameBuffer::DrawString(int u, int v, unsigned scale, const char *string, uint32_t color) {
	int charU = u;
	int charV = v;

	for (const char *ch = string; *ch; ch++) {
		DrawChar(charU, charV, scale, *ch, color);
		
		if (*ch == '\n') {
			charU = u;
			charV += FontSize() * scale;
		} else {
			charU += FontSize() * scale;
		}
	}
}

void FrameBuffer::SetPixel(const V3 &p, const V3 &color) {
	SetPixel((int) p.x(), (int) p.y(), p.z(), color);
}

void FrameBuffer::SetPixel(const V3 &p, uint32_t color) {
	SetPixel((int) p.x(), (int) p.y(), p.z(), color);
}

void FrameBuffer::SetPixel(int u, int v, float z, const V3 &color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h && u >= 0 && v >= 0) {
#endif

	int i = u + v * w;

	if (z > zb[i]) {
		zb[i] = z;
		cb[i] = ColorFromV3(color);
	}

#ifdef WINDOW_SAFE
	} else {
		printf("warning: SetPixel %ux%u out of window %ux%u\n", u, v, w, h);
	}
#endif
}

void FrameBuffer::SetPixel(int u, int v, float z, uint32_t color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h && u >= 0 && v >= 0) {
#endif

	int i = u + v * w;

	if (z > zb[i]) {
		zb[i] = z;
		cb[i] = color;
	}

#ifdef WINDOW_SAFE
	} else {
		printf("warning: SetPixel %ux%u out of window %ux%u\n", u, v, w, h);
	}
#endif
}

void FrameBuffer::DrawZBuffer(void) {
	DrawZBuffer(*this);
}

void FrameBuffer::DrawZBuffer(const FrameBuffer &o) {
	// clamp to this buffer to prevent drawing outside
	int width = std::min(w, o.w);
	int height = std::min(h, o.h);

	// copy inverse z values over, converting them to a grayscale representation
	for (int v = 0; v < height; v++) {
		for (int u = 0; u < width; u++) {
			SetPixel(u, v, ColorFromInverseZ(o.zb[v * o.w + u]));
		}
	}
}

void FrameBuffer::Copy(const FrameBuffer &o) {
	// clamp to this buffer to prevent drawing outside
	int width = std::min(w, o.w);
	int height = std::min(h, o.h);

	// copy inverse z values over, converting them to a grayscale representation
	for (int v = 0; v < height; v++) {
		memcpy(cb + v * w, o.cb + v * o.w, width * sizeof(*cb));
		memcpy(zb + v * w, o.zb + v * o.w, width * sizeof(*zb));
	}
}

void FrameBuffer::DrawPointCloud(const PPCamera &camera, const FrameBuffer &other, const PPCamera &otherCamera) {
	V3 P, PP;
	for (int v = 0; v < other.h; v++) {
		for (int u = 0; u < other.w; u++) {
			float z = other.zb[v * other.w + u];
			if (z == 0.0f) continue;
			P = otherCamera.UnprojectPoint(u, v, z);

			if (camera.ProjectPoint(P, PP))
				SetPixel(PP, other.cb[v * other.w + u]);
		}
	}
}

void FrameBuffer::DrawPoint(const PPCamera &camera, const V3 &point, size_t pointSize, const V3 &color) {
	V3 PP;
	float hsize = pointSize / 2.0f;
	if (camera.ProjectPoint(point, PP)) {
		for (int v = PP.y() - hsize; v < PP.y() + hsize; v++) {
			for (int u = PP.x() - hsize; u < PP.x() + hsize; u++) {
				if (u >= 0 && v >= 0 && u < w && v < h)
					SetPixel(u, v, PP.z(), color);
			}
		}
	}
}

void FrameBuffer::DrawLine(const V3 &p0, const V3 &p1, const V3 &c0, const V3 &c1) {
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	V3 D = p1 - p0;

	float steps = std::max(std::abs(D.x()), std::abs(D.y()));

	V3 iter = p0;

	for (unsigned step = 0; step <= steps; step++) {
		if (iter.x() >= 0 && iter.y() >= 0 && iter.x() < w && iter.y() < h)
			SetPixel(iter, c0.Interpolate(c1, step / steps));

		iter += (D / steps);
	}
}

void FrameBuffer::DrawCamera(const PPCamera &camera, const PPCamera &drawnCamera) {
	constexpr static uint32_t WHITE = ColorFromRGB(255, 255, 255);
	constexpr static float SCALE = 5;

	DrawPoint(camera, drawnCamera.C, 7, V3(1, 1, 1));

	float f = SCALE / camera.GetFocalLength();

	V3 pC;
	bool p0 = camera.ProjectPoint(drawnCamera.C, pC);
	V3 c[4];
	bool p1 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c) * f, c[0]);
	bool p2 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.a * (float) drawnCamera.w) * f, c[1]);
	bool p3 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.a * (float) drawnCamera.w + drawnCamera.b * (float) drawnCamera.h) * f, c[2]);
	bool p4 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.b * (float) drawnCamera.h) * f, c[3]);

	if (p0 && p1) DrawLine((int)pC.x(), (int)pC.y(), (int)c[0].x(), (int)c[0].y(), WHITE);
	if (p0 && p2) DrawLine((int)pC.x(), (int)pC.y(), (int)c[1].x(), (int)c[1].y(), WHITE);
	if (p0 && p3) DrawLine((int)pC.x(), (int)pC.y(), (int)c[2].x(), (int)c[2].y(), WHITE);
	if (p0 && p4) DrawLine((int)pC.x(), (int)pC.y(), (int)c[3].x(), (int)c[3].y(), WHITE);

	if (p1 && p2) DrawLine((int)c[0].x(), (int)c[0].y(), (int)c[1].x(), (int)c[1].y(), WHITE);
	if (p2 && p3) DrawLine((int)c[1].x(), (int)c[1].y(), (int)c[2].x(), (int)c[2].y(), WHITE);
	if (p3 && p4) DrawLine((int)c[2].x(), (int)c[2].y(), (int)c[3].x(), (int)c[3].y(), WHITE);
	if (p4 && p1) DrawLine((int)c[3].x(), (int)c[3].y(), (int)c[0].x(), (int)c[0].y(), WHITE);
}

// TODO: triangle function with callback for pixel color, given B0, B1, B2
void FrameBuffer::DrawTriangle(const PPCamera &camera,
	const V3 &point0, const V3 &point1, const V3 &point2,
	const V3 &c0, const V3 &c1, const V3 &c2)
{
	V3 p0, p1, p2;
	if (!(camera.ProjectPoint(point0, p0) &&
		camera.ProjectPoint(point1, p1) &&
		camera.ProjectPoint(point2, p2))) return;

	auto [bbLeft, bbRight] = std::minmax({p0.x(), p1.x(), p2.x()});
	auto [bbTop, bbBottom] = std::minmax({p0.y(), p1.y(), p2.y()});

	// clip box to window and get point coordinates
	int left = (int) (std::max(bbLeft, 0.0f) - 0.5f);
	int right = (int) (std::min(bbRight, (float) w - 1) + 0.5f);
	int top = (int) (std::max(bbTop, 0.0f) - 0.5f);
	int bottom = (int) (std::min(bbBottom, (float) h - 1) + 0.5f);

	float dy12 = p1.y() - p2.y();
	float dy20 = p2.y() - p0.y();
	float dx21 = p2.x() - p1.x();
	float dx02 = p0.x() - p2.x();
	float div = dy12 * dx02 + dx21 * (p0.y() - p2.y());

	float B0y = (top - p2.y()) * dx21 - dy12 * p2.x();
	float B1y = (top - p2.y()) * dx02 - dy20 * p2.x();

	B0y /= div; B1y /= div; dx21 /= div; dx02 /= div; dy12 /= div; dy20 /= div;

	float B0, B1, B2;

	for (int currPixY = top; currPixY <= bottom; currPixY++, B0y += dx21, B1y += dx02) {
		B0 = B0y + left * dy12;
		B1 = B1y + left * dy20;
		B2 = 1.0f - B0 - B1;

		for (int currPixX = left; currPixX <= right; currPixX++, B0 += dy12, B1 += dy20, B2 -= dy12 + dy20) {
			if (B0 >= 0 && B1 >= 0 && B2 >= 0) {
				V3 color = c0 * B0 + c1 * B1 + c2 * B2;
				float z = p0.z() * B0 + p1.z() * B1 + p2.z() * B2;
				SetPixel(currPixX, currPixY, z, color);
			}
		}
	}

}

void FrameBuffer::DrawTriangle(const V3 &p0, const V3 &p1, const V3 &p2, FragShaderFn frag) {
	auto [bbLeft, bbRight] = std::minmax({p0.x(), p1.x(), p2.x()});
	auto [bbTop, bbBottom] = std::minmax({p0.y(), p1.y(), p2.y()});

	// clip box to window and get point coordinates
	int left = (int) (std::max(bbLeft, 0.0f) - 0.5f);
	int right = (int) (std::min(bbRight, (float) w - 1) + 0.5f);
	int top = (int) (std::max(bbTop, 0.0f) - 0.5f);
	int bottom = (int) (std::min(bbBottom, (float) h - 1) + 0.5f);

	float dy12 = p1.y() - p2.y();
	float dy20 = p2.y() - p0.y();
	float dx21 = p2.x() - p1.x();
	float dx02 = p0.x() - p2.x();
	float div = dy12 * dx02 + dx21 * (p0.y() - p2.y());

	float B0y = (top - p2.y()) * dx21 - dy12 * p2.x();
	float B1y = (top - p2.y()) * dx02 - dy20 * p2.x();

	B0y /= div; B1y /= div; dx21 /= div; dx02 /= div; dy12 /= div; dy20 /= div;

	V3 B;

	for (int currPixY = top; currPixY <= bottom; currPixY++, B0y += dx21, B1y += dx02) {
		B[0] = B0y + left * dy12;
		B[1] = B1y + left * dy20;
		B[2] = 1.0f - B[0] - B[1];

		for (int currPixX = left; currPixX <= right; currPixX++, B[0] += dy12, B[1] += dy20, B[2] -= dy12 + dy20) {
			if (B[0] >= 0 && B[1] >= 0 && B[2] >= 0) {
				float z = p0.z() * B[0] + p1.z() * B[1] + p2.z() * B[2];
				SetPixel(currPixX, currPixY, z, ColorFromV3(frag(B)));
			}
		}
	}
}
