#include <Arduino.h>
#include "head.h"
#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel adafruit(LED_NUM, D1, NEO_GRB | NEO_KHZ800);
Adafruit_NeoPixel adafruit2(LED_NUM2, D2, NEO_GRB | NEO_KHZ800);
byte* const pixels = adafruit.getPixels();
byte* const pixels2 = adafruit2.getPixels();
byte data[2048];

//rgb3 Color = 0;
//byte modeId = 0;

void fill(rgb3 color) {
    rgb3* pix = (rgb3*)pixels;
    rgb3* pixEnd = pix + LED_NUM;
    do *pix = color;
    while (++pix < pixEnd);
	adafruit.show();

    pix = (rgb3*)pixels2;
    pixEnd = pix + LED_NUM2;
    do *pix = color;
    while (++pix < pixEnd);
	adafruit2.show();
}

void iOff() {
    fill(0);
}
void oOff() {}
void iColor() {
    fill(Color);
}
void oColor() {
	if (*(rgb3*)pixels != Color)
		fill(Color);
}

struct str2 {
	int_hsv hsv1;
	rgb3 color0;
	rgb3 color1;
	short time;
};
void i2() {
	auto &s = *(str2*)data;
	ushort g, r, b;
	rgb3* pix = (rgb3*)pixels;
	rgb3* const pixEnd = pix + LED_NUM;
	do {
		g += pix->g;
		r += pix->r;
		b += pix->b;
	} while (++pix < pixEnd);
	
	s.hsv1 = int_hsv(_random() % (256*6), 192 + (randHigh()>>26), 255);
	s.color0 = rgb3(r / LED_NUM, g / LED_NUM, b / LED_NUM);
	s.color1 = to_rgb(s.hsv1);
	s.time = 0;
}
void o2() {
	auto &s = *(str2*)data;
	if (s.time >= 128) {
		s.hsv1.h += 256 + _random() % (256*4);
		s.hsv1.updateRange();
		s.hsv1.s = 224 + (randHigh() >> 27);
		s.color0 = s.color1;
		s.color1 = to_rgb(s.hsv1);
		fill(s.color0);
		s.time = -200;//-30000 + (_random() % 25000);
		return;
	}
	if (s.time > 0)
		fill(mix(s.color0, s.color1, s.time*2));

	s.time++;
}

struct str3 {
	rgb3* strip0;
	rgb3* strip1;
	ushort time;
	ushort timeEnd;
	ushort sep[28];
};
void i3() {
	auto &s = *(str3*)data;
	s.strip0 = (rgb3*)(data + sizeof(str3));
	s.strip1 = s.strip0 + LED_NUM;
	memcpy(s.strip1, pixels, LED_NUM*3);
	s.time = s.timeEnd = 0;
	s.sep[0] = 0;
}
void o3() {
	constexpr uint sepNum = sizeof(str3::sep) / sizeof(ushort);
	auto &s = *(str3*)data;
	if (s.time == s.timeEnd) {
		rgb3* temp = s.strip0;
		s.strip0 = s.strip1;
		s.strip1 = temp;
		memcpy(pixels, s.strip0, LED_NUM*3);
		adafruit.show();

		s.time = 1;
		s.timeEnd = 32;//28 + _random() % 8;
	
		ushort sum = 0;
		for (uint i = 1; i < sepNum; i++)
			s.sep[i] = sum += 256-150 + _random()%150;
		rgb3 prevColor = s.strip1[0] = randLocal(s.strip0[0]);
		byte prevId = 0;
		for (uint i = 1; i < sepNum; i++) {
			byte id = (uint)s.sep[i] * (LED_NUM-1) / sum;
			rgb3 color = s.strip1[id] = randLocal(s.strip0[id]);

			byte len = id - prevId;
			for (uint j = 1; j < len; j++)
				s.strip1[prevId + j] = mix(prevColor, color, j*256/len);

			prevColor = color;
			prevId = id;
		}
	}
	else {
		const byte k = s.time * 256 / s.timeEnd;
		for (uint i = 0; i < LED_NUM; i++)
			((rgb3*)pixels)[i] = mix(s.strip0[i], s.strip1[i], k);
		adafruit.show();
		s.time++;
	};
}

struct str4 {
	ushort* ptr0;
	ushort* ptr1;
	ushort time;
	ushort data[LED_NUM * 3 * 2];
};
void i4() {
	auto &s = *(str4*)data;
	s.ptr0 = s.data;
	s.ptr1 = s.data + LED_NUM * 3;
	s.time = _random() % 128 + 64;

	ushort* ptr = s.ptr0;
	byte* pix = pixels;
	do *ptr++ = *pix++;
	while (ptr < s.ptr1);
}
void o4() {
	auto &s = *(str4*)data;
	constexpr uint normsz = (LED_NUM - 2) * 3;
	constexpr ushort maxc = 256 * 64;
	constexpr byte a = 200;
	constexpr int b = 0xff & ~a;

	if (s.time) s.time--;
	else {
		s.time = _random() % 92 + 32;
		
		int sum = 0;
		byte* pix = pixels;
		byte* const pixend = pixels + LED_NUM * 3;
		do sum += 0xff & ~pix[0];
		while (++pix < pixend);
		
		pix = pixels;
		sum = _random() % sum;
		while (true) {
			sum -= (0xff & ~pix[0]) + (0xff & ~pix[1]) + (0xff & ~pix[2]);
			if (sum <= 0) break;
			pix += 3;
		}
		ushort* ptr = s.ptr0 + (pix - pixels);


		//ushort* ptr = s.ptr0 + (_random() % LED_NUM) * 3;
		ushort m = maxc*3/4 + _random() % (maxc/4);
		ushort c = _random() % m;
		switch (_random() % 6) {
			case 0: ptr[0] = 0; ptr[1] = c; ptr[2] = m; break;
			case 1: ptr[0] = 0; ptr[2] = c; ptr[1] = m; break;
			case 2: ptr[1] = 0; ptr[0] = c; ptr[2] = m; break;
			case 3: ptr[1] = 0; ptr[2] = c; ptr[0] = m; break;
			case 4: ptr[2] = 0; ptr[0] = c; ptr[1] = m; break;
			case 5: ptr[2] = 0; ptr[1] = c; ptr[0] = m; break;
		}
	}

	ushort* ptr0 = s.ptr0;
	ushort* ptr1 = s.ptr1;
	ushort* ptr1end = ptr1 + 3;
	do {
		ptr1[0] = ((2*ptr0[0] + ptr0[3]) * 21620) >> 16;
		ptr1[3 + normsz] = ((2*ptr0[3 + normsz] + ptr0[normsz]) * 21620) >> 16;
		ptr0++;
		ptr1++;
	} while (ptr1 < ptr1end);
	ptr1end += normsz;
	do {
		*ptr1 = ((ptr0[0] + ptr0[-3] + ptr0[3]) * 21620) >> 16;
		ptr0++;
		ptr1++;
	} while (ptr1 < ptr1end);

	ptr1 = s.ptr1;
	byte* pix = pixels;
	byte* const pixend = pixels + LED_NUM * 3;
	do {
		ushort color[3];
		color[0] = ptr1[0];
		color[1] = ptr1[1];
		color[2] = ptr1[2];
		ptr1 += 3;

		ushort max = color[0] > color[1] ? color[0] : color[1];
		if (max < color[2]) max = color[2];

		if (max > 255) {
			ushort diff = (max - 255) / 4;
			color[0] += diff;
			color[1] += diff;
			color[2] += diff;
		}

		pix[0] = color[0] & 0xff00 ? 255 : (byte)color[0];
		pix[1] = color[1] & 0xff00 ? 255 : (byte)color[1];
		pix[2] = color[2] & 0xff00 ? 255 : (byte)color[2];
	} while ((pix += 3) < pixend);

	ptr0 = s.ptr0;
	s.ptr0 = s.ptr1;
	s.ptr1 = ptr0;

	adafruit.show();
}

struct str5 {
	struct dot {
		ushort x;
		short v;
	}
	first,
	pixel0[LED_NUM + 1],
	pixel1[LED_NUM + 1],
	pixel2[LED_NUM + 1];
	ushort t;
};
void i5() {
	auto &s = *(str5*)data;

	s.first = s.pixel0[LED_NUM] = s.pixel1[LED_NUM] = s.pixel2[LED_NUM] = { 0x8000,0 };
	str5::dot* spix0 = s.pixel0;
	str5::dot* spix1 = s.pixel1;
	str5::dot* spix2 = s.pixel2;
	byte* pix = pixels;
	byte* const pixend = pixels + 3*LED_NUM;
	do {
		*spix0++ = { (uint16_t)(pix[0]<<8), 0 };
		*spix1++ = { (uint16_t)(pix[1]<<8), 0 };
		*spix2++ = { (uint16_t)(pix[2]<<8), 0 };
	} while ((pix += 3) < pixend);

	s.t = 100;
	
	/*spix0 = s.pixel0;
	str5::dot* spix1 = s.pixel1;
	str5::dot* spix2 = s.pixel2;
	for (ushort i = 0; i < LED_NUM; i++) {
		spix0[i].x = fsin(5278*i + 0x8000) / 2;
		spix1[i].x = fsin(5718*i + 0x8000) / 2;
		//spix2[i].x = fsin(6158*i + 0x8000) / 2;
		(uint&)(spix2[i]) = _random();
	}*/
	//spix2[70].x = spix2[71].x = spix2[72].x = spix2[73].x = spix2[74].x = spix2[75].x = spix2[76].x = spix2[77].x = spix2[78].x = spix2[79].x = 0x1000;
}
void o5() {
	auto &s = *(str5*)data;

	constexpr uint changeNum = 8;
	if (!--s.t) {
		s.t = 16 + (_random() & 0xff);
		uint id = _random() % (3*(LED_NUM - changeNum));
		if (id >= LED_NUM - changeNum) { id += 1 + changeNum;
			if (id >= 2*(LED_NUM - changeNum)) id += 1 + changeNum;
		}
		str5::dot* ptr = s.pixel0 + id;
		str5::dot* const ptrend = ptr + changeNum;
		uint value = 0;
		do value += ptr->x;
		while (++ptr < ptrend);
		value = (value >= 0x8000*changeNum) ? 2400 : -2400;
		ptr = s.pixel0 + id;
		do ptr->v = value;
		while (++ptr < ptrend);
	}

	str5::dot* spix[] = { s.pixel0,s.pixel1,s.pixel2 };
	byte* pix = pixels;
	byte* const pixend = pixels + 3*LED_NUM;
	ushort temp[] = { 0x8000,0x8000,0x8000 };
	auto physic = [](str5::dot* spix, ushort* prevX) -> byte {
		int x = spix->x;
		int v = spix->v + (*prevX + (int)spix[1].x - 2*x) / 32;
		if (v < INT16_MIN) v = INT16_MIN;
		else if (v > INT16_MAX) v = INT16_MAX;
		*prevX = x;
		x += v;
		if (x < 0 || x > UINT16_MAX) {
			v = -v;
			x += 2*v;
		}
		*spix = { (uint16_t)x,(short)v };

		x -= 0x4000;
		if (x <= 0) return 0;
		x >>= 7;
		if (x > 255) return 255;
		return x;
	};
	do {
		/*temp[0] = physic(spix[0], temp[0]);
		temp[1] = physic(spix[1], temp[1]);
		temp[2] = physic(spix[2], temp[2]);
		pix[0] = (spix[0]++)->x >> 8;
		pix[1] = (spix[1]++)->x >> 8;
		pix[2] = (spix[2]++)->x >> 8;*/

		temp[0] = spix[0]->x;
		temp[1] = spix[1]->x;
		temp[2] = spix[2]->x;
		pix[0] = physic(spix[0], temp + 0);
		pix[1] = physic(spix[1], temp + 1);
		pix[2] = physic(spix[2], temp + 2);
	} while ((pix += 3) < pixend);
	adafruit.show();
}

extern const mode_s mode[] = {
	{ iOff,oOff },
	{ iColor,oColor },
	{ i2,o2 },
	{ i3,o3 },
	{ i4,o4 },
	{ i5,o5 },
	{ 0,0 }
};

void initLed()
{
	adafruit.begin();
	adafruit2.begin();
}