




优化代码，目前原始内容与保存内容不一致，优化代码确保：

将空行作为段落的结束标志，并将空行包含在前一个段落中。
按行分割内容，当行末尾为句号时，将该行作为段落结束标志

1. 保留所有的空行，确保原始内容与保存内容一致
2. 分段正确
3. processParagraphs无需修改
4. 通过Markdown的语法规则，将文件内容通过标题切割成段落，两个标题之前包含标题为一个段落
5. 根据句号切割段落
6. 当段落长度超过m_maxLens时，以行为单位切割为一个段落
7. 不进行代码块处理

"[14:57:45] === 开始翻译任务 ==="
"[14:57:45] Markdown文件: /home/csbobo/my_code/TxtLLMTranslate/build/Desktop_Qt_6_7_2-Debug/inputDir/README.md"
"[14:57:45] 翻译模型: qwen2.5:3b-instruct-q8_0"
"[14:57:45] "
原始内容: "\n### Unix-based systems (Linux, macOS, etc.):\n\n\nfdsafdsafdsafdsafsdafdsafdsafdsafdsd.\n\n##### Input prompt (One-and-done)\n\nfdsafdsa\n```bash\n./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf --prompt \"Once upon a time\"\n```\n##### Conversation mode (Allow for continuous interaction with the model)\n\ndsfaaf\n```bash\n./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf -cnv --chat-template gemma\n```\n\n##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):\n\nafsdfsadfdsa\n```bash\n./llama-cli -m models\\gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1\n```\n"
分段完成，总段落数: 17
---段落详细信息---
段落 1 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 2 :
内容: "### Unix-based systems (Linux, macOS, etc.):\n"
长度: 45
是否为空行: false
----------------
段落 3 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 4 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 5 :
内容: "fdsafdsafdsafdsafsdafdsafdsafdsafdsd.\n"
长度: 38
是否为空行: false
----------------
段落 6 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 7 :
内容: "##### Input prompt (One-and-done)\n"
长度: 34
是否为空行: false
----------------
段落 8 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 9 :
内容: "fdsafdsa\n```bash\n /llama-cli -m models/gemma-1. 1-7b-it. Q4_K_M. gguf --prompt \"Once upon a time\"\n```\n"
长度: 102
是否为空行: false
----------------
段落 10 :
内容: "##### Conversation mode (Allow for continuous interaction with the model)\n"
长度: 74
是否为空行: false
----------------
段落 11 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 12 :
内容: "dsfaaf\n```bash\n /llama-cli -m models/gemma-1. 1-7b-it. Q4_K_M. gguf -cnv --chat-template gemma\n```\n"
长度: 99
是否为空行: false
----------------
段落 13 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 14 :
内容: "##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):\n"
长度: 78
是否为空行: false
----------------
段落 15 :
内容: "\n"
长度: 1
是否为空行: true
----------------
段落 16 :
内容: "afsdfsadfdsa\n```bash\n /llama-cli -m models\\gemma-1. 1-7b-it. Q4_K_M. gguf --ignore-eos -n -1\n```\n"
长度: 97
是否为空行: false
----------------
段落 17 :
内容: "\n"
长度: 1
是否为空行: true
----------------
初始分段完成，共 17 个段落

"[14:57:45] -------------------"
成功保存MD文件: "/home/csbobo/my_code/TxtLLMTranslate/build/Desktop_Qt_6_7_2-Debug/outputDir/README.md"
保存内容: "\n### Unix-based systems (Linux, macOS, etc.):\n\n\nfdsafdsafdsafdsafsdafdsafdsafdsafdsd.\n\n##### Input prompt (One-and-done)\n\nfdsafdsa\n```bash\n /llama-cli -m models/gemma-1. 1-7b-it. Q4_K_M. gguf --prompt \"Once upon a time\"\n```\n##### Conversation mode (Allow for continuous interaction with the model)\n\ndsfaaf\n```bash\n /llama-cli -m models/gemma-1. 1-7b-it. Q4_K_M. gguf -cnv --chat-template gemma\n```\n\n##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):\n\nafsdfsadfdsa\n```bash\n /llama-cli -m models\\gemma-1. 1-7b-it. Q4_K_M. gguf --ignore-eos -n -1\n```\n\n"
"[14:57:45] \n=== 翻译任务完成 ==="

目前段落分割异常，正常应该分为5个段落，修复bug