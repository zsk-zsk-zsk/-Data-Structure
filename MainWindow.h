#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QRegExp>
#include <QStringList>
#include <QShowEvent>
#include "ArrayWidget.h"
#include "LinkedListWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInitClicked();
    void onInsertClicked();
    void onDeleteClicked();
    void onModifyClicked();
    void onTypeChanged(int index);
    void onArrayElementClicked(int index);
    void onLinkedListElementClicked(int index);
    void onInitTextChanged();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void setupUI();
    void setupConnections();
    QStringList parseInputString(const QString &input, bool &isValid);
    void updateIndexRange();
    
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // 控制面板
    QGroupBox *controlGroup;
    QHBoxLayout *controlLayout;
    QComboBox *typeComboBox;
    QPushButton *initButton;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QPushButton *modifyButton;
    
    // 输入面板
    QGroupBox *inputGroup;
    QVBoxLayout *inputMainLayout;
    QHBoxLayout *inputLayout;
    QHBoxLayout *initLayout;
    
    // 初始化输入
    QLabel *initLabel;
    QLineEdit *initEdit;
    QLabel *initHintLabel;
    
    // 操作输入
    QLabel *valueLabel;
    QLineEdit *valueEdit;
    QLabel *indexLabel;
    QSpinBox *indexSpinBox;
    
    // 可视化组件
    ArrayWidget *arrayWidget;
    LinkedListWidget *linkedListWidget;
    QWidget *currentWidget;
    
    int currentType; // 0: 数组, 1: 链表
};

#endif // MAINWINDOW_H

