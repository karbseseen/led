#include <ESP8266WiFi.h>
#include "ArduinoOTA.h"
#include "head.h"

extern byte* pixels;
bool connected = true;

WiFiServer server(80);
WiFiClient client;


/*void write(const char* text) {
	char c;
	while (c = *text++)
		client.write(c);
}
void write(char* text, uint size) {
	char* const textEnd = text + size;
	do client.write(*text);
	while (++text < textEnd);
}
#define WriteRes(name, size)			extern char name[]; extern uint size; client.write(name, size);
inline bool obtainFind(char c, const char* need, byte* count) {
	if (c == need[*count])
		return need[++(*count)] == 0;
	*count = 0;
	return false;
}

class tag { public:
	const char* name;
	inline tag(const char* name) : name(name) {
		client.write('\n');
		client.write('<');
		write(name);
		client.write('>');
	}
	inline tag(const char* name, const char* properties) : name(name) {
		client.write('\n');
		client.write('<');
		write(name);
		client.write(' ');
		write(properties);
		client.write('>');
	}
	inline ~tag() {
		client.write('\n');
		client.write('<');
		client.write('/');
		write(name);
		client.write('>');
	}
};

inline void sendPage()
{
	/*bool mobile = false;
	byte mobileSize = 0;
	int c;
	while ((c = client.read()) != -1) 
		if (obtainFind(c, "Android", &mobileSize)) {
			mobile = true;
			break;
		}
	write(
		"HTTP/1.1 200 OK\n"
		"Content-type: text/html\n\n"
		"<!DOCTYPE HTML>\n"
	);
	{ tag t("html");
		{ tag t("head");
			write("\n<meta charset=\"utf-8\">\n<title>Led-лента на кухне</title>");

			{ tag t("style");
				static char htmlFontSize[] = "html { font-size: 100%; }";
				htmlFontSize[18] = mobile ? '3' : '1';
				write(htmlFontSize);
				WriteRes(css, cssSize);
			}

			{ tag t("script");
				write("var animation=false, selEffect=0, h=0,s=0,v=0;\n");
				WriteRes(script, scriptSize);
			}
		}
		
		WriteRes(body, bodySize);
		
		extern const mode_s mode[];
		const char* name;
		uint i = 0;
		while (name = mode[i+1].name) {
			char str[] = { i/10+'0', i%10+'0' };
			write("<div class=\"effect space button");
			if (i == 1) write("-first");
			write("\" id=\"E");					client.write(str[0]);	client.write(str[1]);
			write("\" onclick=\"swapEffect(");	client.write(str[0]);	client.write(str[1]);
			write(")\">");
			write(name);
			write("</div>");
			i++;
		}
		write("</div></body>");
	}
	client.write('\n');
	client.write('\n');
	
	extern char html[];
	extern uint htmlSize;
	write(
		"HTTP/1.1 200 OK\n"
		"Content-type: text/html\n"
		"Content-length: "
	);
	client.write(htmlSize);
	write("\n\n<!DOCTYPE HTML>\n");
	write(html, htmlSize);
}

bool find(const char* str) {
	char c;
	while (c = *str++)
		if (c != client.read())
			return false;
	return true;
}

int readColor() {
	int ret = 0;
	for (uint i = 0; i < 6; i++) {
		char c = client.read();
		if (c >= '0' && c <= '9') c -= '0';
		else if (c >= 'a' && c <= 'f') c += 10 - 'a';
		else return -1;
		ret <<= 4;
		ret |= c;
	}
	return ret;
};*/

#define WRITE(str)				client.write(str, sizeof(str)-1)
#define FIND(str)				client.find(str, sizeof(str)-1)

bool checkstr(const char* need, const char* str) {
	char c;
	while (c = *need++)
		if (*str++ != c)
			return false;
	return true;
}
int str2i(const char** ptr) {
	const char* str = *ptr;
	uint res = 0;
	char c;
	while (true) {
		c = *str++;
		if (c < '0' || c > '9') break;
		res *= 10;
		res += c - '0';
	}
	*ptr = str;
	return res;
}

void obtainServer()
{
	const bool newConnected = WiFi.status() == WL_CONNECTED;
	if (connected && !newConnected)	{
		Serial.println("\n\nConnecting to ShippingST");
		WiFi.begin("ShippingST", "4545454545");
		connected = false;
		Mode = 1;
		Color = rgb3(255,0,0);
		return;
	}
	else if (!connected && newConnected) {
		Serial.println("WiFi connected");
		server.begin();
		ArduinoOTA.begin();
		connected = true;
		Mode = 1;
		Color = rgb3(0,255,0);
		client.connect(IPAddress(77, 88, 55, 66), 80);
	}
	else if (!connected && !newConnected) return;

	ArduinoOTA.handle();

	if (server.hasClient()) {
		client = server.available();
		client.setDefaultSync(true);
	}

	uint available = client.available();
	if (!available) return;

	char buffer[32];
	if (available <= sizeof(buffer))
		client.read((uint8_t*)buffer, available);
	else {
		client.read((uint8_t*)buffer, sizeof(buffer));
		available -= sizeof(buffer);
		char temp[256];
		while (available >= sizeof(temp))
			available -= client.read((uint8_t*)temp, sizeof(temp));
		client.read((uint8_t*)temp, available);
	}

	if (checkstr("GET /", buffer)) {
		extern char html[], icon[];
		extern uint htmlSize, iconSize;
		if (checkstr("favicon.ico", buffer+5))
			client.write(icon, iconSize);
		else
			client.write(html, htmlSize);
	}
	else if (checkstr("POST /", buffer)) {
		const char* buf = buffer + 6;
		Mode = *buf - '0';
		if (Mode == 1) {
			buf += 2;
			int_hsv color;
			color.h = str2i(&buf);
			color.s = str2i(&buf);
			color.v = 255;
			Color = to_rgb(color);
		}
		else if (Mode == 2) {
			buf += 2;
			Mode = 2 + str2i(&buf);
		}
		WRITE("HTTP/1.1 204 No Content\n\n");
	}
	else WRITE("HTTP/1.1 500 Internal Server Error\n\nERROR((");

	
	
	/*switch (buffer[0]) {
	case 'G':
			Serial.println("0f");
		if (FIND("ET /")) {
			Serial.println("0f");
			extern char html[], icon[];
			extern uint htmlSize, iconSize;
				Serial.println("1f");
			if (!FIND("favicon.ico")) {
				Serial.println("1f");
				Serial.println("0");
				client.write(html, htmlSize);
				Serial.println("1");
			}
			else {
				Serial.println("0");
				client.write(icon, iconSize);
				Serial.println("1");
			}
		}
		else client.write(resp_error);
		break;
	case 'P':
		if (FIND("OST /")) {
			Mode = client.read() - '0';
			WRITE("HTTP/1.1 204 No Content\n\n");
			if (Mode == 1) {
				int_hsv color;
				color.h = client.parseInt();
				color.s = client.parseInt();
				color.v = 255;
				Color = to_rgb(color);
			}
			else if (Mode == 2) {
				Mode = 2 + client.parseInt();
			}
		}
		else WRITE(resp_error);
		break;
	default:
		WRITE(resp_error);
	}

	client.flush();
	*/
}