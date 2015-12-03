#ifndef GLDKEYBOARDBUTTON_H
#define GLDKEYBOARDBUTTON_H

#include <QObject>
#include <QPushButton>
#include "GLDString.h"
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDKeyboardButton : public QPushButton
{
    Q_OBJECT
public:
    explicit GLDKeyboardButton(const QString &text, QWidget *parent = 0);

    void setButtonText(const GString specialSymbolList[],
                       const GString specialSymbolListBlock[],
                       int length);
public slots:
    void switchLetterCase();
    void switchNumberAndSymbol();
    void switchPunctuations();
};

#endif // GLDKEYBOARDBUTTON_H
