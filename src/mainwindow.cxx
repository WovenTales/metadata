#include <mainwindow.hxx>

#include <cmath>
#include <sstream>

// core
#include <QFileDialog>
#include <QKeySequence>
#include <QString>
#include <QStringList>

// widgets
#include <QApplication>
#include <QDesktopWidget>
#include <QMenuBar>


/*! Namespace documentation is included in \link include/mainwindow.hxx
 *  mainwindow.* \endlink as those are the "base" files for the project -- the
 *  header is the file to include to ensure the entire codebase is loaded, and
 *  the source includes the \p main() function.
 *
 *  \todo Find a better base for documentation and main(); likely going to be
 *        most relevant once the GUI/backend distinction is stronger
 *
 *  \todo Implement a deeper hierarchy of namespaces rather than it being flat
 */
namespace metadata {


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

	// TODO: Calculate minimum width so MetadataPanel is always completly visible
	QDesktopWidget dw;
	QRect screenSize = dw.availableGeometry(dw.primaryScreen());
	resize(std::max(width(), (int)(screenSize.width() * 0.7f)), (screenSize.height() * 0.7f));
}


void MainWindow::openDialog() {
	std::ostringstream ss;
	ss << "PNG image files (*.png);;";
	ss << "JPEG image files (*.jpg *.jpeg *.jpe)";

	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr(ss.str().c_str()));
	if (fileName.isEmpty() == false) {
		// TODO: Handle within ImagePanel so don't need extra creation/deletion
		if (core != NULL) {
			core->setParent(NULL);
			delete core;
		}

		core = new ImagePanel(fileName);
		setCentralWidget(core);
	}
}

void MainWindow::saveDialog() {
	// TODO: Handle extension via setDefaultSuffix()

	std::ostringstream ss;
	ss << "PNG image files (*.png);;";
	ss << "JPEG image files (*.jpg *.jpeg *.jpe);;";
	ss << "Any files (*)";

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr(ss.str().c_str()));
	if (fileName.isEmpty() == false) {
		core->write(fileName.toStdString());
	}
}

void MainWindow::quitDialog() {
	close();
}


}


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	metadata::MainWindow w((argc > 1 ? argv[1] : ""));

	w.show();
	return a.exec();
}
