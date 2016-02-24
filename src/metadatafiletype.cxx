#include <metadatafiletype.hxx>


MetadataFileType detectFileType(const std::string& path) {
	if (path.empty()) {
		return MetadataFileType::INVALID;
	}

	// TODO Smarter detection based on headers
	std::string ext = path.substr(path.find_last_of('.'));

	if (ext == ".png") {
		return MetadataFileType::PNG;
	} else if ((ext == ".jpg") || (ext == ".jpeg") || (ext == ".jpe")) {
		return MetadataFileType::JPEG;
	} else {
		return MetadataFileType::INVALID;
	}
}
