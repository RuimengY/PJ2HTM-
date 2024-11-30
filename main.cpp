#include "HTMLTree.h"
#include "CommandManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

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

            // 获取要删除节点的详细信息以便撤销
            Node *node = tree.findNodeById(id);
            if (node)
            {
                std::vector<std::string> delete_args = {id};
                delete_args.insert(delete_args.begin(), node->tag);

                // 找到父节点
                for (auto &[key, parent] : tree.id_map)
                {
                    auto it = std::find(parent->children.begin(), parent->children.end(), node);
                    if (it != parent->children.end())
                    {
                        delete_args.push_back(parent->id);
                        break;
                    }
                }

                delete_args.push_back(node->text);
                delete_args.push_back(node->is_self_closing ? "true" : "false");

                // 找到下一个兄弟节点，以便撤销时插入到正确位置
                Node *sibling = nullptr;
                for (auto &[key, parent] : tree.id_map)
                {
                    auto it = std::find(parent->children.begin(), parent->children.end(), node);
                    if (it != parent->children.end())
                    {
                        if (std::next(it) != parent->children.end())
                        {
                            sibling = *std::next(it);
                        }
                        break;
                    }
                }

                // 添加所有属性
                for (const auto &attr : node->attributes)
                {
                    delete_args.push_back(attr.first);
                    delete_args.push_back(attr.second);
                }

                tree.deleteNode(id);
                cmd_manager.logCommand("delete", delete_args);
            }
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

        else if (command.substr(0, 8) == "find-tag")
        {
            std::istringstream ss(command);
            std::string cmd, tag;
            ss >> cmd >> tag;

            std::vector<Node *> nodes = tree.findNodesByTag(tag);
            if (nodes.empty())
            {
                std::cout << "No nodes found with tag: " << tag << "\n";
            }
            else
            {
                std::cout << "Found " << nodes.size() << " node(s) with tag: " << tag << "\n";
                for (Node *node : nodes)
                {
                    tree.printNodeInfo(node);
                    std::cout << "---\n";
                }
            }
        }
        else if (command.substr(0, 7) == "find-id")
        {
            std::istringstream ss(command);
            std::string cmd, id;
            ss >> cmd >> id;

            Node *node = tree.findNodeById(id);
            if (node)
            {
                tree.printNodeInfo(node);
            }
            else
            {
                std::cout << "No node found with ID: " << id << "\n";
            }
        }
        else if (command.substr(0, 6) == "insert")
        {
            std::istringstream ss(command);
            std::string cmd, tag, id, beforeId;
            std::string text;
            ss >> cmd >> tag >> id >> beforeId;
            std::getline(ss, text);

            text.erase(0, text.find_first_not_of(" \t"));
            text.erase(text.find_last_not_of(" \t") + 1);

            tree.insertNodeBefore(tag, id, beforeId, text);
            cmd_manager.logCommand("insert", {tag, id, beforeId, text});
        }
        else if (command.substr(0, 9) == "edit-text")
        {
            std::istringstream ss(command);
            std::string cmd, id, text;
            ss >> cmd >> id;
            std::getline(ss, text);

            text.erase(0, text.find_first_not_of(" \t"));
            text.erase(text.find_last_not_of(" \t") + 1);

            tree.editNodeText(id, text);
            cmd_manager.logCommand("edit-text", {id, text});
        }
        else if (command.substr(0, 7) == "edit-id")
        {
            std::istringstream ss(command);
            std::string cmd, oldId, newId;
            ss >> cmd >> oldId >> newId;

            tree.editNodeId(oldId, newId);
            cmd_manager.logCommand("edit-id", {oldId, newId});
        }
        else if (command.substr(0, 12) == "print-indent")
        {
            std::istringstream ss(command);
            std::string cmd;
            int indent = 2; // Default indent
            ss >> cmd;
            if (ss >> indent)
            {
                tree.printTree(indent);
            }
            else
            {
                tree.printTree();
            }
        }
        else
        {
            std::cerr << "Unknown command.\n";
        }
    }

    return 0;
}
