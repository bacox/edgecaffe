#!/usr/bin/env bash

sudo swapoff -a && sudo mkswap /swapfile && sudo swapon /swapfile && sudo swapon -a