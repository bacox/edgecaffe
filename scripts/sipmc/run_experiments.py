import collections

from scripts.sipmc.run_single_exp import run_single_exp

QueueItem = collections.namedtuple('QueueItem', 'name configPath')
base_script = 'sudo bash ./scripts/sipmc/exp_base_cgroup.sh'


def exp_call(qi: QueueItem, dry_run: bool = False):
    build_folder = '.'
    record_thermal = True
    exp_file = qi.configPath

    run_single_exp(exp_file, base_script, dry_run=dry_run, build_folder=build_folder, record_thermal=record_thermal)



if __name__ == '__main__':
    print('Run experiments')
    qi = QueueItem("first_test", "experiments/example/configs/2G/masa-example.yaml")
    exp_call(qi, False)