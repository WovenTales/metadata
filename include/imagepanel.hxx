#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H


#include <string>

// core
#include <Qt>

// gui
#include <QPixmap>

// widgets
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QWidget>


class ImagePanel : public QFrame {
private:
	QPixmap  img;
	QPixmap  map;

	QLabel*     label;
	QLabel*     file;
	QBoxLayout* layout;

public:
	ImagePanel(std::string, QWidget* = 0, Qt::WindowFlags = 0);

	virtual void resizeEvent(QResizeEvent*);
};


#endif
