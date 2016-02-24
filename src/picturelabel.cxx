#include <picturelabel.hxx>

// widgets
#include <QSizePolicy>


PictureLabel::PictureLabel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f), img(i) {
	QSizePolicy p = sizePolicy();

	p.setHeightForWidth(true);
	p.setHorizontalPolicy(QSizePolicy::Maximum);
	p.setVerticalPolicy(QSizePolicy::Maximum);
	p.setHorizontalStretch(0);
	p.setVerticalStretch(0);

	setSizePolicy(p);

	setScaledContents(true);
	setPixmap(img);
}


bool PictureLabel::hasHeightForWidth() const {
	return true;
}


int PictureLabel::heightForWidth(int w) const {
	double dw = w;
	double di = img.width();
	double dh = img.height();

	return ((dw * dh) / di);
}

int PictureLabel::widthForHeight(int h) const {
	double dw = img.width();
	double di = img.height();
	double dh = h;

	return ((dw * dh) / di);
}


bool PictureLabel::isValid() const {
	return !img.isNull();
}
