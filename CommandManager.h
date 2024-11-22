#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include <vector>
#include <string>

class CommandManager
{
private:
    std::stack<std::vector<std::string>> undo_stack; // 撤销栈
    std::stack<std::vector<std::string>> redo_stack; // 重做栈

public:
    // 记录命令
    void logCommand(const std::string &command, const std::vector<std::string> &args);

    // 执行撤销
    void undo();

    // 执行重做
    void redo();
};

#endif // COMMANDMANAGER_H
