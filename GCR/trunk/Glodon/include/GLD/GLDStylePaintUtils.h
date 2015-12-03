#ifndef GLDSTYLEPAINTUTILS_H
#define GLDSTYLEPAINTUTILS_H

class QStyleOptionComboBox;
class QStyleOptionSpinBox;
class QStyleOptionButton;
class QRect;
class QString;
class GLDWIDGETSHARED_EXPORT GLDStylePaintUtils
{
public:
    static  void initComboBoxOpt(QStyleOptionComboBox &opt, const QRect &rect);
    static  void initPushButtonOpt(QStyleOptionButton &opt, const QRect &rect, const QString &btnText);
    static  void initSpinBoxOpt(QStyleOptionSpinBox &opt, const QRect &rect);
protected:
    explicit GLDStylePaintUtils();

};

#endif // GLDSTYLEPAINTUTILS_H
