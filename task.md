




参考MdParser,MdPrompt,MdTranslator,编写RST格式的翻译相关类:

根据RST的语法规则，重构splitIntoParagraphs函数：
1. 按行分割内容，当行末尾为句号时，将该行作为段落结束标志
2. 通过RST的语法规则，当遇到标题时，将标题作为段落结束标志（标题一般有两行，实际标题行和语法行）（参考reference.cpp中的代码）
3. 保留所有的空行，确保原始内容与保存内容一致
4. 当段落长度超过m_maxLens时，以行为单位切割为一个段落


以下是示例：

[示例开始]
.. _dlist_api:

Double-linked List
==================

Similar to the single-linked list in many respects, Zephyr includes a
double-linked implementation.


Double-linked List Internals
----------------------------

* An empty list has backpointers to itself in the list struct, which
  can be trivially detected.

* The head and tail of the list can be detected by comparing the
  prev/next pointers of a node vs. the list struct address.

.. figure:: dlist-empty.png
    :align: center
    :alt: dlist example
    :figclass: align-center

    An empty dlist.


Doubly-linked List API Reference
--------------------------------

.. doxygengroup:: doubly-linked-list_apis
[示例结束]


以上示例应该被分为以下段落：

[段落1开始]
.. _dlist_api:

[段落1结束]

[段落2开始]
Double-linked List
==================

Similar to the single-linked list in many respects, Zephyr includes a
double-linked implementation.

[段落2结束]

[段落3开始]


[段落3结束]

[段落4开始]
Double-linked List Internals
----------------------------

* An empty list has backpointers to itself in the list struct, which
  can be trivially detected.
[段落4结束]

[段落5开始]

* The head and tail of the list can be detected by comparing the
  prev/next pointers of a node vs. the list struct address.
[段落5结束]


[段落6开始]

.. figure:: dlist-empty.png
    :align: center
    :alt: dlist example
    :figclass: align-center

    An empty dlist.
[段落6结束]

[段落7开始]


[段落7结束]

[段落8开始]


Doubly-linked List API Reference
--------------------------------

.. doxygengroup:: doubly-linked-list_apis
[段落8结束]
