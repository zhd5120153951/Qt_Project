#ifndef GLDSINGLEINSTANCE_H
#define GLDSINGLEINSTANCE_H

#include <QSharedMemory>
#include "GLDString.h"
#include "GLDGlobal.h"

class GLDUniqueAppInstancePrivate;
class GLDCOMMONSHARED_EXPORT GLDUniqueAppInstance
{
public:
    GLDUniqueAppInstance(const GString &uniqueKey);
    ~GLDUniqueAppInstance();
    bool isRunning();
private:
    GLDUniqueAppInstancePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GLDUniqueAppInstance);
};

#endif // GLDSINGLEINSTANCE_H
