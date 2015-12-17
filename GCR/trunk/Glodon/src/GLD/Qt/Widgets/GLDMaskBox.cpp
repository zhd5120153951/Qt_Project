#include "GLDMaskBox.h"

#include <QDebug>
#include <assert.h>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

namespace GlodonMask
{
    GLDMaskBox::GLDMaskBox(GLDMaskBoxParam& param, QWidget * parent)
        //: QWidget(parent)
        : m_maskColor(GLDMaskBox::GrayColor)
        , m_oMaskBoxParam(param)
        , m_pClippedWgt(param.m_maskWidget)
        , m_pTipWidget(param.m_pTipWgt)
        , m_arrowColor(QColor(1, 169, 240))
        , m_arrowLineWidth(2)
    {
        setFixedSize(QApplication::desktop()->width(), QApplication::desktop()->height());

        m_pClippedWgt->installEventFilter(this);

        connect(m_pTipWidget, &GLDTipWidget::tipWidgetClicked, this, &GLDMaskBox::slotClose);
        connect(m_pTipWidget, &GLDTipWidget::currentBtnClicked, this, &GLDMaskBox::nextBtnClicked);
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

        int topWidgetWidth = topParentWidget(m_pClippedWgt)->width();

        CoordinateParam param = calcPosOfTipInfo();

        if (param.m_quadrant == CoordinateParam::Third || param.m_quadrant == CoordinateParam::Fourth)
        {
            // if(topWidgetWidth - ptGlobalTopRight.x() > m_pTipBox->width() && ptGlobalTopRight.y() > m_pTipBox->height())
            {
                QPoint endPoint;
                drawLeftBottomArrow(param.m_point, endPoint, painter);

                endPoint += QPoint(0, -m_pTipWidget->height());
                m_pTipWidget->move(endPoint);
            }
        }
        else if (param.m_quadrant == CoordinateParam::First || param.m_quadrant == CoordinateParam::Second)
        {
            QPoint endPoint;
            drawLeftTopArrow(param.m_point, endPoint, painter);
            m_pTipWidget->move(endPoint);
        }

        QRegion rect = this->rect();
        rect -= pOwnerRect;
        setMask(rect);

        drawMask(painter);

        update();
    }

    void GLDMaskBox::drawMask(QPainter & painter)
    {
        const int topWidgetWidth = topParentWidget(m_pClippedWgt)->size().width();
        const int topWidgetHeight = topParentWidget(m_pClippedWgt)->size().height();

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
            color = QColor(20, 20, 20);
            break;

        default:
            color = QColor(128, 128, 128);
        }

        painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
        painter.setPen(QPen(color));
        painter.setBrush(color);
        painter.drawRect(1, 1, topWidgetWidth - 1, topWidgetHeight - 1);
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

    void GLDMaskBox::drawTopRightArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter)
    {
        int x = startPoint.x();
        int y = startPoint.y();

        int x1 = x - 70;
        int y1 = y + 50;
        endPoint = QPoint(x1, y1);

        painter.setPen(QPen(m_arrowColor, m_arrowLineWidth));
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        path.moveTo(startPoint);

        QPoint point1((x - (x - x1) * 3 / 10), (y + (y1 - y) * 4 / 7));
        QPoint point2((x - (x - x1) * 4 / 10), (y + (y1 - y) * 6 / 7));
        QPoint point3((x - (x - x1) * 8 / 10), (y + (y1 - y) * 3 / 7));
        path.cubicTo(point1, point2, point3);


        QPoint point4((x - (x - x1) * 5 / 10), (y + (y1 - y) * 1 / 7));
        QPoint point5((x - (x - x1) * 3 / 10), (y + (y1 - y) * 2 / 7));
        QPoint point6((x - (x - x1) * 4 / 10), (y + (y1 - y) * 9 / 14));
        path.cubicTo(point4, point5, point6);

        QPoint point7((x - (x - x1) * 5 / 10), (y + (y1 - y) * 6 / 7));
        QPoint point8((x - (x - x1) * 5 / 10), (y + (y1 - y) * 7 / 7));
        path.cubicTo(point7, point8, endPoint);

        painter.drawPath(path);

        int xOffset = (y1 - y) / 5;
        int yOffset = (x - x1) / 7;
        QLine line1(startPoint, QPoint(x - xOffset * 3 / 2, y + yOffset / 2));
        QLine line2(startPoint, QPoint(x, y + yOffset));
        painter.drawLine(line1);
        painter.drawLine(line2);
    }

    void GLDMaskBox::drawRightBottomArrow(QPoint &startPoint, QPoint &endPoint, QPainter &painter)
    {
        int x = startPoint.x();
        int y = startPoint.y();

        int x1 = x - 70;
        int y1 = y - 50;
        endPoint = QPoint(x1, y1);

        painter.setPen(QPen(m_arrowColor, m_arrowLineWidth));
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        path.moveTo(startPoint);

        QPoint point1((x - (x - x1) * 3 / 10), (y - (y - y1) * 4 / 7));
        QPoint point2((x - (x - x1) * 4 / 10), (y - (y - y1) * 6 / 7));
        QPoint point3((x - (x - x1) * 8 / 10), (y - (y - y1) * 3 / 7));
        path.cubicTo(point1, point2, point3);


        QPoint point4((x - (x - x1) * 5 / 10), (y - (y - y1) * 1 / 7));
        QPoint point5((x - (x - x1) * 3 / 10), (y - (y - y1) * 2 / 7));
        QPoint point6((x - (x - x1) * 4 / 10), (y - (y - y1) * 9 / 14));
        path.cubicTo(point4, point5, point6);

        QPoint point7((x - (x - x1) * 5 / 10), (y - (y - y1) * 6 / 7));
        QPoint point8((x - (x - x1) * 5 / 10), (y - (y - y1) * 7 / 7));
        path.cubicTo(point7, point8, endPoint);

        painter.drawPath(path);

        int xOffset = (x - x1) / 5;
        int yOffset = (y - y1) / 7;
        QLine line1(startPoint, QPoint(x - 2, y - xOffset));
        QLine line2(startPoint, QPoint(x - xOffset, y - yOffset));
        painter.drawLine(line1);
        painter.drawLine(line2);
    }

    bool GLDMaskBox::eventFilter(QObject *watched, QEvent *event)
    {
        if (watched == m_pClippedWgt && event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() == Qt::LeftButton)
            {
                close();
            }
            else
            {
                return false;
            }
        }

        return QWidget::eventFilter(watched, event);
    }

    void GLDMaskBox::openIniFile(const QString& filePath)
    {
        Q_ASSERT(!filePath.isEmpty());
    }

    bool GLDMaskBox::canShow()
    {
        if (!m_btnObjectName.isEmpty())
        {
            QSettings oInis(m_iniPath, QSettings::IniFormat);
            return oInis.value(m_btnObjectName, 0).toInt() == 0;
        }

        return false;
    }

    GlodonMask::GLDMaskBoxParam GLDMaskBox::getMaskBoxParam()
    {
        return m_oMaskBoxParam;
    }

    void GLDMaskBox::setMaskColor(MASKCOLOR maskColor)
    {
        if (maskColor == m_maskColor)
        {
            return;
        }

        m_maskColor = maskColor;
    }

    void GLDMaskBox::setArrowColor(const QColor &color)
    {
        if(m_arrowColor == color)
        {
            return;
        }

        m_arrowColor = color;
    }

    void GLDMaskBox::setArrowLineWidth(const int lineWidth)
    {
        if (m_arrowLineWidth == lineWidth)
        {
            return;
        }

        m_arrowLineWidth = lineWidth;
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
            else if (ptDelta.x() <= 0 && ptDelta.y() <= 0)
            {
                // 第二象限
                coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                    ptParentOwnerTopLeft.y() + m_pClippedWgt->height());
                coordinateParam.m_quadrant = CoordinateParam::Second;
            }
            else if (ptDelta.x() <= 0 && ptDelta.y() >= 0)
            {
                // 第三象限
                coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                    ptParentOwnerTopLeft.y());
                coordinateParam.m_quadrant = CoordinateParam::Third;

            }
            else if (ptDelta.x() >= 0 && ptDelta.y() >= 0)
            {
                // 第四象限
                coordinateParam.m_point = QPoint(ptParentOwnerTopLeft.x() + m_pClippedWgt->width() / 2,
                    ptParentOwnerTopLeft.y());
                coordinateParam.m_quadrant = CoordinateParam::Fourth;
            }

        } while (0);

        return coordinateParam;
    }

    void GLDMaskBox::slotClose()
    {
        this->close();

        if (!m_btnObjectName.isEmpty())
        {
            QSettings oInis(m_iniPath, QSettings::IniFormat);
            oInis.setValue(m_btnObjectName, 1);
        }
    }
}
