
# Read config file
import os

pathToConfig = 'config/exp-python-poisson-rv-nf-100.yaml'

import yaml
import itertools

# Read the configuration that was stored in the bash scripts


def gen_cmd_call(cmd_base , pathToConfig, repetitions, rho, mem_limit, mode):
    # print('Got config for roh: {}, mem limit: {}, and mode: {}'. format(rho, mem_limit, mode))
    output_prefix = 'r{}-{}-{}-'.format(rho, mem_limit, mode)
    cmd_str = './{} --read-config={} --output-prefix={} --mem_limit={} --num-arrivals={} --rho={} --mode={}'.format(cmd_base, pathToConfig, output_prefix, mem_limit, repetitions, rho, mode)
    return cmd_str


def print_run_call(progress_text, cmd, repetitions, rho, mem_limit, mode):
    lines = []
    lines.append('Running with {} LIMIT'.format(mem_limit))
    lines.append('Running with {} MODE'.format(mode))
    lines.append('Running with {} REPETITIONS'.format(repetitions))
    lines.append('Running with {} RHO'.format(rho))
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
memory_constraints = config['memory-constraints']
repetitions = config['repetitions']
modes = config['modes']
cmd_base = config['cmd-base']
build_folder = config['build-folder']
output_path = config['output-path']
cwd = os.getcwd()

variations = list(itertools.product(*[rho, memory_constraints, modes]))
# pathToConfig = './config/pipeline-rv-nf-poisson-arrival.yaml'
numberOfVariations = (len(rho) * len(memory_constraints) * len(modes))

lines = []
lines.append(['command',cmd_base])
lines.append(['build folder',build_folder])
lines.append(['path to config',pathToConfig])
lines.append(['output path', output_path])
lines.append(['number of variations',str(numberOfVariations)])
lines.append(['rho',' '.join([str(r) for r in rho])])
lines.append(['memory constraints',' '.join([str(r) for r in memory_constraints])])
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
for variation in variations:
    # print(gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation))
    CMD = gen_cmd_call(cmd_base, pathToConfig, repetitions, *variation)
    script_cmd = '{} {} {} \'{}\''.format(base_script, variation[1], build_folder, CMD)
    padding = ''
    if idx < 10:
        padding = ' '
    progress_text = '[{}{}/{}]'.format(padding, idx, numberOfVariations)
    print_run_call(progress_text, CMD, repetitions, *variation)
    # print(script_cmd)
    # print(script_cmd)
    os.system(script_cmd)
    idx += 1

print('Done')