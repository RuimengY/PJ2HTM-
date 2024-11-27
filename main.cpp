#include "HTMLTree.h"
#include "CommandManager.h"
#include <iostream>
#include <sstream>

int main()
{
    HTMLTree tree;
    CommandManager cmd_manager(tree);

    std::string command;
    while (true)
    {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        if (command == "exit")
            break;
        else if (command == "print-tree")
        {
            tree.printTree();
        }
        else if (command.substr(0, 6) == "append")
        {
            std::istringstream ss(command);
            std::string cmd, tag, id, parent_id, text;
            ss >> cmd >> tag >> id >> parent_id;
            std::getline(ss, text);
            if (tag == "title")
            {
                tree.appendText(id, text);
            }
            else
            {
                tree.addNode(tag, id, parent_id);
                tree.appendText(id, text);
            }
            cmd_manager.logCommand("append", {tag, id, parent_id, text});
        }
        else if (command.substr(0, 6) == "delete")
        {
            std::istringstream ss(command);
            std::string cmd, id;
            ss >> cmd >> id;
            tree.deleteNode(id);
            cmd_manager.logCommand("delete", {id});
        }
        else if (command == "undo")
        {
            cmd_manager.undo();
        }
        else if (command == "redo")
        {
            cmd_manager.redo();
        }
        else if (command.substr(0, 4) == "read")
        {
            std::istringstream ss(command);
            std::string cmd, path;
            ss >> cmd >> path;

            // 调用解析 HTML 文件并生成命令行
            tree.parseHtmlToCommands(path);
        }

        else if (command.substr(0, 4) == "save")
        {
            std::istringstream ss(command);
            std::string cmd, path;
            ss >> cmd >> path;
            tree.saveToFile(path);
        }

        else
        {
            std::cerr << "Unknown command.\n";
        }
    }

    return 0;
}
