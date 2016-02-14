#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <imagepanel.hxx>

// core
#include <QAction>

// widgets
#include <QMainWindow>
#include <QMenu>


class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	QMenu* fileMenu;

	// File
	QAction* openFile;

	ImagePanel* core = NULL;

public:
	MainWindow();

public slots:
	void openDialog();
};


#endif
