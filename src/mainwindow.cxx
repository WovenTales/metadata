#include <mainwindow.hxx>

// core
#include <QKeySequence>
#include <QString>

// widgets
#include <QApplication>
#include <QMenuBar>


MainWindow::MainWindow() {
	core = new ImagePanel();
	setCentralWidget(core);

	// TODO: Add support for eg. Mac with single bar across all windows (setMenuBar())
	QMenuBar* menu  = menuBar();
	fileMenu = menu->addMenu("&File");

	openFile = new QAction("&Open", fileMenu);
	openFile->setShortcuts(QKeySequence::Open);
	connect(openFile, SIGNAL(triggered()), this, SLOT(openDialog()));
	fileMenu->addAction(openFile);
}


void MainWindow::openDialog() {
	if (core != NULL) {
		core->setParent(NULL);
		delete core;
	}

	// TODO: Handle within ImagePanel so don't need extra creation/deletion
	core = new ImagePanel();
	setCentralWidget(core);
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	w.show();
	return a.exec();
}
