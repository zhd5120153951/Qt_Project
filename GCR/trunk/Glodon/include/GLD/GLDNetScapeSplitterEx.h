#ifndef GLDNETSCAPESPLITTEREX_H
#define GLDNETSCAPESPLITTEREX_H

#include "GLDNetScapeSplitter.h"

const int c_InitialHandleWidthEx = 6;

/*!
  *\brief Used to declare Class inheritance
*/
class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitterButtonEx : public GLDNetScapeSplitterButton
{
public:
    explicit GLDNetScapeSplitterButtonEx(Qt::Orientation o, QWidget *parent = 0);

public:
    void loadStyleSheet(const QString &fileName);

protected:
    void paintEvent(QPaintEvent *e);
};

class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitterHandleEx : public GLDNetScapeSplitterHandle
{
    Q_OBJECT
public:
    explicit GLDNetScapeSplitterHandleEx(Qt::Orientation o,
                                         GLDNetScapeSplitter *parent = 0);
    virtual void initButton();
};

class GLDWIDGETSHARED_EXPORT GLDNetScapeSplitterEx : public GLDNetScapeSplitter
{
    Q_OBJECT
public:
    explicit GLDNetScapeSplitterEx(QWidget *parent = 0);
    explicit GLDNetScapeSplitterEx(Qt::Orientation original, QWidget *parent = 0);
    explicit GLDNetScapeSplitterEx(bool initCollapsed, QWidget *parent = 0);
    explicit GLDNetScapeSplitterEx(Qt::Orientation original,
                                   bool initCollapsed, QWidget *parent = 0);

public:
    void loadStyleSheet(const QString &fileName);

protected:
    virtual GLDSplitterHandle *createHandle();

private:
    void init();
};

#endif // GLDNetScapeSplitterButtonEx_H
