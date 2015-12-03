#ifndef GLDHASH_H
#define GLDHASH_H

#include <QHash>

template <class Key, class T>
class GLDHash : public QHash<Key, T>
{
};

template <class Key, class T>
class GLDMultiHash : public QMultiHash<Key, T>
{
};

#endif // GLDHASH_H
