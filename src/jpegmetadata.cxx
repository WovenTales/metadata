#include <jpegmetadata.hxx>

#include <jpegchunk.hxx>

#include <iostream>


void JPEGMetadata::read(std::istream& file) {
	loopChunks< JPEGChunk >(file);
}

void JPEGMetadata::write(const std::string& path) {
	std::ofstream out(path);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		t->write(out);
	}

	out.close();
}
