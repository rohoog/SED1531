/* 
 * ---- 30/1/2013
 * 		modified begin();
 * 		to be more true to the original sketch this is  based on.
 * 		http://tkkrlab.nl/wiki/Glcd_48x100
 * 		0xe2 does not reset the display don't know why.
 * 
 * ---- 15/2/2013
 * 		fixed an issue related to setCursor();
 * 		which caused text to appear to roll accross the screen
 * 		note:
 * 		lines longer then 20 chars will display text on display
 * 		on lines it isn't supposed to go.
 * ---- 17/2/2013
 *      fixed an issue related to setMarker();
 *      which caused text to appear on the same collumn as the marker
 *      For this to work you have to call the routine resetCursor()
 * 	    after the setMarker() routine
 *      added code to reset the display on init, without it the display
 *   	doesn't work reliable
 * ---- 4/9/2022
 *      remove the built-in character drawing
 *      add the read-mode (and read/modify/write) -> no frame buffer in
 *        ram needed.
 *      add the adafruit GFX interface
 * */

#include <SED1531.h>
#include "Arduino.h"
//#include <lcdFont1.h>

const byte lcdA0 = 12;
const byte lcdRW = 11;
const byte lcdEnable = 10;
const byte lcdDataPins[] = {A0,A1,A2,A3,A4,A5,6,7};

//byte currentLine = 0;
byte hPos = 0;

static void DBmode(byte state)
{
	for(int i=0;i<8;i++){
		pinMode(lcdDataPins[i], state);
	}
}

void SED1531::init(){
	pinMode(lcdA0, OUTPUT);
	pinMode(lcdRW, OUTPUT);
	pinMode(lcdEnable, OUTPUT);
	DBmode(OUTPUT);
	digitalWrite(lcdEnable, LOW);
	digitalWrite(lcdRW, HIGH);
	digitalWrite(lcdA0, LOW);
	delay(100);
	//the following actions are performed to init the lcd
	writecommand(0xe2);                                //reset display by soft
	delay(1);
	writecommand(0xa1);                                //ADC select
	writecommand(0xa2);                                //lcd bias 1/8
	//writecommand(0x2c);                                //power
	//delay(100);
	//writecommand(0x2e);                                //power
	//delay(100);
	writecommand(0x2f);                                //power
	delay(10);
	writecommand(0xa6);                                //normal / reverse
	writecommand(0x8f);                                //set electronic control
	writecommand(0xa4);                                //display off
	writecommand(0xaf);                                //display on
	writecommand(0x40); 				               //sel dram line 1 for com1
}

void SED1531::setContrast(byte contrast){
	writecommand(0x80+contrast);
}

void SED1531::inverse(byte reverse){
	writecommand(0xA6+reverse);
}

void SED1531::setMarker(byte marker, boolean on){
	byte highNibble, lowNibble;
	byte markerLCD;
	switch(marker){
		case 1:
			markerLCD = 20;			// arrows on the left
			break;
		case 2:
			markerLCD = 31;			// 2 hor. dashes
			break;
		case 3:
			markerLCD = 32;			// barcode
			break;
		case 4:
			markerLCD = 57;			// battery low
			break;
		case 5:
			markerLCD = 69;			// ?
			break;
		case 6:
			markerLCD = 78;			// arrow up
			break;
	}
	lowNibble = markerLCD&0xf;
	highNibble = markerLCD >> 4;
	bitSet(highNibble, 4);
	writecommand(0xb6);
	writecommand(highNibble);
	writecommand(lowNibble);
	writePixData(on);
}

void SED1531::setCol(byte col)
{
	writecommand(0x10+(col>>4));
	writecommand(0x00+(col&0x0f));
}

void SED1531::setDCursor(byte row){
	writecommand(0xb0+row);
	setCol(0);
}

void SED1531::setDCursor(byte row, byte col){
	writecommand(0xb0+row);
	setCol(col);
}

static void setDB(byte data)
{
	for (int i = 0; i < 8; i++) {
		digitalWrite(lcdDataPins[i], data&1);
		data = data >> 1;
	}
}

static byte getDB(void)
{
	uint8_t data;
	for (int i = 0; i < 8 ; i++) {
		data = data << 1;
		data |= digitalRead(lcdDataPins[7-i]);
	}
	return (byte)data;
}

static void DBwrite(byte data)
{
	setDB(data);
	digitalWrite(lcdEnable, HIGH);
	delayMicroseconds(1);
	digitalWrite(lcdEnable, LOW);
	delayMicroseconds(1);
	digitalWrite(lcdEnable, HIGH);
}

static void DBrmwrite(byte data, byte mask)
{
	/* data pins to input */
	DBmode(INPUT);
	digitalWrite(lcdRW, HIGH);
	digitalWrite(lcdA0, HIGH);

	digitalWrite(lcdEnable, HIGH);
	delayMicroseconds(1);
	digitalWrite(lcdEnable, LOW); // dummy read
	delayMicroseconds(1);
	digitalWrite(lcdEnable, HIGH);
	delayMicroseconds(1);
	digitalWrite(lcdEnable, LOW);
	delayMicroseconds(1);

	byte rdata = getDB();

	digitalWrite(lcdEnable, HIGH);
	/* data pins to output */
	digitalWrite(lcdRW, LOW);
	DBmode(OUTPUT);

	data = (data & mask) | (rdata & ~mask);

	setDB(data);

	delayMicroseconds(1);
	digitalWrite(lcdEnable, LOW);
	delayMicroseconds(1);
	digitalWrite(lcdEnable, HIGH);
}

void SED1531::writecommand(byte cmd){
	digitalWrite(lcdRW, LOW);
	digitalWrite(lcdA0, LOW);

	byte data = cmd;

	DBwrite(data);
}

#if 0
inline size_t SED1531::write(byte lcdData){
	if(lcdData == '\n'|| hPos>=100){
		hPos = 0;
		currentLine+=1;
		if(currentLine>=6){
			currentLine = 0;
		}
		setCursor(currentLine);
	}
	else if(isprint(lcdData)){
		byte character = lcdData - 32;
		digitalWrite(lcdRW, LOW);
		digitalWrite(lcdA0, HIGH);

		for(int col = 0;col<6;col++){
			byte data = col<5?pgm_read_byte_near(&lcdFonts[character][col]):0;
			setDB(data);
			digitalWrite(lcdEnable, HIGH);
			delayMicroseconds(10);
			digitalWrite(lcdEnable, LOW);
			delayMicroseconds(10);
			digitalWrite(lcdEnable, HIGH);
		}
		hPos+=6;
	}
}
#endif

void SED1531::writePixData(byte lcdData){
	digitalWrite(lcdRW, LOW);
	digitalWrite(lcdA0, HIGH);

	byte data = lcdData;

	DBwrite(data);

	hPos++;
}

void SED1531::writePixData(byte lcdData, byte mask){
	writecommand(0xe0); // read-modify-write

	DBrmwrite(lcdData, mask);

	writecommand(0xee);
	hPos++;
	setCol(hPos);
}

void SED1531::writePixData(byte lcdData[], int len) {
	digitalWrite(lcdRW, LOW);
	digitalWrite(lcdA0, HIGH);

	byte data;
	for (int i=0; i<len; i++) {
		data=0;

		for (int j=0; j<8; j++) {
			data>>=1;
			data|=lcdData[j]&0x80;
			lcdData[j]<<=1;
		}
		DBwrite(data);
	}
	hPos+=len;
}

void SED1531::writePixData(byte ndata, int len, byte mask) {
	writecommand(0xe0); // read-modify-write
	digitalWrite(lcdA0, HIGH);

	byte data;
	for (int i=0; i<len; i++) {

		DBrmwrite(ndata, mask);

	}
	writecommand(0xee);
	hPos+=len;
	setCol(hPos);
}

void SED1531::writePixData(byte lcdData[], int len, byte mask) {
	writecommand(0xe0); // read-modify-write
	digitalWrite(lcdA0, HIGH);

	for (int i=0; i<len; i++) {

		byte ndata=0;
		for (int j=0; j<8; j++) {
			ndata>>=1;
			ndata|=lcdData[j]&0x80;
			lcdData[j]<<=1;
		}
		DBrmwrite(ndata, mask);
	}
	writecommand(0xee);
	hPos+=len;
	setCol(hPos);
}

void SED1531::drawPixel(uint8_t x, uint8_t y, uint8_t val)
{
	setDCursor(y/8, x);
	val=val?0xff:0;
	uint8_t mask=1<<(y%8);
	writePixData(val, mask);
}

void SED1531::drawVline(uint8_t x, uint8_t y, uint8_t h, uint8_t val)
{
	val=val?0xff:0;
	uint8_t mask=~((1<<(y%8))-1);
	h+=y%8;
	y/=8;
	for (;h;h-=8) {
		setDCursor(y, x);
		if (h<8) {
			mask&=(1<<h)-1;
			writePixData(val,mask);
			break;
		}
		writePixData(val,mask);
		mask=~0;
		y++;
	}
}

void SED1531::drawHline(uint8_t x, uint8_t y, uint8_t w, uint8_t val)
{
	setDCursor(y/8, x);
	val=val?0xff:0;
	uint8_t mask=1<<(y%8);
	writePixData(val, w, mask);
}

void SED1531::drawFrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t val)
{
	val=val?0xff:0;
	uint8_t mask=~((1<<(y%8))-1);
	h+=y%8;
	y/=8;
	for (;h;h-=8) {
		setDCursor(y, x);
		if (h<8) {
			mask&=(1<<h)-1;
			writePixData(val,w,mask);
			break;
		}
		writePixData(val,w,mask);
		mask=~0;
		y++;
	}
}

void SED1531::resetCursor() {
	writecommand(0xEE);
	setCol(0);
}

/* Adafruit_GFX i/f */

void SED1531::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if (x&~0xff) return;
	if (y&~0xff) return;
	drawPixel((uint8_t)x, (uint8_t)y, (uint8_t)(color!=0));
}

void SED1531::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	if (x&~0xff) return;
	if (y>0xff) return;
	if (y<0) { h+=y; y=0; }
	if (h<=0) return;
	if (y+h>0xff) { h=0xff-y; }
	drawVline((uint8_t)x, (uint8_t)y, (uint8_t)h, (uint8_t)(color!=0));
}

void SED1531::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	if (y&~0xff) return;
	if (x>0xff) return;
	if (x<0) { w+=x; x=0; }
	if (w<=0) return;
	if (x+w>0xff) { w=0xff-x; }
	drawHline((uint8_t)x, (uint8_t)y, (uint8_t)w, (uint8_t)(color!=0));
}

void SED1531::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if (x>0xff) return;
	if (x<0) { w+=x; x=0; }
	if (w<=0) return;
	if (x+w>0xff) { w=0xff-x; }
	if (y>0xff) return;
	if (y<0) { h+=y; y=0; }
	if (h<=0) return;
	if (y+h>0xff) { h=0xff-y; }
	drawFrect((uint8_t)x, (uint8_t)y, (uint8_t)w, (uint8_t)h, (uint8_t)(color!=0));
}

