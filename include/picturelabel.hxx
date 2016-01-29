#ifndef PICTURELABEL_H
#define PICTURELABEL_H


// core
#include <QSize>

// gui
#include <QPixmap>

// widgets
#include <QLabel>


class PictureLabel : public QLabel {
private:
	const QPixmap& img;

public:
	PictureLabel(const QPixmap&, QWidget* = 0, Qt::WindowFlags = 0);

	virtual int   heightForWidth(int) const;
	virtual int   widthForHeight(int) const;
	virtual QSize sizeHint()          const;

	bool isValid() const;
};


#endif
