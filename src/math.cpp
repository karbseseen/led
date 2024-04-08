#include <Arduino.h>
#include "head.h"


uint seed1 = 0, seed2 = 0;
uint _random() {
	uint b = seed1 ^ (seed1 >> 2) ^ (seed1 >> 6) ^ (seed1 >> 7);
	seed1 = (seed1 >> 1) | (~b << 31);
	b = (seed2 << 1) ^ (seed2 << 2) ^ (seed1 << 3) ^ (seed2 << 4);
	seed2 = (seed2 << 1) | (~b >> 31);
	return seed1 ^ seed2;
}

uint randLow() {
    uint rnd = random() >> 16;
    return rnd * rnd;
}
uint randHigh() {
    uint rnd = random() >> 16;
    return ~(rnd * rnd);
}
rgb3 randLocal(rgb3 color) {
	ushort h = to_hsv(color).h;

	const byte rnd = randHigh() / (0xffffffff / (256+127));
	if (rnd & (1<<30)) {
		h += rnd;
		if (h >= 6*256) h -= 6*256;
	}
	else {
		h -= rnd;
		if (h >= 6*256) h += 6*256;
	}

	return to_rgb(int_hsv(h, 255, 255 - (randLow()>>26)));
}

int_hsv to_hsv(rgb3 col) {
	const byte high_pos[] = { 1,0,0,2,2,1 };
	const byte low_pos[] = { 2,2,1,1,0,0 };
	const byte rise_pos[] = { 0,1,2,0,1,2 };

	int_hsv ret;
	if (col.r > col.g) {		//0,4,5
		if (col.g > col.b) ret.data[1] = 0;
		else if (col.b > col.r) ret.data[1] = 4;
		else ret.data[1] = 5;
	}
	else {				//1,2,3
		if (col.g < col.b) ret.data[1] = 3;
		else if (col.b > col.r) ret.data[1] = 2;
		else ret.data[1] = 1;
	}
	ret.v = col.data[high_pos[ret.data[1]]];
	if (!ret.v) return 0;
	ret.s = col.data[low_pos[ret.data[1]]];
	if (ret.s == ret.v) return ret.ui & 0xff000000;
	ret.data[0] = (col.data[rise_pos[ret.data[1]]] - ret.s) * 255 / (ret.v - ret.s);
	if (ret.data[1] & 1) ret.data[0] = ~ret.data[0];
	ret.s = ~(ret.s * 255 / ret.v);
	return ret;
}
rgb3 to_rgb(int_hsv col) {
	byte low = (byte)(~col.s) * col.v / 255;
	byte rise = (byte)col.h;
	if (col.h & 0x100) rise = ~rise;
	rise = low + rise * (col.v - low) / 255;

	switch (col.h >> 8) {
		case 0: return rgb3(col.v, rise, low);
		case 1: return rgb3(rise, col.v, low);
		case 2: return rgb3(low, col.v, rise);
		case 3: return rgb3(low, rise, col.v);
		case 4: return rgb3(rise, low, col.v);
		case 5: return rgb3(col.v, low, rise);
		default:return 0;
	}
}
rgb3 randContrast() {
	ushort rnd = random() % (256*6);
	switch (rnd >> 8) {
		case 0: return rgb3(255, rnd, 0);
		case 1: return rgb3(rnd, 255, 0);
		case 2: return rgb3(0, 255, rnd);
		case 3: return rgb3(0, rnd, 255);
		case 4: return rgb3(rnd, 0, 255);
		case 5: return rgb3(255, 0, rnd);
		default:return rgb3(0,0,0);
	}
}
rgb3 mix(rgb3 color0, rgb3 color1, byte k) {
	byte k0 = ~k;
	return rgb3(
		((ushort)color0.r*k0 + (ushort)color1.r*k) / 255,
		((ushort)color0.g*k0 + (ushort)color1.g*k) / 255,
		((ushort)color0.b*k0 + (ushort)color1.b*k) / 255
	);
}

ushort fsin(ushort x)
{
	uint value = x & 0x3fff;
	if (!(x & 0x4000)) value ^= 0x3fff;
	value = value * value >> 13;
	if (!(x & 0x8000)) value = ~value;
	return value;
}