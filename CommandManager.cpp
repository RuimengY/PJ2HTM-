#include "CommandManager.h"
#include <iostream>

// 构造函数
CommandManager::CommandManager(HTMLTree &tree) : htmlTree(tree) {}

void CommandManager::logCommand(const std::string &command, const std::vector<std::string> &args)
{
    std::vector<std::string> action = {command};
    action.insert(action.end(), args.begin(), args.end());
    undo_stack.push(action);
    while (!redo_stack.empty())
    {
        redo_stack.pop();
    }
}
// 执行撤销
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

    // 处理撤销逻辑
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
        htmlTree.addNode(action[1], action[2], action[3]);
        htmlTree.appendText(action[2], action[4]);
    }
}

// 执行重做
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

    // 处理重做逻辑
    if (action[0] == "append")
    {
        if (action[1] == "title")
        {
            htmlTree.appendText(action[2], action[3]);
        }
        else
        {
            htmlTree.addNode(action[1], action[2], action[3]);
            htmlTree.appendText(action[2], action[4]);
        }
    }
    else if (action[0] == "delete")
    {
        htmlTree.deleteNode(action[1]);
    }
}