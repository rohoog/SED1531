#ifndef SED1531_h
#define SED1531_h

#include "Arduino.h"
#include "Print.h"

class SED1531 : public Print{
	public:
		void setCursor(byte row);
		void setCursor(byte row, byte col);
		void init();
		void setContrast(byte);
		void doublewide(byte);
		void writecommand(byte);
		void writePixData(byte);
		void writePixData(byte, byte);
		void writePixData(byte[], int);
		void writePixData(byte[], int, byte);
		void inverse(byte);
		void setMarker(byte,boolean);
		virtual size_t write(byte);
		void pixel(uint8_t,uint8_t,uint8_t);
		void rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
		void resetCursor();
	private:
		void setCol(byte);
};
#endif
