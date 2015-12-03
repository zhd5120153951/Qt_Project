#include "GLDCustomCommentFrame.h"
#include "GLDMathUtils.h"
#include "GLDGlobal.h"
#include "GLDAbstractItemView.h"
#include "GLDAbstractItemModel.h"

#define CommentFrameOffset 5
#define CommentFrameArrowInitLineLen 100

/*GCustomCommentFrame*/
GCustomCommentFrame::GCustomCommentFrame(QWidget *parent, QPoint pt) : QFrame(parent, Qt::Widget),
    m_nMarginWidth(10),
    m_nWidth(160),
    m_nHeight(80),
    m_state(m_EnNormal),
    m_angle(0),
    m_isShowCommentPersistent(false)
{
    setAttribute(Qt::WA_WState_Created);
    m_parent = parent;
    m_topLeft = pt;
    m_bottomRight.setX(pt.rx() + CommentFrameArrowInitLineLen);
    m_bottomRight.setY(pt.ry());
    m_nDefaultWidth = 160;
    m_nDefaultHeight = 80;
    m_isShowEllopse = true;
    m_starDraw = false;
    // 参考Excel中的像素为6，但是6像素的时候存在显示很诡异的情况，因此y方向改为8像素
    m_arrowPoints.append(QPoint(0, 0));
    m_arrowPoints.append(QPoint(6, -4));
    m_arrowPoints.append(QPoint(6, 4));

    initPlaintEdit();

    setInitStyle();
    setCommentFrameSize();
    setMouseTracking(true);
    connect(m_plainTextEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
}

GCustomCommentFrame::~GCustomCommentFrame()
{
    freeAndNil(m_plainTextEdit);
}

void GCustomCommentFrame::setFramePosition(QPoint point)
{
    if (m_topLeft != point)
    {
        m_topLeft = point;
        resetFramePosition();
    }
}

/**
 * @brief 初始化编辑框的大小
 */
void GCustomCommentFrame::initPlaintEdit()
{
    m_plainTextEdit = new GLDPlainTextEdit();
    m_plainTextEdit->installEventFilter(this);
    m_plainTextEdit->viewport()->installEventFilter(this);
    m_plainTextEdit->setParent(this);
    m_plainTextEdit->setFrameShape(QFrame::Panel);
    m_plainTextEdit->setFrameShadow(QFrame::Plain);
    m_plainTextEdit->setLineWidth(1);
    m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_plainTextEdit->setGeometry(m_bottomRight.rx(), m_bottomRight.ry(), m_nWidth, m_nHeight);

    // 设置背景Frame和Label的背景色
    QPalette palet = palette();
    palet.setColor(QPalette::Window, QColor(255, 255, 225));
    m_plainTextEdit->setAutoFillBackground(true);
    m_plainTextEdit->setPalette(palet);
    m_plainTextEdit->viewport()->setBackgroundRole(QPalette::Window);
    m_plainTextEdit->setMinimumSize(m_nWidth - m_nMarginWidth, m_nHeight - m_nMarginWidth);
    resetBordrect();
}

/**
 * @brief 设置背景Frame和Label的背景色
 */
void GCustomCommentFrame::setInitStyle()
{
    QPalette palet = palette();
    palet.setColor (QPalette::Background, Qt::color0);
    palet.setColor (QPalette::Foreground, Qt::color1);
    setAutoFillBackground(false);
    setPalette(palet);
}

/**
 * @brief 设置frame的大小，按照父窗口的大小来创建。
 * @param parent
 */
void GCustomCommentFrame::setCommentFrameSize()
{
    setFixedSize(m_parent->width(), m_parent->height());
}

void GCustomCommentFrame::setCommentFrameMask()
{
    QPolygon arrowPolygon(m_arrowPoints);
    QTransform transform;
    transform.rotate(m_angle);
    arrowPolygon = transform.map(arrowPolygon);
    arrowPolygon.translate(m_topLeft);

    QRegion region(arrowPolygon);

    QVector<QPoint> pointLine;
    pointLine.append(QPoint(m_topLeft.x() + 2, m_topLeft.y() - 2));
    pointLine.append(QPoint(m_topLeft.x() - 2, m_topLeft.y() + 2));
    pointLine.append(QPoint(m_bottomRight.x() - 2, m_bottomRight.y() + 2));
    pointLine.append(QPoint(m_bottomRight.x() + 2, m_bottomRight.y() - 2));

    QPolygon polygonLine(pointLine);
    QRegion regionLine(polygonLine);
    region += regionLine;

    region += m_borderRect.adjusted(-10, -10, 10, 10);
    setMask(region);
}

/**
 * @brief 设置edittext的位置
 */
void GCustomCommentFrame::setEditTextPos()
{
    m_plainTextEdit->setGeometry(m_borderRect.left(), m_borderRect.top(), m_nWidth, m_nHeight);
}

/**
 * @brief 根据不同的情况绘制界面
 * @param event
 */
void GCustomCommentFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (m_isShowEllopse)
    {
        // 2画编辑状态
        repainAdjustRect(&painter);
    }
    if (m_starDraw)
    {
        // 3画移动状态
        repainArrowMoving(&painter);
    }
    repainLineAndArrow(&painter);
    QFrame::paintEvent(event);
}

/**
 * @brief 当编辑框小于最小值的时候，里面的内容将看不到，所以要返回移动前的编辑框
 */
void GCustomCommentFrame::resetBorderRect()
{
    if (Abs(m_borderRect.left() - m_borderRect.right()) < (m_nDefaultWidth - m_nMarginWidth))
    {
        resetBordrect();
    }
    if (Abs(m_borderRect.top() - m_borderRect.bottom()) < (m_nDefaultHeight - m_nMarginWidth))
    {
        resetBordrect();
    }
}

void GCustomCommentFrame::resetBordrect()
{
    m_borderRect.setCoords(m_plainTextEdit->geometry().left(),m_plainTextEdit->geometry().top(),
                           m_plainTextEdit->geometry().right(),m_plainTextEdit->geometry().bottom());
}

void GCustomCommentFrame::hide()
{
    m_isShowCommentPersistent = false;
    if (!isVisible())
        return;
    QFrame::hide();
    GlodonAbstractItemView *pParent = dynamic_cast<GlodonAbstractItemView *>(m_parent->parent());
    if (pParent)
    {
        pParent->model()->setData(m_curIndex, m_plainTextEdit->toPlainText(), gidrCommentRole);
    }
}

void GCustomCommentFrame::show(bool isPersistent)
{
    m_bottomRight.setX(m_topLeft.rx() + CommentFrameArrowInitLineLen);
    m_bottomRight.setY(m_topLeft.ry());
    m_borderRect.moveTo(m_bottomRight);
    resetFramePosition();

    m_isShowCommentPersistent = isPersistent;
    QFrame::show();
    QApplication::setActiveWindow(this);
    m_plainTextEdit->setFocus();
    QTextCursor oTextCursor = m_plainTextEdit->textCursor();
    oTextCursor.movePosition(QTextCursor::End);
    m_plainTextEdit->setTextCursor(oTextCursor);
}

void GCustomCommentFrame::move(int x, int y)
{
    move(QPoint(x, y));
}

void GCustomCommentFrame::move(const QPoint &pt)
{
    QFrame::move(pt);
}

void GCustomCommentFrame::move(QModelIndex index)
{
    GlodonAbstractItemView *pParent = dynamic_cast<GlodonAbstractItemView *>(m_parent->parent());
    if (pParent)
    {
        move(pParent->visualRect(index).topRight() + pParent->viewport()->pos());
        m_curIndex = index;
    }
}

void GCustomCommentFrame::moveTo(QPoint point)
{
    QPoint movePoint = point - m_topLeft;
    move(movePoint);
}

void GCustomCommentFrame::setCommentText(const QString &str)
{
    m_plainTextEdit->setPlainText(str);
}

QString GCustomCommentFrame::commentText()
{
    return m_plainTextEdit->toPlainText();
}

void  GCustomCommentFrame::repainArrowMoving(QPainter *painter)
{
    painter->save();
    QColor color = Qt::black;
    QPen pen(color, 1, Qt::DotLine);
    painter->setPen(pen);
    painter->drawRect(m_borderRect);
    painter->restore();
}

void GCustomCommentFrame::repainLineAndArrow(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QColor color = Qt::black;
    QPen pen(color, 1, Qt::SolidLine);
    painter->setPen(pen);
    painter->drawLine(m_topLeft, m_bottomRight);
    repainArrow(painter);
    painter->restore();
}

void GCustomCommentFrame::repainArrow(QPainter *painter)
{
    painter->save();
    QColor color = Qt::black;
    QPen pen(color, 1, Qt::SolidLine);
    painter->setPen(pen);

    //  先画出三脚，然后根据三角进行旋转。
    // 计算角度
    QBrush brush_(color);
    painter->setBrush(brush_);
    // 旋转角度
    QPolygon arrowPolygon(m_arrowPoints);
    QTransform transform;
    transform.rotate(m_angle);
    arrowPolygon = transform.map(arrowPolygon);
    arrowPolygon.translate(m_topLeft);
    painter->setClipRegion(QRegion(arrowPolygon));
//    qDebug() << "repaintArrow;PolygonF" << QRegion(arrowPolygon);

    painter->drawPolygon(arrowPolygon);
    painter->restore();
}

// 画八个提示鼠标移动的小矩形
void GCustomCommentFrame::repainAdjustRect(QPainter *painter)
{
    painter->save();
    QPen pen(Qt::gray, 1, Qt::SolidLine);
    painter->setPen(pen);
    // 防止小框边线与编辑框边线重叠，让出1像素
    int nLeft = m_plainTextEdit->geometry().left() - 1;
    int nTop = m_plainTextEdit->geometry().top() - 1;
    int nRight = m_plainTextEdit->geometry().right() + 1;
    int nBottom = m_plainTextEdit->geometry().bottom() + 1;
    painter->drawRect(nLeft - c_AdjustWith, nTop - c_AdjustWith, c_AdjustWith, c_AdjustWith);
    painter->drawRect(nLeft - c_AdjustWith, nBottom, c_AdjustWith, c_AdjustWith);
    painter->drawRect(nRight, nTop - c_AdjustWith, c_AdjustWith, c_AdjustWith);
    painter->drawRect(nRight, nBottom, c_AdjustWith, c_AdjustWith);
    painter->drawRect(nLeft - c_AdjustWith, (nTop - c_AdjustWith + nBottom) / 2, c_AdjustWith, c_AdjustWith); // nLeft
    painter->drawRect((nLeft - c_AdjustWith + nRight) / 2, nTop - c_AdjustWith, c_AdjustWith, c_AdjustWith);  // nTop
    painter->drawRect(nRight, (nTop - c_AdjustWith + nBottom) / 2, c_AdjustWith, c_AdjustWith);   //nRight
    painter->drawRect((nLeft + nRight) / 2, nBottom, c_AdjustWith, c_AdjustWith);  // nBottom
    painter->restore();
}

/**
 * TODO：这个可以根据需要抽成通用函数
 * 根据反正切计算角度。
 * @brief myGArrowFrame::cacultAngle
 * @return
 */
double GCustomCommentFrame::cacultAngle()
{
    if (Abs(m_topLeft.ry() - m_bottomRight.ry()) < 0.00000001)
    {
        if (m_topLeft.rx() > m_bottomRight.rx())
        {
            return 180.0;
        }
        else
        {
            return 0.0;
        }
    }

    if (Abs(m_topLeft.rx() - m_bottomRight.rx()) < 0.00000001)
    {
        if (m_topLeft.ry() > m_bottomRight.ry())
        {
            return 270.0;
        }
        else
        {
            return 90.0;
        }
    }
    // arctan这个获取的是弧度
    double dRadian =  arctan((double)(m_topLeft.ry() - m_bottomRight.ry())
                            / (double)(m_topLeft.rx() - m_bottomRight.rx()));
    //    qDebug() << "dRadian is " << dRadian;
    // 将弧度转换到角度
    double dAngle = radianToAngle(dRadian);
    // qDebug() << "dRadian is " << dAngle
    //         << "Abs(m_topLeft.rx() - m_bottomRight.rx()) " << Abs(m_topLeft.rx() - m_bottomRight.rx());
    // arctan 的取值范围是[-PI/2, PI/2],所以需要补足
    if (m_bottomRight.rx() - m_topLeft.rx() < 0.00000001)
    {
        dAngle += 180.0;
    }
    return dAngle;
}

void GCustomCommentFrame::mousePressEvent(QMouseEvent *e)
{
    m_mousePos = e->globalPos();
    if (m_borderRect.contains(e->pos()))
    {
        m_isShowEllopse = true;
    }
    else
    {
        if (m_state == m_EnNormal)
        {
            m_isShowEllopse = false;
        }
    }
    if (m_state != m_EnNormal)
    {
        m_isShowEllopse = true;
        m_starDraw = true;
    }
    this->repaint();
}

void GCustomCommentFrame::onMouseMoveEvent(QMouseEvent *e)
{
    int nPosX = e->pos().rx();
    int nPosY = e->pos().ry();

    int commentFrameLeft;
    int commentFrameBottom;
    int commentFrameTop;
    int commentFrameRight;

    QRect rect = m_parent->rect();
    commentFrameLeft = rect.left();
    commentFrameBottom = rect.bottom();
    commentFrameTop = rect.top();
    commentFrameRight = rect.right();

    if (m_state == m_EnMoving)
    {
        int nMoveX = e->globalPos().rx() - m_mousePos.rx();
        int nMoveY = e->globalPos().ry() - m_mousePos.ry();

        m_borderRect.setCoords(m_borderRect.left() + nMoveX, m_borderRect.top() + nMoveY,
                               m_borderRect.right() + nMoveX, m_borderRect.bottom() + nMoveY);

        QRect rect = m_parent->rect();

        // 横向
        if (m_borderRect.left() <= rect.left())
        {
            m_borderRect.setLeft(rect.left());
            m_borderRect.setRight(rect.left() + m_nWidth);
        }

        if (m_borderRect.right() >= rect.right())
        {
            m_borderRect.setRight(rect.right());
            m_borderRect.setLeft(m_borderRect.right() - m_nWidth);
        }

        // 纵向
        if (m_borderRect.top() <= rect.top())
        {
            m_borderRect.setTop(rect.top());
            m_borderRect.setBottom(m_borderRect.top() + m_nHeight);
        }

        if (m_borderRect.bottom() >= rect.bottom())
        {
            m_borderRect.setBottom(rect.bottom());
            m_borderRect.setTop(m_borderRect.bottom() - m_nHeight);
        }

        m_borderRect.setCoords(m_borderRect.left() + nMoveX, m_borderRect.top() + nMoveY,
                               m_borderRect.right() + nMoveX, m_borderRect.bottom() + nMoveY);
        m_mousePos = e->globalPos();
        clearMask();
    }
    else if (m_state == m_EnUP)
    {
        if (nPosY > commentFrameTop)
        {
            m_borderRect.setCoords(m_borderRect.left(), nPosY, m_borderRect.right(), m_borderRect.bottom());
        }
        else
        {
            m_borderRect.setCoords(m_borderRect.left(), commentFrameTop + 2, m_borderRect.right(), m_borderRect.bottom());
        }
    }
    else if (m_state == m_EnDown)
    {
        if (nPosY < commentFrameBottom)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(), m_borderRect.right(), nPosY);
        }
        else
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(), m_borderRect.right(), commentFrameBottom - 2);
        }
    }
    else if (m_state == m_EnLeft)
    {
        if (nPosX > commentFrameLeft)
        {
            m_borderRect.setCoords(nPosX, m_borderRect.top(), m_borderRect.right(), m_borderRect.bottom());
        }
        else
        {
            m_borderRect.setCoords(commentFrameLeft + 2, m_borderRect.top(), m_borderRect.right(), m_borderRect.bottom());
        }
    }
    else if (m_state == m_EnRiht)
    {
        if (nPosX < commentFrameRight)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(),nPosX , m_borderRect.bottom());
        }
        else
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(),commentFrameRight - 2 , m_borderRect.bottom());
        }
    }
    else if (m_state == m_EnLeftUp)
    {
        if (nPosY > commentFrameTop && nPosX > commentFrameLeft)
        {
            m_borderRect.setCoords(nPosX, nPosY, m_borderRect.right(), m_borderRect.bottom());
        }
        else if (nPosY <= commentFrameTop)
        {
            m_borderRect.setCoords(m_borderRect.left(), commentFrameTop + 2, m_borderRect.right(), m_borderRect.bottom());
        }
        else if (nPosX <= commentFrameLeft)
        {
            m_borderRect.setCoords(commentFrameLeft + 2, m_borderRect.top(), m_borderRect.right(), m_borderRect.bottom());
        }
    }
    else if (m_state == m_EnbottomLeft)
    {
        if (nPosX > commentFrameLeft && nPosY < commentFrameBottom)
        {
            m_borderRect.setCoords(nPosX, m_borderRect.top(), m_borderRect.right(), nPosY);
        }
        else if (nPosX <= commentFrameLeft)
        {
            m_borderRect.setCoords(commentFrameLeft + 2, m_borderRect.top(), m_borderRect.right(), m_borderRect.bottom());
        }
        else if (nPosY >= commentFrameBottom)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(), m_borderRect.right(), commentFrameBottom - 2);
        }
    }
    else if (m_state == m_EnRightUp)
    {
        if (nPosY > commentFrameTop && nPosX < commentFrameRight)
        {
            m_borderRect.setCoords(m_borderRect.left(), nPosY, nPosX, m_borderRect.bottom());
        }
        else if (nPosY <= commentFrameTop)
        {
            m_borderRect.setCoords(m_borderRect.left(), commentFrameTop + 2, m_borderRect.right(), m_borderRect.bottom());
        }
        else if (nPosX >= commentFrameRight)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(),commentFrameRight - 2 , m_borderRect.bottom());
        }
    }
    else if (m_state == m_EnRightDown)
    {
        if (nPosY < commentFrameBottom && nPosX < commentFrameRight)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(), nPosX, nPosY);
        }
        else if (nPosY >= commentFrameBottom)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(), m_borderRect.right(), commentFrameBottom - 2);
        }
        else if (nPosX >= commentFrameRight)
        {
            m_borderRect.setCoords(m_borderRect.left(), m_borderRect.top(),commentFrameRight - 2 , m_borderRect.bottom());
        }
    }
    else
    {
        QFrame::mouseMoveEvent(e);
    }
}

void GCustomCommentFrame::mouseMoveEvent(QMouseEvent *e)
{
    // 在选择编辑状态下，需要绘制箭头样式
    if (m_isShowEllopse)
    {
        inMoveRange(e->pos());
    }

    if (!m_starDraw)
    {
       QFrame::mouseMoveEvent(e);
       return;
    }

    onMouseMoveEvent(e);

    this->update();
}

void GCustomCommentFrame::mouseReleaseEvent(QMouseEvent *e)
{
    // 1画非编辑状态。
    if (m_state != m_EnNormal)
    {
        m_state = m_EnNormal;

        resetBorderRect();
        // 获取距离框架x距离最近的点作为x
        int nPointX = Abs(m_borderRect.left() - m_topLeft.rx()) < Abs(m_borderRect.right() - m_topLeft.rx()) ?
                    m_borderRect.left() : m_borderRect.right();
        // 获取距离框架y距离最近的点作为y
        int nPointY = Abs(m_borderRect.top() - m_topLeft.ry()) < Abs(m_borderRect.bottom() - m_topLeft.ry()) ?
                    m_borderRect.top() : m_borderRect.bottom();
        m_bottomRight.setX(nPointX);
        m_bottomRight.setY(nPointY);
        m_nWidth = Abs(m_borderRect.left() - m_borderRect.right());
        m_nHeight = Abs(m_borderRect.top() - m_borderRect.bottom());
        m_starDraw = false;
        setCursor(Qt::ArrowCursor);
        resetFramePosition();
    }

    QFrame::mouseReleaseEvent(e);
}

bool GCustomCommentFrame::eventFilter(QObject *object, QEvent *event)
{
    QWidget *pWidget = dynamic_cast<QWidget*>(object);
    if (NULL == pWidget)
    {
        return false;
    }
    else
    {
        QAbstractScrollArea *pScrollArea = dynamic_cast<QAbstractScrollArea *>(pWidget);
        GLDPlainTextEdit *pPlainTextEdit = dynamic_cast<GLDPlainTextEdit*>(pWidget);
        if (NULL != pScrollArea || NULL != pPlainTextEdit)
        {
            if (event->type() == QEvent::FocusIn)
            {
                m_isShowEllopse = true;
            }
            else if ((event->type() == QEvent::FocusOut && !containPos(m_parent->mapFromGlobal(QCursor::pos()), m_borderRect.adjusted(-10, -10, 10, 10)))
                     && m_isShowEllopse == true
                     && m_starDraw == false)
            {
                emit editFocusOut();
                if (!m_isShowCommentPersistent)
                {
                    hide();
                }
                m_isShowEllopse = false;
            }
        }
    }
    return QFrame::eventFilter(object, event);
}

void GCustomCommentFrame::showEvent(QShowEvent *)
{
    setCommentFrameMask();
}

void GCustomCommentFrame::resizeEvent(QResizeEvent *)
{
    setCommentFrameMask();
}

/**
 * @brief 鼠标已经移除了当前窗体，不在做动作。
 * @param pos
 * @return
 */
bool GCustomCommentFrame::isPosInFrame(QPoint pos)
{
    if ((pos.rx() < 0) || (pos.ry() < 0))
    {
        return false;
    }
    if (pos.rx() > (m_parent->width() - c_AdjustWith))
    {
        return false;
    }
    if (pos.ry() > (m_parent->height() - c_AdjustWith))
    {
        return false;
    }
    return true;
}

void GCustomCommentFrame::resetFramePosition()
{
    m_angle = cacultAngle();
    clearMask();
    setEditTextPos();
    this->repaint();
    setCommentFrameMask();
    m_plainTextEdit->setFocus();
}

void GCustomCommentFrame::inMoveRange(QPoint pos)
{
    if (m_starDraw)
    {
        return;
    }
    if (containPosPoint(pos, m_borderRect.bottomLeft()))
    {
        setCursor(Qt::SizeBDiagCursor);
        m_state = m_EnbottomLeft;
    }
    else if (containPosPoint(pos, m_borderRect.topLeft()))
    {
        setCursor(Qt::SizeFDiagCursor);
        m_state = m_EnLeftUp;
    }
    else if (containPosPoint(pos, m_borderRect.bottomRight()))
    {
        setCursor(Qt::SizeFDiagCursor);
        m_state = m_EnRightDown;
    }
    else if (containPosPoint(pos, m_borderRect.topRight()))
    {
        setCursor(Qt::SizeBDiagCursor);
        m_state = m_EnRightUp;
    }
    else if (containPosPoint(pos, adjustRectUpPoint()))
    {
        setCursor(Qt::SizeVerCursor);
        m_state = m_EnUP;
    }
    else if (containPosPoint(pos,adjustRectBottomPoint()))
    {
        setCursor(Qt::SizeVerCursor);
        m_state = m_EnDown;
    }
    else if (containPosPoint(pos, adjustRectLeftPoint()))
    {
        setCursor(Qt::SizeHorCursor);
        m_state = m_EnLeft;
    }
    else if (containPosPoint(pos, adjustRectRightPoint()))
    {
        setCursor(Qt::SizeHorCursor);
        m_state = m_EnRiht;
    }
    else if (containPos(pos, m_borderRect.adjusted(-10, -10, 10, 10)))
    {
        setCursor(Qt::SizeAllCursor);
        m_state = m_EnMoving;
    }
    else
    {
        unsetCursor();
        m_state = m_EnNormal;
    }
}

/**
 * @brief MyGCustomCommentFrame::containPosPoint 看鼠标点距离当前点的距离是否小时10，如果小于10，则认为在范围之内
 * @param pos
 * @param border
 * @return
 */
double GCustomCommentFrame::getDistance(const QPoint border, const QPoint pos)
{
    double distance = (pos.x() - border.x()) * (pos.x() - border.x())
            + (pos.y() - border.y()) * (pos.y() - border.y());
    return distance;
}

bool GCustomCommentFrame::containPosPoint(const QPoint pos, const QPoint border)
{
    double distance = getDistance(border, pos);
    if (distance < (c_AdjustWith * c_AdjustWith))
    {
        return true;
    }
    return false;
}

bool GCustomCommentFrame::containPos(const QPoint pos, const QRect subBorderRect)
{
    if (!m_borderRect.contains(pos) && subBorderRect.contains(pos))
    {
        return true;
    }
    else
    {
        return false;
    }
}

