import copy
from dataclasses import dataclass, field
from pathlib import Path
from typing import List
import numpy as np

import yaml

def replace_underscore(data: dict) -> dict:
    return dict((k.replace('-','_'),v) for k,v in data.items())

def revert_underscore(data: dict) -> dict:
    new_data =  dict((k.replace('_','-'),v) for k,v in data.items())
    new_data['cmd_base'] = new_data.pop('cmd-base')
    new_data['build_folder'] = new_data.pop('build-folder')
    new_data['mem_limit'] = new_data.pop('mem-limit')

    return new_data


@dataclass
class Experiment:
    experiment_tag: str
    networks: List[str]
    success_label_prob: List[float]
    force_first_layer_nr_first: bool
    n_workers: List[int]
    ait_multiplier: List[int]
    modes: List[str]
    memory_constraints: List[str]
    network_set: str
    n_arrivals: int
    network_stub: dict
    arrivals = []

    @classmethod
    def FromDict(cls, values_in_dict: dict):
        print(replace_underscore(values_in_dict))
        # return {}
        return cls(**replace_underscore(values_in_dict))
        # return cls(**values_in_dict)

@dataclass
class ExpVariation:
    tag: str = ""
    cmd_base:str= 'bin/Exp_poisson_arrival_rv_nf'
    build_folder: str = '.'
    mem_limit: str = '2G'
    mode: str = 'masa'
    memory_key: str = 'valgrind'
    n_arrivals: int = 1
    n_workers: int = 2
    force_input_label: bool= True
    network_path: str = 'networks'
    resources_path: str = 'resources'
    output_path: str = '../analysis/experiments/sipmc/fga-single-2G-1'
    arrival_mode: str = 'batch'
    arrival_list: str = './experiments/sipmc/conditional/fga-single/configs/2G/fga-single-2G-1.exp.yaml'
    output_prefix: str = 'fga-single-2G-1'
    force_first_layer_nr_first: bool = False
    arrivals= []

def parse_nr_stub(path: Path, filename: str):
    with open(path/filename, 'r') as stream:
        try:
            stub_config = yaml.safe_load(stream)
            return stub_config
        except yaml.YAMLError as exc:
            print(f'Got Yaml error: {exc}')

def generate_arrivals(stub: dict, n: int, p: float, config):
    arrivals = []
    gen_prob_deterministic(config)
    prob_list = gen_prob_list(p, n)
    print(prob_list)
    for prob in prob_list:
        stub_copy = copy.deepcopy(stub)
        stub_copy = alter_network_prob(stub_copy, prob, 'FaceNet', 'num-faces')
        extended_stub = {'networks': stub_copy, 'pathToData': 'test_1.jpg', 'time': 0}
        arrivals.append(copy.deepcopy(extended_stub))
    return arrivals

def gen_prob_list(p: float, n: int) -> List[int]:
    return np.random.choice([0,1], n, p=[1-p, p])

def gen_prob_deterministic(config):
    # print(config)
    label_list = len(config.success_label_prob) - 1


def create_variation(exp: Experiment, memory: str, label_prob: float, mode: str, n_workers: int, path: str):
    tag = f'{exp.experiment_tag}-{memory}-{label_prob}'
    variation = ExpVariation(tag=tag, output_prefix=tag, mem_limit=memory, mode=mode, n_workers=n_workers, n_arrivals=exp.n_arrivals, force_first_layer_nr_first= exp.force_first_layer_nr_first)
    output_path = f'../analysis/experiments/sipmc/{tag}'
    filename = f'{tag}.exp.yaml'
    arrival_list= f'{path}/configs/{memory}/{filename}'
    variation.output_path = output_path
    variation.arrival_list = arrival_list
    variation.arrivals= generate_arrivals(exp.network_stub, exp.n_arrivals, label_prob, exp)
    return variation

def create_vatiations(exp: Experiment, path: str):
    variations = []
    for mem in exp.memory_constraints:
        for label_prob in exp.success_label_prob:
            for mode in exp.modes:
                for n_worker in exp.n_workers:
                    variation = create_variation(exp, mem, label_prob, mode, n_worker, path)
                    variations.append(variation)
    return variations

def alter_network_prob(data: dict, value: int, network: str, data_label: str):
    value = int(value)
    for item in data:
        if item['networkName'] != network:
            continue
        item['data-label'][copy.deepcopy(data_label)] = value
    return copy.deepcopy(data)

def parse_experiment(path: Path):
    print(f'Experiment: {path}')
    with open(path/'description.yaml', 'r') as stream:
        try:
            config = yaml.safe_load(stream)
            exp = Experiment.FromDict(config)
            return exp
            # # print(exp)
            # print(f'Generating {exp.n_arrivals} arrivals from stub')
            # stub_obj = copy.deepcopy(exp.network_stub)
            # # nr_stub = parse_nr_stub(path, exp.network_relation_stub)
            # # print(nr_stub)
            # exp.arrivals = generate_arrivals(stub_obj, exp.n_arrivals, 1)
            # # alter_network_prob(exp.network_stub, 0, 'FaceNet', 'num-faces')
            # print(exp)
            # with open('experiment.tmp.yaml', 'w') as file:
            #     documents = yaml.dump(exp.__dict__, file, sort_keys=False)

        except yaml.YAMLError as exc:
            print(f'Got Yaml error: {exc}')

def path_exists(path):
    Path(path).mkdir(parents=True, exist_ok=True)

def read_description(path):
    pass

def generate_networks():
    pass


def main():
    print('Generating experiments')
    tag = 'conditional-exp1'

    base_path = 'experiments/sipmc/conditional'
    path = Path(base_path)
    print(list(path.glob('*')))
    for item in path.iterdir():
        # parse_experiment(item)
        if item.name not in ['yfgae-heavy-opportunistic', 'yfgae-heavy-opportunistic-force-first']:
            continue
        # print(f' Item: {item}')
        tmp_path = str(item)
        print(f'Path: {tmp_path}')
        exp = parse_experiment(item)
        variations = create_vatiations(exp, tmp_path)
        for variation in variations:
            file_path = item /'configs'/ variation.mem_limit
            file_path.mkdir(parents=True, exist_ok=True)
            filename = file_path / f'{variation.tag}.exp.yaml'
            with open(filename, 'w') as file:
                documents = yaml.dump(revert_underscore(variation.__dict__), file, sort_keys=False)
        print('done')

    # ex = Experiment(n_workers=[1], networks=[''], success_label_prob=[1.2], tag='')
    # print(ex)

if __name__ == '__main__':

    # var = ExpVariation(tag='hello')
    # print(var.__dict__)
    # print(revert_underscore(var.__dict__))
    #
    # print(gen_prob_list(.25, 100))

    main()