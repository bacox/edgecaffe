from caffe import layers as L, params as P
import caffe
from random import seed
from random import randint
import yaml
import os, sys
from pathlib import Path


def random_variable(lower=1, upper=10):
    return 2
    # return randint(lower, upper)

def gen_conv_layer(net_spec, layer_name, prev_layer):
    # kernel_size = random_variable()
    kernel_size = 1
    num_of_ouput = 32

    net_spec[layer_name] = L.Convolution(prev_layer, kernel_size=kernel_size, num_output=num_of_ouput, weight_filler=dict(type='gaussian'))

def gen_pool_layer(net_spec, layer_name, prev_layer):
    # kernel_size = random_variable(2,10)
    # For now using pooling with kernel size of 1 to keep a constant parameter shape
    kernel_size = 1
    net_spec[layer_name] = L.Pooling(prev_layer, kernel_size=kernel_size, stride=kernel_size, pool=P.Pooling.MAX)

def gen_fc_layer(net_spec, layer_name, prev_layer):
    
    num_of_ouput = 16
    net_spec[layer_name] = L.InnerProduct(prev_layer, num_output=num_of_ouput,
                           weight_filler=dict(type='gaussian'))

def gen_loss_Layer(net_spec, layer_name, prev_layer):
    num_of_ouput = 16
    net_spec.score = L.InnerProduct(prev_layer, num_output=num_of_ouput, weight_filler=dict(type='gaussian'))
    net_spec.prob = L.Softmax(net_spec.score)

def gen_input_Layer(net_spec, layer_name, prev_layer):
    input_dim = (277,277)
    net_spec.data = L.Input(input_param={'shape': {'dim': [1, 3, *input_dim]}})


def generated_network(numConv=5, numFc=5):
    n = caffe.NetSpec()
    gen_input_Layer(n, "","")
    last_layer_name = 'data'

    c_conv = 1
    c_fc = 0

    for idx in range(0,numConv):
        layer_name = 'conv-{}'.format(idx)
        gen_conv_layer(n, layer_name, n[last_layer_name])
        c_conv += 1
        last_layer_name = layer_name
        layer_name = 'pool-{}'.format(idx)
        gen_pool_layer(n, layer_name, n[last_layer_name])
        c_conv += 1
        last_layer_name = layer_name

    for idx in range(0, numFc):
        layer_name = 'fc-{}'.format(idx)
        gen_fc_layer(n, layer_name, n[last_layer_name])
        c_fc += 1
        last_layer_name = layer_name

    gen_loss_Layer(n, '', n[last_layer_name])
    c_fc += 2
    return (n.to_proto(), c_conv, c_fc)

def generate_description_file(name, network_file, model_file, base_path, partials_location, numConv, numFc, network_stem_name, outputLocation = '.'):

    # Generate filename of the partials
    total_layers = numConv + numFc
    conv_partials = []
    fc_partials = []
    for idx in range(total_layers):
        padding = ''
        if idx < 10:
            padding = '0'
        partial_name = '{}{}{}.partial.caffemodel'.format(network_stem_name, padding, idx)
        if idx < numConv:
            conv_partials.append(partial_name)
        else:
            fc_partials.append(partial_name)
    data = {
        'name': name
        , 'network-file': network_file
        , 'model-file': model_file
        , 'mean-scalar': [78.4263377603, 87.7689143744, 114.895847746]
        , 'uses-result-vector': True
        , 'result-vector': [True, False]
        , 'input-scale': [227, 227]
        , 'base-path': base_path
        , 'partials-location': partials_location
        , 'has-input-layer': True
        , 'num-conv-layers': numConv
        , 'num-fc-layers': numFc
        , 'conv-layers': conv_partials
        , 'fc-layers': fc_partials}
    filename = 'description.yaml'
    file_path = '{}/{}'.format(outputLocation,filename)
    with open(file_path, 'w') as file:
        yaml.dump(data, file, default_flow_style=False)
    pass

def load_model(filename, network_stem_name, output_location = '.'):
    net = caffe.Net(filename,1)
    param_file_name = '{}/{}.caffemodel'.format(output_location, network_stem_name)
    net.save(param_file_name)


# def custom_net(lmdb, batch_size, input_dim = (227,227), kernel_size=2, stride=2):
#     # define your own net!
#     n = caffe.NetSpec()
#
#     if lmdb is None: # "deploy" flavor
#         # assuming your data is of shape 3x224x224
#         n.data = L.Input(input_param={'shape':{'dim':[1,3,*input_dim]}})
#     else:
#         # keep this data layer for all networks
#         n.data, n.label = L.Data(batch_size=batch_size, backend=P.Data.LMDB, source=lmdb,
#                                  ntop=2, transform_param=dict(scale=1. / 255))
#     # the other layers common to all flavors: train/val/deploy...
#     n.conv1 = L.Convolution(n.data, kernel_size=6,
#                             num_output=48, weight_filler=dict(type='xavier'))
#     n.pool1 = L.Pooling(n.conv1, kernel_size=2, stride=2, pool=P.Pooling.MAX)
#
#     n.conv2 = L.Convolution(n.pool1, kernel_size=5,
#                             num_output=48, weight_filler=dict(type='xavier'))
#     n.pool2 = L.Pooling(n.conv2, kernel_size=2, stride=2, pool=P.Pooling.MAX)
#
#     n.conv3 = L.Convolution(n.pool2, kernel_size=4,
#                             num_output=480, weight_filler=dict(type='xavier'))
#     n.pool3 = L.Pooling(n.conv3, kernel_size=2, stride=2, pool=P.Pooling.MAX)
#
#     n.conv4 = L.Convolution(n.pool3, kernel_size=2,
#                             num_output=48, weight_filler=dict(type='xavier'))
#     n.pool4 = L.Pooling(n.conv4, kernel_size=2, stride=2, pool=P.Pooling.MAX)
#
#     n.fc1 = L.InnerProduct(n.pool4, num_output=50,
#                            weight_filler=dict(type='xavier'))
#     # do you "drop" i deploy as well? up to you to decide...
#     n.drop1 = L.Dropout(n.fc1, dropout_param=dict(dropout_ratio=0.5))
#     n.score = L.InnerProduct(n.drop1, num_output=2,
#                              weight_filler=dict(type='xavier'))
#
#     if lmdb is None:
#         n.prob = L.Softmax(n.score)
#     else:
#         # keep this loss layer for all networks apart from "Deploy"
#         n.loss = L.SoftmaxWithLoss(n.score, n.label)
#
#     return n.to_proto()

def main():
    network_name = 'GeneratedNet_1'
    print("Running network generator")
    outputPath = '../../networks/{}'.format(network_name)
    # outputPath = 'output/{}'.format(network_name)
    Path(outputPath).mkdir(parents=True, exist_ok=True)
    network_name = 'gen_1'
    num_conv_layers = 10
    num_fc_layers = 10

    proto, c_conv, c_fc = generated_network(num_conv_layers,num_fc_layers)

    network_stem_name = 'gen_net_1'
    network_file = '{}_deploy.prototxt'.format(network_stem_name)
    model_file = '{}.caffemodel'.format(network_stem_name)
    base_path = 'networks/'.format(network_name)
    partial_location = '{}_partials'.format(network_stem_name)

    generate_description_file(network_name, network_file, model_file, base_path, partial_location, c_conv, c_fc, network_stem_name,outputPath)
    network_file_location = '{}/{}'.format(outputPath, network_file)
    with open(network_file_location, 'w') as f:
        f.write(str(proto))

    load_model(network_file_location, network_stem_name, outputPath)

    # Call modelsplitter
    import subprocess
    model_splitter_cmd = '../../cmake-build-debug/tools/ModelSplitter'
    arg = '{}/{}'.format(outputPath,model_file)
    subprocess.run([model_splitter_cmd, arg])

if __name__ == '__main__':
    main()