# AI翻译工具

一个基于Qt的跨平台翻译工具，支持多种文件格式的批量翻译，可以使用OpenAI API或本地LLM模型进行翻译。

## 功能特点

- 支持多种文件格式:
  - TXT 文本文件
  - SRT 字幕文件
  - Markdown (MD) 文档
  - reStructuredText (RST) 文档
- 支持批量翻译，自动保持原始目录结构
- 支持OpenAI API和本地LLM模型
- 可自定义翻译提示词
- 智能分段翻译，可调节段落长度
- 断点续传功能
- 实时显示翻译进度和日志
- 保留翻译历史供参考
- 支持原文与译文对照显示

## 系统要求

- 支持的操作系统：Windows、Linux、macOS
- Qt 6.2.0 或更高版本
- CMake 3.16 或更高版本

## 使用说明

### 基本设置

1. AI服务设置
   - OpenAI API设置:
     - 填写API Key
     - 设置API URL (默认为本地服务地址: http://127.0.0.1:11434/v1/chat/completions)
     - 选择模型 (如: qwen2.5:3b-instruct-q8_0)
     - 点击"测试连接"验证设置
   
   - 本地AI服务:
     - 选择模型文件路径(.gguf格式)
     - 设置主机地址(默认: 127.0.0.1)和端口(默认: 11434)
     - 可调整温度、上下文大小、NGL等参数
     - 勾选"使用本地AI服务"启动服务，点击"测试连接"验证（注意端口是否一致）

2. 翻译设置
   - 选择输入文件夹(包含待翻译文件)
   - 选择输出文件夹(存放翻译结果)
   - 支持的文件类型(TXT/SRT/MD/RST)将会翻译，其他文件类型会直接复制到输出目录
   - 可选择是否保留历史记录用于参考
   - 可自定义段落长度限制(最大/最小长度)

### 翻译操作

1. 编辑提示词
   - 点击"编辑提示"按钮
   - 可为不同文件类型(TXT/SRT/MD/RST)设置专门的翻译提示词
   - 支持设置系统提示词和用户提示词

2. 开始翻译
   - 点击"开始翻译"按钮
   - 实时显示:
     - 当前翻译文件信息
     - 总体翻译进度
     - 原文与译文对照
     - 运行日志
   - 支持断点续传，可从上次中断处继续
   - 翻译完成后自动清除进度记录

3. 翻译进度
   - 显示总任务数和当前进度
   - 显示每个文件的翻译状态
   - 可手动清除翻译进度记录
   - 支持查看原文与译文实时对照

### 注意事项

- 翻译过程中保持网络连接稳定
- 大文件建议适当调整段落长度:
  - 最大长度必须大于最小长度
- 建议定期备份重要的翻译结果
- 如使用本地AI服务，请确保:
  - 系统资源充足
  - 模型文件(.gguf格式)有效
  - 端口未被占用

## 常见问题

1. 翻译中断
   - 程序会自动保存进度
   - 重启后点击"开始翻译"选择"从中断处继续"即可
   - 也可手动清除进度重新开始

2. 文件格式问题
   - 确保文件编码为UTF-8
   - 检查文件格式是否符合标准
   - 不支持的文件格式会自动复制到输出目录

3. 性能优化
   - 调整段落长度限制
   - 如果使用本地AI服务，合理设置上下文大小
   - 翻译效果不佳可不保留历史记录
   - 尝试各种模型，选择最适合的模型，注意模型的上下文限制

## 许可证

本项目采用 GPL 许可证