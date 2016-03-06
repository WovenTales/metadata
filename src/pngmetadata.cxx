#include <pngmetadata.hxx>

#include <pngchunk.hxx>

#include <algorithm>
#include <iostream>


void PNGMetadata::read(std::istream& file) {
	char header[9];
	// NULL-terminate to emulate C-string
	header[8] = 0x00;

	file.clear();
	file.read(header, 8);  // Not using get() as that would break on newline
	if (file.fail()) {
		std::cout << "ERROR: bad file" << std::endl;
		throw 'F';
	}

	// Check against PNG header
	if (header != std::string("\x89PNG\x0D\x0A\x1A\x0A")) {
		std::cout << "ERROR: wrong file header" << std::endl;
		throw 'H';
	}

	loopChunks< PNGChunk >(file);
}

void PNGMetadata::write(const std::string& path) {
	std::ofstream out(path, std::ofstream::binary);

	out.write("\x89PNG\x0D\x0A\x1A\x0A", 8);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		for (auto c = t->begin(); c != t->end(); ++c) {
			(*c)->write(out);
		}
	}

	out.close();
}
