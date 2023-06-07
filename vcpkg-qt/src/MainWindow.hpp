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
    virtual ~MainWindow();

private slots:
    void on_button_clicked();
    void on_reset_button_clicked();

private:
    std::unique_ptr<Ui::MainWindow> _ui;
    uint32_t _counter = 0;
};

#endif // MAINWINDOW_HPP