#include "GLDPlainTextEdit.h"
#include <QPainter>
#include <QRect>
#include <QTextBlock>
#include <QScrollBar>
#include <QTextLayout>
#include "GLDTableView.h"

//GLDPlainTextEdit::GLDPlainTextEdit(QWidget *parent) : QPlainTextEdit(parent),
//    prevLineCount(-1), prevTopPadding(-1), prevLeftPadding(-1), firstShow(true), gridDrawIndex(0,0)
//{
//    QObject::connect(this, SIGNAL(textChanged()), this, SLOT(adjustVerticalPadding()));
//}

//GLDPlainTextEdit::GLDPlainTextEdit(QWidget *parent, QPoint gridIndex) : QPlainTextEdit(parent), firstShow(true),
//    gridDrawIndex(gridIndex), prevLineCount(-1), prevTopPadding(-1), prevLeftPadding(-1)
//{
//    QObject::connect(this, SIGNAL(textChanged()), this, SLOT(adjustVerticalPadding()));
//}

//void GLDPlainTextEdit::setPaddingByOffset()
//{
//    setPaddingByOffset(gridUsingOffset());
//}

//void GLDPlainTextEdit::setPaddingByOffset(const QPoint &gridUsingOffset)
//{
//    QPointF offset(QPlainTextEdit::contentOffset());
//    resetPadding(gridUsingOffset.x() - offset.x(), gridUsingOffset.y() - (offset.y()<0? 1 :offset.y()));
//}

//void GLDPlainTextEdit::resetPadding(int left, int top, int right,  int bottom)
//{
//    prevLeftPadding = left;
//    prevTopPadding = top;
//    QString styleSheet = right < 0x7fff  ? leftTopRightBottomStyleString().arg(left).arg(top).arg(right).arg(bottom) : leftTopStyleString().arg(left).arg(top) ;
//    styleSheet = "background-color:rgb(255, 255, 255); " + styleSheet;
//    this->setStyleSheet(styleSheet);
//}

//void GLDPlainTextEdit::adjustVerticalPadding()
//{
//    int newLineCount = this->document()->lineCount();
//    if (prevLineCount != newLineCount )
//    {
//        if (QTextLayout *textLayout = this->firstVisibleBlock().layout())
//        {
//            QTextLine firstLine = textLayout->lineAt(0);
//            if (firstLine.isValid())
//            {
//                int lineHeight = firstLine.height();
//                if (lineHeight * newLineCount >= this->viewport()->height())
//                {
//                    prevTopPadding = 1;
//                }
//                else
//                    prevTopPadding = prevTopPadding + lineHeight *(prevLineCount - newLineCount) / 2.0;

//                if (prevTopPadding <= 0)
//                    prevTopPadding = 1;
//                QString styleSheet = leftTopStyleString().arg(prevLeftPadding).arg(prevTopPadding);
//                this->setStyleSheet(styleSheet);
//                prevLineCount = newLineCount;
//            }
//        }
//    }
//}

//void GLDPlainTextEdit::showEvent(QShowEvent *e)
//{
//    QPlainTextEdit::showEvent(e);
//    if (firstShow)
//    {
//        firstShow = false;
//        setPaddingByOffset();
//        prevLineCount = firstVisibleBlock().layout()->lineCount();
//    }
//}

//QPoint GLDPlainTextEdit::gridUsingOffset()
//{
//    QPoint offsetPadding(0,0);
//    QWidget *pViewPort = qobject_cast<QWidget *>(parent());
//    if (pViewPort)
//    {
//        GlodonTableView *pParent = qobject_cast<GlodonTableView *>(pViewPort->parent());
//        if (pParent)
//        {
////            qDebug()<<pParent->index2Offset;
//            if (pParent->index2Offset.contains(this->gridDrawIndex))
//            {
//                offsetPadding = pParent->index2Offset[this->gridDrawIndex];
//            }
//        }
//    }
//    return offsetPadding;
//}
