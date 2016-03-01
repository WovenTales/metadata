#ifndef PICTUREPANEL_H
#define PICTUREPANEL_H


#include <string>

// core
#include <Qt>

// gui
#include <QPixmap>

// widgets
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>


class PicturePanel : public QFrame {
private:
	QVBoxLayout*  layout;
	QHBoxLayout*  cross;

public:
	PicturePanel(const QPixmap&, QWidget* = 0, Qt::WindowFlags = 0);

	virtual void resizeEvent(QResizeEvent*) override;

	bool isValid() const;
};


#endif
