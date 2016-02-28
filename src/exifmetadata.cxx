#include <exifmetadata.hxx>

#include <exifchunk.hxx>

#include <iostream>



void ExifMetadata::read(std::istream& file) {
	char header[8];

	file.clear();
	file.read(header, 8);
	if (file.fail()) {
		std::cout << "ERROR: bad file" << std::endl;
		throw 'F';
	}

	// Check against EXIF header
	if (std::equal(header, (header + 4), "II\x2A\x00")) {
		bigEndian = false;
	} else if (std::equal(header, (header + 4), "MM\x00\x2A")) {
		bigEndian = true;
	} else {
		std::cout << "ERROR: wrong file header" << std::endl;
		throw 'H';
	}

	unsigned int offset = ExifChunk::readBytes((header + 4), 4, bigEndian);

	unsigned int i = 0;
	// TODO Allow custom loop test to consolidate back into single call:
	//loopChunks< ExifChunk >(file, bigEndian, i);
	while (offset != 0) {
		ExifChunk* c = NULL;
		try {
			c = new ExifChunk(file, bigEndian, i);
		} catch (char e) {
			std::cout << "ERROR: couldn't read chunk ";
			switch (e) {
				case 'L':
					std::cout << "length";
					break;
				case 'H':
					std::cout << "header";
					break;
				case 'D':
					std::cout << "data";
					break;
				case 'C':
					std::cout << "CRC";
					break;
			}
			std::cout << std::endl;

			delete c;
			throw 'C';
		}

		// Note that this has to be emplace_* rather than push_*
		//   as otherwise the destructor will free in-use memory
		tags.emplace_back(c);
		offset = c->getNextOffset();
	}
}

void ExifMetadata::write(const std::string& path) {
	std::ofstream out(path);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		t->write(out);
	}

	out.close();
}
