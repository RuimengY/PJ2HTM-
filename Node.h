#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

class Node
{
public:
    std::string tag;              // 标签名
    std::string id;               // 唯一ID
    std::string text;             // 节点文本内容
    std::vector<Node *> children; // 子节点列表

    // 构造函数
    Node(const std::string &tag, const std::string &id = "", const std::string &text = "");

    // 析构函数
    ~Node();
};

#endif // NODE_H
