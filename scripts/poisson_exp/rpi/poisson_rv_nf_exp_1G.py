
# Read config file
import os

pathToConfig = 'config/rpi/exp/exp-python-poisson-1G.yaml'

import time
import yaml
import itertools

# Read the configuration that was stored in the bash scripts
def network_args(list_of_networks):
    return ' '.join(['--network={}'.format(n) for n in list_of_networks])

def gen_cmd_call(cmd_base , pathToConfig, repetitions, rho, mem_limit, mode, ait_multiplier,networks, iat):
    # print('Got config for roh: {}, mem limit: {}, and mode: {}'. format(rho, mem_limit, mode))
    output_prefix = 'r{}-m{}-{}-{}-'.format(rho, ait_multiplier, mem_limit, mode)
    # cmd_str = './{} --read-config={} --output-prefix={} --mem_limit={} --verbose=false --num-arrivals={} --poisson-distribution={} --network={} --mode={}'.format(cmd_base, pathToConfig, output_prefix, mem_limit, repetitions, use_poisson, selected_network, mode)
    cmd_str = './{} --read-config={} --output-prefix={} --mem_limit={} --verbose=false --num-arrivals={} --poisson-distribution=true --rho={} --iat={} --mode={} {}'.format(cmd_base, pathToConfig, output_prefix, mem_limit, repetitions, rho, iat, mode, network_args(networks))
    return cmd_str


def print_run_call(progress_text, cmd, repetitions, rho, mem_limit, mode, ait_multiplier,networks, iat):
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

config = None
with open(pathToConfig, 'r') as stream:
    try:
        # print(yaml.safe_load(stream).keys())
        config = yaml.safe_load(stream)

    except yaml.YAMLError as exc:
        print(exc)
        exit(1)

rho = config['rho']
networks = [['FaceNet', 'FaceNet']]
memory_constraints = config['memory-constraints']
repetitions = config['repetitions']
modes = config['modes']
cmd_base = config['cmd-base']
build_folder = config['build-folder']
output_path = config['output-path']
ait_multipliers = [0.6, 0.7, 0.8, 0.9, 0.925, 0.95, 0.975, 1, 1.025, 1.05, 1.075, 1.1]
# ait_multipliers = [1.1]

cwd = os.getcwd()

variations = list(itertools.product(*[rho, memory_constraints, modes, ait_multipliers, networks]))
# pathToConfig = './config/pipeline-rv-nf-poisson-arrival.yaml'
numberOfVariations = (len(rho) * len(memory_constraints) * len(modes) * len(ait_multipliers) * len(networks))


lines = []
lines.append(['command',cmd_base])
lines.append(['build folder',build_folder])
lines.append(['path to config',pathToConfig])
lines.append(['output path', output_path])
lines.append(['number of variations',str(numberOfVariations)])
lines.append(['number of repetitions',str(repetitions)])
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

# input("Press Enter to continue...")
print("Input free mode. Experiment will start after 10 seconds, cancel before that to abort!")
time.sleep(10)

# Start running commands
base_script = 'sudo bash ./scripts/poisson_exp/rpi/poisson_rv_nf_base_limit.sh'

idx = 1
# execute = False
for rho, memory_constraints, modes, ait_multipliers, networks in variations:
    # print(gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation))

    # if variation ==  (1, '1G', 'deepeye', 0.975):
    #     print('continue from here')
    #     execute = True
    # if not execute:
    #     continue
    print(rho, memory_constraints, modes, ait_multipliers, networks)
    service_time = config['mean-service-time'][memory_constraints]
    iat = service_time * ait_multipliers
    CMD = gen_cmd_call(cmd_base, pathToConfig, repetitions, rho, memory_constraints, modes, ait_multipliers, networks, iat)




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