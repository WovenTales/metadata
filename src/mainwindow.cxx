#include <mainwindow.hxx>

// core
#include <QFileDialog>
#include <QKeySequence>
#include <QString>
#include <QStringList>

// widgets
#include <QApplication>
#include <QDesktopWidget>
#include <QMenuBar>


MainWindow::MainWindow(const std::string& path) {
	core = new ImagePanel(path.c_str());
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

	quitAction = new QAction("Quit", fileMenu);
	quitAction->setShortcuts(QKeySequence::Quit);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(quitDialog()));
	fileMenu->addAction(quitAction);

	QDesktopWidget dw;
	QRect screenSize = dw.availableGeometry(dw.primaryScreen());
	setFixedSize(QSize(screenSize.width() * 0.7f, screenSize.height() * 0.7f));
}


void MainWindow::openDialog() {
	QStringList filters;
	filters << "PNG image files (*.png)"
	        << "JPEG image files (*.jpg *.jpeg *.jpe)";

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilters(filters);

	if (dialog.exec()) {
		// TODO: Handle within ImagePanel so don't need extra creation/deletion
		if (core != NULL) {
			core->setParent(NULL);
			delete core;
		}

		core = new ImagePanel(dialog.selectedFiles()[0]);
		setCentralWidget(core);
	}
}

void MainWindow::saveDialog() {
	// TODO: Handle extension via setDefaultSuffix()

	QStringList filters;
	filters << "PNG image files (*.png)"
	        << "JPEG image files (*.jpg *.jpeg *.jpe)"
	        << "Any files (*)";

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilters(filters);

	if (dialog.exec()) {
		core->write(dialog.selectedFiles()[0].toStdString());
	}
}

void MainWindow::quitDialog() {
	close();
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w((argc > 1 ? argv[1] : ""));

	w.show();
	return a.exec();
}
