#include <imagepanel.hxx>

// core
#include <QString>

// widgets
#include <QApplication>
#include <QMainWindow>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMainWindow w;

	ImagePanel* img = new ImagePanel(argv[1]);

	w.setCentralWidget(img);
	w.show();

	return a.exec();
}
