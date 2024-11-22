#ifndef HTMLTREE_H
#define HTMLTREE_H

#include "Node.h"
#include <string>
#include <unordered_map>

class HTMLTree
{
private:
    Node *root;                                     // HTML树的根节点
    std::unordered_map<std::string, Node *> id_map; // ID到节点的映射表

    // 递归打印树
    void printTreeRecursive(Node *node, int depth) const;

public:
    // 构造函数
    HTMLTree();

    // 析构函数
    ~HTMLTree();

    // 查找节点
    Node *findNodeById(const std::string &id);

    // 添加节点
    void addNode(const std::string &tag, const std::string &id, const std::string &parent_id, const std::string &text = "");

    // 删除节点
    void deleteNode(const std::string &id);

    // 打印树状结构
    void printTree() const;

    // 打印缩进结构
    void printIndentedTree(int indent = 2) const;
};

#endif // HTMLTREE_H