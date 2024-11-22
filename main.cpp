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
        else
        {
            std::cerr << "Unknown command.\n";
        }
    }

    return 0;
}

/*
int main()
{
    HTMLTree tree;
    tree.addNode("div", "node1", "body", "Hello World");
    tree.addNode("div", "node2", "body", "Another Node");
    tree.addNode("p", "node4", "node1", "Appended Node");

    std::cout << "Initial tree:\n";
    tree.printTree();

    std::cout << "\nAfter deleting node1:\n";
    tree.deleteNode("node1");
    tree.printTree();

    std::cout << "\nAfter editing node2 ID to node5:\n";
    tree.editNodeId("node2", "node5");
    tree.printTree();

    std::cout << "\nAfter editing node5 text:\n";
    tree.editNodeText("node5", "Updated Text");
    tree.printTree();

    return 0;
}

*/
