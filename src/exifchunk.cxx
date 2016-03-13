#include <exifchunk.hxx>

#include <exififdmetadata.hxx>

#include <cmath>

#include <algorithm>
#include <map>
#include <string>
#include <sstream>
#include <utility>


ExifChunk::ExifChunk(std::istream& file, bool b, unsigned int& i) : Chunk(file) {
	bigEndian = b;
	exifIndex = i;
	++i;

	// As this is simply a wrapper for IFD entries, don't need to save raw data
	subChunks.push_back(new ExifIFDMetadata(file, bigEndian));
}

ExifChunk::ExifChunk(const ExifChunk& c) : Chunk(c) {
		bigEndian = c.bigEndian;
		exifIndex = c.exifIndex;
		nextOffset = c.nextOffset;
}


std::map< std::string, std::pair< std::string, ChunkType > > ExifChunk::convertMap(
		std::map< std::string, std::pair< std::string, ChunkType > > map) {
	std::map< std::string, std::pair< std::string, ChunkType > > out;

	// Swap the byte order of (numerical) keys from the standard big endian map
	for (auto a : map) {
		out.emplace(std::string({ a.first[1], a.first[0] }), a.second);
	}

	return out;
}

std::string ExifChunk::toBigEndian(std::string in, bool isBigEndian) {
	if (isBigEndian == false) {
		std::reverse(&*(in.begin()), &*(in.rbegin()));
	}

	return in;
}


std::string ExifChunk::data(ChunkType) const {
	std::ostringstream ss;

	if (subChunks.empty()) {
		return "(Empty)";
	} else {
		unsigned int count = (*(subChunks.begin()))->size();
		ss << count << " tag" << (count == 1 ? "" : "s");

		return ss.str();
	}
}

std::string ExifChunk::defaultChunkName() const {
	std::ostringstream ss;

	ss << "IFD table " << exifIndex;
	return ss.str();
}

std::string ExifChunk::printableTypeCode() const {
	return std::to_string(exifIndex);
}


ChunkType ExifChunk::type() const {
	return ChunkType::OTHER;
}


float ExifChunk::exifRational(const char* offset, bool bigEndian) {
	return std::abs(exifSRational(offset, bigEndian));
}

float ExifChunk::exifSRational(const char* offset, bool bigEndian) {
	float out = (int)readBytes(offset, 4, bigEndian);
	out /= (int)readBytes((offset + 4), 4, bigEndian);

	return out;
}


bool ExifChunk::required() const {
	// TODO After generalizing to TIFF files, will not be true in ¿most? cases
	return false;
}


void ExifChunk::write(std::ostream& out) const {
	out.put(0xFF);
	out.put(typeCode[0]);

	if (typeCode == "\xDA") {
		out.write(raw, length);
	} else if (length != 0) {
		// Automatically truncates to single byte
		// Copy byte-by-byte to ensure proper order
		out.put((length + 2) >> 8);
		out.put(length + 2);

		out.write(raw, length);
	}
}
