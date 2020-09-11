import argparse
import collections
from pathlib import Path
from typing import List

available_commands = [
    'list'
    , 'size'
    , 'add'
    , 'halt'
    , 'run'
]

queue_file = 'queue.tmp.txt'
pre_queue_file = 'pre-queue.tmp.txt'

QueueItem = collections.namedtuple('QueueItem', 'name configPath')


class ExperimentQueue:
    def __init__(self):
        self.queueItems: List[QueueItem] = []
        self.delimiter = ' '

    def size(self) -> int:
        return len(self.queueItems)

    def add_from_line(self, line: str):
        parts = line.split(self.delimiter)
        self.add(QueueItem(name=parts[0], configPath=parts[1]))

    def add(self, item: QueueItem):
        self.queueItems.append(item)

    def next(self) -> QueueItem:
        return self.queueItems.pop(0)

    def to_lines(self) -> List[str]:
        lines = []
        for item in self.queueItems:
            lines.append(self.delimiter.join([item.name, item.configPath]))
        return lines

    @classmethod
    def load_from_lines(cls, lines):
        eq = cls()
        for line in lines:
            eq.add_from_line(line)
        return eq


def load_queue(filename: str) -> ExperimentQueue:
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    p = Path(filename)
    with p.open(mode='r') as f:
        lines = f.readlines()
        lines = [x.rstrip() for x in lines if x.rstrip() != '']
        return ExperimentQueue.load_from_lines(lines)


def reload_queue(eq: ExperimentQueue, filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    save_queue(eq, filename)
    return load_queue(filename)


def write_queue(eq: ExperimentQueue, filename: str, append=False):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    mode = 'w'
    if append:
        mode = 'a'
    p = Path(filename)
    with p.open(mode=mode) as f:
        lines = eq.to_lines()
        lines = [x + '\n' for x in lines]
        f.writelines(lines)
        f.flush()


def append_queue(eq: ExperimentQueue, filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    write_queue(eq, filename, append=True)


def save_queue(eq: ExperimentQueue, filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    write_queue(eq, filename, append=False)


def clear_file_contents(filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    p = Path(filename)
    with p.open(mode='w') as f:
        f.write('')
        f.flush()


def file_exists(filename: str) -> bool:
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    return Path(filename).is_file()


def ensure_files_exists():
    create_file(queue_file)
    create_file(pre_queue_file)


def create_file(filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    p = Path(filename)
    p.touch()


def cmd_run(dry_run=False):
    queue = load_queue(queue_file)
    count = queue.size()
    while count:
        next_item = queue.next()
        save_queue(queue, queue_file)
        print(f'Found a new item {next_item}')

        # Execute experiment

        # Update count
        queue = load_queue(queue_file)
        count = queue.size()
    print('Executing cmd run')


def cmd_halt(dry_run=False):
    queue = load_queue(queue_file)
    append_queue(queue, pre_queue_file)
    clear_file_contents(queue_file)
    print(f'{queue.size()} experiments are set back to the pre-queue')


def cmd_add(dry_run=False):
    pre_queue = load_queue(pre_queue_file)
    append_queue(pre_queue, queue_file)
    clear_file_contents(pre_queue_file)
    print(f'{pre_queue.size()} experiments are added to the queue')


def cmd_list(dry_run=False):
    queue = load_queue(queue_file)
    count = queue.size()
    save_queue(queue, queue_file)
    print(f'Number of queued experiments is {count}')


def exec_sumcommand(cmd, **kwargs):
    if cmd == 'list':
        cmd_list()
    elif cmd == 'size':
        cmd_list()
    elif cmd == 'run':
        cmd_run()
    elif cmd == 'add':
        cmd_add()
    elif cmd == 'halt':
        cmd_halt()
    else:
        print(f'Invalid command \'{cmd}\'!')
        exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("InfoCom Experiment runner. Use this tool to run multiple experiments")
    parser.add_argument("subcommand", type=str, help="Command to execute", choices=available_commands)
    parser.add_argument("--file", type=str, help="File to be used. The usage depends on the subcommand. 'list' and "
                                                 "'size' do not use the file command, while 'add' uses the file "
                                                 "arguments to point to the correct file to add to the queue")
    parser.add_argument("--dry-run", help="Do a dry run; No calling the generated command", action='store_true')
    args = parser.parse_args()
    ensure_files_exists()
    exec_sumcommand(args.subcommand)
