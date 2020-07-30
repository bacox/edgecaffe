import os
from typing import List
import yaml
import itertools

def gen_cmd_config_call(cmd_base: str, pathToConfig: str) -> str :
    cmd_str = './{} --read-config={}'.format(cmd_base, pathToConfig)
    return cmd_str


def run_single_exp(config_file, base_script, dry_run : bool = False, build_folder='.'):
    config = None
    with open(config_file, 'r') as stream:
        try:
            config = yaml.safe_load(stream)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    memory_constraint = config['mem_limit']
    # build_folder = config['build_folder']
    n_workers = config['num_workers']
    cmd_base = config['cmd_base']
    CMD = gen_cmd_config_call(cmd_base, config_file)

    # Get data to construct a output file for thermal measurements
    exp_output_base_path = config['output-path']
    exp_tag = config['tag']
    thermal_filename = 'thermal.csv'
    # This file path below is unique for the experiment to be run
    thermal_file_path = '{}/{}-{}'.format(exp_output_base_path, exp_tag, thermal_filename)

    script_cmd = '{} {} {} \'{}\''.format(base_script, memory_constraint, build_folder, CMD)

    print(script_cmd)

    ##
    # @TODO: Insert start measuring thermal values here
    ##
    if not dry_run:
        os.system(script_cmd)
    ##
    # @TODO: Insert stop measuring thermal values here
    ##
