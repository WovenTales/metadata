#include <exififdmetadata.hxx>

#include <exififdchunk.hxx>

#include <iostream>



ExifIFDMetadata::ExifIFDMetadata(std::istream& file, bool b) {
	bigEndian = b;

	create(file);
}

void ExifIFDMetadata::read(std::istream& file) {
	char bytes[2];

	// Get field count
	file.read(bytes, 2);
	if (file.fail()) {
		std::cout << "ERROR: bad file" << std::endl;
		throw 'F';
	}
	if (bigEndian) {
		exifCount = ((((unsigned int)bytes[0] & 0xFF) << 8) + ((unsigned int)bytes[1] & 0xFF));
	} else {
		exifCount = (((unsigned int)bytes[0] & 0xFF) + (((unsigned int)bytes[1] & 0xFF) << 8));
	}

	unsigned int count = 0;
	// TODO Allow custom loop test to consolidate back into single call:
	//loopChunks< ExifIFDChunk >(file, bigEndian);
	while (count < exifCount) {
		ExifIFDChunk* c = NULL;
		try {
			c = new ExifIFDChunk(file, bigEndian);
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
		++count;
	}
}

void ExifIFDMetadata::write(const std::string& path) {
	std::ofstream out(path);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		t->write(out);
	}

	out.close();
}
