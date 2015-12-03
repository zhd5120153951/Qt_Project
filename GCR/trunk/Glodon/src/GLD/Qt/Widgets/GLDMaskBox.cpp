#include "GLDMaskBox.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

GLDMaskBox* GLDMaskBox::m_pMaskBox = nullptr;

GLDMaskBox::GLDMaskBox(QWidget *parent)
    : QWidget(parent)
    , m_maskColor(GLDMaskBox::GrayColor)
    , m_pClippedWgt(nullptr)
    , m_pTipBox(nullptr)
    , m_pSettings(nullptr)
    , m_bShowMask(false)
{
    m_pTipBox = new GLDIrregularForm(""/*exePath() + "/images/Msg/login.png"*/,""/* exePath() + "/images/Msg/know.png"*/, this);

    setFixedSize(QApplication::desktop()->width(), QApplication::desktop()->height());

    connect(m_pTipBox, &GLDIrregularForm::irregularFormClicked, this, &GLDMaskBox::slotClose);
}

GLDMaskBox::GLDMaskBox(GLDMaskBoxParam& param, QWidget * parent)
    : QWidget(parent)
    , m_maskColor(GLDMaskBox::GrayColor)
    , m_pClippedWgt(nullptr)
    , m_pTipBox(nullptr)
    , m_pSettings(nullptr)
    , m_bShowMask(false)
    , m_arrowColor(QColor(1, 169, 240))
    , m_arrowLineWidth(2)
{
    m_oMaskBoxParam = param;
    m_pClippedWgt = param.m_maskWidget;
    m_pTipBox = new GLDIrregularForm(param.m_strTipPath, param.m_strBtnPath, this);

    setFixedSize(QApplication::desktop()->width(), QApplication::desktop()->height());

    connect(m_pTipBox, &GLDIrregularForm::irregularFormClicked, this, &GLDMaskBox::slotClose);
}

GLDMaskBox::~GLDMaskBox()
{

}

void GLDMaskBox::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    const int nClippedWidgetWidth = m_pClippedWgt->size().width();
    const int nClippedWidgetHeight = m_pClippedWgt->size().height();

    QPoint clippedWgtTopLeft = m_pClippedWgt->rect().topLeft();
    QPoint ptGlobalTopLeft = m_pClippedWgt->mapToParent(clippedWgtTopLeft);
    QRect pOwnerRect(ptGlobalTopLeft.x(), ptGlobalTopLeft.y(), nClippedWidgetWidth, nClippedWidgetHeight);



    QPoint clippedWgtTopRight = m_pClippedWgt->rect().topRight();
    QPoint ptGlobalTopRight = m_pClippedWgt->mapToParent(clippedWgtTopRight);


    int w = GLDCBB::topParentWidget(m_pClippedWgt)->width();

    CoordinateParam param = calcPosOfTipInfo();

    if(param.m_quadrant == CoordinateParam::Third || param.m_quadrant == CoordinateParam::Fourth)
    {
        if(w - ptGlobalTopRight.x() > m_pTipBox->width() && ptGlobalTopRight.y() > m_pTipBox->height())
        {
            QPoint endPoint;
            drawLeftBottomArrow(param.m_point, endPoint, painter);
            endPoint += QPoint(0, -m_pTipBox->height());
            m_pTipBox->move(endPoint);
        }
    }
    else if(param.m_quadrant == CoordinateParam::First || param.m_quadrant == CoordinateParam::Second)
    {
        QPoint endPoint;
        drawLeftTopArrow(param.m_point, endPoint, painter);
        m_pTipBox->move(endPoint);
    }

    QRegion rect = this->rect();
    rect -= pOwnerRect;
    painter.setClipRegion(rect);

    drawMask(painter);
}

void GLDMaskBox::drawMask(QPainter & painter)
{
    int w = GLDCBB::topParentWidget(m_pClippedWgt)->size().width();
    int h = GLDCBB::topParentWidget(m_pClippedWgt)->size().height();

    QColor color;

    switch (m_maskColor)
    {
    case GLDMaskBox::GrayColor:      // 128, 128, 128
        color = QColor(0, 0, 0, 100);
        break;

    case GLDMaskBox::GlassColor:     // 201, 120, 12
        color = QColor(201, 120, 12);
        break;

    case GLDMaskBox::CalaeattaColor: // 252, 239, 232
        color = QColor(252, 239, 232);
        break;

    case GLDMaskBox::CreamColor:      // 233, 241, 246
        color = QColor(233, 241, 246);
        break;

    default:
        color = QColor(128, 128, 128);
    }

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    painter.setPen(QPen(color));
    painter.setBrush(color);
    painter.drawRect(1, 1, w - 2, h - 2);

    painter.setBrush(color);
    QPainter::CompositionMode mode = painter.compositionMode();
    painter.setCompositionMode(QPainter::CompositionMode_Xor);

    int clippedWgtWidth = m_pClippedWgt->size().width();
    int clippedWgtHeight = m_pClippedWgt->size().height();
    QPoint globalTopLeft = m_pClippedWgt->mapToParent(m_pClippedWgt->rect().topLeft());
    painter.drawRect(globalTopLeft.x(), globalTopLeft.y(), clippedWgtWidth, clippedWgtHeight);

    painter.setCompositionMode(mode);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(color, 2));
    painter.drawRect(globalTopLeft.x(), globalTopLeft.y(), clippedWgtWidth, clippedWgtHeight);
}

void GLDMaskBox::drawLeftTopArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter)
{
    int x = startPoint.x();
    int y = startPoint.y();

    int x1 = x + 70;
    int y1 = y + 50;
    endPoint = QPoint(x1, y1);

    painter.setPen(QPen(m_arrowColor, m_arrowLineWidth));
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.moveTo(startPoint);

    QPoint point1((x + (x1 - x) * 3 / 10), (y + (y1 - y) * 4 / 7));
    QPoint point2((x + (x1 - x) * 4 / 10), (y + (y1 - y) * 6 / 7));
    QPoint point3((x + (x1 - x) * 8 / 10), (y + (y1 - y) * 3 / 7));
    path.cubicTo(point1, point2, point3);


    QPoint point4((x + (x1 - x) * 5 / 10), (y + (y1 - y) * 1 / 7));
    QPoint point5((x + (x1 - x) * 3 / 10), (y + (y1 - y) * 2 / 7));
    QPoint point6((x + (x1 - x) * 4 / 10), (y + (y1 - y) * 9 / 14));
    path.cubicTo(point4, point5, point6);

    QPoint point7((x + (x1 - x) * 5 / 10), (y + (y1 - y) * 6 / 7));
    QPoint point8((x + (x1 - x) * 5 / 10), (y + (y1 - y) * 7 / 7));
    path.cubicTo(point7, point8, endPoint);

    painter.drawPath(path);

    int xOffset = (x1 - x) / 5;
    QLine line1(startPoint, QPoint(x + 2, y + xOffset));
    int yOffset = (y1 - y) / 7;
    QLine line2(startPoint, QPoint(x + xOffset, y + yOffset));
    painter.drawLine(line1);
    painter.drawLine(line2);
}

void GLDMaskBox::drawLeftBottomArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter)
{
    int x = startPoint.x();
    int y = startPoint.y();

    int x1 = x + 70;
    int y1 = y + 50;
    endPoint = QPoint(x1, y - 50);

    painter.setPen(QPen(m_arrowColor, m_arrowLineWidth));
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.moveTo(startPoint);

    QPoint point1((x + (x1 - x) * 3 / 10), (y - (y1 - y) * 4 / 7));
    QPoint point2((x + (x1 - x) * 4 / 10), (y - (y1 - y) * 6 / 7));
    QPoint point3((x + (x1 - x) * 8 / 10), (y - (y1 - y) * 3 / 7));
    path.cubicTo(point1, point2, point3);


    QPoint point4((x + (x1 - x) * 5 / 10), (y - (y1 - y) * 1 / 7));
    QPoint point5((x + (x1 - x) * 3 / 10), (y - (y1 - y) * 2 / 7));
    QPoint point6((x + (x1 - x) * 4 / 10), (y - (y1 - y) * 9 / 14));
    path.cubicTo(point4, point5, point6);

    QPoint point7((x + (x1 - x) * 5 / 10), y - 50);
    QPoint point8((x + (x1 - x) * 5 / 10), y - 50);
    path.cubicTo(point7, point8, endPoint);

    painter.drawPath(path);

    int xOffset = (x1 - x) * 2 / 10;
    QLine line1(startPoint, QPoint(x + 2, y - xOffset));
    int yOffset = (y1 - y) * 1 / 7;
    QLine line2(startPoint, QPoint(x + xOffset, y - yOffset));
    painter.drawLine(line1);
    painter.drawLine(line2);
}

void GLDMaskBox::mousePressEvent(QMouseEvent * event)
{
    QPoint ptGlobalOwnerCenter = m_pClippedWgt->mapToParent(m_pClippedWgt->rect().topLeft());
    QRect rect(ptGlobalOwnerCenter.rx(), ptGlobalOwnerCenter.ry(),
        m_pClippedWgt->width(), m_pClippedWgt->height());

    if (rect.contains(event->pos()))
    {
        emit customClicked();
    }

    QWidget::mousePressEvent(event);
}

void GLDMaskBox::openIniFile(const QString& filePath)
{
    Q_ASSERT(!filePath.isEmpty());

    if (filePath.isEmpty())
    {
        return;
    }

    m_pSettings = new QSettings(filePath, QSettings::IniFormat, this);

    Q_ASSERT(m_pSettings->status() == QSettings::NoError);

    m_pSettings->sync();
    m_pSettings->setFallbacksEnabled(true);

    setMaskShow();
}

GLDMaskBox* GLDMaskBox::createMaskFor(QWidget* widget, const QString & tipInfoPath, const QString & btnInfoPath)
{
    GLDMaskBox* pTip = nullptr;

//    if (!m_bShowMask)
//    {
//        return nullptr;
//    }

    do
    {
        if (!widget)
        {
            break;
        }

        GLDMaskBoxParam param;
        param.m_maskWidget = widget;
        param.m_strTipPath = tipInfoPath;
        param.m_strBtnPath = btnInfoPath;

        QWidget* pWidget = GLDCBB::topParentWidget(widget);
        pTip = new GLDMaskBox(param, pWidget);

        m_pMaskBox = pTip;

        pTip->show();

    } while (0);

    return pTip;
}

void GLDMaskBox::setMaskColor(MASKCOLOR maskColor)
{
    if(maskColor == m_maskColor)
    {
        return;
    }

    m_maskColor = maskColor;
}

void GLDMaskBox::setArrowColor(const QColor &color)
{
    m_arrowColor = color;
}

void GLDMaskBox::setArrowLineWidth(const int lineWidth)
{
    m_arrowLineWidth = lineWidth;
}

QPoint GLDMaskBox::calcPosOfOwner()
{
    QPoint pt(0, 0);

    do
    {
        // 计算owner位置对应屏幕中心的象限
        if (!m_pClippedWgt)
        {
            break;
        }

        QPoint ptGlobalOwnerCenter = m_pClippedWgt->mapToGlobal(m_pClippedWgt->rect().center());
        QPoint ptGlobalScreen = QApplication::desktop()->screenGeometry().center();
        QPoint ptDelta = ptGlobalOwnerCenter - ptGlobalScreen;

        if (ptDelta.x() >= 0 && ptDelta.y() <= 0)
        {
            // 第一象限
            pt = QPoint(ptGlobalOwnerCenter.x() - m_pClippedWgt->width()/2,
                            ptGlobalOwnerCenter.y() + m_pClippedWgt->height()/2);
            pt += QPoint(-this->width()/2, 0);
        }
        else if (ptDelta.x() <= 0 && ptDelta.y() <= 0)
        {
            // 第二象限
            pt = QPoint(ptGlobalOwnerCenter.x() + m_pClippedWgt->width()/2,
                            ptGlobalOwnerCenter.y() + m_pClippedWgt->height()/2);
            pt += QPoint(-this->width()/2, 0);
        }
        else if (ptDelta.x() <= 0 && ptDelta.y() >= 0)
        {
            // 第三象限
            pt = QPoint(ptGlobalOwnerCenter.x() + m_pClippedWgt->width()/2,
                            ptGlobalOwnerCenter.y() - m_pClippedWgt->height()/2);
            pt += QPoint(-this->width()/2, -this->height());
        }
        else if (ptDelta.x() >= 0 && ptDelta.y() >= 0)
        {
            // 第四象限
            pt = QPoint(ptGlobalOwnerCenter.x() - m_pClippedWgt->width()/2,
                        ptGlobalOwnerCenter.y() - m_pClippedWgt->height()/2);
            pt += QPoint(-this->width()/2, -this->height());
        }

        // 超出屏幕范围的校准
        QRect rcThis(pt, pt + QPoint(this->width(), this->height()));
        QRect rcScreen = QApplication::desktop()->screenGeometry();

        if (!rcScreen.contains(rcThis.topLeft()))
        {
            int nXOffset = rcThis.topLeft().x() - rcScreen.topLeft().x();
            int nYOffset = rcThis.topLeft().y() - rcScreen.topLeft().y();

            if (nXOffset < 0)
            {
                pt.setX(pt.x() - nXOffset);
            }

            if (nYOffset < 0)
            {
                pt.setY(pt.y() - nYOffset);
            }
        }
        else if (!rcScreen.contains(rcThis.topRight()))
        {
            int nXOffset = rcThis.topRight().x() - rcScreen.topRight().x();
            int nYOffset = rcThis.topRight().y() - rcScreen.topRight().y();

            if (nXOffset > 0)
            {
                pt.setX(pt.x() - nXOffset);
            }

            if (nYOffset < 0)
            {
                pt.setY(pt.y() - nYOffset);
            }
        }
        else if (!rcScreen.contains(rcThis.bottomLeft()))
        {
            int nXOffset = rcThis.bottomLeft().x() - rcScreen.bottomLeft().x();
            int nYOffset = rcThis.bottomLeft().y() - rcScreen.bottomLeft().y();

            if (nXOffset < 0)
            {
                pt.setX(pt.x() - nXOffset);
            }

            if (nYOffset > 0)
            {
                pt.setY(pt.y() - nYOffset);
            }
        }
        else if (!rcScreen.contains(rcThis.bottomRight()))
        {
            int nXOffset = rcThis.bottomRight().x() - rcScreen.bottomRight().x();
            int nYOffset = rcThis.bottomRight().y() - rcScreen.bottomRight().y();

            if (nXOffset > 0)
            {
                pt.setX(pt.x() - nXOffset);
            }

            if (nYOffset > 0)
            {
                pt.setY(pt.y() - nYOffset);
            }
        }

    } while(0);

    return pt;
}

CoordinateParam GLDMaskBox::calcPosOfTipInfo()
{
    CoordinateParam coordinateParam;

    do
    {
        // 计算owner位置对应屏幕中心的象限
        if (!m_pClippedWgt)
        {
            break;
        }

        QPoint clippedWgtTopLeft = m_pClippedWgt->rect().topLeft();
        QPoint ptParentOwnerTopLeft = m_pClippedWgt->mapToParent(clippedWgtTopLeft);

        QPoint ptGlobalOwnerCenter = m_pClippedWgt->mapToGlobal(m_pClippedWgt->rect().center());
        QPoint ptGlobalScreen = QApplication::desktop()->screenGeometry().center();
        QPoint ptDelta = ptGlobalOwnerCenter - ptGlobalScreen;

        if (ptDelta.x() >= 0 && ptDelta.y() <= 0)
        {
            // 第一象限
            coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                ptParentOwnerTopLeft.y() + m_pClippedWgt->height());
            coordinateParam.m_quadrant = CoordinateParam::First;
        }
        else if(ptDelta.x() <= 0 && ptDelta.y() <= 0)
        {
            // 第二象限
            coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                ptParentOwnerTopLeft.y() + m_pClippedWgt->height());
            coordinateParam.m_quadrant = CoordinateParam::Second;
        }
        else if(ptDelta.x() <= 0 && ptDelta.y() >= 0)
        {
            // 第三象限
             coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                  ptParentOwnerTopLeft.y());
             coordinateParam.m_quadrant = CoordinateParam::Third;

        }
        else if(ptDelta.x() >= 0 && ptDelta.y() >= 0)
        {
            coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                 ptParentOwnerTopLeft.y());
            coordinateParam.m_quadrant = CoordinateParam::Fourth;
        }

    } while (0);

    return coordinateParam;
}

void GLDMaskBox::setMaskShow()
{
    m_bShowMask = getMaskShow(tr("Mask"), tr("MaskShow"));
}

bool GLDMaskBox::getMaskShow(const QString &prefix, const QString &key)
{
    Q_ASSERT(m_pSettings != NULL);

    QString strValue = getValue(prefix, key);

    if (!strValue.isEmpty())
    {
        if (strValue == "true")
        {
            // 这个函数是在最终发布的时候打开，那么蒙版只会在第一次启动的时候被调用
            // 暂时在测试功能中，先注掉，方便调试
            // setValue(prefix, key);
            return true;
        }

        return false;
    }

    return false;
}

QString GLDMaskBox::getValue(const QString &prefix, const QString &key)
{
    Q_ASSERT(m_pSettings != NULL);

    QString strValue;
    m_pSettings->beginGroup(prefix);
    QVariant value = m_pSettings->value(key);

    if (value.isValid() && value.type() == QVariant::String)
    {
        strValue = value.toString();
    }

    m_pSettings->endGroup();

    return strValue;
}

void GLDMaskBox::setValue(const QString &prefix, const QString &key)
{
    Q_ASSERT(m_pSettings != NULL);

    m_pSettings->beginGroup(prefix);
    m_pSettings->setValue(key, "fasle");

    m_pSettings->endGroup();
}

void GLDMaskBox::slotClose()
{
    this->close();
}
