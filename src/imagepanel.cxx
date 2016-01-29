#include <imagepanel.hxx>

#include <cstddef>

// core
#include <QSize>
#include <QString>

// gui
#include <QResizeEvent>

// widgets
#include <QVBoxLayout>


ImagePanel::ImagePanel(std::string path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	img = QPixmap(QString(path.c_str()));

	label  = new QLabel(this);
	file   = new QLabel(this);
	layout = new QVBoxLayout();

	label->setScaledContents(true);

	file->setText(path.c_str());

	layout->addWidget(label, Qt::AlignHCenter);
	layout->addWidget(file, Qt::AlignHCenter);
	this->setLayout(layout);

	// Setting label picture occurs in resizeEvent()
}


void ImagePanel::resizeEvent(QResizeEvent* e) {
	QWidget::resizeEvent(e);

	QSize s = e->size();

	int l, t, r, b;
	layout->getContentsMargins(&l, &t, &r, &b);

	int h, w;
	w = (s.width()  - l - r);
	h = (s.height() - t - b - layout->spacing() - file->size().height());

	// FIXME: Aspect ratio not maintained after encapsulation into layout
	map = img.scaled(w, h, Qt::KeepAspectRatio);

	label->setPixmap(map);
	label->adjustSize();
}
