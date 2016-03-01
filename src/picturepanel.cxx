#include "picturepanel.hxx"


PicturePanel::PicturePanel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QFrame(parent) {

}

/*
// imagelabel2.h

class ImageLabel2 : public QWidget
{
    Q_OBJECT

public:
    explicit ImageLabel2(QWidget *parent = 0);
    const QPixmap* pixmap() const;

public slots:
    void setPixmap(const QPixmap&);

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap pix;
};

// imagelabel2.cpp

ImageLabel2::ImageLabel2(QWidget *parent) :
    QWidget(parent)
{
}

void ImageLabel2::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (pix.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = pix.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPixmap scaledPix = pix.scaled(pixSize,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation
                                   );

    painter.drawPixmap(QPoint(), scaledPix);

}

const QPixmap* ImageLabel2::pixmap() const {
    return &pix;
}

void ImageLabel2::setPixmap (const QPixmap &pixmap){
    pix = pixmap;
}
*/