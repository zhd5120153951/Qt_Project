#ifndef GLDSTACK_H
#define GLDSTACK_H

#include <stack>
#include <QStack>

using namespace std;

template<class T>
class GLDStack : public stack<T>
{
};

template <class T>
class GStack : public QStack<T>
{
};

#endif // GLDSTACK_H
