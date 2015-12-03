#ifndef GLDPlainTextEdit_H
#define GLDPlainTextEdit_H
#include <QRect>
#include <QStyleOption>
#include <QPlainTextEdit>
#include <QStyleOptionViewItem>

class GLDIndexPoint : public QPoint
{
public:
    explicit GLDIndexPoint() : QPoint(){}
    explicit GLDIndexPoint(int x, int y) : QPoint(x, y){}
    explicit GLDIndexPoint(const QPoint &rhs) : QPoint(rhs.x(), rhs.y()){}
    explicit GLDIndexPoint(const GLDIndexPoint &rhs) : QPoint(rhs.x(), rhs.y()){}
    bool isValide() const{
        return x() >= 0 && y() >= 0;
    }
    bool operator<(const GLDIndexPoint &rhs) const
    {
        if (this->x() < rhs.x())
            return true;
        else if (x() > rhs.x())
            return false;
        else if (y() < rhs.y())
            return true;
        else
            return false;
    }
};

//class GLDPlainTextEdit : public QPlainTextEdit
//{
//Q_OBJECT
//public:
//    GLDPlainTextEdit(QWidget *parent);
//    GLDPlainTextEdit(QWidget *parent, QPoint gridIndex);
//    void setPaddingByOffset();
//    void setPaddingByOffset(const QPoint &gridUsingOffset);
//    void resetPadding(int left = 0x7fff, int top = 0x7fff, int right = 0x7fff, int bottom = 0x7fff);

//protected Q_SLOTS:
//    void adjustVerticalPadding();

//protected:
//    virtual void showEvent(QShowEvent *);

//    QPoint gridUsingOffset();
//    static QString leftTopStyleString()
//    {
//        static QString str = "padding-left:%1 px; padding-top: %2 px;";
//        return str;
//    }
//    static QString leftTopRightBottomStyleString()
//    {
//        static QString str = "padding-left:%1 px; padding-top: %2 px; padding-right: %3 px; padding-bottom: %4 px;";
//        return str;
//    }

//protected:
//    int prevLineCount;
//    int prevLeftPadding;
//    int prevTopPadding;
//    bool firstShow;
//    GLDIndexPoint gridDrawIndex;
//private:
//};

#endif // GLDPlainTextEdit_H
