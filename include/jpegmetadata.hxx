#ifndef JPEG_METADATA_H
#define JPEG_METADATA_H


#include <metadata.hxx>

#include <fstream>
#include <string>


namespace metadata {


class JPEGMetadata : public Metadata {
	METADATA_CONSTRUCTORS(JPEGMetadata);

protected:
	virtual void read(std::ifstream&) override;

public:
	virtual void write(const std::string&) const override;
};


}
#endif
