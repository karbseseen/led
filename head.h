
#pragma once

#define LED_NUM 150
#define LED_NUM2 209

struct mode_s {
	void (*init)();
	void (*obtain)();
};

union rgb3 {
	byte data[3];
	struct { byte g, r, b; };
	rgb3() {}
	inline rgb3(uint ui) : g(ui), r(ui>>8), b(ui>>16) {}
	inline rgb3(byte r, byte g, byte b) : g(g), r(r), b(b) {}
	inline operator uint () { return (uint)g | ((uint)r << 8)  | ((uint)b << 16); }
};
union int_hsv {
	uint ui;
	byte data[4];
	struct { ushort h; byte s, v; };
	int_hsv() {}
	inline int_hsv(uint ui) : ui(ui) {}
	inline int_hsv(ushort h, byte s, byte v) : h(h), s(s), v(v) {}
	inline void updateRange() { if (h >= 256*6) h -= 256*6; }
	inline operator uint () { return ui; }
};
int_hsv to_hsv(rgb3 col);
rgb3 to_rgb(int_hsv col);
rgb3 mix(rgb3 color0, rgb3 color1, byte k);


extern byte Mode;
extern rgb3 Color;
extern byte effect;
#define OFF				254
#define COLOR			255


uint _random();
uint randLow();
uint randHigh();
rgb3 randLocal(rgb3 color);
rgb3 randContrast();
ushort fsin(ushort x);
