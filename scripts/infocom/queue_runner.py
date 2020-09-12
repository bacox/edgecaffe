import argparse
import collections
from pathlib import Path
from typing import List
from datetime import datetime
from run_single_exp import run_single_exp
import glob

available_commands = [
    'list'
    , 'size'
    , 'add'
    , 'halt'
    , 'run'
    , 'config2queue'
]
current_path = Path(__file__).parent

queue_file = str(current_path / 'queue.tmp.txt')
pre_queue_file = str(current_path / 'pre-queue.tmp.txt')
dry_run_backup_file = str(current_path / 'dry-run.tmp.txt')
QueueItem = collections.namedtuple('QueueItem', 'name configPath')
base_script = 'sudo bash ./scripts/infocom/exp_base_cgroup.sh'


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


def print_progress(idx: int, total: int):
    print("")
    print('*' * 25)
    print('Running experiment {}'.format(idx + 1))
    print('Progress {}/{}'.format(idx + 1, total))
    print('*' * 25)


def write_progress_to_log(idx: int, total: int, exp_file: str, log_file: str = './out.log', started: bool = False):
    now = datetime.now()
    datetime_str = now.strftime("%d/%m/%Y %H:%M:%S")
    action = 'Finished'
    if started:
        action = 'Started\t'
    line = '{} {}\t{}/{}\t{}\n'.format(action, datetime_str, idx, total - 1, exp_file)
    with open(log_file, "a") as log_file:
        log_file.write(line)
        log_file.flush()


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
    create_file(dry_run_backup_file)


def create_file(filename: str):
    assert isinstance(filename, str), f'Argument of wrong type, should be a string and is a {type(filename)}!'
    p = Path(filename)
    p.touch()


def exp_call(qi: QueueItem, dry_run: bool = False):
    build_folder = '.'
    record_thermal = True
    exp_file = qi.configPath

    run_single_exp(exp_file, base_script, dry_run=dry_run, build_folder=build_folder, record_thermal=record_thermal)


def cmd_run(dry_run=False):
    queue = load_queue(queue_file)
    total = count = queue.size()
    print(f'There are {queue.size()} experiments to be run')
    save_queue(queue, dry_run_backup_file)
    while count:
        next_item = queue.next()
        save_queue(queue, queue_file)
        idx = total - count
        # print(f'Found a new item {next_item}')
        print_progress(idx, total)
        # Execute experiment
        try:
            write_progress_to_log(idx, total, next_item.configPath, started=True)
            exp_call(next_item, dry_run=dry_run)
        except FileNotFoundError:
            print(f'Cannot find file: {next_item.configPath}')
        finally:
            write_progress_to_log(idx, total, next_item.configPath)
        # Update count
        queue = load_queue(queue_file)
        count = queue.size()
    if dry_run:
        dry_run_queue = load_queue(dry_run_backup_file)
        save_queue(dry_run_queue, queue_file)


def cmd_halt(dry_run=False):
    queue = load_queue(queue_file)
    append_queue(queue, pre_queue_file)
    clear_file_contents(queue_file)
    print(f'{queue.size()} experiments are set back to the pre-queue')


def cmd_add(dry_run=False, filename: str = pre_queue_file):
    pre_queue = load_queue(filename)
    append_queue(pre_queue, queue_file)
    clear_file_contents(filename)
    print(f'{pre_queue.size()} experiments are added to the queue')


def cmd_list(dry_run=False):
    queue = load_queue(queue_file)
    count = queue.size()
    save_queue(queue, queue_file)
    print(f'Number of queued experiments is {count}')

def parse_name(path: str) -> str:
    parts = path.split('/')
    return '-'.join([parts[3], parts[4], parts[6]])

def cmd_configs_to_queue_file(dry_run=False, filename: str = 'experiments.tmp.txt'):
    if filename is None:
        filename = 'experiments.tmp.txt'
    glob_str = './experiments/infocom/stochastic-thesis/*rpi-4*/configs/*/*.yaml'
    files = glob.glob(glob_str)
    files = [parse_name(f) + ' ' + f for f in files]
    queue = ExperimentQueue.load_from_lines(files)
    save_queue(queue, filename)
    print(f'Saved {queue.size()} experiments to the file: \'{filename}\'')

def exec_sumcommand(cmd, **kwargs):
    if cmd == 'list':
        cmd_list(**kwargs)
    elif cmd == 'size':
        cmd_list(**kwargs)
    elif cmd == 'run':
        cmd_run(**kwargs)
    elif cmd == 'add':
        cmd_add(**kwargs)
    elif cmd == 'halt':
        cmd_halt(**kwargs)
    elif cmd == 'config2queue':
        cmd_configs_to_queue_file(**kwargs)
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
    exec_sumcommand(args.subcommand, dry_run=args.dry_run, filename=args.file)
