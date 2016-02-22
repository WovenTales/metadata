#ifndef PNG_METADATA_H
#define PNG_METADATA_H


#include <metadata.hxx>

#include <string>


class PNGMetadata : public Metadata {
	using Metadata::Metadata;

public:
	PNGMetadata(const std::string&);

	virtual void write(const std::string&) const override;
};


#endif
