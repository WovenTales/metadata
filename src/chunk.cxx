#include <chunk.hxx>

#include <algorithm>
#include <iomanip>
#include <sstream>


namespace metadata {


// Preprocessor

/*! \def CHUNK_CONSTRUCTORS
 *
 *  Provides default copy and move constructors (can be overridden if the
 *  subclass introduces any new member variables), but it is still expected
 *  to provide its own prototype following the pattern:
 *  \code _SUBTYPE_(std::istream&); \endcode
 *
 *  Should be called as early as possible in the subclass definition, with
 *  the name of the class itself passed as \p _SUBTYPE_.
 *
 *  \todo Can we remove the requirement for the outside \p istream prototype?
 *        (ie. is it properly signposted and do all subtypes have it, and does
 *        EXIF *require* passing around endianness at construction).
 *  \todo See if it's possible to prevent inheritance of the \p protected
 *        constructor, but (due not access restrictions) should still be safe.
 */



// Types


/*! \class Chunk
 *
 *  Due to the many formats and the frequent complexity of reading the binary
 *  data, it is necessary to provide an abstract, unified wrapper.
 *
 *  \todo Include snippet showing proper subclass construction (with macro)
 *  \todo Try to find a way to avoid having to \\copybrief on every overridden
 *        virtual method to remove automatically copied details
 *
 *  \sa CHUNK_CONSTRUCTORS: Required macro call for subclass definitions.
 */


/*! \enum Chunk::Type
 *
 *  The GUI might treat some formats differently, whether through
 *  further processing before display, or streamlining construction
 *  by not constructing, for example, an empty label.
 *
 *  \todo Better descriptions of values
 *  \todo Implement IMAGE rendering method (for thumbnails and files with
 *        multiples). Make sure that all image data is included rather than
 *        being cropped or otherwise hidden.
 */



// Variables


/*! \var Chunk::raw
 *
 *  Not all segments of the data (for example, the CRC checksum of PNG) need to
 *  be saved in this field, as long as they may be perfectly reconstructed when
 *  writing out the:image.
 */


/*! \var Chunk::typeCode
 *
 *  This provides the lookup into typeMap, so ensure that the code retrieved
 *  from the file matches the key there.
 *
 *  \warning For filetypes where type is indicated by a numerical value rather
 *           than ASCII, be aware that any \p 0x00 byte will terminate the code.
 *
 *  \todo Is \p string really the best way to store this, or would a \p char
 *        array or even a template be better?
 */


/*! \var Chunk::typeMap
 *
 *  When constructing an entry for this, the key should be one of the potential
 *  codes identifying a metadata tag, while the first parameter in the mapped
 *  value will be displayed as the title of all tags of that type and the second
 *  determines how they will be rendered.
 *
 *  Not every possible type code needs to be included; if the key is not found,
 *  the value from \ref defaultChunkName() will be used and the tag will be
 *  displayed according to Type::OTHER.
 *
 *  \sa typeCode: Used to determine which key any given Chunk will search for.
 */


/*! \var Chunk::Type Chunk::HIDE
 *
 *  \attention This should only be used for unnecessary chunks *not represented
 *             in the image*, as the user should be able to see every byte.
 */



// Constructors, destructors, and operators


/*! \p Protected due to the highly context-sensitive nature of \p file, which
 *  could easily cause issues if not carefully managed, and the requirement
 *  that the keys of \p tm be hard-coded in the same format as read from the file.
 *
 *  \todo Is \p file really necessary to include in the call?
 *
 *  \warning Declare the map passed to \p tm in global scope, or at least
 *           somewhere guaranteed to not leave scope until after every object
 *           using it has also left scope.
 *
 *  \see typeMap for the format of the map to pass to \p tm; that member is meant
 *       to emulate a subclass-specific static variable using this reference.
 */
Chunk::Chunk(std::istream& file, const Chunk::ChunkTypeMap& tm) : typeMap(tm) {
	file.clear();
}

Chunk::Chunk(const Chunk& c) : typeMap(c.typeMap) {
	length = c.length;
	typeCode = c.typeCode;

	raw = new char[length];
	std::copy(c.raw, (c.raw + length), raw);
}

Chunk::Chunk(Chunk&& c) : typeMap(c.typeMap) {
	length = c.length;
	typeCode = c.typeCode;

	raw = c.raw;
	c.raw = NULL;
}


Chunk::~Chunk() {
	if (raw != NULL) {
		delete[] raw;
	}
}


// Functions


std::string Chunk::data() const {
	//! \todo Don't call \ref sanitize until the Qt-specific code
	return sanitize(data(type()));
}

/*! Subclasses are expected to override this -- especially in the case of
 *  \p t \p == \p Type::CUSTOM -- to support formatting specific to that
 *  image format, but to call back to this version in the default case.
 *
 *  \todo Include snippet of skeleton method override
 */
std::string Chunk::data(Chunk::Type t) const {
	unsigned int print = 0, hex = 0;
	unsigned long sum = 0;
	std::ostringstream out;

	switch (t) {
		case Type::OTHER:
			out << hexString(false, 0, 0, &print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				return std::string(raw, length);
			} else {
				return out.str();
			}
		case Type::NONE:
			return "Should not be shown";
		case Type::HIDE:
			return "Should not be shown";
		case Type::CUSTOM:
			return "SUBCLASS-DEFINED";


		case Type::HEX:
			return hexString();


		case Type::DIGIT:
			// Read through each byte and add to output, independent of the
			//   size of the datatypes used by the system
			//! \todo Move little-endian support here from the EXIF extension
			for (unsigned int i = 0; i < length; ++i) {
				sum = sum << 8;
				sum += (unsigned char)raw[i];
			}
			return std::to_string(sum);
		case Type::TEXT:
			return std::string(raw, length);


		case Type::COLOR:
			//! \todo Display color swatch
			return "TODO";
		case Type::COUNT:
			// Although this is a very legitimate type, it only has meaning when
			//   considering multiple Chunks, and so is handled in MetadataTag
			return "Should not be shown";
		case Type::TIME:
			//! \todo Format timecode
			return "TODO";
	}

	return "ERROR";
}


/*! \fn Chunk::defaultChunkName
 *
 *  The resulting string should be properly formatted for all possible codes,
 *  but should still be unique for each separate type of unrecognised tag.
 *
 *  \todo Include snippet of skeleton method override
 *
 *  \sa Chunk::printableTypeCode: Recommended means of disambiguation.
 */


/*! Resulting string is comprised of eight space-separated hex bytes (in two-
 *  digit groups) with the ASCII representation of that line to the right; any
 *  non-printable characters are represented by a period.
 *
 *  \todo Remove HTML tags to improve applicable locations; we can add them
 *        later for, eg., the Qt UI, but they won't do anything for console
 *
 *  \param line    Whether to insert a new line before the hex representation.
 *  \param offset  How many bytes after the beginning of \ref raw to begin reading.
 *  \param span    How many bytes to read (\p 0 = "to end of tag").
 *  \param print   \p int in which to store the number of printable characters.
 *  \param hex     \p int in which to store the number of non-printable characters.
 *
 *  \note For both \p print and \p hex, a value of \p NULL indicates that that
 *        particular count will not be saved anywhere. If either is not \p NULL,
 *        that \p int will be set to 0 before counting the relevant characters.
 */
std::string Chunk::hexString(bool line, unsigned int offset, unsigned int span,
		unsigned int* print, unsigned int* hex) const {
	std::ostringstream ss;

	unsigned int i = offset;
	// Reframe 'span' as representing the endstop to parse to
	if (span == 0) {
		span = length;
	} else {
		span += offset;

		if (span > length) {
			span = length;
		}
	}

	unsigned int size = (span - offset);
	//! \todo Move to the Qt usage as specifically outside scope of representation
	if (line) {
		ss << "<br>";
	}

	// Reset counts
	if (print != NULL) {
		*print = 0;
	}
	if (hex != NULL) {
		*hex = 0;
	}

	if (size > 256) {
		//! \todo Move to the Qt usage rather than forcing it on every UI
		// Long QLabels cause a very noticeable slowdown
		ss << size << " bytes";
	} else {
		ss << std::hex << "<code>";

		while (i < span) {
			std::ostringstream str;

			// NOTE: i is incremented here
			for (unsigned int j = 0; j < 8; ++i, ++j) {
				// Insert spaces to incomplete final line to line up the ASCII
				if (i == span) {
					for (; j < 8; ++j) {
						// Should never occur on j == 0
						ss << "&nbsp;&nbsp;&nbsp;";
					}
					break;
				}

				// Adding padding at start helps set off from non-hex text
				ss << "&nbsp;";

				// Insert numerical value as two-digit hex
				ss << std::setw(2) << std::setfill('0');
				ss << ((unsigned int)raw[i] & 0xFF);

				if (std::isprint(raw[i])) {
					if (print != NULL) {
						++(*print);
					}

					// Insert ASCII character directly
					if (raw[i] == '&') {
						str << "&amp;";
					} else if (raw[i] == '<') {
						str << "&lt;";
					} else if (raw[i] == '>') {
						str << "&gt;";
					} else {
						str << raw[i];
					}
				} else {
					if (hex != NULL) {
						++(*hex);
					}

					str << '.';
				}
			}

			// &nbsp; before <br> is a hack to prevent the scroll bar from overlapping text
			ss << "&nbsp;&nbsp;" << str.str() << (i != span ? "&nbsp;&nbsp;&nbsp;<br>" : "");
		}

		ss << "</code>";
	}

	return ss.str();
}


std::string Chunk::name() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return name(i->second.second, i->second.first);
	} else {
		return defaultChunkName(typeCode);
	}
}

/*! The title retrieved from the \ref typeMap is passed into the function to
 *  allow, for example, appending the type code to an application-specific tag.
 *  If no modification is desired in a particular case, just return \p title.
 *
 *  \note This will never be called if the code lookup was unsuccessful; see
 *        \ref defaultChunkName for customising the title in that situation.
 */
std::string Chunk::name(Chunk::Type, const std::string& title) const {
	return title;
}


/*! \fn Chunk::printableTypeCode
 *
 *  The resulting string should be properly formatted for all possible codes,
 *  so a lookup table is not advised.
 *
 *  \todo Include snippet of skeleton method override
 */


/*! \fn Chunk::required
 *
 *  Should only return \p true if the image actually *breaks* if the tag is
 *  removed, and not if it only scales the image, for example.
 */


/*! \todo Move to one of the Qt files, as there's no backend benefit of HTML
 *  \todo Reimplement escaping of \&amp; and \&lt;
 */
std::string Chunk::sanitize(std::string str) {
	size_t i = -1;
	while ((i = str.find("\r\n")) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}
	while ((i = str.find('\n')) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}

	return str;
}


/*! \todo \p unsigned \p int is really not the best type for the output
 *  \todo Might be nice to add a \#define to support little-endian systems
 */
unsigned int Chunk::readBytes(const char* data, unsigned char length, bool bigEndian) {
	unsigned int out   = 0;
	// Awkward assignment is required with shift *crements after the out increment,
	//   but still a lot easier than switching those and having little-endian awkwardness
	         int shift = (bigEndian ? ((length - 1) * 8) : 0);

	for (unsigned char i = 0; i < length; ++i, ++data) {
		out += ((unsigned char)(*data) << shift);

		if (bigEndian) {
			// Will underflow on the last loop, but (i < length) fails before next use
			shift -= 8;
		} else {
			shift += 8;
		}
	}

	return out;
}


Chunk::Type Chunk::type() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return i->second.second;
	} else {
		return Type::OTHER;
	}
}


/*! \fn Chunk::write
 *
 *  If the tag hasn't been changed, should output the data *exactly* as it was
 *  in the original file.
 */


}
