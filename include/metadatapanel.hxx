#ifndef METADATAPANEL_H
#define METADATAPANEL_H


#include <string>
#include <utility>
#include <vector>

// core
#include <Qt>
#include <QStringList>

// widgets
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>


class MetadataPanel : public QFrame {
private:
	QImage      img;
	QStringList keys;

	QGridLayout* layout;

	std::vector< std::pair< QLabel*, QLabel* > > labels;

public:
	MetadataPanel(std::string, QWidget* = 0, Qt::WindowFlags = 0);
};


#endif
