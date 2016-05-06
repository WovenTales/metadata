#ifndef METADATAFACTORY_H
#define METADATAFACTORY_H


#include <metadata.hxx>

#include <string>


namespace metadata {


struct MetadataFactory {
public:
	MetadataFactory() = delete;

	static Metadata* generate(const std::string&);
	static Metadata* generate(const Metadata*);
};


}
#endif
