#include "GLDKeyboardButton.h"

const int c_nOneSize                                = 11;
const int c_nTwoSize                                = 10;

const GString numbertaglistblock[c_nOneSize]        = {"@","#","$","%","^","&","*","(",")","~","!"};
const GString numbertaglist[c_nOneSize]             = {"2","3","4","5","6","7","8","9","0","`","1"};

const GString specialSymbolListBlock[c_nTwoSize]    = {"<",">","_","+","|","?","{","}",":","\""};
const GString specialSymbolList[c_nTwoSize]         = {",",".","/","-","=","\\","[","]",";","'"};

GLDKeyboardButton::GLDKeyboardButton(const QString &text, QWidget *parent) :
    QPushButton(text, parent)
{
    setFocusProxy(parent);
}

void GLDKeyboardButton::switchLetterCase()
{
    GString strtext = text();

    if (strtext >= "a" && strtext <= "z")
    {
        setText(strtext.toUpper());
    }

    if (strtext >= "A" && strtext <= "Z")
    {
        setText(strtext.toLower());
    }
}

void GLDKeyboardButton::switchNumberAndSymbol()
{
    setButtonText(numbertaglist, numbertaglistblock, c_nOneSize);
}

void GLDKeyboardButton::switchPunctuations()
{
    setButtonText(specialSymbolList, specialSymbolListBlock, c_nTwoSize);
}

void GLDKeyboardButton::setButtonText(const GString specialSymbolList[], const GString specialSymbolListBlock[], int length)
{
    GString strtext = text();

    for (int i = 0; i < length; i++)
    {
        if (strtext == specialSymbolList[i])
        {
            setText(specialSymbolListBlock[i]);
        }

        if (strtext == specialSymbolListBlock[i])
        {
            setText(specialSymbolList[i]);
        }
    }
}


