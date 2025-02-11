import re
import numpy as np
import matplotlib.pyplot as plt

def convert_energy_to_keV(energy):
    energy = np.array(energy)
    if 'eV' in energy:
        value = float(energy[0].replace(',', '.'))
        return value / 1e3
    elif 'keV' in energy:
        return float(energy[0].replace(',', '.'))
    elif 'MeV' in energy:
        return float(energy[0].replace(',', '.'))*1000
    elif 'GeV' in energy:
        return float(energy[0].replace(',', '.'))*1e6
    return float(energy)


def convert_distance_to_um(distance):
    distance = np.array(distance)
    if 'um' in distance:
        return float(distance[0].replace(',', '.'))
    elif 'mm' in distance:
        return float(distance[0].replace(',', '.')) * 1000
    elif 'cm' in distance:
        return float(distance[0].replace(',', '.')) * 10000
    elif 'm' in distance:
        return float(distance[0].replace(',', '.')) * 1000000
    elif 'km' in distance:
        return float(distance[0].replace(',', '.')) * 1e9
    elif 'A' in distance:
        return float(distance[0].replace(',', '.')) * 0.1
    return float(distance)


def parse_data_line(line):
    parts = line.split()
    pairs = [(parts[i], parts[i + 1]) for i in range(0, len(parts), 2)]
    if len(pairs) == 5:
        energy = convert_energy_to_keV(pairs[0])
        dE_dx_e = float(np.array(pairs[1])[0].replace(',', '.'))
        dE_dx_n = float(np.array(pairs[1])[1].replace(',', '.'))
        projected_range = convert_distance_to_um(pairs[2])
        longitudinal_straggling = convert_distance_to_um(pairs[3])
        lateral_straggling = convert_distance_to_um(pairs[4])
        return (energy, dE_dx_e, dE_dx_n, projected_range, longitudinal_straggling, lateral_straggling)

    return None


def extract_data(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    data_start = None
    data_end = None
    data_lines = []

    for i, line in enumerate(lines):
        if '--------------  ---------- ---------- ----------  ----------  ----------' in line:
            data_start = i + 1
        if '-----------------------------------------------------------' in line:
            data_end = i
            break

    if data_start is not None and data_end is not None:
        data_lines = lines[data_start:data_end]

    data = []
    for line in data_lines:
        parsed_line = parse_data_line(line)
        if parsed_line:
            data.append(parsed_line)

    return data


# Usage example
filename = '/home/onno/satellite_test/pythonProject/Hydrogen in Silicon_1GeV.txt'
SRIM_data_Silicon = np.array(extract_data(filename))

filename2 = '/home/onno/satellite_test/pythonProject/Hydrogen in Air, Dry (ICRU-104) (gas)_1GeV.txt'
SRIM_data_Air = np.array(extract_data(filename2))

filename3 = '/home/onno/satellite_test/pythonProject/Hydrogen in Aluminum_1GeV.txt'
SRIM_data_Aluminium = np.array(extract_data(filename3))



