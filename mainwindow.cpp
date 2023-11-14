#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dxwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    DxWindow *dxwindow = new DxWindow(this);
    setCentralWidget(dxwindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

