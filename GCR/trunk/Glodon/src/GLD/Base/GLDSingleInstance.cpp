#include "GLDSingleInstance.h"

class GLDUniqueAppInstancePrivate
{
public:
  GLDUniqueAppInstancePrivate(GLDUniqueAppInstance *parent)
    : q_ptr(parent)
  {
  }

private:
  GLDUniqueAppInstance * const q_ptr;
  Q_DECLARE_PUBLIC(GLDUniqueAppInstance);

  bool m_isRunning;
  GString m_uniqueKey;
  QSharedMemory m_sharedMemory;
};

GLDUniqueAppInstance::GLDUniqueAppInstance(const GString &uniqueKey)
    : d_ptr(new GLDUniqueAppInstancePrivate(this))

{
    Q_D(GLDUniqueAppInstance);
    d->m_uniqueKey = uniqueKey;
    d->m_sharedMemory.setKey(d->m_uniqueKey);
    if (d->m_sharedMemory.attach())
    {
        d->m_isRunning = true;
    }
    else
    {
        d->m_isRunning = false;
        // create shared memory.
        if (!d->m_sharedMemory.create(1))
        {
            qDebug("Unable to create single instance.");
            return;
        }
    }
}

GLDUniqueAppInstance::~GLDUniqueAppInstance()
{
    Q_D(GLDUniqueAppInstance);
    freePtr(d);
}

bool GLDUniqueAppInstance::isRunning()
{
    Q_D(GLDUniqueAppInstance);
    return d->m_isRunning;
}
