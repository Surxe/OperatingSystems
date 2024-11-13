import copy

class Task:
    """
    A class to represent a Task.
    """

    # Constructor
    def __init__(self, name, arrival, duration):
        self.name = name
        self.arrival = arrival
        self.duration = duration
        self.orig_duration = duration
        self.is_complete = False

    def __str__(self):
        return f'{self.name}: Arrival={self.arrival}, Duration={self.duration})'

    def _is_complete(self):
        self.is_complete = self.duration == 0
        return self.is_complete

    def tick(self):
        self.duration -= 1
        return self._is_complete()



class Scheduler:
    """
    A class to represent a scheduler.
    """

    # Constructor
    def __init__(self, tasks):
        self.tasks = tasks

    def _tick_tasks(self, schedule, task_to_run, current_tick):
        # Mark the current running task
        schedule[task_to_run.name].append('#')

        # Mark the tasks that aren't running
        for task in self.tasks:
            if task != task_to_run:
                if task.duration == 0:
                    symbol = '' # already completed
                elif task.arrival <= current_tick:
                    symbol = '_' #arrived but not running
                else:
                    symbol = '-' #not yet arrived
                    
                schedule[task.name].append(symbol)

        # Return whether or not the task has completed, and tick the current task
        return task_to_run.tick()
    
    def _num_tasks_remaining(self):
        return sum([1 for task in self.tasks if not task.is_complete])
        
    # Private members  
    def _schedule_fifo(self, verbose):
        self.tasks = sorted(self.tasks, key=lambda task: task.arrival)
        # i.e. [Task('A', 0, 5), Task('B', 1, 3), Task('C', 2, 2)]

        # return i.e. {"A": ["-", "-", "_", "#", "#", ""]}
        
        # Construct a schedule from the ordered tasks
        schedule = {}

        # Initialize the tasks in the schedule
        schedule = {task.name: [] for task in self.tasks}

        current_running_task = None
        current_tick = 0
        while self._num_tasks_remaining() > 0:
            if verbose:
                print(f'\nTick {current_tick}: ', end='')
            # If there is no task running, determine the task to run
            if current_running_task is None:
                # Otherwise, check if there is a task that has arrived
                # If multiple have arrived, the first one thats defined is used
                for task in self.tasks:
                    if not task.is_complete and task.arrival <= current_tick:
                        current_running_task = task
                        break

            # If there is a task now running, tick it
            if current_running_task is not None:
                task_complete = self._tick_tasks(schedule, current_running_task, current_tick)
                if verbose:
                    print(f'Ticked task {current_running_task.name}, remaining duration {current_running_task.duration}')

                # Delay the task nullification to give a chance to print it
                if task_complete:
                    current_running_task = None

            current_tick += 1

        if verbose:
            print()

        return schedule
    
    def _schedule_sjf(self, verbose):
        # Sort tasks by arrival time as primary key and duration as secondary key for SJF
        self.tasks = sorted(self.tasks, key=lambda task: (task.arrival, task.duration))
        
        # Initialize the schedule with each task's name mapped to an empty list
        schedule = {task.name: [] for task in self.tasks}
        
        current_running_task = None
        current_tick = 0

        while self._num_tasks_remaining() > 0:
            if verbose:
                print(f'\nTick {current_tick}: ', end='')

            # Check if we need to pick a new task to run
            if current_running_task is None:
                # Filter tasks that have arrived by the current tick
                available_tasks = [task for task in self.tasks if not task.is_complete and task.arrival <= current_tick]

                # Pick the shortest job (task with the lowest duration)
                if available_tasks:
                    # Sort available tasks by duration first, then by name to break ties
                    available_tasks.sort(key=lambda task: (task.duration, task.name))
                    current_running_task = available_tasks[0]

            # Tick the current task if there is one
            if current_running_task is not None:
                # Record the tick in the task's schedule entry
                task_complete = self._tick_tasks(schedule, current_running_task, current_tick)

                if verbose:
                    print(f'Ticked task {current_running_task.name}, remaining duration {current_running_task.duration}')

                # Set to None if the task is complete
                if task_complete:
                    current_running_task = None

            current_tick += 1

        if verbose:
            print()
        
        return schedule
    
    def _schedule_srtf(self, verbose):
        schedule = {task.name: [] for task in self.tasks}  # Initialize schedule output
        current_tick = 0
        current_running_task = None

        while self._num_tasks_remaining() > 0:
            if verbose:
                print(f'\nTick {current_tick}: ', end='')

            # Filter tasks that have arrived by the current tick
            available_tasks = [task for task in self.tasks if not task.is_complete and task.arrival <= current_tick]

            # Select the task with the shortest remaining duration, possibly preempting the current task
            if available_tasks:
                # Sort by remaining duration, then by name to break ties
                available_tasks.sort(key=lambda task: (task.duration, task.name))
                selected_task = available_tasks[0]

                # Preempt the current running task if a shorter one is available
                if current_running_task is None or selected_task.duration < current_running_task.duration:
                    current_running_task = selected_task

            # Tick the selected task
            if current_running_task is not None:
                task_complete = self._tick_tasks(schedule, current_running_task, current_tick)

                if verbose:
                    print(f'Ticked task {current_running_task.name}, remaining duration {current_running_task.duration}')

                # Remove the completed task
                if task_complete:
                    current_running_task = None  # Allow selecting a new task on the next tick

            current_tick += 1

        if verbose:
            print()

        return schedule
    
    def _schedule_rr(self, verbose):
        schedule = {task.name: [] for task in self.tasks}  # Initialize schedule output
        current_tick = 0
        queue = list(self.tasks)  # Queue of tasks to process in a round-robin manner

        while queue:
            # Pop the first task in the queue
            current_task = queue.pop(0)

            # Run the task for up to quantum=1 ticks
            ticks_run = min(current_task.duration, 1)

            # Record ticks in the schedule for the task
            for _ in range(ticks_run):
                self._tick_tasks(schedule, current_task, current_tick)
                current_tick += 1

            if verbose:
                print(f'Tick {current_tick - ticks_run} to {current_tick - 1}: Task {current_task.name} ran for {ticks_run} ticks; remaining duration {current_task.duration}')

            # If the task still has remaining duration, add it back to the end of the queue
            if current_task.duration > 0:
                queue.append(current_task)

        return schedule
    
    def _calc_avg_wait(self, schedule):
        # Calculate the average wait time
        total_wait = 0
        for task in self.tasks:
            wait = schedule[task.name].index('#') - task.arrival
            total_wait += wait

        return total_wait / len(self.tasks)

    def _calc_avg_response(self, schedule):
        # Calculate the average response time
        total_response = 0
        for task in self.tasks:
            response = schedule[task.name].index('#') - task.arrival
            total_response += response

        return total_response / len(self.tasks)

    def _calc_avg_throughput(self, schedule, num_cycles=10):
        # Calculate the average throughput
        total_throughput = 0
        for task in self.tasks:
            if '' not in schedule[task.name]:
                end = len(schedule[task.name])+1
            else:
                end = schedule[task.name].index('')
            throughput = num_cycles / (end - task.arrival)
            total_throughput += throughput

        return total_throughput / len(self.tasks)
    
    def calc_metrics(self, schedule):
        metrics = {}
        metrics['avg_wait'] = self._calc_avg_wait(schedule)
        metrics['avg_response'] = self._calc_avg_response(schedule)
        metrics['avg_throughput'] = self._calc_avg_throughput(schedule)
        return metrics
    
    def print_metrics(self, metrics):
        for key, value in metrics.items():
            print(f'{key}: {value:.2f}')

    # Public members
    def schedule(self, schedule_type='FIFO', verbose=True):
        schedule_map = {
            'FIFO': self._schedule_fifo,
            'SJF': self._schedule_sjf,
            'SRTF': self._schedule_srtf,
            'RR': self._schedule_rr
        }

        if schedule_type not in schedule_map:
            raise ValueError(f'Invalid schedule type {schedule_type}, valid types are {schedule_map.keys()}')
        
        return schedule_map[schedule_type](verbose)

    def print_schedule(self, schedule):
        # schedule i.e. 
        # {
        # 'A': ['-', '-', '_', '#', '#', '']
        # }
        # symbols:
        # '-' - not yet arrived
        # '_' - arrived, but not started
        # '#' - started/running
        # '' - completed

        for task, symbols in schedule.items():
            print(f'{task}: {"".join(symbols)}')

def print_tasks(tasks):
    print('Current tasks:')
    for task in tasks:
        print(task)
    print()

def main():
    """
    Main function to test various scheduling techniques.
    """
    tasks = [
        Task('A', 0, 5),
        Task('B', 1, 3),
        Task('C', 2, 2),
        Task('D', 3, 4),
        Task('E', 4, 1)
    ]

    print_tasks(tasks)

    # Test all scheduler types
    scheduler_types_to_test = ['FIFO', 'SJF', 'SRTF', 'RR']
    for schedule_type in scheduler_types_to_test:
        # Schedule the tasks
        print(f'==={schedule_type}===')
        scheduler = Scheduler(copy.deepcopy(tasks))
        schedule = scheduler.schedule(schedule_type, verbose=False)
        
        # Calculate and retrieve metrics
        metrics = scheduler.calc_metrics(schedule)
        scheduler.print_metrics(metrics)

        # Print the schedule
        scheduler.print_schedule(schedule)
        print()

if __name__ == '__main__':
    main()