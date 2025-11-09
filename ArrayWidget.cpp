#include "ArrayWidget.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QMouseEvent>
#include <cmath>

ArrayWidget::ArrayWidget(QWidget *parent)
    : QWidget(parent)
    , arraySize(0)
    , m_animationProgress(0.0)
    , animatingIndex(-1)
    , elementWidth(80)
    , elementHeight(60)
    , spacing(10)
    , elementsPerRow(1)
    , totalRows(1)
{
    setMinimumHeight(200);
    animation = new QPropertyAnimation(this, "animationProgress", this);
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::OutCubic);
}

void ArrayWidget::initialize(int size)
{
    if (size <= 0) size = 5;  // 默认大小
    if (size > 30) size = 30;  // 限制最大大小
    
    // 停止所有动画
    if (animation) {
        animation->stop();
    }
    
    elements.clear();
    elements.resize(size);
    
    // 初始化为空位置，确保所有状态都被清除
    for (int i = 0; i < size; ++i) {
        elements[i] = ArrayElement();
        elements[i].isHighlighted = false;
        elements[i].isAnimating = false;
    }
    
    arraySize = size;
    animatingIndex = -1;  // 重置动画索引
    m_animationProgress = 0.0;  // 重置动画进度
    
    updateLayout();
    update();
    // 强制刷新
    repaint();
}

void ArrayWidget::initialize(const QStringList &values)
{
    int size = values.size();
    if (size <= 0) size = 5;  // 默认大小
    if (size > 30) size = 30;  // 限制最大大小
    
    // 停止所有动画
    if (animation) {
        animation->stop();
    }
    
    elements.clear();
    elements.resize(size);
    
    // 初始化元素，确保所有状态都被清除
    for (int i = 0; i < size; ++i) {
        if (i < values.size()) {
            elements[i] = ArrayElement(values[i]);
        } else {
            elements[i] = ArrayElement();  // 空位置
        }
        elements[i].isHighlighted = false;  // 确保不高亮
        elements[i].isAnimating = false;
    }
    
    arraySize = size;
    animatingIndex = -1;  // 重置动画索引
    m_animationProgress = 0.0;  // 重置动画进度
    
    updateLayout();
    update();
    // 强制刷新
    repaint();
}

QStringList ArrayWidget::getValues() const
{
    QStringList result;
    for (int i = 0; i < elements.size(); ++i) {
        if (!elements[i].isEmpty) {
            result << elements[i].value;
        }
    }
    return result;
}

int ArrayWidget::getElementCount() const
{
    int count = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (!elements[i].isEmpty) {
            count++;
        }
    }
    return count;
}

bool ArrayWidget::insertElement(int index, const QString &value)
{
    // 数组插入：在索引index之后插入
    // index = -1 表示在开头插入
    // index >= 0 表示在索引index之后插入
    // 如果目标位置为空，直接填充；否则插入新位置
    
    if (index < -1) {
        return false;
    }
    
    int insertPos;
    bool fillEmpty = false;
    
    if (index == -1) {
        // 在开头插入
        insertPos = 0;
        // 检查位置0是否为空
        if (insertPos < arraySize && elements[insertPos].isEmpty) {
            fillEmpty = true;
        }
    } else if (index >= arraySize) {
        // 在末尾插入
        insertPos = arraySize;
    } else {
        // 在索引index之后插入
        insertPos = index + 1;
        // 检查目标位置是否为空
        if (insertPos < arraySize && elements[insertPos].isEmpty) {
            fillEmpty = true;
        }
    }
    
    if (fillEmpty) {
        // 直接填充空位置
        elements[insertPos].value = value;
        elements[insertPos].isEmpty = false;
        elements[insertPos].isHighlighted = true;
    } else {
        // 插入新位置（动态扩展）
        // 限制最大元素数量
        if (arraySize >= 30) {
            return false;
        }
        
        elements.insert(insertPos, ArrayElement(value));
        elements[insertPos].isHighlighted = true;
        arraySize = elements.size();  // 更新数组大小
    }
    
    // 立即更新布局和界面
    updateLayout();
    update();
    
    animateElement(insertPos);
    
    QTimer::singleShot(600, this, [this, insertPos]() {
        elements[insertPos].isHighlighted = false;
        update();
    });
    
    return true;
}

bool ArrayWidget::deleteElement(int index)
{
    // 数组删除：清空该位置，但位置保留
    if (index < 0 || index >= arraySize) {
        return false;
    }
    
    if (elements[index].isEmpty) {
        return false;  // 该位置已经为空
    }
    
    elements[index].isHighlighted = true;
    update();
    
    animateElement(index);
    
    QTimer::singleShot(500, this, [this, index]() {
        elements[index].isEmpty = true;
        elements[index].value = "";
        elements[index].isHighlighted = false;
        update();
    });
    
    return true;
}

bool ArrayWidget::modifyElement(int index, const QString &value)
{
    if (index < 0 || index >= arraySize) {
        return false;
    }
    
    if (elements[index].isEmpty) {
        return false;  // 该位置为空，不能修改
    }
    
    elements[index].isHighlighted = true;
    update();
    
    animateElement(index);
    
    QTimer::singleShot(500, this, [this, index, value]() {
        elements[index].value = value;
        elements[index].isHighlighted = false;
        update();
    });
    
    return true;
}

void ArrayWidget::setAnimationProgress(qreal progress)
{
    m_animationProgress = progress;
    // 动画过程中持续更新界面
    update();
}

void ArrayWidget::animateElement(int index)
{
    animatingIndex = index;
    animation->stop();
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void ArrayWidget::updateLayout()
{
    // 如果组件还没有正确的尺寸，延迟更新
    if (width() <= 0 || height() <= 0 || arraySize <= 0) {
        return;
    }
    
    // 计算可用空间
    int availableWidth = width() - 2 * HORIZONTAL_MARGIN;
    int availableHeight = height() - 2 * VERTICAL_MARGIN;
    
    // 计算每行能放多少个元素（尝试不同的元素大小）
    int bestElementsPerRow = 1;
    int bestElementWidth = MIN_ELEMENT_WIDTH;
    int bestElementHeight = MIN_ELEMENT_HEIGHT;
    int bestSpacing = MIN_SPACING;
    
    // 尝试找到最佳的布局配置
    for (int testWidth = MAX_ELEMENT_WIDTH; testWidth >= MIN_ELEMENT_WIDTH; testWidth -= 5) {
        for (int testSpacing = MIN_SPACING; testSpacing <= 15; testSpacing += 2) {
            int cols = (availableWidth + testSpacing) / (testWidth + testSpacing);
            if (cols < 1) cols = 1;
            
            int rows = (arraySize + cols - 1) / cols;  // 向上取整
            int testHeight = (availableHeight - (rows - 1) * 25) / rows;  // 减去行间距（包括索引空间）
            
            if (testHeight >= MIN_ELEMENT_HEIGHT && testHeight <= MAX_ELEMENT_HEIGHT) {
                if (cols * rows >= arraySize) {
                    bestElementsPerRow = cols;
                    bestElementWidth = testWidth;
                    bestElementHeight = testHeight;
                    bestSpacing = testSpacing;
                    break;
                }
            }
        }
        if (bestElementsPerRow > 1) break;
    }
    
    // 如果没找到合适的，使用最小尺寸
    if (bestElementsPerRow == 1) {
        bestElementWidth = qMin(MAX_ELEMENT_WIDTH, availableWidth - MIN_SPACING);
        bestElementHeight = qMin(MAX_ELEMENT_HEIGHT, availableHeight);
        bestSpacing = MIN_SPACING;
        bestElementsPerRow = (availableWidth + bestSpacing) / (bestElementWidth + bestSpacing);
        if (bestElementsPerRow < 1) bestElementsPerRow = 1;
    }
    
    // 更新成员变量
    elementWidth = bestElementWidth;
    elementHeight = bestElementHeight;
    spacing = bestSpacing;
    elementsPerRow = bestElementsPerRow;
    totalRows = (arraySize + elementsPerRow - 1) / elementsPerRow;
    
    // 计算总宽度和总高度（用于居中）
    int totalWidth = elementsPerRow * elementWidth + (elementsPerRow - 1) * spacing;
    int totalHeight = totalRows * elementHeight + (totalRows - 1) * 25;  // 行间距25（为索引留出空间）
    
    int startX = HORIZONTAL_MARGIN + (availableWidth - totalWidth) / 2;
    int startY = VERTICAL_MARGIN + (availableHeight - totalHeight) / 2;
    
    // 确保elements数组大小与arraySize一致
    if (elements.size() < arraySize) {
        elements.resize(arraySize);
    }
    
    // 计算每个元素的位置（支持换行）
    for (int i = 0; i < arraySize; ++i) {
        int row = i / elementsPerRow;
        int col = i % elementsPerRow;
        
        int x = startX + col * (elementWidth + spacing);
        int y = startY + row * (elementHeight + 25);  // 行间距25（为索引留出空间）
        
        QPointF basePos(x, y);
        elements[i].targetPosition = basePos;
        elements[i].position = basePos;
    }
}

void ArrayWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLayout();
    update();  // 确保窗口大小变化时立即刷新
}

void ArrayWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // 组件显示后，确保布局正确更新
    updateLayout();
    update();
}

void ArrayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    painter.fillRect(rect(), QColor(240, 240, 240));
    
    // 如果数组未初始化，不绘制任何内容
    if (arraySize <= 0) {
        return;
    }
    
    // 确保布局已更新
    if (elements.isEmpty() || elements[0].targetPosition.isNull()) {
        updateLayout();
    }
    
    // 先绘制所有元素，然后绘制索引（避免索引被遮挡）
    for (int i = 0; i < arraySize; ++i) {
        QPointF pos = elements[i].targetPosition;
        
        // 计算动画位置
        QPointF currentPos = pos;
        if (i == animatingIndex && animation->state() == QAbstractAnimation::Running) {
            qreal scale = 1.0 + 0.2 * sin(m_animationProgress * 3.14159);
            currentPos.setY(pos.y() - 10 * (1.0 - qAbs(m_animationProgress - 0.5) * 2));
        }
        
        // 绘制元素框（使用动态计算的尺寸）
        QRectF elementRect(currentPos.x(), currentPos.y(), elementWidth, elementHeight);
        
        // 选择颜色
        QColor bgColor;
        QColor borderColor;
        
        if (elements[i].isEmpty) {
            bgColor = QColor(255, 255, 255);
            borderColor = QColor(200, 200, 200);
        } else if (elements[i].isHighlighted) {
            bgColor = QColor(100, 200, 255);
            borderColor = QColor(50, 150, 255);
        } else {
            bgColor = QColor(200, 230, 255);
            borderColor = QColor(100, 180, 255);
        }
        
        // 绘制元素
        QPen pen(borderColor, 2);
        painter.setPen(pen);
        painter.setBrush(bgColor);
        painter.drawRoundedRect(elementRect, 5, 5);
        
        // 绘制值（根据元素大小调整字体）
        if (!elements[i].isEmpty) {
            painter.setPen(QColor(0, 0, 0));
            // 根据元素大小动态调整字体
            int fontSize = qMax(8, qMin(14, elementWidth / 6));
            QFont valueFont("Arial", fontSize, QFont::Bold);
            painter.setFont(valueFont);
            painter.drawText(elementRect, Qt::AlignCenter, elements[i].value);
        } else {
            painter.setPen(QColor(180, 180, 180));
            int fontSize = qMax(8, qMin(12, elementWidth / 7));
            QFont valueFont("Arial", fontSize);
            painter.setFont(valueFont);
            painter.drawText(elementRect, Qt::AlignCenter, "空");
        }
    }
    
    // 最后绘制索引标签（在元素上方，避免被下一行遮挡）
    QFont indexFont("Arial", 10);
    painter.setFont(indexFont);
    painter.setPen(QColor(100, 100, 100));
    
    for (int i = 0; i < arraySize; ++i) {
        QPointF pos = elements[i].targetPosition;
        
        // 计算动画位置（索引使用原始位置，不受动画影响）
        QPointF indexPos = pos;
        
        // 将索引放在元素上方，避免被下一行遮挡
        QRectF indexRect(indexPos.x(), indexPos.y() - 18, elementWidth, 15);
        if (indexRect.bottom() < height() && indexRect.top() >= 0) {
            painter.drawText(indexRect, Qt::AlignCenter, QString::number(i));
        }
    }
}

void ArrayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF clickPos = event->pos();
        
        // 检查点击了哪个元素（使用动态尺寸）
        for (int i = 0; i < arraySize; ++i) {
            QRectF elementRect(elements[i].targetPosition.x(), 
                              elements[i].targetPosition.y(), 
                              elementWidth, elementHeight);
            
            if (elementRect.contains(clickPos)) {
                // 只有非空位置才能点击
                if (!elements[i].isEmpty) {
                    emit elementClicked(i);
                }
                return;
            }
        }
    }
    QWidget::mousePressEvent(event);
}

