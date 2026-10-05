#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
#include <chrono>
#include <ctime>
#include <string>

#include "Task.h"

using namespace std;

//===============================//
// Scheduler
//===============================//
class Scheduler {
public:
    static vector<Task*> basicSchedule(vector<Task*> tasks) {
        sort(tasks.begin(), tasks.end(),
            [](Task* a, Task* b) {
                return *a < *b;
            });

        return tasks;
    }

    static vector<Task*> urgencySchedule(vector<Task*> tasks) {
        int currentTime = 0;

        sort(tasks.begin(), tasks.end(),
            [currentTime](Task* a, Task* b) {
                return a->urgencyScore(currentTime) > b->urgencyScore(currentTime);
            });

        return tasks;
    }

    static vector<Task*> optimalSchedule(vector<Task*> tasks) {
        sort(tasks.begin(), tasks.end(),
            [](Task* a, Task* b) {
                return a->getDeadline() < b->getDeadline();
            });

        priority_queue<
            Task*,
            vector<Task*>,
            function<bool(Task*, Task*)>
        > pq(
            [](Task* a, Task* b) {
                return a->getDuration() < b->getDuration();
            }
        );

        int currentTime = 0;

        for (auto t : tasks) {
            currentTime += t->getDuration();
            pq.push(t);

            if (currentTime > t->getDeadline()) {
                currentTime -= pq.top()->getDuration();
                pq.pop();
            }
        }

        vector<Task*> result;

        while (!pq.empty()) {
            result.push_back(pq.top());
            pq.pop();
        }

        // 重新按 deadline 排序
        sort(result.begin(), result.end(),
            [](Task* a, Task* b) {
                return a->getDeadline() < b->getDeadline();
            });

        return result;
    }

    static void showSchedule(vector<Task*> tasks) {
        int currentTime = 0;
        int late = 0;
        int totalPenalty = 0;

        for (auto t : tasks) {
            currentTime += t->getDuration();

            int p = t->penalty(currentTime);
            totalPenalty += p;

            cout << "[" << t->getType() << "] "
                << t->getName()
                << " -> 完成时间: " << currentTime << " min"
                << " | 剩余DDL: " << t->getDeadline() << " min"
                << " | penalty: " << p;

            if (currentTime > t->getDeadline()) {
                cout << " (Late)";
                late++;
            }

            cout << endl;
        }

        cout << "延误任务数: " << late << endl;
        cout << "总惩罚值: " << totalPenalty << endl;
    }
};

//===============================//
// TimeUtils
//===============================//
class TimeUtils {
public:
    static long long nowMinutes() {
        auto now = chrono::system_clock::now();
        auto mins = chrono::time_point_cast<chrono::minutes>(now);
        return mins.time_since_epoch().count();
    }
    //默认DDL为当天23:59
    static long long parseDateTimeToMinutes(const string& s) {
        int y = 0, m = 0, d = 0, hh = 23, mm = 59;

        if (s.find(' ') != string::npos) {
            sscanf_s(s.c_str(), "%d-%d-%d %d:%d", &y, &m, &d, &hh, &mm);
        }
        else {
            sscanf_s(s.c_str(), "%d-%d-%d", &y, &m, &d);
        }

        tm t = {};
        t.tm_year = y - 1900;
        t.tm_mon = m - 1;
        t.tm_mday = d;
        t.tm_hour = hh;
        t.tm_min = mm;
        t.tm_sec = 0;

        return static_cast<long long>(mktime(&t)) / 60;
    }

    static long long parseToRemainingMinutes(const string& input) {
        if (input.find('-') != string::npos) {
            long long ddl = parseDateTimeToMinutes(input);
            return ddl - nowMinutes();
        }

        return stoll(input);
    }

    inline string formatRemainingTime(int minutes) {
        if (minutes < 0) {
            return "已过期";
        }

        int days = minutes / (24 * 60);
        int rest = minutes % (24 * 60);
        int hours = rest / 60;
        int mins = rest % 60;

        if (days > 0) {
            return to_string(days) + "天 " + to_string(hours) + "小时";
        }

        return to_string(hours) + "小时 " + to_string(mins) + "分钟";
    }
};

//===============================//
// Menu
//===============================//
inline void menu() {
    cout << "\n===== DDL Task System =====\n";
    cout << "1. 添加任务\n";
    cout << "2. 删除任务\n";
    cout << "3. 查看任务\n";
    cout << "4. 基础调度\n";
    cout << "5. 最优调度（推荐）\n";
    cout << "6. 紧急度调度\n";
    cout << "7. 保存\n";
    cout << "8. 读取\n";
    cout << "9. 查看长期任务阶段计划\n";
    cout << "10. 查看长期任务今日建议\n";
    cout << "11. 标记长期任务阶段完成\n";
    cout << "0. 退出\n";
}