#include "CommandManager.h"
#include <iostream>

// 构造函数
CommandManager::CommandManager(HTMLTree &tree) : htmlTree(tree) {}

// 日志记录命令
void CommandManager::logCommand(const std::string &command, const std::vector<std::string> &args)
{
    std::vector<std::string> action = {command};
    action.insert(action.end(), args.begin(), args.end());
    undo_stack.push(action);

    // 清空重做栈
    while (!redo_stack.empty())
    {
        redo_stack.pop();
    }
}

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
            // 从树中完全删除节点
            htmlTree.deleteNode(action[2]);
        }
    }
    else if (action[0] == "delete")
    {
        // 重新添加被删除的节点
        htmlTree.addNode(action[1], action[2], action[3], action[4]);
        Node *node = htmlTree.findNodeById(action[2]);

        if (node)
        {
            // 恢复自闭合标签状态
            node->is_self_closing = (action[5] == "true");

            // 恢复所有属性
            for (size_t i = 6; i < action.size(); i += 2)
            {
                node->attributes[action[i]] = action[i + 1];
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
            // 添加节点并追加文本
            htmlTree.addNode(action[1], action[2], action[3], action[4]);
            htmlTree.appendText(action[2], action[4]);
        }
    }
    else if (action[0] == "delete")
    {
        // 删除指定节点
        htmlTree.deleteNode(action[1]);
    }
}
