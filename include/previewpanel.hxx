#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H


#include <string>

// core
#include <Qt>

// gui
#include <QPixmap>
#include <QResizeEvent>

// widgets
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>


class PreviewPanel : public QFrame {
private:
	const QPixmap& img;
	      QPixmap  map;

	QLabel*      label;
	QVBoxLayout* layout;

public:
	PreviewPanel(const QPixmap&, QWidget* = 0, Qt::WindowFlags = 0);

	virtual void resizeEvent(QResizeEvent*);
};


#endif
