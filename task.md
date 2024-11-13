




使用C++和QT编写代码

代码要求：
- 代码要尽可能简化，尽量不使用信号和槽，而是使用函数调用的方法
- 不同功能的代码封装在不同的类中，主函数负责调用这些类的方法
- 日志使用QDebug输出,日志使用中文
- 尽量使用C++标准库，以及QT库，编写的代码要能够跨平台运行
- 代码中要添加必要的中文注释，说明每个函数的功能和用途
- 代码中要添加必要的异常处理，确保程序的健壮性


第一阶段需求：
- 根据SrtParser类编写一个TxtParser类，代码编写至TxtParser.cpp与TxtParser.h中
  TxtParser类中包含TxtInfo结构体，
    TxtInfo结构体包含:
    QString content;       // 原内容
    QString translateContent; // 翻译内容
  TxtParser类包含parse段落解析方法，parse方法先将文件内容根据"."或者空行或者等于最大段落长度MAX_LEN分为多个小段落保存在QList<QString>，
     然后再根据最大段落长度MAX_LEN和最大段落长度MIN_LEN,
     将QList<QString>中的内容要大于MAX_LEN，但要尽可能接近MAX_LEN，将段落解析保存在QList<TxtInfo> 中的content原内容中

- 根据SrtTranslator类编写一个TxtTranslator类，代码编写至TxtTranslator.cpp与TxtTranslator.h中
- 根据StrPrompt类编写一个TxtPrompt类，代码编写至TxtPrompt.cpp与TxtPrompt.h中

