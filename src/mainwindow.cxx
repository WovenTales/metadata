#include <mainwindow.hxx>

// core
#include <QFileDialog>
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

	openFile = new QAction("&Open...", fileMenu);
	openFile->setShortcuts(QKeySequence::Open);
	connect(openFile, SIGNAL(triggered()), this, SLOT(openDialog()));
	fileMenu->addAction(openFile);

	saveFile = new QAction("Save &As...", fileMenu);
	saveFile->setShortcuts(QKeySequence::Save);
	connect(saveFile, SIGNAL(triggered()), this, SLOT(saveDialog()));
	fileMenu->addAction(saveFile);
}


void MainWindow::openDialog() {
	if (core != NULL) {
		core->setParent(NULL);
		delete core;
	}

	// TODO: Handle within ImagePanel so don't need extra creation/deletion
	core = new ImagePanel(QFileDialog::getOpenFileName(0, "", "", "Images (*.png)"));
	setCentralWidget(core);
}

void MainWindow::saveDialog() {
	// TODO: Handle extension via setDefaultSuffix()
	QString path = QFileDialog::getSaveFileName(0, "", "", "Images (*.png);;All files (*)");

	core->write(path.toStdString());
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	w.show();
	return a.exec();
}
