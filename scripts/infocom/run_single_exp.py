import os
import sys
from typing import List
import yaml
import itertools

def gen_cmd_config_call(cmd_base: str, pathToConfig: str) -> str :
    cmd_str = 'nice -n -15 ./{} --read-config={}'.format(cmd_base, pathToConfig)
    return cmd_str


def run_single_exp(config_file, base_script, dry_run : bool = False, build_folder='.', record_thermal: bool = True):
    config = None
    with open(config_file, 'r') as stream:
        try:
            config = yaml.safe_load(stream)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    memory_constraint = config['mem_limit']
    # build_folder = config['build_folder']
    n_workers = config['n-workers']
    cmd_base = config['cmd_base']
    CMD = gen_cmd_config_call(cmd_base, config_file)

    # Get data to construct a output file for thermal measurements
    exp_output_base_path = config['output-path']
    exp_tag = config['tag']
    thermal_filename = 'cpu_temp.log'
    # This file path below is unique for the experiment to be run
    thermal_file_path = '{}/{}-{}'.format(exp_output_base_path, exp_tag, thermal_filename)

    script_cmd = '{} {} {} \'{}\' {}'.format(base_script, memory_constraint, build_folder, CMD, n_workers)

    print(script_cmd)
    if record_thermal:
        ##
        # start measuring thermal values here
        ##
        start_thermal_cmd = 'sudo systemctl enable --now temperature.timer'
        os.system(start_thermal_cmd)
    if not dry_run:
        os.system(script_cmd)

    if record_thermal:
        ##
        # stop measuring thermal values here
        ##
        stop_thermal_cmd = 'sudo systemctl enable --now temperature.timer'
        os.system(stop_thermal_cmd)
        # Move file to corresponding experiment directory
        os.system('sudo mv /opt/analysis/thermal/cpu_temp.log {} 2>/dev/null'.format(thermal_file_path))


if __name__ == "__main__":
    config_file = sys.argv[1]
    base_script = sys.argv[2]
    run_single_exp(config_file=config_file, base_script=base_script, dry_run=False, build_folder='.', record_thermal=True)