




代码优化需求：
- 重构MdParser::splitIntoParagraphs方法，
  1,通过Markdown的语法规则，将文件内容通过标题切割成段落，两个标题之前包含标题为一个段落
  2,根据句号切割段落
  3,根据空行切割段落
  4,注意使用splitIntoParagraphs切割processParagraphs合并后的内容与直接从文件中读取一致，包括空行与格式都要一致
  5,processParagraphs无需修改，只需要重构splitIntoParagraphs方法


以下为运行日志，当前分段异常，优化代码：
"[14:04:33] === 开始翻译任务 ==="
"[14:04:33] Markdown文件: /home/csbobo/my_code/TxtLLMTranslate/build/Desktop_Qt_6_7_2-Debug/inputDir/README.md"
"[14:04:33] 翻译模型: qwen2.5:3b-instruct-q8_0"
"[14:04:33] "
分段完成，总段落数: 26
---段落详细信息---
段落 1 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 2 :
内容: "\"### Unix-based systems (Linux, macOS, etc.):\""
长度: 44
是否为空行: false
----------------
段落 3 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 4 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 5 :
内容: "\"fdsafdsafdsafdsafsdafdsafdsafdsafdsd.\""
长度: 37
是否为空行: false
----------------
段落 6 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 7 :
内容: "\"##### Input prompt (One-and-done)\""
长度: 33
是否为空行: false
----------------
段落 8 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 9 :
内容: "\"fdsafdsa\""
长度: 8
是否为空行: false
----------------
段落 10 :
内容: "\"```bash\""
长度: 7
是否为空行: false
----------------
段落 11 :
内容: "\"./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf --prompt \"Once upon a time\"\""
长度: 77
是否为空行: false
----------------
段落 12 :
内容: "\"```\""
长度: 3
是否为空行: false
----------------
段落 13 :
内容: "\"##### Conversation mode (Allow for continuous interaction with the model)\""
长度: 73
是否为空行: false
----------------
段落 14 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 15 :
内容: "\"dsfaaf\""
长度: 6
是否为空行: false
----------------
段落 16 :
内容: "\"```bash\""
长度: 7
是否为空行: false
----------------
段落 17 :
内容: "\"./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf -cnv --chat-template gemma\""
长度: 76
是否为空行: false
----------------
段落 18 :
内容: "\"```\""
长度: 3
是否为空行: false
----------------
段落 19 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 20 :
内容: "\"##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):\""
长度: 77
是否为空行: false
----------------
段落 21 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
段落 22 :
内容: "\"afsdfsadfdsa\""
长度: 12
是否为空行: false
----------------
段落 23 :
内容: "\"```bash\""
长度: 7
是否为空行: false
----------------
段落 24 :
内容: "\"./llama-cli -m models\\gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1\""
长度: 68
是否为空行: false
----------------
段落 25 :
内容: "\"```\""
长度: 3
是否为空行: false
----------------
段落 26 :
内容: "\"\""
长度: 0
是否为空行: true
----------------
初始分段完成，共 26 个段落

以下为原始文本：

### Unix-based systems (Linux, macOS, etc.):


fdsafdsafdsafdsafsdafdsafdsafdsafdsd.

##### Input prompt (One-and-done)

fdsafdsa
```bash
./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf --prompt "Once upon a time"
```
##### Conversation mode (Allow for continuous interaction with the model)

dsfaaf
```bash
./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf -cnv --chat-template gemma
```

##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):

afsdfsadfdsa
```bash
./llama-cli -m models\gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1
```


希望分段的结果为：

第一段：
### Unix-based systems (Linux, macOS, etc.):


fdsafdsafdsafdsafsdafdsafdsafdsafdsd.

第二段：
##### Input prompt (One-and-done)

fdsafdsa
```bash
./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf --prompt "Once upon a time"
```

第三段：
##### Conversation mode (Allow for continuous interaction with the model)

dsfaaf
```bash
./llama-cli -m models/gemma-1.1-7b-it.Q4_K_M.gguf -cnv --chat-template gemma
```

第四段：
##### Infinite text from a starting prompt (you can use `Ctrl-C` to stop it):

afsdfsadfdsa
```bash
./llama-cli -m models\gemma-1.1-7b-it.Q4_K_M.gguf --ignore-eos -n -1
```
