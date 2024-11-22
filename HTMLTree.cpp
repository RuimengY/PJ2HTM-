#include "HTMLTree.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

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
    Node *parent = nullptr;
    for (auto [key, value] : id_map)
    {
        if (std::find(value->children.begin(), value->children.end(), node) != value->children.end())
        {
            parent = value;
            break;
        }
    }

    if (parent)
    {
        // 递归删除所有子节点
        std::vector<Node *> stack;
        stack.push_back(node);

        while (!stack.empty())
        {
            Node *current = stack.back();
            stack.pop_back();

            for (Node *child : current->children)
            {
                stack.push_back(child);
            }

            id_map.erase(current->id);
            // delete current;
        }

        // 最后删除父节点的直接子节点
        parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), node), parent->children.end());
    }
}

void HTMLTree::editNodeId(const std::string &oldId, const std::string &newId)
{
    Node *node = findNodeById(oldId);
    if (!node || id_map.count(newId))
    {
        std::cerr << "Error: Invalid old ID or duplicate new ID.\n";
        return;
    }
    id_map.erase(oldId);
    node->id = newId;
    id_map[newId] = node;
}

void HTMLTree::editNodeText(const std::string &id, const std::string &text)
{
    Node *node = findNodeById(id);
    if (!node)
    {
        std::cerr << "Error: Invalid node ID.\n";
        return;
    }
    node->text = text;
}

// 递归打印树
void HTMLTree::printTreeRecursive(Node *node, int depth) const
{
    if (!node)
        return;
    if (node->tag.empty()) // 检查节点标签是否为空
        return;

    std::string indent(depth * 2, ' ');
    std::cout << indent << "<" << node->tag;
    if (!node->id.empty())
        std::cout << " id=\"" << node->id << "\"";
    std::cout << ">";

    if (!node->text.empty())
    {
        std::cout << node->text;
    }

    if (!node->children.empty())
    {
        std::cout << "\n";
        for (Node *child : node->children)
        {
            if (child) // 确保子节点不为空
            {
                printTreeRecursive(child, depth + 1);
            }
        }
        std::cout << indent;
    }

    std::cout << "</" << node->tag << ">\n";
}

// 打印树的入口函数
void HTMLTree::printTree() const
{
    if (root)
    {
        printTreeRecursive(root, 0);
    }
}

// 打印缩进结构
void HTMLTree::printIndentedTree(int indent) const
{
    // 可扩展缩进打印逻辑
    std::cerr << "Not implemented yet.\n";
}
