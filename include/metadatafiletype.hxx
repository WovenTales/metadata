#ifndef METADATAFILETYPE_H
#define METADATAFILETYPE_H


#include <string>


namespace metadata {


enum struct MetadataFileType {
	INVALID,
	JPEG,
	PNG,
};

MetadataFileType detectFileType(const std::string&);


}
#endif
