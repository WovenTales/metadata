#include <metadatafactory.hxx>

#include <jpegmetadata.hxx>
#include <metadatafiletype.hxx>
#include <pngmetadata.hxx>

#include <cstddef>

#include <fstream>


namespace metadata {


Metadata* MetadataFactory::generate(const std::string& path) {
	Metadata* out;

	switch (detectFileType(path)) {
		case MetadataFileType::JPEG:
			out = new JPEGMetadata(path);
			out->type = MetadataFileType::JPEG;
			break;
		case MetadataFileType::PNG:
			out = new PNGMetadata(path);
			out->type = MetadataFileType::PNG;
			break;
		default:
			return NULL;
	}

	return out;
}

Metadata* MetadataFactory::generate(const Metadata* m) {
	Metadata* out;

	switch (m->type) {
		case MetadataFileType::JPEG:
			out = new JPEGMetadata();
			break;
		case MetadataFileType::PNG:
			out = new PNGMetadata();
			break;
		default:
			return NULL;
	}
	out->type = m->type;
	out->tags = m->tags;

	return out;
}


}
