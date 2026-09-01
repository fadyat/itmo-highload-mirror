use crate::runtime::ContextManager;

use super::task::{Task, TaskId};

use futures::task::ArcWake;
use log::debug;
use rayon::{ThreadPool, ThreadPoolBuilder};

use std::{
    collections::HashMap,
    sync::{Arc, Mutex, MutexGuard, Weak},
    task::Context,
};

#[derive(Clone, Copy, Default)]
struct TaskState {
    running: bool,
    queued: bool,
}

struct TaskEntry {
    task: Option<Task>,
    state: TaskState,
}

impl TaskEntry {
    pub fn should_reschedule(&mut self) -> bool {
        match (self.state.running, self.state.queued) {
            (true, _) => {
                self.state.queued = true;
                false
            }
            (false, true) => false,
            (false, false) => {
                self.state.queued = true;
                true
            }
        }
    }
}

impl From<Task> for TaskEntry {
    fn from(task: Task) -> Self {
        TaskEntry {
            task: Some(task),
            state: TaskState {
                running: false,
                queued: true,
            },
        }
    }
}

pub(crate) struct SharedState {
    tasks: Mutex<HashMap<TaskId, TaskEntry>>,
    context_manager: Arc<ContextManager>,
    thread_pool: ThreadPool,
}

impl SharedState {
    fn new(context_manager: ContextManager, num_threads: usize) -> Arc<Self> {
        Arc::new(SharedState {
            tasks: Default::default(),
            context_manager: Arc::new(context_manager),
            thread_pool: ThreadPoolBuilder::new()
                .num_threads(num_threads)
                .build()
                .unwrap(),
        })
    }

    fn lock_tasks(&self) -> MutexGuard<'_, HashMap<TaskId, TaskEntry>> {
        self.tasks.lock().expect("failed to lock tasks")
    }

    fn schedule_poll(self: &Arc<Self>, task_id: TaskId) {
        let weak = Arc::downgrade(self);
        let cm = self.context_manager.clone();
        self.thread_pool.spawn_fifo(move || {
            cm.install();
            if let Some(shared) = weak.upgrade() {
                crate::scheduler::multi_thread::poll_one(shared, task_id);
            }
        });
    }

    fn take_task_for_poll(&self, task_id: TaskId) -> Option<Task> {
        if let Some(entry) = self.lock_tasks().get_mut(&task_id) {
            entry.state.running = true;
            entry.state.queued = false;
            return entry.task.take();
        }
        None
    }

    fn finish_poll_pending(&self, task_id: TaskId, task: Task) -> bool {
        if let Some(entry) = self.lock_tasks().get_mut(&task_id) {
            entry.task = Some(task);
            entry.state.running = false;
            if entry.state.queued {
                entry.state.queued = false;
                return true;
            }
            return false;
        }
        false
    }
}

pub struct MultiThreadScheduler {
    shared_state: Arc<SharedState>,
}

impl MultiThreadScheduler {
    pub fn new(context_manager: ContextManager, num_threads: usize) -> Self {
        Self {
            shared_state: SharedState::new(context_manager, num_threads),
        }
    }

    pub fn submit(&self, task: Task) {
        let task_id = task.id();

        {
            let mut tasks = self.shared_state.lock_tasks();
            let prev = tasks.insert(task_id, TaskEntry::from(task));
            assert!(prev.is_none(), "task #{} already exists", task_id);
        }

        debug!("submitted task #{}", task_id);
        self.shared_state.schedule_poll(task_id);
    }
}

struct Waker {
    shared_state: Weak<SharedState>,
    task_id: TaskId,
}

impl ArcWake for Waker {
    fn wake_by_ref(arc_self: &Arc<Self>) {
        let task_id = arc_self.task_id;

        let Some(shared) = arc_self.shared_state.upgrade() else {
            debug!("failed to wake task #{}: no scheduler", task_id);
            return;
        };

        let should_schedule = {
            let mut tasks = shared.lock_tasks();
            match tasks.get_mut(&task_id) {
                Some(entry) => entry.should_reschedule(),
                None => {
                    debug!("failed to wake task #{}: no task", task_id);
                    return;
                }
            }
        };

        if should_schedule {
            shared.schedule_poll(task_id);
        }
    }
}

pub(crate) fn poll_one(shared: Arc<SharedState>, task_id: TaskId) {
    let mut task = match shared.take_task_for_poll(task_id) {
        Some(t) => t,
        None => {
            debug!("no task #{} to poll", task_id);
            return;
        }
    };

    debug!("polling task #{}", task_id);
    let waker = futures::task::waker(Arc::new(Waker {
        shared_state: Arc::downgrade(&shared),
        task_id,
    }));
    let mut cx = Context::from_waker(&waker);

    if task.poll(&mut cx).is_pending() {
        if shared.finish_poll_pending(task_id, task) {
            shared.schedule_poll(task_id);
        }
    } else {
        {
            let mut tasks = shared.lock_tasks();
            tasks.remove(&task_id);
        }

        debug!("task #{} completed", task_id);
    }
}
