#ifndef SED1531_h
#define SED1531_h

#include "Arduino.h"
#include "Print.h"
#include "Adafruit_GFX.h"

class SED1531 : public Adafruit_GFX{
	public:
		using Adafruit_GFX::Adafruit_GFX;
		void setDCursor(byte row);
		void setDCursor(byte row, byte col);
		void init();
		void setContrast(byte);
		void writecommand(byte);
		void writePixData(byte);
		void writePixData(byte, byte);
		void writePixData(byte, int, byte);
		void writePixData(byte[], int);
		void writePixData(byte[], int, byte);
		void inverse(byte);
		void setMarker(byte,boolean);
		void drawPixel(uint8_t,uint8_t,uint8_t);
		void drawVline(uint8_t x, uint8_t y, uint8_t h, uint8_t val);
		void drawHline(uint8_t x, uint8_t y, uint8_t w, uint8_t val);
		void drawFrect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
		void resetCursor();
		virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
		virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	private:
		void setCol(byte);
};
#endif
