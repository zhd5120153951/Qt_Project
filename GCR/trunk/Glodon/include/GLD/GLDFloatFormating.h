#ifndef GLDFLOATFORMATING_H
#define GLDFLOATFORMATING_H

#include "GLDString.h"
#include "GLDCommon_Global.h"

struct FloatRec
{
    int exponent;
    bool negative;
    char digits[20];
};

class GLDFloatFormatingPrivate;
class GLDCOMMONSHARED_EXPORT GLDFloatFormating
{
public:
    GLDFloatFormating();
    ~GLDFloatFormating();
public:
    static GString formatFloat(double value, const GString &format);
private:
    GString internalFloatToTextFmt(double value, const GString &format);
    int findSection(int index);
    GString scanSection(int pos);
    void applyFormat(int sectionIndex);
    int digitsLength();
    void putFmtDigit();
    void writeDigit(char digit);
    void addDigit();
    void putExponent(QChar eChar, QChar sign, int zeros, int exponent);

private:
    GLDFloatFormatingPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDFloatFormating);
};

#endif // GLDFLOATFORMATING_H
