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

    // 撤销逻辑
    if (action[0] == "append")
    {
        if (action[1] == "title")
        {
            htmlTree.removeText(action[2]);
        }
        else
        {
            // 防止重复删除
            htmlTree.deleteNode(action[2]);
        }
    }
    else if (action[0] == "delete")
    {
        // 重新添加被删除的节点
        if (action.size() >= 6)
        {
            htmlTree.addNode(action[1], action[2], action[3], action[4]);
            Node *node = htmlTree.findNodeById(action[2]);

            if (node)
            {
                // 恢复自闭合标签状态
                node->is_self_closing = (action[5] == "true");

                // 恢复所有属性（如果存在）
                if (action.size() > 6)
                {
                    for (size_t i = 6; i < action.size(); i += 2)
                    {
                        node->attributes[action[i]] = action[i + 1];
                    }
                }
            }
        }
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

    // 重做逻辑
    if (action[0] == "append")
    {
        if (action[1] == "title")
        {
            htmlTree.appendText(action[2], action[3]);
        }
        else
        {
            // 添加节点并设置文本
            htmlTree.addNode(action[1], action[2], action[3], action[4]);
        }
    }
    else if (action[0] == "delete")
    {
        // 删除指定节点
        htmlTree.deleteNode(action[1]);
    }
}

void CommandManager::logCommand(const std::string &command, const std::vector<std::string> &args)
{
    std::vector<std::string> action = {command};
    action.insert(action.end(), args.begin(), args.end());

    // 对于删除操作，保存更多详细信息
    if (command == "delete")
    {
        Node *node = htmlTree.findNodeById(args[0]);
        if (node)
        {
            // 获取父节点 ID
            std::string parent_id;
            for (auto &[key, value] : htmlTree.id_map)
            {
                auto it = std::find(value->children.begin(), value->children.end(), node);
                if (it != value->children.end())
                {
                    parent_id = value->id;
                    break;
                }
            }

            action.push_back(node->tag);
            action.push_back(parent_id);
            action.push_back(node->text);
            action.push_back(node->is_self_closing ? "true" : "false");

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