#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "math/v3.hpp"
#include "ppcamera.hpp"

#include <cstdint>
#include <functional>

// arguments:
//	V3 of barycentric coordinates
//	z value of pixel
//	u, v of the pixel on the screen
// returns:
//	color at that pixel
using FragShaderFn = std::function<V3(const V3 &, float, int, int)>;

struct FrameBuffer {

	int w, h;

	// color buffer pointer
	uint32_t *cb;
	// z buffer pointer
	float *zb;

	FrameBuffer(unsigned width, unsigned height);
	~FrameBuffer();

	void Resize(unsigned width, unsigned height);

	// TIFF file IO
	// copied and modified from framebuffer.cpp example code
	bool SaveToTiff(const char *path) const;
	bool LoadFromTiff(const char *path);

	// basic drawing functionality
	void SetPixel(int u, int v, uint32_t color);
	void Clear(uint32_t color);
	float GetZ(int u, int v) const;

	V3 GetColor(float s, float t) const;

	// more advanced drawing
	void DrawRect(int u, int v, unsigned width, unsigned height, uint32_t color);
	void DrawCircle(int u, int v, unsigned radius, uint32_t color);
	void DrawLine(int u0, int v0, int u1, int v1, uint32_t color);
	void DrawLine(int u0, int v0, int u1, int v1, const V3 &c0, const V3 &c1);
	void DrawTriangle(int u0, int v0, int u1, int v1, int u2, int v2, uint32_t color);
	void DrawChar(int u, int v, unsigned scale, char ch, uint32_t color);
	void DrawString(int u, int v, unsigned scale, const char *string, uint32_t color);

	// 3d drawing
	void SetPixel(const V3 &p, const V3 &color);
	void SetPixel(const V3 &p, uint32_t);
	void SetPixel(int u, int v, float z, const V3 &color);
	void SetPixel(int u, int v, float z, uint32_t color);

	// draw this z buffer
	void DrawZBuffer(void);
	// draw another frame buffer's z buffer
	void DrawZBuffer(const FrameBuffer &other);

	// copy data from another frame buffer
	void Copy(const FrameBuffer &o);

	void DrawPointCloud(const PPCamera &camera, const FrameBuffer &other, const PPCamera &otherCamera);

	void DrawPoint(const PPCamera &camera, const V3 &point, size_t pointSize, const V3 &color);
	// note: assumes the points are already projected
	void DrawLine(const V3 &p0, const V3 &p1, const V3 &c0, const V3 &c1);
	void DrawCamera(const PPCamera &camera, const PPCamera &drawnCamera);
	void DrawTriangle(const PPCamera &camera, const V3 &p0, const V3 &p1, const V3 &p2, const V3 &c0, const V3 &c1, const V3 &c2);

	void DrawTriangle(const V3 &p0, const V3 &p1, const V3 &p2, FragShaderFn frag);
	void DrawTriangleCorrect(const V3 &p0, const V3 &p1, const V3 &p2, FragShaderFn frag);

};

#endif
