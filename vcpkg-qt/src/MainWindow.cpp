#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
}

MainWindow::~MainWindow() {}

void MainWindow::on_button_clicked() {
    _ui->label->setText("Hello world !");
    _counter++;
    _ui->counter_label->setText(QString::number(_counter));
}

void MainWindow::on_reset_button_clicked() {
    _counter = 0;
    _ui->label->clear();
    _ui->counter_label->clear();
}