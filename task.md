




使用C++和QT编写代码

代码要求：
- 代码要尽可能简化，尽量不使用信号和槽，而是使用函数调用的方法
- 不同功能的代码封装在不同的类中，主函数负责调用这些类的方法
- 日志使用QDebug输出,日志使用中文
- 尽量使用C++标准库，以及QT库，编写的代码要能够跨平台运行
- 代码中要添加必要的中文注释，说明每个函数的功能和用途
- 代码中要添加必要的异常处理，确保程序的健壮性


第一阶段需求：
- 根据TxtParser类编写一个MdParser类，代码编写至MdParser.cpp与MdParser.h中
  MdParser类中包含MdInfo结构体，
    MdInfo结构体包含:
    QString content;       // 原内容
    QString translateContent; // 翻译内容
  MdParser类包含parse段落解析方法，splitIntoParagraphs方法增加根据Markdown的语法将文件内容根据标题切割段落，其他切割与TxtParser类中splitIntoParagraphs方法一致，
     processParagraphs保持与TxtParser类中的processParagraphs方法一致，
      确保切割合并后的content与直接从文件中读取一致

- 根据TxtTranslator类编写一个MdTranslator类，代码编写至MdTranslator.cpp与MdTranslator.h中
- 根据TxtPrompt类编写一个MdPrompt类，代码编写至MdPrompt.cpp与MdPrompt.h中
- 在TabTranslation类中添加相关调用


