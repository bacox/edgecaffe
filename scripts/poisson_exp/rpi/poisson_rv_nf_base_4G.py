
# Read config file
import os

pathToConfig = 'config/rpi/exp-python-poisson-base-4G.yaml'

import yaml
import itertools

# Read the configuration that was stored in the bash scripts


def gen_cmd_call(cmd_base , pathToConfig, repetitions, mem_limit, mode, selected_network, use_poisson):
    # print('Got config for roh: {}, mem limit: {}, and mode: {}'. format(rho, mem_limit, mode))
    output_prefix = '{}-{}-{}-'.format(selected_network, mem_limit, mode)
    cmd_str = './{} --read-config={} --output-prefix={} --mem_limit={} --verbose=false --num-arrivals={} --poisson-distribution={} --network={} --mode={}'.format(cmd_base, pathToConfig, output_prefix, mem_limit, repetitions, use_poisson, selected_network, mode)
    return cmd_str


def print_run_call(progress_text, cmd, repetitions, mem_limit, mode, selected_network):
    lines = []
    lines.append('Running with {} LIMIT'.format(mem_limit))
    lines.append('Running with {} MODE'.format(mode))
    lines.append('Running with {} REPETITIONS'.format(repetitions))
    lines.append('Running with {} NETWORKS'.format(selected_network))
    # lines.append('Running with {} RHO'.format(rho))
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

# rho = config['rho']
rho = []
# networks = ['AgeNet', 'GenderNet','FaceNet', 'SoS_GoogleNet', 'SoS']
networks = ['FaceNet']
memory_constraints = config['memory-constraints']
repetitions = config['repetitions']
modes = config['modes']
cmd_base = config['cmd-base']
build_folder = config['build-folder']
output_path = config['output-path']
poisson_distribution = config['poisson-distribution']
cwd = os.getcwd()

variations = list(itertools.product(*[memory_constraints, modes, networks]))
# pathToConfig = './config/pipeline-rv-nf-poisson-arrival.yaml'
numberOfVariations = (len(memory_constraints) * len(modes) * len(networks))

lines = []
lines.append(['command',cmd_base])
lines.append(['build folder',build_folder])
lines.append(['path to config',pathToConfig])
lines.append(['output path', output_path])
lines.append(['number of variations',str(numberOfVariations)])
lines.append(['networks',' '.join([str(r) for r in networks])])
lines.append(['memory constraints',' '.join([str(r) for r in memory_constraints])])
lines.append(['modes',' '.join([str(r) for r in modes])])
lines.append(['poisson-distribution', str(poisson_distribution)])
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
base_script = 'sudo bash ./scripts/poisson_exp/rpi/poisson_rv_nf_base.sh'

idx = 1
for variation in variations:
    # print(gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation))
    CMD = gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation,poisson_distribution)
    script_cmd = '{} {} {} \'{}\''.format(base_script, variation[0], build_folder, CMD)
    padding = ''
    if idx < 10:
        padding = ' '
    progress_text = '[{}{}/{}]'.format(padding, idx, numberOfVariations)
    print_run_call(progress_text, CMD, repetitions, *variation)
    print(script_cmd)
    os.system(script_cmd)
    idx += 1

print('Done')