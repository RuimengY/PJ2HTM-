#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <unordered_map>

struct Node
{
    std::string tag;                                         // 标签名
    std::string id;                                          // 唯一标识
    std::string text;                                        // 节点内容
    std::vector<Node *> children;                            // 子节点
    bool is_self_closing;                                    // 是否为自闭合标签
    std::unordered_map<std::string, std::string> attributes; // 节点的额外属性

    // 构造函数
    Node(const std::string &tag = "",
         const std::string &id = "",
         const std::string &text = "",
         bool self_closing = false);

    // 析构函数
    ~Node();
};

#endif // NODE_H
