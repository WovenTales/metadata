#ifndef METADATAFILETYPE_H
#define METADATAFILETYPE_H


#include <string>


enum struct MetadataFileType {
	INVALID,
	PNG,
};

MetadataFileType detectFileType(const std::string&);


#endif
