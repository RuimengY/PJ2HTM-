#include "CommandManager.h"
#include <iostream>
#include <unordered_map>
#include <algorithm> // Add this line to include the <algorithm> header

// 构造函数
CommandManager::CommandManager(HTMLTree &tree) : htmlTree(tree) {}

void CommandManager::undo()
{
    if (undo_stack.empty())
    {
        std::cerr << "Nothing to undo.\n";
        return;
    }

    auto action = undo_stack.top();
    undo_stack.pop();
    redo_stack.push(action);

    std::cout << "Undo: " << action[0] << "\n";

    if (action[0] == "append")
    {
        if (action[1] == "title")
        {
            htmlTree.removeText(action[2]);
        }
        else
        {
            htmlTree.deleteNode(action[2]);
        }
    }
    else if (action[0] == "delete")
    {
        if (action.size() >= 6)
        {
            std::string tag = action[1];
            std::string id = action[2];
            std::string parent_id = action[3];
            std::string text = action[4];
            bool is_self_closing = (action[5] == "true");

            // 获取父节点
            Node *parent = htmlTree.findNodeById(parent_id);
            if (!parent)
            {
                std::cerr << "Error: Parent node not found.\n";
                return;
            }

            // 找到插入位置
            auto it = parent->children.begin();
            while (it != parent->children.end())
            {
                if ((*it)->id == action[6]) // action[6] 是兄弟节点的 ID
                    break;
                ++it;
            }

            // 创建新节点
            Node *new_node = new Node(tag, id, text, is_self_closing);

            // 恢复属性
            if (action.size() > 7)
            {
                for (size_t i = 7; i + 1 < action.size(); i += 2)
                {
                    new_node->attributes[action[i]] = action[i + 1];
                }
            }

            // 在特定位置插入节点
            parent->children.insert(it, new_node);
            htmlTree.id_map[id] = new_node;
        }
    }
    else if (action[0] == "edit-id")
    {
        // 恢复原来的 ID
        htmlTree.editNodeId(action[2], action[1]);
    }
    else if (action[0] == "edit-text")
    {
        // 恢复原来的文本
        htmlTree.editNodeText(action[1], action[2]);
    }
    else if (action[0] == "insert")
    {
        // 删除新插入的节点
        htmlTree.deleteNode(action[2]);
    }
}

void CommandManager::redo()
{
    if (redo_stack.empty())
    {
        std::cerr << "Nothing to redo.\n";
        return;
    }

    auto action = redo_stack.top();
    redo_stack.pop();
    undo_stack.push(action);

    std::cout << "Redo: " << action[0] << "\n";

    if (action[0] == "append")
    {
        if (action[1] == "title")
        {
            htmlTree.appendText(action[2], action[3]);
        }
        else
        {
            htmlTree.addNode(action[1], action[2], action[3], action[4]);
        }
    }
    else if (action[0] == "delete")
    {
        // 删除指定节点
        htmlTree.deleteNode(action[2]);
    }
    else if (action[0] == "edit-id")
    {
        // 重新修改 ID
        htmlTree.editNodeId(action[1], action[2]);
    }
    else if (action[0] == "edit-text")
    {
        // 重新修改文本
        htmlTree.editNodeText(action[1], action[3]);
    }
    else if (action[0] == "insert")
    {
        // 重新插入节点
        htmlTree.insertNodeBefore(action[1], action[2], action[3], action[4]);
    }
}

void CommandManager::logCommand(const std::string &command, const std::vector<std::string> &args)
{
    std::vector<std::string> action = {command};
    action.insert(action.end(), args.begin(), args.end());

    if (command == "delete")
    {
        Node *node = htmlTree.findNodeById(args[0]);
        if (node)
        {
            // 获取父节点和兄弟节点
            Node *parent = nullptr;
            Node *sibling = nullptr;
            for (auto &[key, value] : htmlTree.id_map)
            {
                auto it = std::find(value->children.begin(), value->children.end(), node);
                if (it != value->children.end())
                {
                    parent = value;

                    // 找到当前节点的下一个兄弟节点
                    if (std::next(it) != parent->children.end())
                    {
                        sibling = *std::next(it);
                    }
                    break;
                }
            }

            // 保存父节点和兄弟节点的 ID
            action.push_back(parent ? parent->id : "");
            action.push_back(node->text);
            action.push_back(node->is_self_closing ? "true" : "false");
            action.push_back(sibling ? sibling->id : ""); // 保存兄弟节点的 ID

            // 保存所有属性
            for (const auto &attr : node->attributes)
            {
                action.push_back(attr.first);
                action.push_back(attr.second);
            }
        }
    }

    undo_stack.push(action);

    // 清空重做栈
    while (!redo_stack.empty())
    {
        redo_stack.pop();
    }
}