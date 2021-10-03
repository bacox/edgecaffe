import collections
import itertools
from pathlib import Path
import random

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
# import matplotlib.ticker as Ticker
# from pathlib import Path
# import yaml
# import glob
# import re
from typing import List
import copy

def create_filtered_df():
    df = pd.read_csv('edub_seg_extra_data.csv')
    # print(df)
    new_data = []
    for idx, item in df.iterrows():
        # print(f'Frame: {item["frames"].strip().split(" ")}')
        frame_start, frame_end = item['frames'].strip().split(' ')
        people = item['people']
        if people == '5+':
            people = '5'
        people = int(people)
        extra_tag = item['extra_data_tag']
        for frame in range(int(frame_start), int(frame_end) + 1):
            new_data.append([frame, people, extra_tag])

    new_df = pd.DataFrame(new_data, columns=['frame', 'people', 'extra_tag'])

    # sns.displot(data=new_df, x="people")
    # plt.show()
    print(f'Total data size is {len(new_df)}')
    return new_df

def parse_file(file_name):
    items = file_name.split('_')
    if len(items) == 1:
        return items[0]
    if len(items) == 2:
        return items[1]
    if len(items) == 3:
        return items[1]

def prep_file_data(base_path: str):
    path = Path(base_path)
    all_files = path.glob('*/*.jpg')
    prepped_file_data = []
    for file in all_files:
        file_frame = parse_file(file.stem)
        prepped_file_data.append((file, file_frame))
    return prepped_file_data


def find_all_frames(df: pd.DataFrame, file_data):
    counter = 0
    for idx, row in df.iterrows():
        frame = row['frame']
        for file, file_frame in file_data:
            if str(frame) in file_frame:
                print(f'FOUND frame {frame} in {file}!!')
        if counter > 10:
            break
        counter += 1

    # for idx, row in df.iterrows():
    #     print(row['frame'])


if __name__ == '__main__':

    df = create_filtered_df()
    base_path = 'tmp_data/EDUB-Seg/images'
    file_data = prep_file_data(base_path)
    find_all_frames(df, file_data)

    # import scipy.io
    # mat = scipy.io.loadmat('tmp_data/EDUB-Seg/images/Subject1_Set1/infoCNN_outputClasses.mat')
    # print('done')

