#pragma once

#include <string>

#include "Task.h"
#include "LongTask.h"

using namespace std;

class ShortTaskFactory {
public:
    static Task* create(
        int type,
        int id,
        const string& name,
        int duration,
        int deadline,
        int importance,
        const string& courseName
    ) {
        if (type == 1) {
            return new AssignmentTask(
                id,
                name,
                duration,
                deadline,
                importance,
                courseName
            );
        }

        if (type == 2) {
            return new ExamTask(
                id,
                name,
                duration,
                deadline,
                importance,
                courseName
            );
        }

        return nullptr;
    }
};

class LongTaskFactory {
public:
    static Task* create(
        int type,
        int id,
        const string& name,
        int deadline,
        int importance
    ) {
        if (type == 3) {
            return new CertificateTask(
                id,
                name,
                deadline,
                importance
            );
        }

        if (type == 4) {
            return new PaperWritingTask(
                id,
                name,
                deadline,
                importance
            );
        }

        if (type == 5) {
            return new CustomLongTermTask(
                id,
                name,
                deadline,
                importance
            );
        }

        return nullptr;
    }
};