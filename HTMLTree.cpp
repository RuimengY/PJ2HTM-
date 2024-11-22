#include "HTMLTree.h"
#include <iostream>
#include <iomanip>

// 构造函数
HTMLTree::HTMLTree()
{
    root = new Node("html");
    Node *head = new Node("head");
    Node *title = new Node("title");
    head->children.push_back(title);

    Node *body = new Node("body");
    root->children.push_back(head);
    root->children.push_back(body);

    id_map["html"] = root;
    id_map["head"] = head;
    id_map["title"] = title;
    id_map["body"] = body;
}

// 析构函数
HTMLTree::~HTMLTree()
{
    delete root;
}

// 查找节点
Node *HTMLTree::findNodeById(const std::string &id)
{
    return id_map.count(id) ? id_map[id] : nullptr;
}

// 添加节点
void HTMLTree::addNode(const std::string &tag, const std::string &id, const std::string &parent_id, const std::string &text)
{
    Node *parent = findNodeById(parent_id);
    if (!parent || id_map.count(id))
    {
        std::cerr << "Error: Invalid parent ID or duplicate ID.\n";
        return;
    }
    Node *new_node = new Node(tag, id, text);
    parent->children.push_back(new_node);
    id_map[id] = new_node;
}

// 删除节点
void HTMLTree::deleteNode(const std::string &id)
{
    if (id == "html")
    {
        std::cerr << "Error: Cannot delete root node.\n";
        return;
    }

    Node *node = findNodeById(id);
    if (!node)
    {
        std::cerr << "Error: Node not found.\n";
        return;
    }

    // 从父节点的子节点列表中删除
    for (auto &[key, value] : id_map)
    {
        if (value == node)
        {
            id_map.erase(key);
            break;
        }
    }
    delete node;
}

// 打印树状结构
void HTMLTree::printTree() const
{
    printTreeRecursive(root, 0);
}

// 递归打印树
void HTMLTree::printTreeRecursive(Node *node, int depth) const
{
    if (!node)
        return;
    std::string indent(depth * 2, ' ');
    std::cout << indent << node->tag;
    if (!node->id.empty())
        std::cout << "#" << node->id;
    std::cout << "\n";
    for (Node *child : node->children)
    {
        printTreeRecursive(child, depth + 1);
    }
}

// 打印缩进结构
void HTMLTree::printIndentedTree(int indent) const
{
    // 可扩展缩进打印逻辑
    std::cerr << "Not implemented yet.\n";
}
