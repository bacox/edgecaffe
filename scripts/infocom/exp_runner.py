import yaml
import glob
import argparse
from scripts.infocom.run_single_exp import run_single_exp


def main(path_base : str = './experiments/infocom/batch', mem_limit : str = '*', reverse: bool = False, dry_run : bool = False):

    config_directory = '{}/*/configs/{}/*.exp.yaml'.format(path_base, mem_limit)
    exp_config_files = [f for f in glob.glob(config_directory)]
    if reverse:
        exp_config_files.reverse()
    base_script = 'sudo bash ./scripts/poisson_exp/poisson_rv_nf_base.sh'

    for exp_file in exp_config_files:
        run_single_exp(exp_file, base_script, dry_run=dry_run)

if __name__ == "__main__":
    parser = argparse.ArgumentParser("InfoCom Experiment runner. Use this tool to run multiple experiments")
    parser.add_argument("--dry-run", type=bool, help="Do a dry run; No calling the generated command")
    parser.add_argument("--reverse", type=bool, help="Run all the scripts in reverse order")
    parser.add_argument("--mem-limit", type=str, help="Filter the scripts by the memory limit: [2G, 1G 512M]")
    parser.add_argument("--path", type=str, help="Set the path pointing to the experiments")
    args = parser.parse_args()
    args_dict = {
        'dry_run': False,
        'reverse': False
    }
    if args.dry_run:
        args_dict['dry_run'] = True
    if args.reverse:
        args_dict['reverse'] = True
    if args.mem_limit:
        args_dict['mem_limit'] = args.mem_limit
    if args.path:
        args_dict['path_base'] = args.path

    main(**args_dict)
