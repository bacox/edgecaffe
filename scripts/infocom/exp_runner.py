import yaml
import glob
import argparse
from datetime import datetime
from scripts.infocom.run_single_exp import run_single_exp

def print_progress(idx: int, total: int):
    print("")
    print('*'*25)
    print('Running experiment {}'.format(idx))
    print('Progress {}/{}'.format(idx, total-1))
    print('*'*25)

def write_progress_to_log(idx: int, total: int, exp_file: str, log_file: str = './out.log'):
    now = datetime.now()
    datetime_str = now.strftime("%d/%m/%Y %H:%M:%S")
    line = 'Finished {}\t{}/{}\t{}\n'.format(datetime_str, idx, total-1, exp_file)
    with open(log_file, "a") as log_file:
        log_file.write(line)
        log_file.flush()

def main(path_base : str = './experiments/infocom/batch', mem_limit : str = '*', reverse: bool = False, dry_run : bool = False, logging: bool=False):

    config_directory = '{}/*/configs/{}/*.exp.yaml'.format(path_base, mem_limit)
    exp_config_files = [f for f in glob.glob(config_directory)]
    if reverse:
        exp_config_files.reverse()
    base_script = 'sudo bash ./scripts/infocom/exp_base_cgroup.sh'
    total_files = len(exp_config_files)
    for idx, exp_file in enumerate(exp_config_files):
        print_progress(idx, total_files)
        run_single_exp(exp_file, base_script, dry_run=dry_run)
        if logging:
            write_progress_to_log(idx, total_files, exp_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser("InfoCom Experiment runner. Use this tool to run multiple experiments")
    parser.add_argument("--dry-run", type=bool, help="Do a dry run; No calling the generated command")
    parser.add_argument("--reverse", type=bool, help="Run all the scripts in reverse order")
    parser.add_argument("--mem-limit", type=str, help="Filter the scripts by the memory limit: [2G, 1G 512M]")
    parser.add_argument("--path", type=str, help="Set the path pointing to the experiments")
    parser.add_argument("--logging", type=bool, help="Set write basic logging to the file out.log")
    args = parser.parse_args()
    args_dict = {
        'dry_run': False,
        'reverse': False,
        'logging': False
    }
    if args.dry_run:
        args_dict['dry_run'] = True
    if args.logging:
        args_dict['logging'] = True
    if args.reverse:
        args_dict['reverse'] = True
    if args.mem_limit:
        args_dict['mem_limit'] = args.mem_limit
    if args.path:
        args_dict['path_base'] = args.path

    main(**args_dict)
