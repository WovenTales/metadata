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

	loopChunks< ExifIFDChunk >(file, bigEndian);
}

void ExifIFDMetadata::write(const std::string& path) {
	std::ofstream out(path);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		t->write(out);
	}

	out.close();
}
