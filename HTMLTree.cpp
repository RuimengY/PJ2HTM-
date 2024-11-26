#include "HTMLTree.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <functional>
#include <stack>

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
            // delete current;
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
        id_map[id]->text += text;
    }
    else
    {
        std::cerr << "Error: Node with ID " << id << " not found.\n";
    }
}

// 从指定节点删除文本
void HTMLTree::removeText(const std::string &id)
{
    if (id_map.find(id) != id_map.end())
    {
        id_map[id]->text.clear();
    }
    else
    {
        std::cerr << "Error: Node with ID " << id << " not found.\n";
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

/*
void HTMLTree::read(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file.\n";
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    GumboOutput *output = gumbo_parse(content.c_str());
    std::function<void(GumboNode *, Node *)> parseNode = [&](GumboNode *gumbo_node, Node *parent)
    {
        if (gumbo_node->type != GUMBO_NODE_ELEMENT)
        {
            return;
        }

        GumboElement *element = &gumbo_node->v.element;
        std::string tag = gumbo_normalized_tagname(element->tag);
        std::string id = "";

        GumboAttribute *id_attr = gumbo_get_attribute(&element->attributes, "id");
        if (id_attr)
        {
            id = id_attr->value;
        }

        std::string text = ""; // 假设你有获取文本内容的逻辑

        // 使用已有的构造函数创建Node对象
        Node *node = new Node(tag, id, text);

        // 建立子树关系
        if (parent)
        {
            parent->children.push_back(node);
        }
        else
        {
            root = node; // 如果没有父节点，则这是根节点
        }

        // 将节点添加到id_map中
        id_map[id] = node;

        // 递归解析子节点
        GumboVector *children = &element->children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            parseNode(static_cast<GumboNode *>(children->data[i]), node);
        }
    };

    parseNode(output->root, nullptr);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}



void HTMLTree::save(const std::string &path) const
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file.\n";
        return;
    }

    std::function<void(std::ofstream &, Node *, int)> saveNode = [&](std::ofstream &out, Node *node, int indent)
    {
        for (int i = 0; i < indent; ++i)
        {
            out << "  ";
        }
        out << "<" << node->tag << " id=\"" << node->id << "\">" << node->text << "\n";
        for (auto child : node->children)
        {
            saveNode(out, child, indent + 1);
        }
        for (int i = 0; i < indent; ++i)
        {
            out << "  ";
        }
        out << "</" << node->tag << ">\n";
    };

    saveNode(file, root, 0);
}

*/

void HTMLTree::readFromFile(const std::string &path)
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

    std::stack<Node *> node_stack; // 用于管理当前父子关系
    root = nullptr;                // 重置根节点

    std::string line;
    while (std::getline(file, line))
    {
        // 去掉行两端的空格
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty())
            continue; // 跳过空行

        if (line.substr(0, 4) == "<!--")
        {
            // 跳过注释
            continue;
        }
        else if (line.substr(0, 2) == "</")
        {
            // 闭合标签，出栈
            if (!node_stack.empty())
            {
                node_stack.pop();
            }
        }
        else if (line[0] == '<' && line[1] != '/')
        {
            // 解析起始标签
            size_t end_tag_pos = line.find('>');
            if (end_tag_pos == std::string::npos)
                continue;

            std::string tag_content = line.substr(1, end_tag_pos - 1);
            bool self_closing = tag_content.back() == '/';
            if (self_closing)
                tag_content.pop_back();

            // 解析标签名和属性
            std::istringstream tag_stream(tag_content);
            std::string tag, attr;
            tag_stream >> tag;

            std::string id = "";
            while (tag_stream >> attr)
            {
                if (attr.find("id=") == 0)
                {
                    size_t start = attr.find('"') + 1;
                    size_t end = attr.find('"', start);
                    id = attr.substr(start, end - start);
                }
            }

            // 创建新节点
            Node *new_node = new Node(tag, id);
            if (!node_stack.empty() && node_stack.top())
            {
                node_stack.top()->children.push_back(new_node);
            }
            else if (!root)
            {
                root = new_node; // 设置为根节点
            }

            if (!id.empty())
            {
                if (id_map.count(id))
                {
                    std::cerr << "Error: Duplicate ID \"" << id << "\" found.\n";
                    delete new_node;
                    continue;
                }
                id_map[id] = new_node;
            }

            if (!self_closing)
            {
                node_stack.push(new_node);
            }
        }
        else
        {
            // 处理文本内容
            if (!node_stack.empty() && node_stack.top())
            {
                node_stack.top()->text += line + " ";
            }
        }
    }

    file.close();
    std::cout << "HTML loaded successfully from " << path << ".\n";
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
    output << indent << "<" << node->tag;
    if (!node->id.empty())
        output << " id=\"" << node->id << "\"";
    output << ">";
    if (!node->text.empty())
        output << node->text;

    if (!node->children.empty())
    {
        output << "\n";
        for (Node *child : node->children)
        {
            saveNodeToFile(child, output, depth + 1);
        }
        output << indent;
    }

    output << "</" << node->tag << ">\n";
}