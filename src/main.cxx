#include <imagepanel.hxx>
#include <metadatapanel.hxx>

// widgets
#include <QApplication>
#include <QFrame>
#include <QMainWindow>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMainWindow w;

	QFrame*        frame  = new QFrame();
	ImagePanel*    img    = new ImagePanel(argv[1], frame);
	MetadataPanel* data   = new MetadataPanel(argv[1], frame);
	QHBoxLayout*   layout = new QHBoxLayout();

	layout->addWidget(img);
	layout->addWidget(data);
	frame->setLayout(layout);

	w.setCentralWidget(frame);
	w.show();

	return a.exec();
}
