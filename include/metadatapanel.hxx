#ifndef METADATAPANEL_H
#define METADATAPANEL_H


#include <metadata.hxx>

#include <string>
#include <tuple>
#include <vector>

// core
#include <Qt>
#include <QString>

// widgets
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>


class MetadataPanel : public QScrollArea {
	Q_OBJECT

private:
	Metadata     data;

	QFrame*      grid;
	QGridLayout* layout;

	std::vector< std::tuple< unsigned int, QLabel*, QLabel*, QPushButton* > > labels;

public:
	MetadataPanel(const QString&, QWidget* = 0, Qt::WindowFlags = 0);

	bool isValid() const;

	void write(const std::string& path) { data.write(path); };

public slots:
	void clearTag(unsigned int);
};


#endif
