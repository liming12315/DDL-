#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Task.h"
#include "LongTask.h"
#include "Create.h"
using namespace std;

class TaskManager {
private:
    vector<Task*> tasks;

public:
    ~TaskManager() {
        for (auto t : tasks) {
            delete t;
        }
    }

    void addTask(Task* task) {
        if (task != nullptr) {
            tasks.push_back(task);
        }
    }

    bool deleteTask(int id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getId() == id) {
                delete* it;
                tasks.erase(it);
                return true;
            }
        }
        return false;
    }

    void showAll() const {
        if (tasks.empty()) {
            cout << "No tasks.\n";
            return;
        }

        for (auto t : tasks) {
            t->display();
        }
    }

    vector<Task*> getAll() const {
        return tasks;
    }

    vector<Task*> getSchedulableTasks() const {
        vector<Task*> result;

        for (auto t : tasks) {
            if (t->isSchedulable()) {
                result.push_back(t);
            }
        }

        return result;
    }

    vector<LongTermTask*> getLongTermTasks() const {
        vector<LongTermTask*> result;

        for (auto t : tasks) {
            LongTermTask* lt = dynamic_cast<LongTermTask*>(t);
            if (lt != nullptr) {
                result.push_back(lt);
            }
        }

        return result;
    }

    void saveToFile(const string& filename) const {
        ofstream ofs(filename);

        if (!ofs) {
            cout << "文件打开失败。\n";
            return;
        }

        for (auto t : tasks) {
            ofs << t->serialize() << endl;
        }

        cout << "保存成功。\n";
    }

    void loadFromFile(const string& filename) {
        ifstream ifs(filename);

        if (!ifs) {
            cout << "文件打开失败。\n";
            return;
        }

        for (auto t : tasks) {
            delete t;
        }
        tasks.clear();

        string type, name, extraInfo;
        int id, duration, deadline, importance;
        char comma;

        while (getline(ifs, type, ',')) {
            ifs >> id >> comma;

            getline(ifs, name, ',');

            ifs >> duration >> comma
                >> deadline >> comma
                >> importance;

            if (ifs.peek() == ',') {
                ifs.get();
                getline(ifs, extraInfo);
            }
            else {
                extraInfo = "";
                ifs.ignore();
            }

            Task* task = nullptr;

            if (type == "Assignment") {
                task = ShortTaskFactory::create(
                    1,
                    id,
                    name,
                    duration,
                    deadline,
                    importance,
                    extraInfo
                );
            }
            else if (type == "Exam") {
                task = ShortTaskFactory::create(
                    2,
                    id,
                    name,
                    duration,
                    deadline,
                    importance,
                    extraInfo
                );
            }
            else if (type == "CertificatePrep") {
                task = LongTaskFactory::create(
                    3,
                    id,
                    name,
                    deadline,
                    importance
                );
            }
            else if (type == "PaperWriting") {
                task = LongTaskFactory::create(
                    4,
                    id,
                    name,
                    deadline,
                    importance
                );
            }
            else if (type == "CustomLongTerm") {
                task = LongTaskFactory::create(
                    5,
                    id,
                    name,
                    deadline,
                    importance
                );
            }

            addTask(task);
        }

        cout << "读取成功。\n";
    }
};