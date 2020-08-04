import os
from typing import List

import dataclasses as dataclass
import yaml
import itertools
import collections
from pathlib import Path

def path_exists(path):
    Path(path).mkdir(parents=True, exist_ok=True)

BaseConfig = collections.namedtuple('BaseConfig', 'network_path resources_path output_path arrival_mode n_arrivals '
                                                  'cmd_base build_folder')
Network = collections.namedtuple('Network', 'networkName pathToNetwork')
Arrival = collections.namedtuple('Arrival', 'networks pathToData time')
ExpConfig = collections.namedtuple('ExpConfig', 'network_path resources_path output_path arrival_mode n_arrivals '
                                             'cmd_base build_folder tag mem_limit mode n_workers arrivals')

network_dict = {
    'AgeNet': Network(networkName='AgeNet', pathToNetwork='AgeNet'),
    'GenderNet': Network(networkName='GenderNet', pathToNetwork='GenderNet'),
    'FaceNet': Network(networkName='FaceNet', pathToNetwork='FaceNet'),
    'ObjectNet': Network(networkName='ObjectNet', pathToNetwork='ObjectNet')
    , 'TinyYolo': Network(networkName='TinyYolo', pathToNetwork='TinyYolo')
    , 'EmotionNet': Network(networkName='EmotionNet', pathToNetwork='EmotionNet')
    , 'MemNet': Network(networkName='MemNet', pathToNetwork='MemNet')
    , 'SceneNet': Network(networkName='SceneNet', pathToNetwork='SceneNet')
    , 'SoS': Network(networkName='SoS', pathToNetwork='SoS')
}


def generateArrivalList(n_arrivals: int, arrivalMode: str, network_tags: List[str]) -> List[Arrival]:
    pathToData = 'test_1.jpg'
    arrival_list = []
    if arrivalMode == 'batch':
        for arrival_idx in range(n_arrivals):
            networks = [{**network_dict.get(x)._asdict()} for x in network_tags]
            arrival_list.append({**Arrival(networks, pathToData, 0)._asdict()})

    else:
        pass
    return arrival_list

def generate_exp_config(base: BaseConfig, mode: str, mem_limit: str, n_worker: int, network_tags: List[Network]):
    exp_tag_name = 'first_exp'
    arrivals = generateArrivalList(base.repetitions, base.arrival_mode, network_tags)
    exp_config = ExpConfig(**base.__dict__, tag=exp_tag_name, mem_limit=mem_limit, mode=mode, num_workers=n_worker, arrivals=arrivals)
    return exp_config

def base_exp_config(config):
    experiments = config['experiments']

    for experiment_name in experiments:
        path_to_exp = '{}/{}'.format(config['path_to_base_exp'], experiment_name)
        path_to_exp_file = '{}/{}'.format(path_to_exp, 'description.yaml')
        with open(path_to_exp_file, 'r') as stream:
            exp_config = yaml.safe_load(stream)
            exp_config = {**exp_config, **config}
            # exp_config.pop()
            exp_config['path-to-exp'] = path_to_exp
            generate_variations(exp_config)
        # print(path_to_exp_file)

def reset_config_position(dict, key: str):
    tmp_value = dict[key]
    dict.pop(key)
    dict[key] = tmp_value

def dict_underscore_to_dash(dict, key: str):
    new_key = key.replace('_','-')
    dict[new_key] = dict[key]
    dict.pop(key)

# network_path resource_path output_path arrival_mode repetitions '
# 'cmd_base build_folder
def generate_variations(config):
    memory_constraints = config['memory-constraints']


    networks = config['networks']
    ait_multipliers = config['ait-multiplier']
    exp_base_tag = config['experiment-tag']
    modes = config['modes']
    n_workers_list = config['n_workers']
    base_config = BaseConfig(config['network-path'], config['resources-path'], config['output-path-base']
                             , config['arrival-mode'], config['repetitions']
                             , config['cmd-base'], config['build-folder'] )
    for memory_constraint in memory_constraints:
        variations = list(itertools.product(*[modes, n_workers_list, ait_multipliers]))
        for mode, n_workers, ait in variations:
            exp_tag = '{}-{}-{}-w{}-a{}'.format(exp_base_tag, memory_constraint, mode, n_workers,ait)
            config_file_path = '{}/{}/{}'.format(config['path-to-exp'], 'configs', memory_constraint)
            config_file = '{}/{}.exp.yaml'.format(config_file_path, exp_tag)
            path_exists(config_file_path)
            # tag mem_limit mode num_workers arrivals
            arrivals = generateArrivalList(base_config.n_arrivals, base_config.arrival_mode, networks)
            exp_config = ExpConfig(**base_config._asdict(), tag=exp_tag, mem_limit=memory_constraint, mode=mode, n_workers=n_workers, arrivals=arrivals)
            # exp_config.output_path = '{}/{}'.format(exp_config.output_path, exp_base_tag)

            with open(config_file, 'w') as yaml_file:
                # print(yaml.safe_dump({**exp_config._asdict()}))
                tmp_dict = {**exp_config._asdict()}
                tmp_dict['output_path'] = '{}/{}'.format(exp_config.output_path, exp_base_tag)

                dict_underscore_to_dash(tmp_dict, 'n_arrivals')
                dict_underscore_to_dash(tmp_dict, 'n_workers')
                dict_underscore_to_dash(tmp_dict, 'network_path')
                dict_underscore_to_dash(tmp_dict, 'resources_path')
                dict_underscore_to_dash(tmp_dict, 'output_path')
                dict_underscore_to_dash(tmp_dict, 'arrival_mode')
                tmp_dict['arrival-list'] = config_file
                tmp_dict['output-prefix'] = exp_tag
                reset_config_position(tmp_dict, 'arrivals')
                yaml.safe_dump(tmp_dict, yaml_file, sort_keys=False)

    print(memory_constraints)

def main():
    path_to_exp_base = './experiments/infocom/batch'

    path_to_base_config = '{}/base.yaml'.format(path_to_exp_base)

    with open(path_to_base_config, 'r') as stream:
        try:
            # print(yaml.safe_load(stream).keys())
            config = yaml.safe_load(stream)
            config['path_to_base_exp'] = path_to_exp_base
            base_exp_config(config)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    print(config)

if __name__ == "__main__":
    main()
