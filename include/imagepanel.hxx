#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H


#include <string>

// gui
#include <QPixmap>

// widgets
#include <QFrame>
#include <QLabel>
#include <QLayout>


class ImagePanel : public QFrame {
private:
	QPixmap  img;
	QPixmap  map;

	QLabel*     label;
	QLabel*     file;
	QBoxLayout* layout;

public:
	ImagePanel(std::string);

	virtual void resizeEvent(QResizeEvent*);
};


#endif
