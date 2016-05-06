#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <imagepanel.hxx>

// core
#include <QAction>

// widgets
#include <QMainWindow>
#include <QMenu>


//! Top-level namespace enclosing the entire project
namespace metadata {


class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	QMenu* fileMenu;

	// File
	QAction* openFile;
	QAction* saveFile;
	QAction* quitAction;

	ImagePanel* core = NULL;

public:
	MainWindow(const std::string& = "");

public slots:
	void openDialog();
	void saveDialog();
	void quitDialog();
};


}
#endif
