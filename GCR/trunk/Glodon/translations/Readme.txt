公共资源最近增加了英文功能。
若想使用英文，默认地，我们不需要作任何修改。
若想使用中文，我们需要做如下步骤:

1.在main函数中声明定义了QApplication后，调用如下语句:
    GLDTranslations cbbtr;
    cbbtr.loadConfigFile(exePath() + "translations");
    cbbtr.installTranslators();
    其中translations是公共资源新增加的文件夹。loadConfigFile的参数就是该文件夹的路径。

2.增加对应的头文件包含
    #include "GLDTranslations.h"
    #include "GLDFileUtils.h" //不使用exePath()则不必包含

