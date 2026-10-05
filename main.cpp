#include <iostream>
#include <string>

#include "Task.h"
#include "Scheduler.h"
#include "Manager.h"
#include "Create.h"
using namespace std;

template <typename T>
T* findById(const vector<T*>& items, int id) {
    for (auto item : items) {
        if (item != nullptr && item->getId() == id) {
            return item;
        }
    }
    return nullptr;
}

WorkState inputWorkState() {
    int choice;

    cout << "今日工作状态:\n";
    cout << "1. 高效\n";
    cout << "2. 普通\n";
    cout << "3. 低效\n";
    cout << "请选择: ";
    cin >> choice;

    if (choice == 1) {
        return WorkState::High;
    }
    else if (choice == 2) {
        return WorkState::Normal;
    }
    else {
        return WorkState::Low;
    }
}

int main() {
    TaskManager manager;
    int choice;

    while (true) {
        menu();
        cin >> choice;

        if (choice == 0) break;

        if (choice == 1) {
            int type, id, importance;
            long long duration, deadline;
            string name, durationInput, deadlineInput, extraInfo;

            cout << "Task Type:\n";
            cout << "1. Assignment\n";
            cout << "2. Exam\n";
            cout << "3. Project\n";
            cout << "请选择任务类型: ";
            cin >> type;

            cout << "ID: ";
            cin >> id;

            cout << "Name: ";
            cin >> name;

            cout << "Duration（分钟）: ";
            cin >> durationInput;

            cout << "Deadline（分钟 or YYYY-MM-DD or YYYY-MM-DD HH:MM）: ";
            cin.ignore();
            getline(cin, deadlineInput);

            cout << "Importance: ";
            cin >> importance;

            if (type == 1 || type == 2) {
                cout << "Course Name: ";
                cin >> extraInfo;
            }
            else if (type == 3) {
                cout << "Prep Time（分钟）: ";
                cin >> extraInfo;
            }
            else {
                cout << "任务类型输入错误。\n";
                continue;
            }

            duration = stoll(durationInput);
            deadline = TimeUtils::parseToRemainingMinutes(deadlineInput);

            Task* task = nullptr;

            if (type == 1 || type == 2) {
                task = ShortTaskFactory::create(
                    type,
                    id,
                    name,
                    static_cast<int>(duration),
                    static_cast<int>(deadline),
                    importance,
                    extraInfo
                );
            }
            else if (type == 3 || type == 4 || type == 5) {
                task = LongTaskFactory::create(
                    type,
                    id,
                    name,
                    static_cast<int>(deadline),
                    importance
                );
            }

            manager.addTask(task);

            if (task == nullptr) {
                cout << "任务创建失败。\n";
            }
            else {
                cout << "任务创建成功。\n";
            }
        }

        else if (choice == 2) {
            int id;
            cout << "输入要删除的ID: ";
            cin >> id;
            manager.deleteTask(id);
        }

        else if (choice == 3) {
            manager.showAll();
        }

        else if (choice == 4) {
            auto tasks = Scheduler::basicSchedule(manager.getSchedulableTasks());
            Scheduler::showSchedule(tasks);
        }

        else if (choice == 5) {
            auto tasks = Scheduler::optimalSchedule(manager.getSchedulableTasks());
            Scheduler::showSchedule(tasks);
        }

        else if (choice == 6) {
            auto tasks = Scheduler::urgencySchedule(manager.getSchedulableTasks());
            Scheduler::showSchedule(tasks);
        }

        else if (choice == 7) {
            manager.saveToFile("tasks.txt");
        }

        else if (choice == 8) {
            manager.loadFromFile("tasks.txt");
        }
        else if (choice == 9) {
            int id;

            cout << "输入长期任务ID: ";
            cin >> id;

            auto longTasks = manager.getLongTermTasks();
            LongTermTask* task = findById(longTasks, id);

            if (task != nullptr) {
                task->showPlan();
            }
            else {
                cout << "未找到该长期任务。\n";
            }
        }
        else if (choice == 10) {
            int id;

            cout << "输入长期任务ID: ";
            cin >> id;

            WorkState state = inputWorkState();

            auto longTasks = manager.getLongTermTasks();
            LongTermTask* task = findById(longTasks, id);

            if (task != nullptr) {
                task->showTodaySuggestion(state);
            }
            else {
                cout << "未找到该长期任务。\n";
            }
}
        else if (choice == 11) {
            int taskId, milestoneIndex;

            cout << "输入长期任务ID: ";
            cin >> taskId;

            cout << "输入要标记完成的阶段编号: ";
            cin >> milestoneIndex;

            auto longTasks = manager.getLongTermTasks();
            LongTermTask* task = findById(longTasks, taskId);

            if (task != nullptr) {
                if (task->markMilestoneFinished(milestoneIndex)) {
                    cout << "阶段已标记完成。\n";
                }
                else {
                    cout << "未找到该阶段。\n";
                }
            }
            else {
                cout << "未找到该长期任务。\n";
            }
        }
        else {
            cout << "无效选择。\n";
        }
    }
    return 0;
}