#ifndef PNG_METADATA_H
#define PNG_METADATA_H


#include <metadata.hxx>

#include <fstream>
#include <string>


class PNGMetadata : public Metadata {
	METADATA_CONSTRUCTORS(PNGMetadata);

protected:
	virtual void read(std::ifstream&) override;

public:
	virtual void write(const std::string&) const override;
};


#endif
