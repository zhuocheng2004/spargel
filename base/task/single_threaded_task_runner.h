#ifndef BASE_TASK_SINGLE_THREADED_TASK_RUNNER_H_
#define BASE_TASK_SINGLE_THREADED_TASK_RUNNER_H_

#include "base/task/task_runner.h"

namespace base::task {

class SingleThreadedTaskRunner : public TaskRunner {
 public:
  SingleThreadedTaskRunner();
  ~SingleThreadedTaskRunner() override;
};

}  // namespace base::task

#endif
