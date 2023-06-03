#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "QMainWindow"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    std::unique_ptr<Ui::MainWindow> _ui;
};

#endif // MAINWINDOW_HPP