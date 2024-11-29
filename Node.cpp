#include "Node.h"

// 构造函数实现
Node::Node(const std::string &tag, const std::string &id, const std::string &text, bool self_closing)
    : tag(tag), id(id), text(text), is_self_closing(self_closing) {}

// 析构函数，递归释放子节点
Node::~Node()
{
    for (Node *child : children)
    {
        delete child;
    }
}
