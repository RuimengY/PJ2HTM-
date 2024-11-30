#include "HTMLTree.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stack>
#include <regex>

// 构造函数
HTMLTree::HTMLTree()
{
    root = new Node("html", "html");
    Node *head = new Node("head", "head");
    Node *title = new Node("title", "title");
    head->children.push_back(title);

    Node *body = new Node("body", "body");
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

// 添加标题内容
void HTMLTree::appendTitle(const std::string &text)
{
    if (id_map.find("title") != id_map.end())
    {
        id_map["title"]->text = text;
    }
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
            delete current;
        }

        // 最后删除父节点的直接子节点
        parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), node), parent->children.end());
    }
}

// 添加文本到指定节点
void HTMLTree::appendText(const std::string &id, const std::string &text)
{
    if (id_map.find(id) != id_map.end())
    {
        // 如果原文本为空，直接赋值；否则添加换行后再追加
        if (id_map[id]->text.empty())
        {
            id_map[id]->text = text;
        }
        else
        {
            id_map[id]->text += "\n" + text;
        }
    }
    else
    {
        std::cerr << "Error: Node with ID " << id << " not found.\n";
    }
}

void HTMLTree::editNodeId(const std::string &oldId, const std::string &newId)
{
    Node *node = findNodeById(oldId);
    if (!node)
    {
        std::cerr << "Error: Node with ID " << oldId << " not found.\n";
        return;
    }

    if (id_map.count(newId))
    {
        std::cerr << "Error: Duplicate ID " << newId << " already exists.\n";
        return;
    }

    // 更新 ID
    id_map.erase(oldId);
    node->id = newId;
    id_map[newId] = node;

    std::cout << "Node ID updated from " << oldId << " to " << newId << ".\n";
}

void HTMLTree::editNodeText(const std::string &id, const std::string &text)
{
    Node *node = findNodeById(id);
    if (!node)
    {
        std::cerr << "Error: Node with ID " << id << " not found.\n";
        return;
    }

    node->text = text;
    std::cout << "Node with ID " << id << " updated text to: " << text << ".\n";
}

void HTMLTree::removeText(const std::string &id)
{
    Node *node = findNodeById(id);
    if (!node)
    {
        std::cerr << "Error: Node with ID " << id << " not found.\n";
        return;
    }

    node->text.clear();
    std::cout << "Text cleared for Node with ID " << id << ".\n";
}

std::vector<Node *> HTMLTree::findNodesByTag(const std::string &tag) const
{
    std::vector<Node *> result;
    std::stack<Node *> stack;
    stack.push(root);

    while (!stack.empty())
    {
        Node *current = stack.top();
        stack.pop();

        if (current->tag == tag)
        {
            result.push_back(current);
        }

        // Push children in reverse order to maintain order
        for (auto it = current->children.rbegin(); it != current->children.rend(); ++it)
        {
            stack.push(*it);
        }
    }

    return result;
}

void HTMLTree::printNodeInfo(Node *node, std::ostream &output) const
{
    if (!node)
    {
        output << "Node not found.\n";
        return;
    }

    output << "Tag: " << node->tag << "\n";
    output << "ID: " << node->id << "\n";

    if (!node->text.empty())
    {
        output << "Text: " << node->text << "\n";
    }

    Node *parent = findParentNode(node->id);
    if (parent)
    {
        output << "Parent Tag: " << parent->tag << "\n";
    }
}

Node *HTMLTree::findParentNode(const std::string &childId) const
{
    for (auto &[key, value] : id_map)
    {
        auto it = std::find_if(value->children.begin(), value->children.end(),
                               [&childId](Node *child)
                               { return child->id == childId; });
        if (it != value->children.end())
        {
            return value;
        }
    }
    return nullptr;
}

void HTMLTree::insertNodeBefore(const std::string &tag, const std::string &id,
                                const std::string &beforeId, const std::string &text)
{
    if (id_map.count(id))
    {
        std::cerr << "Error: Node ID already exists.\n";
        return;
    }

    Node *before_node = findNodeById(beforeId);
    if (!before_node)
    {
        std::cerr << "Error: Before node not found.\n";
        return;
    }

    Node *parent = findParentNode(beforeId);
    if (!parent)
    {
        std::cerr << "Error: Parent node not found.\n";
        return;
    }

    Node *new_node = new Node(tag, id, text);

    // Find and insert before the specific node
    auto it = std::find(parent->children.begin(), parent->children.end(), before_node);
    if (it != parent->children.end())
    {
        parent->children.insert(it, new_node);
    }

    id_map[id] = new_node;
}

void HTMLTree::printTree() const
{
    printTreeRecursive(root, 0, 2);
}

void HTMLTree::printTree(int indent) const
{
    if (root)
    {
        printTreeRecursive(root, 0, indent);
    }
}

void HTMLTree::printTreeRecursive(Node *node, int depth, int indent) const
{
    if (!node)
        return;
    if (node->tag.empty())
        return;

    std::string indentation(depth * indent, ' ');
    std::cout << indentation << "<" << node->tag << " id=\"" << node->id << "\"";

    // Print other attributes
    for (const auto &attr : node->attributes)
    {
        if (attr.first != "id")
        {
            std::cout << " " << attr.first << "=\"" << attr.second << "\"";
        }
    }

    // Self-closing tag handling
    if (node->is_self_closing)
    {
        std::cout << " />\n";
        return;
    }

    std::cout << ">";

    // Improved text content printing
    if (!node->text.empty())
    {
        std::istringstream iss(node->text);
        std::string line;
        bool first_line = true;

        while (std::getline(iss, line))
        {
            if (!first_line)
                std::cout << indentation << std::string(indent, ' ');
            std::cout << line;
            first_line = false;
        }
    }

    // Print children
    if (!node->children.empty())
    {
        std::cout << "\n";
        for (Node *child : node->children)
        {
            if (child)
                printTreeRecursive(child, depth + 1, indent);
        }
        std::cout << indentation;
    }

    // Print closing tag
    std::cout << "</" << node->tag << ">\n";
}

void HTMLTree::parseHtmlToCommands(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << path << ".\n";
        return;
    }

    // 清空当前树
    delete root;
    id_map.clear();

    // 初始化根节点
    std::stack<std::pair<Node *, std::string>> node_stack;
    root = new Node("html", "html");
    id_map["html"] = root;
    node_stack.push({root, "html"});
    std::string line;

    while (std::getline(file, line))
    {
        // 如果是"<html>"标签，直接跳过
        if (line.find("<html>") != std::string::npos)
            continue;
        // 去掉行两端的空格
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty())
            continue;
        if (line.substr(0, 4) == "<!--")
            continue;

        if (line.substr(0, 2) == "</")
        {
            if (!node_stack.empty())
                node_stack.pop();
            continue;
        }

        if (line[0] != '<')
        {
            // 处理标签外部的文本内容
            Node *parent_node = node_stack.top().first;
            parent_node->text += line;
            continue;
        }

        // 处理起始标签
        size_t end_tag_pos = line.find('>');
        if (end_tag_pos == std::string::npos)
            continue;

        std::string tag_content = line.substr(1, end_tag_pos - 1);
        bool is_self_closing = (tag_content.back() == '/');
        if (is_self_closing)
            tag_content.pop_back();

        // 提取标签名和属性
        std::istringstream tag_stream(tag_content);
        std::string tag, attr;
        tag_stream >> tag;

        std::unordered_map<std::string, std::string> attributes;
        std::string id_value = tag; // 默认ID为标签名
        while (tag_stream >> attr)
        {
            size_t eq_pos = attr.find('=');
            if (eq_pos != std::string::npos)
            {
                std::string key = attr.substr(0, eq_pos);
                std::string value = attr.substr(eq_pos + 1);
                if (!value.empty() && value.front() == '"' && value.back() == '"')
                {
                    value = value.substr(1, value.size() - 2);
                }

                // 特别处理ID属性
                if (key == "id")
                {
                    id_value = value;
                }
                attributes[key] = value;
            }
        }

        Node *parent_node = node_stack.top().first;

        // 如果是自闭合标签，直接创建节点并跳过后续处理
        if (is_self_closing)
        {
            Node *new_node = new Node(tag, id_value, "", true);
            new_node->attributes = attributes; // 保留所有属性
            parent_node->children.push_back(new_node);
            id_map[new_node->id] = new_node;
            continue;
        }

        // 创建新节点
        Node *new_node = new Node(tag, id_value, "");
        new_node->attributes = attributes; // 保留所有属性

        // 添加新节点到父节点
        parent_node->children.push_back(new_node);
        id_map[new_node->id] = new_node;

        // 处理标签后文本
        if (end_tag_pos + 1 < line.size())
        {
            size_t text_start = end_tag_pos + 1;
            size_t text_end = line.find('<', text_start);
            if (text_end == std::string::npos)
                text_end = line.size();

            std::string text_content = line.substr(text_start, text_end - text_start);
            text_content.erase(0, text_content.find_first_not_of(" \t"));
            text_content.erase(text_content.find_last_not_of(" \t") + 1);

            // 如果提取到的文本非空，设置到当前节点
            if (!text_content.empty())
                new_node->text = text_content;
        }

        // 如果不是自闭合标签，入栈
        if (!is_self_closing && line.find("</") == std::string::npos)
        {
            node_stack.push({new_node, new_node->id});
        }
    }

    file.close();
}

void HTMLTree::saveToFile(const std::string &path)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << path << ".\n";
        return;
    }

    saveNodeToFile(root, file, 0);
    file.close();
    std::cout << "HTML saved successfully to " << path << ".\n";
}

void HTMLTree::saveNodeToFile(Node *node, std::ostream &output, int depth) const
{
    if (!node)
        return;

    std::string indent(depth * 2, ' ');

    // 打开标签
    output << indent << "<" << node->tag;

    // 始终添加id属性
    output << " id=\"" << node->id << "\"";

    // 添加其他属性
    for (const auto &attr : node->attributes)
    {
        // 排除重复的ID
        if (attr.first != "id")
        {
            output << " " << attr.first << "=\"" << attr.second << "\"";
        }
    }

    // 处理自闭合标签
    if (node->is_self_closing)
    {
        output << " />\n";
        return;
    }

    output << ">";

    // 添加文本内容
    if (!node->text.empty())
        output << node->text;

    // 递归处理子节点
    if (!node->children.empty())
    {
        output << "\n";
        for (Node *child : node->children)
        {
            saveNodeToFile(child, output, depth + 1);
        }
        output << indent;
    }

    // 关闭标签
    output << "</" << node->tag << ">\n";
}