#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;
class TaskManager;
class ShortTaskFactory;
//===============================//
// Task 抽象基类
//===============================//
class Task {
protected:
    int id;
    string name;
    int duration;      // 执行时长，单位：分钟
    int deadline;      // 距离DDL的剩余分钟
    int importance;    // 重要性

public:
    Task(int id, string name, int duration, int deadline, int importance)
        : id(id), name(name), duration(duration),
        deadline(deadline), importance(importance) {
    }

    virtual ~Task() {}

    virtual bool isSchedulable() const {
        return true;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    int getDuration() const { return duration; }
    int getDeadline() const { return deadline; }
    int getImportance() const { return importance; }

    virtual string getType() const = 0;
    //贪心评分
    virtual int urgencyScore(int currentTime) const {
        int slack = deadline - currentTime - duration;
        return -slack + importance * 10;
    }

    int getDelay(int finishTime) const {
        return max(0, finishTime - deadline);
    }

    //惩罚系数
    virtual int penalty(int finishTime) const {
        return getDelay(finishTime) * importance;
    }

    bool operator<(const Task& other) const {
        if (deadline == other.deadline) {
            return importance > other.importance;
        }
        return deadline < other.deadline;
    }
    //友元输出，避免实例化
    friend ostream& operator<<(ostream& os, const Task& task) {
        os << "[" << task.getType() << "] "
            << "ID: " << task.id
            << " | " << task.name
            << " | dur: " << task.duration << " min"
            << " | ddl left: " << task.deadline << " min"
            << " | importance: " << task.importance;
        return os;
    }

    virtual void display() const {
        cout << *this << endl;
    }

    virtual string serialize() const {
        return getType() + "," + to_string(id) + "," + name + ","
            + to_string(duration) + "," + to_string(deadline) + ","
            + to_string(importance);
    }
};

//===============================//
// 第二层：学术任务
//===============================//
class AcademicTask : public Task {
protected:
    string courseName;

    AcademicTask(int id, string name, int duration, int deadline,
        int importance, string courseName)
        : Task(id, name, duration, deadline, importance),
        courseName(courseName) {
    }

public:
    string getCourseName() const {
        return courseName;
    }

    int urgencyScore(int currentTime) const override {
        return Task::urgencyScore(currentTime) + 20;
    }

    string serialize() const override {
        return Task::serialize() + "," + courseName;
    }
};

//===============================//
// 第三层：作业
//===============================//
class AssignmentTask : public AcademicTask {
    friend class ShortTaskFactory;

private:
    AssignmentTask(int id, string name, int duration, int deadline,
        int importance, string courseName)
        : AcademicTask(id, name, duration, deadline, importance, courseName) {
    }

public:
    string getType() const override {
        return "Assignment";
    }
};

//===============================//
// 第三层：课程考试
//===============================//
class ExamTask : public AcademicTask {
    friend class ShortTaskFactory;

private:
    ExamTask(int id, string name, int duration, int deadline,
        int importance, string courseName)
        : AcademicTask(id, name, duration, deadline, importance, courseName) {
    }

public:
    string getType() const override {
        return "Exam";
    }

    int penalty(int finishTime) const override {
        int delay = getDelay(finishTime);
        return delay * delay * importance;//考试惩罚更重
    }

    int urgencyScore(int currentTime) const override {
        return AcademicTask::urgencyScore(currentTime) + 50;
    }
};