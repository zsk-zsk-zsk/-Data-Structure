#ifndef ARRAYWIDGET_H
#define ARRAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QMouseEvent>
#include <QShowEvent>

struct ArrayElement {
    QString value;
    bool isEmpty;
    bool isHighlighted;
    bool isAnimating;
    QPointF position;
    QPointF targetPosition;
    
    ArrayElement() : isEmpty(true), isHighlighted(false), isAnimating(false) {}
    ArrayElement(const QString &val) : value(val), isEmpty(false), isHighlighted(false), isAnimating(false) {}
};

class ArrayWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal animationProgress READ animationProgress WRITE setAnimationProgress)

signals:
    void elementClicked(int index);

public:
    explicit ArrayWidget(QWidget *parent = nullptr);
    
    void initialize(int size);
    void initialize(const QStringList &values);
    bool insertElement(int index, const QString &value);  // 在索引index之后插入（动态数组）
    bool deleteElement(int index);  // 删除元素，位置保留为空（固定数组）
    bool modifyElement(int index, const QString &value);
    int getSize() const { return arraySize; }  // 返回数组容量
    int getElementCount() const;  // 返回实际元素数量
    QStringList getValues() const;
    
    qreal animationProgress() const { return m_animationProgress; }
    void setAnimationProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void updateLayout();
    void animateElement(int index);
    
    QVector<ArrayElement> elements;
    int arraySize;  // 数组容量（固定大小）
    qreal m_animationProgress;
    QPropertyAnimation *animation;
    int animatingIndex;
    
    // 动态计算的尺寸
    int elementWidth;
    int elementHeight;
    int spacing;
    int elementsPerRow;  // 每行元素数量
    int totalRows;  // 总行数
    
    static const int MIN_ELEMENT_WIDTH = 50;
    static const int MAX_ELEMENT_WIDTH = 120;
    static const int MIN_ELEMENT_HEIGHT = 50;
    static const int MAX_ELEMENT_HEIGHT = 80;
    static const int MIN_SPACING = 5;
    static const int HORIZONTAL_MARGIN = 20;
    static const int VERTICAL_MARGIN = 20;
};

#endif // ARRAYWIDGET_H

