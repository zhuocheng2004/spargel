#ifndef BASE_TASK_TASK_RUNNER_H_
#define BASE_TASK_TASK_RUNNER_H_

#include "base/callback/callback.h"

namespace base::task {

class TaskRunner {
 public:
  TaskRunner();
  virtual ~TaskRunner();

  TaskRunner(TaskRunner const&) = delete;
  TaskRunner& operator=(TaskRunner const&) = delete;

  virtual void PostTask(callback::OnceClosure task) = 0;
};

}  // namespace base::task

#endif
