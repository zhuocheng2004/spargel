#ifndef BASE_TASK_SEQUENCED_TASK_RUNNER_H_
#define BASE_TASK_SEQUENCED_TASK_RUNNER_H_

#include "base/task/task_runner.h"

namespace base::task {

class SequencedTaskRunner : public TaskRunner {
 public:
  SequencedTaskRunner();
  ~SequencedTaskRunner() override;
};

}  // namespace base::task

#endif
