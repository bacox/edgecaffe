import os
from typing import List
import yaml
import itertools

# Read config file
pathToConfig = 'config/masa/exp-python-poisson-rv-nf-memory-matters.yaml'


# Read the configuration that was stored in the bash scripts
def network_args(list_of_networks: List[str]) -> str:
    """
    Format list of networks into cli arguments
    :param list_of_networks:
    :return: formatted string
    """
    return ' '.join(['--network={}'.format(n) for n in list_of_networks])

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


def main():
    config = None
    with open(pathToConfig, 'r') as stream:
        try:
            # print(yaml.safe_load(stream).keys())
            config = yaml.safe_load(stream)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    rho = config['rho']
    # networks = [['AgeNet', 'GenderNet', 'FaceNet']]
    memory_constraints = config['memory-constraints']
    repetitions = config['repetitions']
    modes = config['modes']
    cmd_base = config['cmd-base']
    build_folder = config['build-folder']
    output_path = config['output-path']

    networks = config['networks']
    ait_multipliers = config['ait_multipliers']
    n_workers = config['n_workers']
    # ait_multipliers = [0.6, 0.7, 0.8, 0.9, 0.925, 0.95, 0.975, 1, 1.025, 1.05, 1.075, 1.1]
    # ait_multipliers = [0.6, 0.7, 0.8, 0.9, 0.925, 0.95, 0.975, 1]
    # ait_multipliers = [1.1]

    cwd = os.getcwd()

    variations = list(itertools.product(*[rho, memory_constraints, modes, ait_multipliers, networks, n_workers]))
    # pathToConfig = './config/pipeline-rv-nf-poisson-arrival.yaml'
    numberOfVariations = (len(rho) * len(memory_constraints) * len(modes) * len(ait_multipliers) * len(networks) * len(n_workers))


    lines = []
    lines.append(['command',cmd_base])
    lines.append(['build folder',build_folder])
    lines.append(['path to config',pathToConfig])
    lines.append(['output path', output_path])
    lines.append(['Number of workers', ' '.join([str(n) for n in n_workers])])
    lines.append(['number of variations',str(numberOfVariations)])
    lines.append(['rho',' '.join([str(r) for r in rho])])
    lines.append(['networks',' '.join([str(r) for r in networks])])
    lines.append(['memory constraints',' '.join([str(r) for r in memory_constraints])])
    lines.append(['ait-multipliers',' '.join([str(r) for r in ait_multipliers])])
    lines.append(['modes',' '.join([str(r) for r in modes])])
    separator = ': '

    ## Print base config
    max_len = max([len(line[0]) + len(separator) +  len(line[1]) for line in lines])
    header = 'Base Configuration'
    print('*'*(max_len+4))
    print('* {}{} *'.format(header, ' '*(max_len - len(header))))
    print('*'*(max_len+4))
    for line in lines:
        print('* {}{}{}{} *'.format(line[0], separator, ' '*(max_len - (len(line[0]) + len(separator)+len(line[1]))), line[1]))
    print('*'*(max_len+4))

    input("Press Enter to continue...")

    # Start running commands
    base_script = 'sudo bash ./scripts/poisson_exp/poisson_rv_nf_base.sh'

    idx = 1
    # execute = False
    for rho, memory_constraints, modes, ait_multipliers, networks, n_worker in variations:
        # print(gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation))

        # if variation ==  (1, '1G', 'deepeye', 0.975):
        #     print('continue from here')
        #     execute = True
        # if not execute:
        #     continue
        print(rho, memory_constraints, modes, ait_multipliers, networks)
        service_time = config['mean-service-time'][memory_constraints]
        iat = service_time * ait_multipliers
        CMD = gen_cmd_call(cmd_base, pathToConfig, repetitions, rho, memory_constraints, modes, ait_multipliers, networks, iat, n_worker)




        script_cmd = '{} {} {} \'{}\''.format(base_script, memory_constraints, build_folder, CMD)
        padding = ''
        if idx < 10:
            padding = ' '
        progress_text = '[{}{}/{}]'.format(padding, idx, numberOfVariations)
        print_run_call(progress_text, CMD, repetitions, rho, memory_constraints, modes, ait_multipliers, networks, iat)
        print(script_cmd)
        # print(script_cmd)
        os.system(script_cmd)
        idx += 1

    print('Done')


if __name__ == "__main__":
    main()