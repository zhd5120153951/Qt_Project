#ifndef GLDENUM
#define GLDENUM

enum GLDValueRelationship
{
    gvrLessThanValue = -1,
    gvrEqualsValue = 0,
    gvrGreaterThanValue = 1
};

// TableView.ItemDelegate选项
enum GEditStyle
{
    esNone,                     // 不能编辑
    esSimple,                   // 默认编辑框,根据数据类型创建编辑方式
    esPlainEdit,                // 多行编辑框
    esLineEdit,                 // 单行编辑框
    esPlainEllipsis,            // 省略号按钮
    esLineEllipsis,             // 省略号按钮，单行编辑
    esMultiButton,              // 多个按钮
    esBool,                     // 布尔类型的格子
    esMonthCalendar,            // 日期选择框
    esImage,                    // 图片
    esVectorGraph,              // 矢量图
    esFontList,                 // 字体选择框
    esColorList,                // 颜色选择列表
    esLineWidthList,            // 线框选择列表
    esUpDown,                   // 增量设置框
    esDropDown,                 // 下拉选择框
    esTreeView,                 // 树形框
    esDropDownWindow,           // 下拉的浮动窗体
    esDropDownWindowEllipsis,   // 下拉的浮动窗体, button的样式为三个点
    esDropDownWindowNone        // 下拉的浮动窗体, 不显示button
};

// GridSetting选项
enum GLDGridSettingEditStyle
{
    gesAuto,
    gesEdit,
    gesDropDownList,
    gesDropDown,
    gesEllipsisReadOnly,
    gesEllipsis,
    gesDateTimePicker,
    gesTreeView,
    gesCheckBox,
    gesDropDownWindowReadOnly,
    gesSpin,
    gesDropDownWindow,
    gesLineEdit
};

enum FontStyle
{
    fsBold = 0x01,
    fsItalic = 0x02,
    fsUnderline = 0x04,
    fsStrikeOut = 0x08
};

//MoveCurrent的原因
enum MoveReason { mrKey, mrMouse, mrMouseWheel, mrProgram };

typedef GLDGridSettingEditStyle GLDEditStyle;

#endif // GLDENUM

