#!/usr/bin/env python
"""
Draw a graph of the net architecture.
"""

import edgecaffe
from caffe.proto import caffe_pb2
import numpy as np
import cv2


def main():


    path = '../resources/test_3.jpg'
    # Using cv2.imread() method
    img = cv2.imread(path)

    ageNet = edgecaffe.BaseNet("../cmake-build-debug/networks/AgeNet")
    genderNet = edgecaffe.BaseNet("../cmake-build-debug//networks/GenderNet")
    networks = [ageNet, genderNet]

    print("Number of layers in AgeNet is {}".format(ageNet.numLayers()))

    [net.setInputFromString(path, False) for net in networks]
    [net.loadNetworkStructure() for net in networks]
    [net.loadInputToNetwork() for net in networks]
    [net.runNetwork(True) for net in networks]
    [net.showResult() for net in networks]

    # Displaying the image
    cv2.imshow('image', img)
    cv2.waitKey(0)



if __name__ == '__main__':
    main()
