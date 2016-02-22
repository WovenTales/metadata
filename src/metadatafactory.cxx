#include <metadatafactory.hxx>

#include <metadatafiletype.hxx>
#include <pngmetadata.hxx>

#include <cstddef>

#include <fstream>


Metadata* MetadataFactory::generate(const std::string& path) {
	Metadata* out;

	switch (detectFileType(path)) {
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
		case MetadataFileType::PNG:
			out = new PNGMetadata();
			out->type = MetadataFileType::PNG;
			break;
		default:
			return NULL;
	}
	out->tags = m->tags;

	return out;
}
