#include "MainWindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentType(0)
{
    setWindowTitle("线性表可视化 - 数组与链表");
    setMinimumSize(1000, 700);
    resize(1000, 800);  // 设置初始大小
    
    setupUI();
    setupConnections();
    
    // 窗口居中显示（Qt 5.12兼容方式）
    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop) {
        QRect screenGeometry = desktop->screenGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    // 窗口显示后，确保布局正确更新
    if (currentWidget) {
        currentWidget->update();
        currentWidget->repaint();
    }
    
    // 处理事件队列，确保界面正确显示
    QApplication::processEvents();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);  // 设置边距
    mainLayout->setSpacing(10);  // 设置间距
    
    // 控制面板
    controlGroup = new QGroupBox("操作控制", this);
    controlLayout = new QHBoxLayout(controlGroup);
    
    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("数组 (Array)");
    typeComboBox->addItem("链表 (LinkedList)");
    
    initButton = new QPushButton("初始化", this);
    insertButton = new QPushButton("插入元素", this);
    deleteButton = new QPushButton("删除元素", this);
    modifyButton = new QPushButton("修改元素", this);
    
    controlLayout->addWidget(new QLabel("类型:", this));
    controlLayout->addWidget(typeComboBox);
    controlLayout->addWidget(initButton);
    controlLayout->addWidget(insertButton);
    controlLayout->addWidget(deleteButton);
    controlLayout->addWidget(modifyButton);
    controlLayout->addStretch();
    
    // 输入面板
    inputGroup = new QGroupBox("输入参数", this);
    inputMainLayout = new QVBoxLayout(inputGroup);
    
    // 初始化输入行
    initLayout = new QHBoxLayout();
    initLabel = new QLabel("初始化数据 (空格分隔):", this);
    initEdit = new QLineEdit(this);
    initEdit->setPlaceholderText("例如: 1 2 3 4 5");
    initEdit->setMinimumWidth(300);
    initHintLabel = new QLabel("", this);
    initHintLabel->setStyleSheet("color: green;");
    
    initLayout->addWidget(initLabel);
    initLayout->addWidget(initEdit);
    initLayout->addWidget(initHintLabel);
    initLayout->addStretch();
    
    // 操作输入行
    inputLayout = new QHBoxLayout();
    indexLabel = new QLabel("索引:", this);
    indexSpinBox = new QSpinBox(this);
    indexSpinBox->setMinimum(0);
    indexSpinBox->setMaximum(29);
    indexSpinBox->setValue(0);
    
    valueLabel = new QLabel("值:", this);
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("输入元素值");
    valueEdit->setMaximumWidth(1500);
    
    inputLayout->addWidget(indexLabel);
    inputLayout->addWidget(indexSpinBox);
    inputLayout->addWidget(valueLabel);
    inputLayout->addWidget(valueEdit);
    inputLayout->addStretch();
    
    inputMainLayout->addLayout(initLayout);
    inputMainLayout->addLayout(inputLayout);
    
    // 可视化组件
    arrayWidget = new ArrayWidget(this);

    linkedListWidget = new LinkedListWidget(this);
    
    currentWidget = arrayWidget;

    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(currentWidget, 1);
    
    // 确保初始状态正确显示
    currentWidget->show();
    currentWidget->update();
    // 强制刷新界面，清除所有高亮状态
    currentWidget->update();
    currentWidget->repaint();

    // 确保布局正确更新
    QApplication::processEvents();
}

void MainWindow::setupConnections()
{
    connect(typeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onTypeChanged(int)));
    connect(initButton, SIGNAL(clicked()), this, SLOT(onInitClicked()));
    connect(insertButton, SIGNAL(clicked()), this, SLOT(onInsertClicked()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    connect(modifyButton, SIGNAL(clicked()), this, SLOT(onModifyClicked()));
    connect(initEdit, SIGNAL(textChanged(QString)), this, SLOT(onInitTextChanged()));
    connect(arrayWidget, SIGNAL(elementClicked(int)), this, SLOT(onArrayElementClicked(int)));
    connect(linkedListWidget, SIGNAL(elementClicked(int)), this, SLOT(onLinkedListElementClicked(int)));
}

void MainWindow::onTypeChanged(int index)
{
    currentType = index;
    
    mainLayout->removeWidget(currentWidget);
    currentWidget->hide();
    
    if (index == 0) {
        currentWidget = arrayWidget;
    } else {
        currentWidget = linkedListWidget;
    }
    
    currentWidget->show();
    mainLayout->addWidget(currentWidget, 1);
    updateIndexRange();
    
    // 强制刷新界面，清除所有高亮状态
    currentWidget->update();
    currentWidget->repaint();
    
    // 确保布局正确更新
    QApplication::processEvents();
}

QStringList MainWindow::parseInputString(const QString &input, bool &isValid)
{
    QStringList result;
    isValid = true;
    
    if (input.trimmed().isEmpty()) {
        isValid = false;
        return result;
    }
    
    QStringList parts = input.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    // Qt 5.12兼容：如果上面的方法不行，使用下面的
    // QStringList parts = input.split(' ', QString::SkipEmptyParts);
    
    if (parts.isEmpty()) {
        isValid = false;
        return result;
    }
    
    // 检查每个部分是否都是有效的数字或字符串
    for (const QString &part : parts) {
        QString trimmed = part.trimmed();
        if (!trimmed.isEmpty()) {
            // 允许数字和字符串
            result << trimmed;
        }
    }
    
    if (result.size() > 20) {
        result = result.mid(0, 20); // 限制最大20个元素
    }
    
    isValid = result.size() > 0;
    return result;
}

void MainWindow::onInitTextChanged()
{
    QString text = initEdit->text();
    bool isValid = false;
    QStringList values = parseInputString(text, isValid);
    
    if (text.isEmpty()) {
        initHintLabel->setText("");
        initHintLabel->setStyleSheet("color: gray;");
    } else if (isValid) {
        initHintLabel->setText(QString("有效: %1 个元素").arg(values.size()));
        initHintLabel->setStyleSheet("color: green;");
    } else {
        initHintLabel->setText("输入格式错误");
        initHintLabel->setStyleSheet("color: red;");
    }
}

void MainWindow::onInitClicked()
{
    QString input = initEdit->text();
    bool isValid = false;
    QStringList values = parseInputString(input, isValid);
    
    if (!isValid || values.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入有效的初始化数据！\n格式：用空格分隔的数字或字符串，例如：1 2 3 4 5");
        return;
    }
    
    if (currentType == 0) {
        arrayWidget->initialize(values);
    } else {
        linkedListWidget->initialize(values);
    }
    
    // 强制刷新当前显示的组件
    if (currentType == 0) {
        arrayWidget->update();
        arrayWidget->repaint();
    } else {
        linkedListWidget->update();
        linkedListWidget->repaint();
    }
    
    updateIndexRange();
    initEdit->clear();
    initHintLabel->setText("");
}

void MainWindow::updateIndexRange()
{
    int maxIndex = 0;
    if (currentType == 0) {
        // 数组：索引范围是0到size-1
        maxIndex = arrayWidget->getSize() - 1;
        if (maxIndex < 0) maxIndex = 0;
    } else {
        // 链表：可以在-1到size之间插入（-1表示开头，0到size-1表示在对应索引之后，size表示末尾）
        maxIndex = linkedListWidget->getSize();
        if (maxIndex < -1) maxIndex = -1;
    }
    
    indexSpinBox->setMaximum(maxIndex);
    if (indexSpinBox->value() > maxIndex) {
        indexSpinBox->setValue(currentType == 0 ? 0 : -1);
    }
    
    // 更新标签文本
    if (currentType == 0) {
        indexLabel->setText("索引:");
        indexSpinBox->setMinimum(0);
        indexSpinBox->setSpecialValueText("");
    } else {
        indexLabel->setText("索引 (在之后插入):");
        indexSpinBox->setMinimum(-1);
        indexSpinBox->setSpecialValueText("开头");
    }
}

void MainWindow::onInsertClicked()
{
    QString value = valueEdit->text();
    if (value.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入元素值！");
        return;
    }
    
    int index = indexSpinBox->value();
    
    if (currentType == 0) {
        // 数组：在索引index之后插入（动态扩展）
        int insertIndex = index >= arrayWidget->getSize() ? -1 : index;
        if (!arrayWidget->insertElement(insertIndex, value)) {
            QMessageBox::warning(this, "警告", "插入失败！数组元素数量已达上限(30个)。");
        } else {
            updateIndexRange();
        }
    } else {
        // 链表：在索引index之后插入（index=-1表示开头，index>=0表示在index之后）
        linkedListWidget->addElement(index, value);
        updateIndexRange();
    }
    
    valueEdit->clear();
}

void MainWindow::onDeleteClicked()
{
    int index = indexSpinBox->value();
    
    if (currentType == 0) {
        if (!arrayWidget->deleteElement(index)) {
            QMessageBox::warning(this, "警告", "删除失败！索引超出范围。");
        } else {
            updateIndexRange();
        }
    } else {
        if (!linkedListWidget->deleteElement(index)) {
            QMessageBox::warning(this, "警告", "删除失败！索引超出范围。");
        } else {
            updateIndexRange();
        }
    }
}

void MainWindow::onArrayElementClicked(int index)
{
    // 点击元素时，自动选中该索引并高亮显示
    indexSpinBox->setValue(index);
    
    // 询问是否删除
    int ret = QMessageBox::question(this, "操作选择", 
                                     QString("您点击了索引 %1 的元素\n\n选择操作：").arg(index),
                                     "删除", "修改", "取消", 0, 2);
    
    if (ret == 0) {
        // 删除
        if (arrayWidget->deleteElement(index)) {
            updateIndexRange();
        } else {
            QMessageBox::warning(this, "警告", "删除失败！");
        }
    } else if (ret == 1) {
        // 修改
        QStringList values = arrayWidget->getValues();
        if (index < values.size()) {
            valueEdit->setText(values[index]);
            valueEdit->setFocus();
        }
    }
}

void MainWindow::onLinkedListElementClicked(int index)
{
    // 点击元素时，自动选中该索引并高亮显示
    indexSpinBox->setValue(index);
    
    // 询问是否删除
    int ret = QMessageBox::question(this, "操作选择", 
                                     QString("您点击了索引 %1 的元素\n\n选择操作：").arg(index),
                                     "删除", "修改", "取消", 0, 2);
    
    if (ret == 0) {
        // 删除
        if (linkedListWidget->deleteElement(index)) {
            updateIndexRange();
        } else {
            QMessageBox::warning(this, "警告", "删除失败！");
        }
    } else if (ret == 1) {
        // 修改
        QStringList values = linkedListWidget->getValues();
        if (index < values.size()) {
            valueEdit->setText(values[index]);
            valueEdit->setFocus();
        }
    }
}

void MainWindow::onModifyClicked()
{
    QString value = valueEdit->text();
    if (value.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入新值！");
        return;
    }
    
    int index = indexSpinBox->value();
    
    if (currentType == 0) {
        if (!arrayWidget->modifyElement(index, value)) {
            QMessageBox::warning(this, "警告", "修改失败！索引超出范围。");
        }
    } else {
        if (!linkedListWidget->modifyElement(index, value)) {
            QMessageBox::warning(this, "警告", "修改失败！索引超出范围。");
        }
    }
    
    valueEdit->clear();
}

