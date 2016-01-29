#include <picturelabel.hxx>

// widgets
#include <QSizePolicy>


PictureLabel::PictureLabel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f), img(i) {
	QSizePolicy p = sizePolicy();
	p.setHeightForWidth(true);
	p.setWidthForHeight(true);
	setSizePolicy(p);

	setScaledContents(true);
	setPixmap(img);
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

QSize PictureLabel::sizeHint() const {
	return QSize(300, heightForWidth(300));
}


bool PictureLabel::isValid() const {
	return !img.isNull();
}
