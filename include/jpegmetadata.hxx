#ifndef JPEG_METADATA_H
#define JPEG_METADATA_H


#include <metadata.hxx>

#include <fstream>
#include <string>


class JPEGMetadata : public Metadata {
	METADATA_CONSTRUCTORS(JPEGMetadata);

protected:
	virtual void read(std::ifstream&) override;

public:
	virtual void write(const std::string&) override;
};


#endif
