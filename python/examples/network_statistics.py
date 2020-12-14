import glob
import os
os.environ['GLOG_minloglevel'] = '2' # suprress Caffe verbose prints
import caffe
caffe.set_mode_cpu()
import numpy as np
from numpy import prod, sum
from pprint import pprint
import glob
import argparse

def get_layer_stats(deploy_file):
    network_name = deploy_file.split('/')[-2]
    if network_name in ['GeneratedNet_1']:
        return None
    net = caffe.Net(deploy_file, caffe.TEST)
    layer_stats= [(k, v[0].data.shape) for k, v in net.params.items()]
    return (network_name, layer_stats)

def save_to_file(path: str, network_name: str, layer_stats, write_to_network: bool = False):
    if write_to_network:
        filename = '{}/{}-layers.stat.csv'.format(path, 'parameters'.replace(' ', '-'))
    else:
        filename = '{}/{}-layers.stat.csv'.format(path, network_name.replace(' ', '-'))
    with open(filename, 'w') as csv_file:
        csv_file.write(','.join(['layer','parameters'])+'\n')
        for layer_stat in layer_stats:
            line_items = [layer_stat[0], str(prod(layer_stat[1]))]
            line = ','.join(line_items)+'\n'
            csv_file.write(line)

def main(path_to_networks: str = '../networks', out: str = '.', write_to_network: bool = False):
    all_prototxt_files = glob.glob( path_to_networks + '/*/*.prototxt*')
    for prototxt_file in all_prototxt_files:
        network_path_base = '/'.join(prototxt_file.split('/')[:-1])
        if write_to_network:
            out = network_path_base
        result = get_layer_stats(prototxt_file)
        if result:
            save_to_file(path=out, network_name=result[0], layer_stats=result[1], write_to_network=write_to_network)
            print('Results for {} saved to at {}'.format(prototxt_file, out))
        else:
            print('Unable to get the layers stats from {}'.format(prototxt_file))




if __name__ == "__main__":
    parser = argparse.ArgumentParser("Get all the parameters for each layer for each network")
    parser.add_argument("--out", type=str, help="Give the location where the results needs to be written to disk")
    parser.add_argument("--write-to-network", type=bool, help="Write the csv file to the directory of the network. "
                                                              "With this option enabled will ignore the 'out' "
                                                              "argument")
    parser.add_argument("--path-to-networks", type=str, help="Point to the location where the networks folder is "
                                                             "located")
    args = parser.parse_args()
    args_dict = {
        'write_to_network': False
    }
    if args.path_to_networks:
        args_dict['path_to_networks'] = args.path_to_networks
    if args.write_to_network:
        args_dict['write_to_network'] = True
    elif args.out:
        args_dict['out'] = args.out
    main(**args_dict)