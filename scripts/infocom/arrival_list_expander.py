import argparse
import yaml
import copy

def main(arrival_list: str, out: str, n: int):
    print(f'Using the list {arrival_list}')
    config = None
    with open(arrival_list, 'r') as stream:
        try:
            config = yaml.safe_load(stream)

        except yaml.YAMLError as exc:
            print(exc)
            exit(1)
    network = config['arrivals'][0]['networks'][0]

    arrival = config['arrivals'][0]
    networks_per_arrival = 5
    num_arrivals = int(n / networks_per_arrival)
    list_of_networks = []
    for idx in range(networks_per_arrival):
        list_of_networks.append(copy.deepcopy(network))

    arrival['networks'] = list_of_networks
    list_of_arrivals = []
    for idx in range(num_arrivals):
        list_of_arrivals.append(copy.deepcopy(arrival))
    config['arrivals'] = list_of_arrivals
    with open(out, 'w') as file:
        documents = yaml.dump(config, file)
    print('done')





if __name__ == "__main__":
    parser = argparse.ArgumentParser("InfoCom Arrival list expander. Multiply the networks in an arrival list")
    parser.add_argument("--list", type=str, help="The arrival list to be modified")
    parser.add_argument("--out", type=str, help="Output file")
    parser.add_argument("--n", type=int, help="Number of networks")
    args = parser.parse_args()

    main(arrival_list = args.list, out=args.out, n=args.n)