#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "Task.h"

using namespace std;
class TaskManager;
class LongTaskFactory;
// ===============================
// 今日工作状态
// ===============================
enum class WorkState {
    High = 1,    // 高效
    Normal = 2,  // 普通
    Low = 3      // 低效
};

inline string workStateToString(WorkState state) {
    switch (state) {
    case WorkState::High:
        return "高效";
    case WorkState::Normal:
        return "普通";
    case WorkState::Low:
        return "低效";
    default:
        return "未知";
    }
}

// 状态权重：表示今天适合推进长期任务的程度
inline double workStateWeight(WorkState state) {
    switch (state) {
    case WorkState::High:
        return 1.0;
    case WorkState::Normal:
        return 0.65;
    case WorkState::Low:
        return 0.35;
    default:
        return 0.5;
    }
}

// ===============================
// 长期任务类型
// ===============================
enum class LongTaskType {
    CertificatePrep = 1,   // 六级 / 雅思 / 托福 / GRE
    PaperWriting = 2,      // 论文写作
    CustomLongTerm = 3     // 自定义长期任务
};

inline string longTaskTypeToString(LongTaskType type) {
    switch (type) {
    case LongTaskType::CertificatePrep:
        return "CertificatePrep";
    case LongTaskType::PaperWriting:
        return "PaperWriting";
    case LongTaskType::CustomLongTerm:
        return "CustomLongTerm";
    default:
        return "Unknown";
    }
}

// ===============================
// 阶段节点
// ===============================
struct Milestone {
    int index;
    string name;
    int deadline;     // 距离现在的剩余分钟
    int weight;       // 阶段提醒强度
    bool finished;

    Milestone(int index, string name, int deadline, int weight)
        : index(index), name(name), deadline(deadline),
        weight(weight), finished(false) {
    }
};


// ===============================
// LongTermTask
// ===============================
class LongTermTask : public Task {
protected:
    vector<Milestone> milestones;

protected:
    LongTermTask(int id, string name, int deadline, int importance)
        : Task(id, name, 0, deadline, importance) {
    }

    bool isSchedulable() const override {
        return false;
    }

    int clampDeadline(int value) const {
        if (value < 0) return 0;
        if (value > deadline) return deadline;
        return value;
    }

    int makeDeadlineByRatio(double ratio) const {
        return clampDeadline(static_cast<int>(deadline * ratio));
    }

    void addMilestone(const string& milestoneName, double ratio, int weight) {
        int index = static_cast<int>(milestones.size()) + 1;

        milestones.emplace_back(
            index,
            milestoneName,
            makeDeadlineByRatio(ratio),
            weight
        );
    }

public:
    virtual ~LongTermTask() {}

    virtual void generateMilestones() = 0;

    int getDuration() const {
        return 0;
    }

    int penalty(int finishTime) const override {
        return 0;
    }

    int urgencyScore(int currentTime) const override {
        const Milestone* current = getCurrentMilestone();

        if (current == nullptr) {
            return -9999;
        }

        int slack = current->deadline - currentTime;
        return -slack + current->weight * 10 + importance * 5;
    }

    const vector<Milestone>& getMilestones() const {
        return milestones;
    }

    Milestone* getCurrentMilestone() {
        for (auto& m : milestones) {
            if (!m.finished) {
                return &m;
            }
        }
        return nullptr;
    }

    const Milestone* getCurrentMilestone() const {
        for (const auto& m : milestones) {
            if (!m.finished) {
                return &m;
            }
        }
        return nullptr;
    }

    bool markMilestoneFinished(int index) {
        for (auto& m : milestones) {
            if (m.index == index) {
                m.finished = true;
                return true;
            }
        }
        return false;
    }

    void showPlan() const {
        cout << "\n[" << getType() << "] "
            << name
            << " | Final DDL left: " << deadline << " min"
            << " | importance: " << importance << endl;

        if (milestones.empty()) {
            cout << "No milestones.\n";
            return;
        }

        for (const auto& m : milestones) {
            cout << setw(2) << m.index << ". "
                << m.name
                << " | ddl left: " << m.deadline << " min"
                << " | weight: " << m.weight
                << " | " << (m.finished ? "Finished" : "Unfinished")
                << endl;
        }
    }

    int recommendTodayMinutes(WorkState state) const {
        const Milestone* current = getCurrentMilestone();

        if (current == nullptr) {
            return 0;
        }

        double stateFactor = workStateWeight(state);

        int base = 60;
        double milestoneFactor = current->weight / 20.0;

        int result = static_cast<int>(base * stateFactor * milestoneFactor);

        if (result < 15) result = 15;
        if (result > 240) result = 240;

        return result;
    }

    void showTodaySuggestion(WorkState state) const {
        const Milestone* current = getCurrentMilestone();

        if (current == nullptr) {
            cout << "长期任务已无未完成阶段。\n";
            return;
        }

        cout << "\n今日状态: " << workStateToString(state) << endl;
        cout << "当前阶段: " << current->name << endl;
        cout << "阶段DDL剩余: " << current->deadline << " min" << endl;
        cout << "建议投入: " << recommendTodayMinutes(state) << " min" << endl;
    }

    string serialize() const override {
        return getType() + "," + to_string(id) + "," + name + ","
            + "0," + to_string(deadline) + ","
            + to_string(importance) + ",";
    }
};

// ===============================
// 证书考试准备
// ===============================
class CertificateTask : public LongTermTask {
    friend class LongTaskFactory;

private:
    CertificateTask(int id, string name, int deadline, int importance)
        : LongTermTask(id, name, deadline, importance) {
        generateMilestones();
    }

public:
    string getType() const override {
        return "CertificatePrep";
    }

    void generateMilestones() override {
        milestones.clear();

        addMilestone("诊断测试 / 明确薄弱项", 0.35, 5);
        addMilestone("基础输入：词汇 / 题型熟悉", 0.55, 10);
        addMilestone("分项训练：听力 / 阅读 / 写作 / 口语", 0.72, 15);
        addMilestone("限时训练与错题整理", 0.85, 25);
        addMilestone("完整模拟与复盘", 0.94, 25);
        addMilestone("考前回顾 / 资料整理", 0.99, 20);
    }
};

// ===============================
// 论文写作
// ===============================
class PaperWritingTask : public LongTermTask {
    friend class LongTaskFactory;

private:
    PaperWritingTask(int id, string name, int deadline, int importance)
        : LongTermTask(id, name, deadline, importance) {
        generateMilestones();
    }

public:
    string getType() const override {
        return "PaperWriting";
    }

    void generateMilestones() override {
        milestones.clear();

        addMilestone("确定题目与基本立意", 0.35, 5);
        addMilestone("资料收集 / 文献整理", 0.55, 10);
        addMilestone("论文结构与提纲", 0.70, 15);
        addMilestone("初稿完成", 0.84, 30);
        addMilestone("修改与格式检查", 0.94, 25);
        addMilestone("提交前检查", 0.99, 15);
    }
};

// ===============================
// 自定义长期任务
// ===============================
class CustomLongTermTask : public LongTermTask {
    friend class LongTaskFactory;

private:
    CustomLongTermTask(int id, string name, int deadline, int importance)
        : LongTermTask(id, name, deadline, importance) {
        generateMilestones();
    }

public:
    string getType() const override {
        return "CustomLongTerm";
    }

    void generateMilestones() override {
        milestones.clear();

        addMilestone("启动检查", 0.40, 10);
        addMilestone("中期检查", 0.65, 20);
        addMilestone("阶段成果", 0.82, 30);
        addMilestone("最终检查", 0.94, 25);
        addMilestone("提交 / 完成", 0.99, 15);
    }
};