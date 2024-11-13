




使用C++和QT编写代码

代码要求：
- 代码要尽可能简化，尽量不使用信号和槽，而是使用函数调用的方法
- 不同功能的代码封装在不同的类中，主函数负责调用这些类的方法
- 日志使用QDebug输出,日志使用中文
- 尽量使用C++标准库，以及QT库，编写的代码要能够跨平台运行
- 代码中要添加必要的中文注释，说明每个函数的功能和用途
- 代码中要添加必要的异常处理，确保程序的健壮性


第一阶段需求：
- 根据StrPrompt类编写一个FilePrompt类，将FilePrompt类作为StrPrompt类的父类，代码编写至FilePrompt.cpp与FilePrompt.h中
  FilePrompt类包含save，load，getPrompt,changePrompt，resetToDefault,getPromptPath的虚方法，由子类实现
- 根据SrtTranslator类编写一个FileTranslator类，将FileTranslator类作为SrtTranslator类的父类，代码编写至FileTranslator.cpp与FileTranslator.h中
  FileTranslator类包含translate,setLog的虚方法，由子类实现
