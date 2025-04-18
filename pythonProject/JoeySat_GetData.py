import numpy as np
import os
from datetime import datetime
from zoneinfo import ZoneInfo

# Data paths
folder_path_23march2024 = "/home/onno/satellite_test/pythonProject/One Web JoeySat data/sampling_lists_23march2024/"
folder_path_24march2024 = "/home/onno/satellite_test/pythonProject/One Web JoeySat data/sampling_lists_24march2024/"

# Files
file_names = {
    "all": "sampling_list_all.npy",
    "class1": "sampling_list_1.npy",
    "class2": "sampling_list_2.npy",
    "class3": "sampling_list_3.npy",
}

# X-Axis limits
xlim_23 = (1711148453.419, 1711201201.419)  # 23rd March (00:06 CET - 14:40 CET)
xlim_24 = (1711277868.419, 1711317686.421)  # 24th March (11:00 CET - 22:50 CET)

# Titles mapping
title_map = {
    "all": "All particles",
    "class1": "Protons + Ions",
    "class2": "HE Electrons and LE Gamma Rays",
    "class3": "X rays + LE electrons",
}

# Define colours
color_map = {
    "all": "black",
    "class1": "blue",
    "class2": "red",
    "class3": "green",
}

# Load datasets
arrays_23 = {key: np.load(os.path.join(folder_path_23march2024, file_names[key])) for key in file_names}
arrays_24 = {key: np.load(os.path.join(folder_path_24march2024, file_names[key])) for key in file_names}

# Extract Unix timestamps
unixtimes_23 = {key: arrays_23[key][:, 4].astype(float) for key in file_names}
unixtimes_24 = {key: arrays_24[key][:, 4].astype(float) for key in file_names}

# Convert timestamps to CET time
def format_time(ts):
    return datetime.fromtimestamp(ts, ZoneInfo("Europe/Amsterdam")).strftime('%d-%H:%M')

dates_23 = {key: np.array([format_time(ts) for ts in unixtimes_23[key]]) for key in file_names}
dates_24 = {key: np.array([format_time(ts) for ts in unixtimes_24[key]]) for key in file_names}

