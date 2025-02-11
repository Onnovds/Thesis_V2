import pandas as pd


def read_data(file_path):
    """
    Reads the data from the text file and returns a DataFrame.
    Assumes the first row contains column names and the second row contains units.
    """
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract the column names and units
    column_names = lines[0].strip().split()
    units = lines[1].strip().split()

    # Read the actual data
    data = [line.strip().split() for line in lines[2:]]

    # Create a DataFrame
    df = pd.DataFrame(data, columns=column_names)

    return df, column_names, units


def filter_columns(df, columns):
    """
    Filters the DataFrame to keep only the specified columns.
    """
    return df[columns]


def write_data(df, file_path):
    """
    Writes the filtered DataFrame to a new file.
    """
    df.to_csv(file_path, index=False, sep='\t')


def main(input_file, output_file, columns_to_keep):
    # Read the data
    df, column_names, units = read_data(input_file)

    # Filter the DataFrame
    filtered_df = filter_columns(df, columns_to_keep)

    # Write the filtered data to a new file
    write_data(filtered_df, output_file)


if __name__ == "__main__":
    # Define the input and output files
    
    input_file = '/home/onno/project_test/pythonProject/Raw_data_processed/Onno_sep2024/HE p Krakow june2022/A1 TPX3 D05 sat rot scans/A1/39 8 MeV 30deg/D05 10ms 80V/Output_DPE_CAL_1s./File/EventListExt.advelist'
    output_file = '/home/onno/satellite_test/pythonProject/HE p Krakow june2022 processed outputs/protons/39 8 MeV 30deg output.txt'

    # Define the columns to keep (use the exact column names from the input file)
    columns_to_keep = ['DetectorID', 'EventID', 'X', 'Y', 'E','EpixMean',  'LET']

    # Run the main function
    main(input_file, output_file, columns_to_keep)

