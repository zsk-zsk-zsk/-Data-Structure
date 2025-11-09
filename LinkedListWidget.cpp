#include "LinkedListWidget.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QMouseEvent>
#include <cmath>

LinkedListWidget::LinkedListWidget(QWidget *parent)
    : QWidget(parent)
    , head(nullptr)
    , listSize(0)
    , m_animationProgress(0.0)
    , animatingIndex(-1)
    , nodeWidth(100)
    , nodeHeight(60)
    , arrowLength(40)
    , spacing(20)
    , nodesPerRow(1)
    , totalRows(1)
{
    setMinimumHeight(200);
    animation = new QPropertyAnimation(this, "animationProgress", this);
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::OutCubic);
}

LinkedListWidget::~LinkedListWidget()
{
    clearList();
}

void LinkedListWidget::clearList()
{
    ListNode *current = head;
    while (current) {
        ListNode *next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    listSize = 0;
}

ListNode* LinkedListWidget::getNodeAt(int index)
{
    // 返回索引为index的实际节点（不包括头节点）
    // index从0开始，对应第一个实际节点
    if (index < 0 || !head) {
        return nullptr;
    }
    
    ListNode *current = head->next;  // 从第一个实际节点开始
    int count = 0;
    while (current && count < index) {
        current = current->next;
        count++;
    }
    return current;
}

void LinkedListWidget::initialize(int size)
{
    clearList();
    
    // 停止所有动画
    if (animation) {
        animation->stop();
    }
    
    if (size <= 0) {
        animatingIndex = -1;
        m_animationProgress = 0.0;
        update();
        return;
    }
    
    head = new ListNode();
    head->value = "头节点";
    head->index = -1;
    head->isHighlighted = false;  // 确保不高亮
    
    ListNode *current = head;
    for (int i = 0; i < size; ++i) {
        ListNode *newNode = new ListNode();
        newNode->value = QString("节点%1").arg(i);
        newNode->index = i;
        newNode->isHighlighted = false;  // 确保不高亮
        current->next = newNode;
        current = newNode;
    }
    
    listSize = size;
    animatingIndex = -1;  // 重置动画索引
    m_animationProgress = 0.0;  // 重置动画进度
    
    updateLayout();
    update();
}

void LinkedListWidget::initialize(const QStringList &values)
{
    clearList();
    
    // 停止所有动画
    if (animation) {
        animation->stop();
    }
    
    if (values.isEmpty()) {
        animatingIndex = -1;
        m_animationProgress = 0.0;
        update();
        return;
    }
    
    head = new ListNode();
    head->value = "头节点";
    head->index = -1;
    head->isHighlighted = false;  // 确保不高亮
    
    ListNode *current = head;
    for (int i = 0; i < values.size(); ++i) {
        ListNode *newNode = new ListNode();
        newNode->value = values[i];
        newNode->index = i;
        newNode->isHighlighted = false;  // 确保不高亮
        current->next = newNode;
        current = newNode;
    }
    
    listSize = values.size();
    animatingIndex = -1;  // 重置动画索引
    m_animationProgress = 0.0;  // 重置动画进度
    
    updateLayout();
    update();
}

QStringList LinkedListWidget::getValues() const
{
    QStringList result;
    ListNode *current = head;
    if (current) {
        current = current->next; // 跳过头节点
    }
    while (current) {
        result << current->value;
        current = current->next;
    }
    return result;
}

void LinkedListWidget::addElement(int index, const QString &value)
{
    // 支持在任意位置插入，包括在末尾之后
    // index = -1: 在开头插入
    // index >= 0: 在索引index之后插入（如果index >= listSize，则在末尾插入）
    
    if (index < -1) {
        return;
    }
    
    ListNode *newNode = new ListNode();
    newNode->value = value;
    newNode->isHighlighted = true;
    
    ListNode *prev = nullptr;
    int insertIndex = 0;
    
    if (index == -1) {
        // 在开头插入
        prev = head;
        insertIndex = 0;
    } else if (index >= listSize) {
        // 在末尾插入
        prev = getNodeAt(listSize - 1);
        if (!prev) {
            // 如果链表为空，使用head作为前驱
            prev = head;
        }
        insertIndex = listSize;
    } else {
        // 在索引index之后插入
        prev = getNodeAt(index);
        if (!prev) {
            delete newNode;
            return;
        }
        insertIndex = index + 1;
    }
    
    newNode->index = insertIndex;
    newNode->next = prev->next;
    prev->next = newNode;
    
    // 更新后续节点的索引
    ListNode *current = newNode->next;
    int count = insertIndex + 1;
    while (current) {
        current->index = count++;
        current = current->next;
    }
    
    listSize++;
    // 立即更新布局和界面
    updateLayout();
    update();
    
    animateNode(insertIndex);
    
    QTimer::singleShot(600, this, [this, insertIndex]() {
        ListNode *node = getNodeAt(insertIndex);
        if (node) {
            node->isHighlighted = false;
        }
        update();
    });
}

bool LinkedListWidget::deleteElement(int index)
{
    if (index < 0 || index >= listSize || !head || !head->next) {
        return false;
    }
    
    // 找到要删除节点的前驱节点
    ListNode *prev = nullptr;
    if (index == 0) {
        // 删除第一个节点，前驱是head
        prev = head;
    } else {
        // 删除其他节点，前驱是index-1的节点
        prev = getNodeAt(index - 1);
    }
    
    if (!prev || !prev->next) {
        return false;
    }
    
    ListNode *toDelete = prev->next;
    toDelete->isHighlighted = true;
    update();
    
    animateNode(index);
    
    QTimer::singleShot(500, this, [this, index, prev, toDelete]() {
        prev->next = toDelete->next;
        delete toDelete;
        
        // 更新后续节点的索引
        ListNode *current = prev->next;
        int count = index;
        while (current) {
            current->index = count++;
            current = current->next;
        }
        
        listSize--;
        updateLayout();
        update();
    });
    
    return true;
}

bool LinkedListWidget::modifyElement(int index, const QString &value)
{
    if (index < 0 || index >= listSize) {
        return false;
    }
    
    ListNode *node = getNodeAt(index);
    if (!node) {
        return false;
    }
    
    node->isHighlighted = true;
    update();
    
    animateNode(index);
    
    QTimer::singleShot(500, this, [this, index, value]() {
        ListNode *node = getNodeAt(index);
        if (node) {
            node->value = value;
            node->isHighlighted = false;
        }
        update();
    });
    
    return true;
}

void LinkedListWidget::setAnimationProgress(qreal progress)
{
    m_animationProgress = progress;
    if (animatingIndex >= 0) {
        update();
    }
}

void LinkedListWidget::animateNode(int index)
{
    animatingIndex = index;
    animation->stop();
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void LinkedListWidget::updateLayout()
{
    if (!head || listSize <= 0) {
        return;
    }
    
    // 如果组件还没有正确的尺寸，延迟更新
    if (width() <= 0 || height() <= 0) {
        return;
    }
    
    // 计算可用空间（不包括头节点）
    int availableWidth = width() - 2 * HORIZONTAL_MARGIN;
    int availableHeight = height() - 2 * VERTICAL_MARGIN;
    
    // 计算每行能放多少个节点（包括箭头）
    int bestNodesPerRow = 1;
    int bestNodeWidth = MIN_NODE_WIDTH;
    int bestNodeHeight = MIN_NODE_HEIGHT;
    int bestArrowLength = MIN_ARROW_LENGTH;
    int bestSpacing = MIN_SPACING;
    
    // 尝试找到最佳的布局配置
    for (int testWidth = MAX_NODE_WIDTH; testWidth >= MIN_NODE_WIDTH; testWidth -= 5) {
        for (int testArrow = MIN_ARROW_LENGTH; testArrow <= MAX_ARROW_LENGTH; testArrow += 5) {
            for (int testSpacing = MIN_SPACING; testSpacing <= 20; testSpacing += 2) {
                int nodeUnitWidth = testWidth + testArrow + testSpacing;
                int cols = (availableWidth + testSpacing) / nodeUnitWidth;
                if (cols < 1) cols = 1;
                
                int rows = (listSize + cols - 1) / cols;
                int testHeight = (availableHeight - (rows - 1) * 30) / rows;  // 行间距30（包括索引空间）
                
                if (testHeight >= MIN_NODE_HEIGHT && testHeight <= MAX_NODE_HEIGHT) {
                    if (cols * rows >= listSize) {
                        bestNodesPerRow = cols;
                        bestNodeWidth = testWidth;
                        bestNodeHeight = testHeight;
                        bestArrowLength = testArrow;
                        bestSpacing = testSpacing;
                        break;
                    }
                }
            }
            if (bestNodesPerRow > 1) break;
        }
        if (bestNodesPerRow > 1) break;
    }
    
    // 如果没找到合适的，使用最小尺寸
    if (bestNodesPerRow == 1) {
        bestNodeWidth = qMin(MAX_NODE_WIDTH, (availableWidth - MIN_ARROW_LENGTH - MIN_SPACING) / 2);
        bestNodeHeight = qMin(MAX_NODE_HEIGHT, availableHeight);
        bestArrowLength = MIN_ARROW_LENGTH;
        bestSpacing = MIN_SPACING;
        int nodeUnitWidth = bestNodeWidth + bestArrowLength + bestSpacing;
        bestNodesPerRow = (availableWidth + bestSpacing) / nodeUnitWidth;
        if (bestNodesPerRow < 1) bestNodesPerRow = 1;
    }
    
    // 更新成员变量
    nodeWidth = bestNodeWidth;
    nodeHeight = bestNodeHeight;
    arrowLength = bestArrowLength;
    spacing = bestSpacing;
    nodesPerRow = bestNodesPerRow;
    totalRows = (listSize + nodesPerRow - 1) / nodesPerRow;
    
    // 计算头节点位置（单独一行或第一行）
    int headX = HORIZONTAL_MARGIN;
    int headY = VERTICAL_MARGIN;
    
    // 计算实际节点区域的总宽度和总高度
    int nodeUnitWidth = nodeWidth + arrowLength + spacing;
    int totalNodeWidth = nodesPerRow * nodeUnitWidth - spacing;  // 最后一行不需要间距
    int totalNodeHeight = totalRows * nodeHeight + (totalRows - 1) * 30;  // 行间距30（包括索引空间）
    
    int startX = HORIZONTAL_MARGIN + (availableWidth - totalNodeWidth) / 2;
    int startY = headY + nodeHeight + 20;  // 头节点下方留20像素间距
    
    // 设置头节点位置
    head->targetPosition = QPointF(headX, headY);
    head->position = QPointF(headX, headY);
    
    // 计算每个实际节点的位置（支持换行）
    ListNode *current = head->next;
    int index = 0;
    
    while (current) {
        int row = index / nodesPerRow;
        int col = index % nodesPerRow;
        
        int x = startX + col * nodeUnitWidth;
        int y = startY + row * (nodeHeight + 30);  // 行间距30（包括索引空间）
        
        QPointF basePos(x, y);
        current->targetPosition = basePos;
        current->position = basePos;
        
        current = current->next;
        index++;
    }
}

void LinkedListWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLayout();
    update();  // 确保窗口大小变化时立即刷新
}

void LinkedListWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    painter.fillRect(rect(), QColor(240, 240, 240));
    
    if (!head) {
        return;
    }
    
    // 先绘制所有箭头（在节点下方，避免被节点遮挡）
    ListNode *current = head;
    int index = -1;
    
    // 绘制头节点到第一个节点的箭头
    if (head && head->next && listSize > 0) {
        QPointF headPos = head->targetPosition;
        ListNode *firstNode = head->next;
        QPointF firstPos = firstNode->targetPosition;
        
        // 计算箭头起点和终点
        QPointF arrowStart(headPos.x() + nodeWidth, headPos.y() + nodeHeight / 2);
        QPointF arrowEnd(firstPos.x() - arrowLength, firstPos.y() + nodeHeight / 2);
        
        // 如果第一个节点在下一行，绘制向下的箭头
        if (firstPos.y() > headPos.y() + nodeHeight) {
            arrowStart = QPointF(headPos.x() + nodeWidth / 2, headPos.y() + nodeHeight);
            arrowEnd = QPointF(firstPos.x() + nodeWidth / 2, firstPos.y());
        }
        
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawLine(arrowStart, arrowEnd);
        
        // 绘制箭头头部
        QPointF arrowHead = arrowEnd;
        QPolygonF arrowHeadPoly;
        if (firstPos.y() > headPos.y() + nodeHeight) {
            // 向下箭头
            arrowHeadPoly << arrowHead << QPointF(arrowHead.x() - 4, arrowHead.y() - 8)
                         << QPointF(arrowHead.x() + 4, arrowHead.y() - 8);
        } else {
            // 水平箭头
            arrowHeadPoly << arrowHead << QPointF(arrowHead.x() - 8, arrowHead.y() - 4)
                         << QPointF(arrowHead.x() - 8, arrowHead.y() + 4);
        }
        painter.setBrush(QColor(100, 100, 100));
        painter.drawPolygon(arrowHeadPoly);
    }
    
    // 绘制实际节点之间的箭头
    current = head->next;
    index = 0;
    while (current && current->next) {
        QPointF pos = current->targetPosition;
        int currentRow = index / nodesPerRow;
        int nextIndex = index + 1;
        int nextRow = nextIndex / nodesPerRow;
        
        // 如果下一个节点在同一行，绘制水平箭头
        if (currentRow == nextRow && nextIndex < listSize) {
            QPointF arrowStart(pos.x() + nodeWidth, pos.y() + nodeHeight / 2);
            QPointF arrowEnd(arrowStart.x() + arrowLength, arrowStart.y());
        
            painter.setPen(QPen(QColor(100, 100, 100), 2));
            painter.drawLine(arrowStart, arrowEnd);
            
            // 绘制箭头头部
            QPointF arrowHead = arrowEnd;
            QPolygonF arrowHeadPoly;
            arrowHeadPoly << arrowHead << QPointF(arrowHead.x() - 8, arrowHead.y() - 4)
                         << QPointF(arrowHead.x() - 8, arrowHead.y() + 4);
            painter.setBrush(QColor(100, 100, 100));
            painter.drawPolygon(arrowHeadPoly);
        }
        // 如果下一行，绘制向下的箭头
        else if (nextRow > currentRow && nextIndex < listSize) {
            QPointF arrowStart(pos.x() + nodeWidth / 2, pos.y() + nodeHeight);
            QPointF arrowEnd(arrowStart.x(), arrowStart.y() + 30);  // 行间距30
            
            painter.setPen(QPen(QColor(100, 100, 100), 2));
            painter.drawLine(arrowStart, arrowEnd);
            
            // 绘制向下的箭头头部
            QPointF arrowHead = arrowEnd;
            QPolygonF arrowHeadPoly;
            arrowHeadPoly << arrowHead << QPointF(arrowHead.x() - 4, arrowHead.y() - 8)
                         << QPointF(arrowHead.x() + 4, arrowHead.y() - 8);
            painter.setBrush(QColor(100, 100, 100));
            painter.drawPolygon(arrowHeadPoly);
        }
        
        current = current->next;
        index++;
    }
    
    // 再绘制所有节点（在箭头上方）
    current = head;
    index = -1;
    
    while (current) {
        QPointF pos = current->targetPosition;
        
        // 计算动画位置
        QPointF currentPos = pos;
        if (index == animatingIndex && animation->state() == QAbstractAnimation::Running) {
            qreal scale = 1.0 + 0.2 * sin(m_animationProgress * 3.14159);
            currentPos.setY(pos.y() - 10 * (1.0 - qAbs(m_animationProgress - 0.5) * 2));
        }
        
        // 绘制节点框（使用动态计算的尺寸）
        QRectF nodeRect(currentPos.x(), currentPos.y(), nodeWidth, nodeHeight);
        
        // 选择颜色
        QColor bgColor;
        QColor borderColor;
        
        if (index == -1) {
            // 头节点
            bgColor = QColor(255, 220, 200);
            borderColor = QColor(255, 150, 100);
        } else if (current->isHighlighted) {
            bgColor = QColor(100, 200, 255);
            borderColor = QColor(50, 150, 255);
        } else {
            bgColor = QColor(200, 230, 255);
            borderColor = QColor(100, 180, 255);
        }
        
        // 绘制节点
        QPen pen(borderColor, 2);
        painter.setPen(pen);
        painter.setBrush(bgColor);
        painter.drawRoundedRect(nodeRect, 5, 5);
        
        // 绘制值（根据节点大小调整字体）
        painter.setPen(QColor(0, 0, 0));
        // 根据节点大小动态调整字体
        int fontSize = qMax(8, qMin(12, nodeWidth / 8));
        QFont valueFont("Arial", fontSize, QFont::Bold);
        painter.setFont(valueFont);
        painter.drawText(nodeRect, Qt::AlignCenter, current->value);
        
        // 绘制索引（如果不是头节点，放在节点上方以避免被下一行遮挡）
        if (index >= 0) {
            QFont indexFont("Arial", 8);
            painter.setFont(indexFont);
            painter.setPen(QColor(100, 100, 100));
            // 将索引放在节点上方
            QRectF indexRect(currentPos.x(), currentPos.y() - 18, nodeWidth, 15);
            painter.drawText(indexRect, Qt::AlignCenter, QString("[%1]").arg(index));
        }
        
        current = current->next;
        index++;
    }
}

void LinkedListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF clickPos = event->pos();
        
        // 检查点击了哪个节点（跳过头节点，使用动态尺寸）
        ListNode *current = head;
        int index = -1;
        
        while (current) {
            int w = (index == -1) ? nodeWidth : nodeWidth;  // 头节点和普通节点使用相同宽度
            int h = (index == -1) ? nodeHeight : nodeHeight;
            QRectF nodeRect(current->targetPosition.x(), 
                          current->targetPosition.y(), 
                          w, h);
            
            if (nodeRect.contains(clickPos)) {
                // 如果不是头节点，发出信号
                if (index >= 0) {
                    emit elementClicked(index);
                }
                return;
            }
            
            current = current->next;
            index++;
        }
    }
    QWidget::mousePressEvent(event);
}

