#include "CommandManager.h"
#include <iostream>

// 记录命令
void CommandManager::logCommand(const std::string &command, const std::vector<std::string> &args)
{
    undo_stack.push({command});
    undo_stack.top().insert(undo_stack.top().end(), args.begin(), args.end());
    while (!redo_stack.empty())
        redo_stack.pop(); // 清空重做栈
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
}
