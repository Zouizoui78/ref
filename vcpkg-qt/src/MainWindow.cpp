#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
}

MainWindow::~MainWindow() {}

void MainWindow::on_pushButton_clicked() {
    _ui->label->setText("Hello world !");
}