/*!
 * @file      GLDScopeExit.h
 *
 * Copyright (C) 1998-2015 Glodon Corporation
 *
 * @author    xg
 * @date      2015/04/17
 * @version   v 1.0.0.0
 * @brief     退出作用域进行的操作
 *            使用方式如下, 当退出作用域(即作用域结束)时会调用到 delete pInt; 语句释放内存
 *            //!在下面的作用域
 *            {//作用域开始
 *                int *pInt = new int(0);
 *                GLD_SCOPEEXIT
 *                {
 *                    delete pInt;
 *                    pInt = nullptr;
 *                };
 *            }//作用域结束
 */
#ifndef __D47C9781_7DE3_4B0E_897B_AC02646636FA_GLDSCOPEEXIT_H__
#define __D47C9781_7DE3_4B0E_897B_AC02646636FA_GLDSCOPEEXIT_H__

template <class ReleaseFun>
class GLDScopeGuard
{
public:
    GLDScopeGuard(ReleaseFun fun)
        : m_releaseFun(fun)
    {}
    ~GLDScopeGuard()
    {
        m_releaseFun();
    }

private:
    ReleaseFun m_releaseFun;
};

class GLDScopeGuardOnExit{};
template <typename ReleaseFun>
GLDScopeGuard<ReleaseFun> operator+(GLDScopeGuardOnExit, ReleaseFun &&fn)
{
    return GLDScopeGuard<ReleaseFun>(fn);
}

#define _GLD_JOINT(s1, s2) s1##s2
#define GLD_JOINT(s1, s2) _GLD_JOINT(s1, s2)
#define GLD_VARIABLE(str) GLD_JOINT(str, __LINE__)

#define GLD_SCOPEEXIT \
    auto GLD_VARIABLE(SCOPE_EXIT_STATE) = GLDScopeGuardOnExit() + [&]()

#endif /* __D47C9781_7DE3_4B0E_897B_AC02646636FA_GLDSCOPEEXIT_H__ */
