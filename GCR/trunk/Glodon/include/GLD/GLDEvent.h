#ifndef GLDEVENT_H
#define GLDEVENT_H

#include <QObject>
#include <QEvent>
#include "GLDSystem.h"
#include "GLDGlobal.h"

const unsigned short c_GM_BASE = 2000;

enum GLDEventType
{
    GM_BASE = c_GM_BASE,
    GM_PREV = c_GM_BASE + 0, // 上一行
    GM_NEXT = c_GM_BASE + 1, // 下一行
    GM_FIRST = c_GM_BASE + 2, // 首行
    GM_Last = c_GM_BASE + 3, // 末行
    GM_MOVEUP = c_GM_BASE + 4, // 上移一行
    GM_MOVEDOWN = c_GM_BASE + 5, // 下移一行
    GM_MOVELEFT = c_GM_BASE + 6, // 左移一列
    GM_MOVERIGHT = c_GM_BASE + 7, // 右移一列
    GM_INSERTCOL = c_GM_BASE + 8, // 插入一列
    GM_DELETECOL = c_GM_BASE + 9, // 删除一列
    GM_INSERTROW = c_GM_BASE + 10, // 插入一行
    GM_DELETEROW = c_GM_BASE + 11, // 删除一行
    GM_DUPLICATEROW = c_GM_BASE + 12, // 复制一行
    GM_ZAP = c_GM_BASE + 13, // 删除所有行
    GM_EXPAND = c_GM_BASE + 14, // 展开
    GM_COLLAPSE = c_GM_BASE + 15, // 折叠
    GM_EXPANDALL = c_GM_BASE + 16, // 展开全部
    GM_COLLAPSEALL = c_GM_BASE + 17, // 折叠全部
    GM_LEVELUP = c_GM_BASE + 18, // 级别上移
    GM_LEVELDOWN = c_GM_BASE + 19, // 级别下移
    GM_SETCOLROW = c_GM_BASE + 20, // 设置行列
    GM_SETSEL = c_GM_BASE + 21, // 全选
    GM_CUT = c_GM_BASE + 22,    // 剪切
    GM_COPY = c_GM_BASE + 23,   // 复制
    GM_PASTE = c_GM_BASE + 24, // 粘贴
    GM_CLEAR = c_GM_BASE + 25, // 删除
    GM_PASTEASCHILD = c_GM_BASE + 26, // 作为子粘贴

    GM_QUERYPREV = c_GM_BASE + 50, // 允许上一行
    GM_QUERYNEXT = c_GM_BASE + 51, // 允许下一行
    GM_QUERYFIRST = c_GM_BASE + 52, // 允许首行
    GM_QUERYLAST = c_GM_BASE + 53, // 允许末行
    GM_QUERYMOVEUP = c_GM_BASE + 54, // 允许上移一行
    GM_QUERYMOVEDOWN = c_GM_BASE + 55, // 允许下移一行
    GM_QUERYMOVELEFT = c_GM_BASE + 56, // 允许左移一列
    GM_QUERYMOVERIGHT = c_GM_BASE + 57, // 允许右移一列
    GM_QUERYINSERTCOL = c_GM_BASE + 58, // 允许插入一列
    GM_QUERYDELETECOL = c_GM_BASE + 59, // 允许删除一列
    GM_QUERYINSERTROW = c_GM_BASE + 60, // 允许插入一行
    GM_QUERYDELETEROW = c_GM_BASE + 61, // 允许删除一行
    GM_QUERYDUPLICATEROW = c_GM_BASE + 62, // 允许复制一行
    GM_QUERYZAP = c_GM_BASE + 63, // 允许删除所有行
    GM_QUERYEXPAND = c_GM_BASE + 64, // 允许展开
    GM_QUERYCOLLAPSE = c_GM_BASE + 65, // 允许折叠
    GM_QUERYEXPANDALL = c_GM_BASE + 66, // 允许展开全部
    GM_QUERYCOLLAPSEALL = c_GM_BASE + 67, // 允许折叠全部
    GM_QUERYLEVELUP = c_GM_BASE + 68, // 允许级别上移
    GM_QUERYLEVELDOWN = c_GM_BASE + 69, // 允许级别下移
    GM_QUERYLESETCOLROW = c_GM_BASE + 70, // 允许设置行列
    GM_QUERYSETSEL = c_GM_BASE + 71, // 允许全选
    GM_QUERYCUT = c_GM_BASE + 72, // 允许剪切
    GM_QUERYCOPY = c_GM_BASE + 73, // 允许复制
    GM_QUERYPASTE = c_GM_BASE + 74, // 允许粘贴
    GM_QUERYCLEAR = c_GM_BASE + 75, // 允许删除
    GM_QUERYPASTEASCHILD = c_GM_BASE + 76, // 允许作为子粘贴
    GM_DELAYSETRECORD = c_GM_BASE + 77, // 延迟设置当前record

    GM_SetScrollBarStep = c_GM_BASE + 101,
    GM_DocViewCalculatePage = c_GM_BASE + 102,

    GM_GSPBASE = c_GM_BASE + 1000,
    GM_GTPBASE = c_GM_BASE + 1500,
    GM_APPBASE = c_GM_BASE + 2000
};

typedef PtrInt GLDWParam;
typedef PtrInt GLDLParam;
typedef PtrInt GLDEventResult;

class GLDCOMMONSHARED_EXPORT GLDEvent : public QEvent
{
public:
    GLDEvent(GLDEventType type, GLDWParam wParam = 0, GLDLParam lParam = 0);

    GLDWParam wParam() const;
    GLDWParam lParam() const;

    GLDEventType type() const;
    GLDEventResult result() const;
    void setResult(GLDEventResult value);
private:
    GLDWParam m_wParam;
    GLDLParam m_lParam;
    GLDEventResult m_result;
};

#endif // GLDEVENT_H
