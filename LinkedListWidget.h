#ifndef LINKEDLISTWIDGET_H
#define LINKEDLISTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QMouseEvent>

struct ListNode {
    QString value;
    int index;
    bool isHighlighted;
    bool isAnimating;
    QPointF position;
    QPointF targetPosition;
    ListNode *next;
    
    ListNode() : index(-1), isHighlighted(false), isAnimating(false), next(nullptr) {}
};

class LinkedListWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal animationProgress READ animationProgress WRITE setAnimationProgress)

signals:
    void elementClicked(int index);

public:
    explicit LinkedListWidget(QWidget *parent = nullptr);
    ~LinkedListWidget();
    
    void initialize(int size);
    void initialize(const QStringList &values);
    void addElement(int index, const QString &value);
    bool deleteElement(int index);
    bool modifyElement(int index, const QString &value);
    int getSize() const { return listSize; }
    QStringList getValues() const;
    
    qreal animationProgress() const { return m_animationProgress; }
    void setAnimationProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void updateLayout();
    void animateNode(int index);
    void clearList();
    ListNode* getNodeAt(int index);
    
    ListNode *head;
    int listSize;
    qreal m_animationProgress;
    QPropertyAnimation *animation;
    int animatingIndex;
    
    // 动态计算的尺寸
    int nodeWidth;
    int nodeHeight;
    int arrowLength;
    int spacing;
    int nodesPerRow;  // 每行节点数量
    int totalRows;  // 总行数
    
    static const int MIN_NODE_WIDTH = 60;
    static const int MAX_NODE_WIDTH = 120;
    static const int MIN_NODE_HEIGHT = 50;
    static const int MAX_NODE_HEIGHT = 80;
    static const int MIN_ARROW_LENGTH = 30;
    static const int MAX_ARROW_LENGTH = 50;
    static const int MIN_SPACING = 10;
    static const int HORIZONTAL_MARGIN = 20;
    static const int VERTICAL_MARGIN = 20;
};

#endif // LINKEDLISTWIDGET_H

