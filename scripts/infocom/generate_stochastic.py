import collections
import itertools
from pathlib import Path
import random

import pandas as pd
import numpy as np
# import seaborn as sns
# import matplotlib.pyplot as plt
# import matplotlib.ticker as Ticker
# from pathlib import Path
# import yaml
# import glob
# import re
from typing import List
import copy

# from typing import Tuple
# from dataclasses import dataclass
# import os
# from typing import List
#
import dataclasses as dataclass
# import yaml
# import itertools
# import collections
# from pathlib import Path

#
# @dataclass
# class Network:
#     networkName: str
#     pathToNetwork: str
#
# @dataclass
# class Arrival:
#     networks: List[Network]
#     pathToData: str
#     time: int
import yaml


def find_close_series(target_mean: float, mu: float, sigma: float, n: int, offset: float= 0.005):
    delta = 1000
    while delta > offset:
        series = np.random.normal(mu, sigma, n)
        mean = np.mean(series)
        diff = np.abs(target_mean - mean)
        delta = diff / target_mean
    return series


def create_network_dict(net_name):
    return {'networkName': str(net_name), 'pathToNetwork': str(net_name)}

def generate_arrivals_all_random(device: str, mem_limit: str, duration_df: pd.DataFrame, n: int, batch_size: int = 1):
    local_df = duration_df[(duration_df['device'] == device) & (duration_df['mem_limit'] == mem_limit)]
    average_duration = local_df['duration'].mean()
    # arrival_times = np.random.poisson(average_duration, n)
    sigma = 500
    arrival_times_normal = find_close_series(average_duration*batch_size, average_duration*batch_size, sigma, n)
    # arrival_times_normal = np.random.normal(average_duration, sigma , n)

    network_names = local_df['network'].unique().tolist()
    path_to_data = 'test_1.jpg'
    arrivals = []
    for arrival_time in list(arrival_times_normal):
        # net_names = list(np.random.choice(networks, 3))
        networks = []
        for item in range(batch_size):
            net_name = str(random.choice(network_names))
            networks.append(create_network_dict(net_name))
# for net_name in net_names:
#             networks.append(create_network_dict(net_name).copy())

        # Really weird why python keeps hanging
        # networks = [create_network_dict(x) for x in net_names]
        # networks = [x for x in net_names]
        arrivals.append({
            'networks': networks,
            'pathToData': path_to_data,
            'time': float(arrival_time)
        })

    return arrivals


def augment_arrivals(arrivals, iat_mul):
    for arrival in arrivals:
        arrival['time'] = int(arrival['time'] * iat_mul)
    return arrivals

def get_network_durations() -> pd.DataFrame:

    data = [
        ['AgeNet', 'rpi-4', '2G', 1500]
        , ['FaceNet', 'rpi-4', '1G', 2100]
        , ['SceneNet', 'rpi-4', '512M', 3350]
        , ['AgeNet', 'rpi-4', '1G', 2150]
        , ['AgeNet', 'rpi-1', '1G', 2150]
        , ['AgeNet', 'rpi-2', '1G', 2150]
        , ['AgeNet', 'rpi-2', '512M', 2150]
        , ['AgeNet', 'rpi-2', '2G', 2150]
        , ['AgeNet', 'rpi-1', '512M', 2150]
    ]

    columns = ['network', 'device', 'mem_limit', 'duration']
    return pd.read_csv('./network_stats.tmp.csv')
    # return pd.DataFrame(data, columns=columns)


def calc_selection(df: pd.DataFrame, device: str, mem_limit: str):
    selection = df[(df['device'] == device) & (df['mem_limit'] == mem_limit)]

    total_execution = selection['duration'].sum()
    selection['ratio'] = selection['duration'] / total_execution
    return selection


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
    , 'SoS_GoogleNet': Network(networkName='SoS_GoogleNet', pathToNetwork='SoS_GoogleNet')
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

def base_exp_config(config, network_values):
    experiments = config['experiments']

    for experiment_name in experiments:
        path_to_exp = '{}/{}'.format(config['path_to_base_exp'], experiment_name)
        path_to_exp_file = '{}/{}'.format(path_to_exp, 'description.yaml')
        with open(path_to_exp_file, 'r') as stream:
            exp_config = yaml.safe_load(stream)
            exp_config = {**exp_config, **config}
            # exp_config.pop()
            exp_config['path-to-exp'] = path_to_exp
            generate_variations(exp_config, network_values)
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
def generate_variations(config, network_values):
    memory_constraints = config['memory-constraints']


    networks = config['networks']
    ait_multipliers = config['ait-multiplier']
    exp_base_tag = config['experiment-tag']
    modes = config['modes']
    n_workers_list = config['n_workers']
    device = config['device']
    base_config = BaseConfig(config['network-path'], config['resources-path'], config['output-path-base']
                             , config['arrival-mode'], config['repetitions']
                             , config['cmd-base'], config['build-folder'])

    for memory_constraint in memory_constraints:
        batch_size = 1
        base_arrivals = generate_arrivals_all_random(device, memory_constraint, network_values, base_config.n_arrivals, batch_size)
        variations = list(itertools.product(*[modes, n_workers_list, ait_multipliers]))
        for mode, n_workers, ait in variations:
            arrivals_copy = copy.deepcopy(base_arrivals)
            arrivals = augment_arrivals(arrivals_copy, ait)

            exp_tag = '{}-{}-{}-w{}-a{}'.format(exp_base_tag, memory_constraint, mode, n_workers,ait)
            config_file_path = '{}/{}/{}'.format(config['path-to-exp'], 'configs', memory_constraint)
            config_file = '{}/{}.exp.yaml'.format(config_file_path, exp_tag)
            path_exists(config_file_path)

            exp_config = ExpConfig(**base_config._asdict(), tag=exp_tag, mem_limit=memory_constraint, mode=mode, n_workers=n_workers, arrivals=arrivals)
            # exp_config.output_path = '{}/{}'.format(exp_config.output_path, exp_base_tag)

            with open(config_file, 'w') as yaml_file:
                # print(yaml.safe_dump({**exp_config._asdict()}))
                tmp_dict = {**exp_config._asdict()}
                tmp_dict['output_path'] = '{}/{}'.format(exp_config.output_path, exp_base_tag)
                tmp_dict['memory-key'] = config['memory-key']
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
    path_to_exp_base = './experiments/infocom/stochastic'

    path_to_base_config = '{}/base.yaml'.format(path_to_exp_base)

    setting = ['rpi_4', '2G']
    network_values = get_network_durations()

    # selection = calc_selection(network_values, setting[0], setting[1])
    # arrivals = generate_arrivals_all_random('rpi_4', '2G', network_values, 50)

    with open(path_to_base_config, 'r') as stream:
        try:
            # print(yaml.safe_load(stream).keys())
            config = yaml.safe_load(stream)
            config['path_to_base_exp'] = path_to_exp_base
            base_exp_config(config, network_values)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    print(config)



if __name__ == "__main__":
    main()
