#include "GLDDockDataBuilder.h"
#include <memory>
#include "GLDGlobal.h"

GLDDockDataBuilder::GLDDockDataBuilder(void)
{
}

GLDDockDataBuilder::~GLDDockDataBuilder(void)
{
}

QByteArray GLDDockDataBuilder::toByteArray()
{
    QByteArray ba(sizeof(data_) + 1, '\0');
    gMemCopy(ba.data(), &data_, sizeof(data_));
    return ba;
}

bool GLDDockDataBuilder::fromByteArray(const QByteArray &ba)
{
    if (ba.size() != sizeof(data_) + 1)
    {
        return false;
    }

    gMemCopy(&data_, ba.data(), sizeof(data_));
    return true;
}
