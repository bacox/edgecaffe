import os
from typing import List
import yaml
import itertools



# Read the configuration that was stored in the bash scripts
def network_args(list_of_networks: List[str]) -> str:
    """
    Format list of networks into cli arguments
    :param list_of_networks:
    :return: formatted string
    """
    return ' '.join(['--network={}'.format(n) for n in list_of_networks])


def gen_cmd_config_call(cmd_base: str, pathToConfig: str) -> str :
    cmd_str = './{} --read-config={}'.format(cmd_base, pathToConfig)
    return cmd_str

def gen_cmd_call(cmd_base: str, pathToConfig: str, repetitions: int, rho: int, mem_limit: str, mode: str, ait_multiplier: float, networks: List[str], iat: float, n_workers: int) -> str:
    """
    Generate command combined with all parameters correctly formatted.
    :param cmd_base: path to binary to call
    :param pathToConfig: path to config file for binary
    :param repetitions: number of arrivals
    :param rho: not actively used right now
    :param mem_limit: Mmory limit: one of ['512M', '1G', '2G', '4G']
    :param mode: scheduling algorithm: one of [bulk, partial, deepeye, masa-e, linear, ...]
    :param ait_multiplier: to set the traffic intensity by multiplying it to the inter-arrival-time. Lower is more  traffic intensity, higher is less traffic
    :param networks: Set of networks to arrive simultaneously: e.g. [AgeNet, GenderNet]
    :param iat: the inter-arrival-time in ms
    :param n_workers: number of workers/threads available for the algorithm
    :return: Formatted string the can be called by the OS
    """
    output_prefix = 'r{}-m{}-{}-{}-w{}'.format(rho, ait_multiplier, mem_limit, mode, n_workers)
    cmd_str = './{} --read-config={} --output-prefix={} --mem_limit={} --verbose=false --num-arrivals={} --poisson-distribution=true --rho={} --iat={} --mode={} --n-workers={} {}'.format(cmd_base, pathToConfig, output_prefix, mem_limit, repetitions, rho, iat, mode, n_workers, network_args(networks))
    return cmd_str


def print_run_call(progress_text, cmd, repetitions, rho, mem_limit, mode, ait_multiplier,networks, iat):
    """
    Print the most important parameters for this run
    :param progress_text: progress in a series of experiments: [N/#]
    :param cmd: The full command to be executed including its formatted cli arguments
    :param repetitions: Number of arrivals
    :param rho: rho
    :param mem_limit: Memory capacity
    :param mode: scheduling algorithm to be used
    :param ait_multiplier: Traffic intensity settings
    :param networks: Set of networks in an arrival
    :param iat: Inter arrival time in ms
    :return:
    """
    lines = []
    lines.append('Running with {} LIMIT'.format(mem_limit))
    lines.append('Running with {} MODE'.format(mode))
    lines.append('Running with {} REPETITIONS'.format(repetitions))
    lines.append('Running with {} NETWORKS'.format('-'.join(networks)))
    lines.append('Running with {} RHO'.format(rho))
    lines.append('Running with {} AIT'.format(iat))
    lines.append('Running variation {}'.format(progress_text))
    max_len = max([len(line) for line in lines])
    print('\n' + ('*'*(max_len+4)))
    for line in lines:
        print('* {}{} *'.format(line, ' '*(max_len - len(line))))
    print('*'*(max_len+4))
    print(cmd)


def run_single_exp(config_file, base_script):
    config = None
    with open(config_file, 'r') as stream:
        try:
            # print(yaml.safe_load(stream).keys())
            config = yaml.safe_load(stream)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    memory_constraint = config['mem_limit']
    build_folder = config['build_folder']
    n_workers = config['num_workers']
    cmd_base = config['cmd_base']
    CMD = gen_cmd_config_call(cmd_base, config_file)

    # print(rho, memory_constraints, modes, ait_multipliers, networks)
    # service_time = config['mean-service-time'][memory_constraints]
    # iat = service_time * ait_multipliers
    # CMD = gen_cmd_call(cmd_base, pathToConfig, repetitions, rho, memory_constraints, modes, ait_multipliers, networks, iat, n_worker)




    script_cmd = '{} {} {} \'{}\''.format(base_script, memory_constraint, build_folder, CMD)
    # padding = ''
    # if idx < 10:
    #     padding = ' '
    # progress_text = '[{}{}/{}]'.format(padding, idx, numberOfVariations)
    # print_run_call(progress_text, CMD, repetitions, rho, memory_constraints, modes, ait_multipliers, networks, iat)
    print(script_cmd)
    # print(script_cmd)
    os.system(script_cmd)
    # idx += 1