import yaml
import glob

from scripts.infocom.run_single_exp import run_single_exp

mem_limit = '1G'
reverse = False
# Load list of experiments
path_base = './experiments/infocom/batch'
base_file = '{}/{}'.format(path_base, 'base.yaml')
# with open(base_file, 'r') as stream:
#     try:
#         # print(yaml.safe_load(stream).keys())
#         config = yaml.safe_load(stream)
#
#     except yaml.YAMLError as exc:
#         print(exc)
#         exit(1)

config_directory = '{}/*/configs/{}/*.exp.yaml'.format(path_base, mem_limit)
print(config_directory)
exp_config_files = [f for f in glob.glob(config_directory)]
base_script = 'sudo bash ./scripts/poisson_exp/poisson_rv_nf_base.sh'

for exp_file in exp_config_files:
    print(exp_file)
    run_single_exp(exp_file, base_script)
    break;

