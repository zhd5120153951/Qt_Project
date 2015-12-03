#ifndef GLDRBTREE_H
#define GLDRBTREE_H

#include "GLDVector.h"

template <class T>
class GLDRBTreeNode
{
public:
    inline GLDRBTreeNode() : data(0), parent(0), left(0), right(0), red(true) { }
    inline ~GLDRBTreeNode() {if (left) delete left; if (right) delete right;}
    T data;
    GLDRBTreeNode *parent;
    GLDRBTreeNode *left;
    GLDRBTreeNode *right;
    bool red;
};

template <class T>
class GLDRBTree
{
public:
    inline GLDRBTree() : m_root(0), freeList(0) { }
    inline ~GLDRBTree();

    inline void clear();

    void attachBefore(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child);
    void attachAfter(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child);

    inline GLDRBTreeNode<T> *front(GLDRBTreeNode<T> *node) const;
    inline GLDRBTreeNode<T> *back(GLDRBTreeNode<T> *node) const;
    GLDRBTreeNode<T> *next(GLDRBTreeNode<T> *node) const;
    GLDRBTreeNode<T> *previous(GLDRBTreeNode<T> *node) const;

    inline void deleteNode(GLDRBTreeNode<T> *&node);
    inline GLDRBTreeNode<T> *newNode();

    // Return 1 if 'left' comes after 'right', 0 if equal, and -1 otherwise.
    // 'left' and 'right' cannot be null.
    int order(GLDRBTreeNode<T> *left, GLDRBTreeNode<T> *right);
    inline bool validate() const;

private:
    void rotateLeft(GLDRBTreeNode<T> *node);
    void rotateRight(GLDRBTreeNode<T> *node);
    void update(GLDRBTreeNode<T> *node);

    inline void attachLeft(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child);
    inline void attachRight(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child);

    int blackDepth(GLDRBTreeNode<T> *top) const;
    bool checkRedBlackProperty(GLDRBTreeNode<T> *top) const;

    void swapNodes(GLDRBTreeNode<T> *n1, GLDRBTreeNode<T> *n2);
    void detach(GLDRBTreeNode<T> *node);

    // 'node' must be black. rebalance will reduce the depth of black nodes by one in the sibling tree.
    void rebalance(GLDRBTreeNode<T> *node);
public:
    GLDRBTreeNode<T> *root() const { return m_root; }
private:
    GLDRBTreeNode<T> *m_root;
    GLDRBTreeNode<T> *freeList;
};

template <class T>
inline GLDRBTree<T>::~GLDRBTree()
{
    clear();
    while (freeList)
    {
        // Avoid recursively calling the destructor, as this list may become large.
        GLDRBTreeNode<T> *next = freeList->right;
        freeList->right = 0;
        delete freeList;
        freeList = next;
    }
}

template <class T>
inline void GLDRBTree<T>::clear()
{
    if (m_root)
        delete m_root;
    m_root = 0;
}

template <class T>
void GLDRBTree<T>::rotateLeft(GLDRBTreeNode<T> *node)
{
    //   |            |      //
    //   N            B      //
    //  / \          / \     //
    // A   B  --->  N   D    //
    //    / \      / \       //
    //   C   D    A   C      //

    GLDRBTreeNode<T> *&ref = (node->parent ? (node == node->parent->left ? node->parent->left : node->parent->right) : m_root);
    ref = node->right;
    node->right->parent = node->parent;

    //   :        //
    //   N        //
    //  / :|      //
    // A   B      //
    //    / \     //
    //   C   D    //

    node->right = ref->left;
    if (ref->left)
        ref->left->parent = node;

    //   :   |     //
    //   N   B     //
    //  / \ : \    //
    // A   C   D   //

    ref->left = node;
    node->parent = ref;

    //     |       //
    //     B       //
    //    / \      //
    //   N   D     //
    //  / \        //
    // A   C       //
}

template <class T>
void GLDRBTree<T>::rotateRight(GLDRBTreeNode<T> *node)
{
    //     |            |        //
    //     N            A        //
    //    / \          / \       //
    //   A   B  --->  C   N      //
    //  / \              / \     //
    // C   D            D   B    //

    GLDRBTreeNode<T> *&ref = (node->parent ? (node == node->parent->left ? node->parent->left : node->parent->right) : m_root);
    ref = node->left;
    node->left->parent = node->parent;

    node->left = ref->right;
    if (ref->right)
        ref->right->parent = node;

    ref->right = node;
    node->parent = ref;
}

template <class T>
void GLDRBTree<T>::update(GLDRBTreeNode<T> *node) // call this after inserting a node
{
    while (true)
    {
        GLDRBTreeNode<T> *parent = node->parent;

        // if the node is the m_root, color it black
        if (!parent)
        {
            node->red = false;
            return;
        }

        // if the parent is black, the node can be left red
        if (!parent->red)
            return;

        // at this point, the parent is red and cannot be the m_root
        GLDRBTreeNode<T> *grandpa = parent->parent;
        Q_ASSERT(grandpa);

        GLDRBTreeNode<T> *uncle = (parent == grandpa->left ? grandpa->right : grandpa->left);
        if (uncle && uncle->red)
        {
            // grandpa's black, parent and uncle are red.
            // let parent and uncle be black, grandpa red and recursively update grandpa.
            Q_ASSERT(!grandpa->red);
            parent->red = false;
            uncle->red = false;
            grandpa->red = true;
            node = grandpa;
            continue;
        }

        // at this point, uncle is black
        if (node == parent->right && parent == grandpa->left)
            rotateLeft(node = parent);
        else if (node == parent->left && parent == grandpa->right)
            rotateRight(node = parent);
        parent = node->parent;

        if (parent == grandpa->left)
        {
            rotateRight(grandpa);
            parent->red = false;
            grandpa->red = true;
        }
        else
        {
            rotateLeft(grandpa);
            parent->red = false;
            grandpa->red = true;
        }
        return;
    }
}

template <class T>
inline void GLDRBTree<T>::attachLeft(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child)
{
    Q_ASSERT(!parent->left);
    parent->left = child;
    child->parent = parent;
    update(child);
}

template <class T>
inline void GLDRBTree<T>::attachRight(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child)
{
    Q_ASSERT(!parent->right);
    parent->right = child;
    child->parent = parent;
    update(child);
}

template <class T>
void GLDRBTree<T>::attachBefore(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child)
{
    if (!m_root)
    {
        update(m_root = child);
    }
    else if (!parent)
    {
        attachRight(back(m_root), child);
    }
    else if (parent->left)
    {
        attachRight(back(parent->left), child);
    }
    else
    {
        attachLeft(parent, child);
    }
}

template <class T>
void GLDRBTree<T>::attachAfter(GLDRBTreeNode<T> *parent, GLDRBTreeNode<T> *child)
{
    if (!m_root)
    {
        update(m_root = child);
    }
    else if (!parent)
    {
        attachLeft(front(m_root), child);
    }
    else if (parent->right)
    {
        attachLeft(front(parent->right), child);
    }
    else
    {
        attachRight(parent, child);
    }
}

template <class T>
void GLDRBTree<T>::swapNodes(GLDRBTreeNode<T> *n1, GLDRBTreeNode<T> *n2)
{
    // Since iterators must not be invalidated, it is not sufficient to only swap the data.
    if (n1->parent == n2)
    {
        n1->parent = n2->parent;
        n2->parent = n1;
    }
    else if (n2->parent == n1)
    {
        n2->parent = n1->parent;
        n1->parent = n2;
    }
    else
    {
        qSwap(n1->parent, n2->parent);
    }

    qSwap(n1->left, n2->left);
    qSwap(n1->right, n2->right);
    qSwap(n1->red, n2->red);

    if (n1->parent)
    {
        if (n1->parent->left == n2)
            n1->parent->left = n1;
        else
            n1->parent->right = n1;
    }
    else
    {
        m_root = n1;
    }

    if (n2->parent)
    {
        if (n2->parent->left == n1)
            n2->parent->left = n2;
        else
            n2->parent->right = n2;
    }
    else
    {
        m_root = n2;
    }

    if (n1->left)
        n1->left->parent = n1;
    if (n1->right)
        n1->right->parent = n1;

    if (n2->left)
        n2->left->parent = n2;
    if (n2->right)
        n2->right->parent = n2;
}

template <class T>
void GLDRBTree<T>::detach(GLDRBTreeNode<T> *node) // call this before removing a node.
{
    if (node->right)
    {
        swapNodes(node, front(node->right));
    }

    GLDRBTreeNode<T> *child = (node->left ? node->left : node->right);

    if (!node->red)
    {
        if (child && child->red)
        {
            child->red = false;
        }
        else
        {
            rebalance(node);
        }
    }

    GLDRBTreeNode<T> *&ref = (node->parent ? (node == node->parent->left ? node->parent->left : node->parent->right) : m_root);
    ref = child;
    if (child)
    {
        child->parent = node->parent;
    }
    node->left = node->right = node->parent = 0;
}

// 'node' must be black. rebalance will reduce the depth of black nodes by one in the sibling tree.
template <class T>
void GLDRBTree<T>::rebalance(GLDRBTreeNode<T> *node)
{
    Q_ASSERT(!node->red);
    while (true)
    {
        if (!node->parent) return;

        // at this point, node is not a parent, it is black, thus it must have a sibling.
        GLDRBTreeNode<T> *sibling = (node == node->parent->left ? node->parent->right : node->parent->left);
        Q_ASSERT(sibling);

        if (sibling->red)
        {
            sibling->red = false;
            node->parent->red = true;
            if (node == node->parent->left)
            {
                rotateLeft(node->parent);
            }
            else
            {
                rotateRight(node->parent);
            }
            sibling = (node == node->parent->left ? node->parent->right : node->parent->left);
            Q_ASSERT(sibling);
        }

        // at this point, the sibling is black.
        Q_ASSERT(!sibling->red);

        if ((!sibling->left || !sibling->left->red) && (!sibling->right || !sibling->right->red))
        {
            bool bParentWasRed = node->parent->red;
            sibling->red = true;
            node->parent->red = false;
            if (bParentWasRed)
                return;
            node = node->parent;
            continue;
        }

        // at this point, at least one of the sibling's children is red.

        if (node == node->parent->left)
        {
            if (!sibling->right || !sibling->right->red)
            {
                Q_ASSERT(sibling->left);
                sibling->red = true;
                sibling->left->red = false;
                rotateRight(sibling);

                sibling = sibling->parent;
                Q_ASSERT(sibling);
            }
            sibling->red = node->parent->red;
            node->parent->red = false;

            Q_ASSERT(sibling->right->red);
            sibling->right->red = false;
            rotateLeft(node->parent);
        }
        else
        {
            if (!sibling->left || !sibling->left->red)
            {
                Q_ASSERT(sibling->right);
                sibling->red = true;
                sibling->right->red = false;
                rotateLeft(sibling);

                sibling = sibling->parent;
                Q_ASSERT(sibling);
            }
            sibling->red = node->parent->red;
            node->parent->red = false;

            Q_ASSERT(sibling->left->red);
            sibling->left->red = false;
            rotateRight(node->parent);
        }
        return;
    }
}

template <class T>
inline GLDRBTreeNode<T> *GLDRBTree<T>::front(GLDRBTreeNode<T> *node) const
{
    while (node->left)
        node = node->left;
    return node;
}

template <class T>
inline GLDRBTreeNode<T> *GLDRBTree<T>::back(GLDRBTreeNode<T> *node) const
{
    while (node->right)
        node = node->right;
    return node;
}

template <class T>
GLDRBTreeNode<T> *GLDRBTree<T>::next(GLDRBTreeNode<T> *node) const
{
    if (node->right)
        return front(node->right);
    while (node->parent && node == node->parent->right)
    {
        node = node->parent;
    }
    return node->parent;
}

template <class T>
GLDRBTreeNode<T> *GLDRBTree<T>::previous(GLDRBTreeNode<T> *node) const
{
    if (node->left)
        return back(node->left);
    while (node->parent && node == node->parent->left)
        node = node->parent;
    return node->parent;
}

template <class T>
int GLDRBTree<T>::blackDepth(GLDRBTreeNode<T> *top) const
{
    if (!top)
        return 0;
    int leftDepth = blackDepth(top->left);
    int rightDepth = blackDepth(top->right);
    if (leftDepth != rightDepth)
        return -1;
    if (!top->red)
        ++leftDepth;
    return leftDepth;
}

template <class T>
bool GLDRBTree<T>::checkRedBlackProperty(GLDRBTreeNode<T> *top) const
{
    if (!top)
        return true;
    if (top->left && !checkRedBlackProperty(top->left))
        return false;
    if (top->right && !checkRedBlackProperty(top->right))
        return false;
    return !(top->red && ((top->left && top->left->red) || (top->right && top->right->red)));
}

template <class T>
inline bool GLDRBTree<T>::validate() const
{
    return checkRedBlackProperty(m_root) && blackDepth(m_root) != -1;
}

template <class T>
inline void GLDRBTree<T>::deleteNode(GLDRBTreeNode<T> *&node)
{
    Q_ASSERT(node);
    detach(node);
    node->right = freeList;
    freeList = node;
    node = 0;
}

template <class T>
inline GLDRBTreeNode<T> *GLDRBTree<T>::newNode()
{
    if (freeList) {
        GLDRBTreeNode<T> *node = freeList;
        freeList = freeList->right;
        node->parent = node->left = node->right = 0;
        node->red = true;
        return node;
    }
    return new GLDRBTreeNode<T>;
}

// Return 1 if 'left' comes after 'right', 0 if equal, and -1 otherwise.
// 'left' and 'right' cannot be null.
template <class T>
int GLDRBTree<T>::order(GLDRBTreeNode<T> *left, GLDRBTreeNode<T> *right)
{
    Q_ASSERT(left && right);
    if (left == right)
        return 0;

    GLDVector<GLDRBTreeNode<T> *> leftAncestors;
    GLDVector<GLDRBTreeNode<T> *> rightAncestors;
    while (left) {
        leftAncestors.push_back(left);
        left = left->parent;
    }
    while (right) {
        rightAncestors.push_back(right);
        right = right->parent;
    }
    Q_ASSERT(leftAncestors.back() == m_root && rightAncestors.back() == m_root);

    while (!leftAncestors.empty() && !rightAncestors.empty() && leftAncestors.back() == rightAncestors.back()) {
        leftAncestors.pop_back();
        rightAncestors.pop_back();
    }

    if (!leftAncestors.empty())
        return (leftAncestors.back() == leftAncestors.back()->parent->left ? -1 : 1);

    if (!rightAncestors.empty())
        return (rightAncestors.back() == rightAncestors.back()->parent->right ? -1 : 1);

    // The code should never reach this point.
    Q_ASSERT(!leftAncestors.empty() || !rightAncestors.empty());
    return 0;
}

#endif // GLDRBTREE_H
