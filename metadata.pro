######################################################################
# Automatically generated by qmake (3.0) Thu Jan 28 14:52:22 2016
######################################################################

TEMPLATE     = app
INCLUDEPATH += .

QT          += core gui widgets
CONFIG      += c++11
DESTDIR      = bin
INCLUDEPATH += include
OBJECTS_DIR  = obj
DOCS_DIR     = docs

# Input
HEADERS += include/chunk.hxx \
           include/imagepanel.hxx \
           include/jpegchunk.hxx \
           include/jpegmetadata.hxx \
           include/mainwindow.hxx \
           include/metadata.hxx \
           include/metadatafactory.hxx \
           include/metadatafiletype.hxx \
           include/metadatapanel.hxx \
           include/pngchunk.hxx \
           include/pngmetadata.hxx \
           include/previewpanel.hxx

SOURCES += src/chunk.cxx \
           src/imagepanel.cxx \
           src/jpegchunk.cxx \
           src/jpegmetadata.cxx \
           src/mainwindow.cxx \
           src/metadata.cxx \
           src/metadatafactory.cxx \
           src/metadatafiletype.cxx \
           src/metadatapanel.cxx \
           src/pngchunk.cxx \
           src/pngmetadata.cxx \
           src/previewpanel.cxx

# Scopes
debug {
	QMAKE_CXXFLAGS_RELEASE -= -O
	QMAKE_CXXFLAGS_RELEASE -= -O1
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE -= -O3

	QMAKE_CXXFLAGS_RELEASE += -Og
}

win32 {
	# Check if Doxygen is installed on Windows (tested on Win7)
	DOXYGEN_BIN = $$system(where doxygen)

	Release {
		# Copy the DLLs if necessary
		QMAKE_POST_LINK = windeployqt --compiler-runtime $$system_quote($$system_path($${OUT_PWD}/bin/))
	}
}

unix|max {
	# Check if Doxygen is installed on Linux or Mac (tested on Ubuntu, not yet on the Mac)
	DOXYGEN_BIN = $$system(which doxygen)
}


# Doxygen test code from answer by spacemig
# <http://stackoverflow.com/questions/30162668/check-if-executable-is-in-the-path-using-qmake>

# TODO: Set HAVE_DOT = NO in Doxyfile if not on system ('dot' part of Graphviz)
# TODO: Create 'clean' command for docs

isEmpty(DOXYGEN_BIN) {
	message("Doxygen not found; 'docs' targets not added to makefile")
} else {
	# Create a separate target for proper naming
	docs.depends = docs_dummy

	# Choose one of the resulting folders to determine whether to regenerate
	docs_dummy.target = $$DOCS_DIR/html
	docs_dummy.depends = $$DOCS_DIR/Doxyfile $$HEADERS $$SOURCES
	docs_dummy.commands = $$DOXYGEN_BIN $$DOCS_DIR/Doxyfile

	QMAKE_EXTRA_TARGETS += docs docs_dummy
}
