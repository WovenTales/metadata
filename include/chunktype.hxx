#ifndef CHUNKTYPE_H
#define CHUNKTYPE_H


enum struct ChunkType {
	OTHER,   // autodetect text, otherwise hex
	NONE,    // do not display contents
	HIDE,    // do not display contents or name
	WRAPPER, // optimized for wrapping tags, but not required
	CUSTOM,  // specific to format of subclass

	HEX,     // raw hex value

	DIGIT,   // numerical value (big endian, unsigned)
	TEXT,    // text string

	COLOR,   // color swatch
	COUNT,   // count and display occurrences of tag
	TIME,    // timestamp
};


#endif
