#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

class MainWindow: public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);
private slots:
    void digitPressed();
    void dotPressed();
    void backspacePressed();
    void clearPressed();
    void equalsPressed();
    void modeChanged(int);
    void toggleUKUS();
    void showHelp();
private:
    enum Mode{HbA1cMode,EAGMode};
    QString currentInput; Mode mode{EAGMode}; bool uk{false}; bool didCompute{false};
    QGridLayout *keypad;
    QPushButton *modeEAGBtn;
    QPushButton *modeA1cBtn;
    QButtonGroup *modeGroup;
    QCheckBox* ukBox;
    QLabel* promptLabel;
    QLabel* inputLabel;
    QLabel* resultLabel1;
    QLabel* resultValue1;
    QLabel* resultLabel2;
    QLabel* resultValue2;
    QWidget* displayCard;
    void updateDisplay();
    void updateResultLabels();
    double parseInput() const;
};

#endif
