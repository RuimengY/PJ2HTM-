#include "Node.h"

// 构造函数
Node::Node(const std::string &tag, const std::string &id, const std::string &text)
    : tag(tag), id(id), text(text) {}

// 析构函数，递归释放子节点
Node::~Node()
{
    for (Node *child : children)
    {
        delete child;
    }
}