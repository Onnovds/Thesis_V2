import matplotlib.pyplot as plt
import numpy as np
import uproot
import os

# --- Part 1: Load and Process Geant4 Simulation Data ---

def load_simulation_data():
    """
    Loads and combines deposited energy data from all Geant4 ROOT files.
    """
    # These are the initial energies from your C++ script
    energies = [31.7, 32.5, 35, 40, 50, 100, 200]
    
    # Helper to format filenames just like in your C++ code
    def format_energy_str(e):
        return f"{e:.2f}".replace('.', '_').rstrip('0').rstrip('_')

    all_sim_edep = []
    print("Loading simulation files...")

    for energy in energies:
        energy_str = format_energy_str(energy)
        # Construct the filename assuming files are in a "Protons/" subdirectory
        filename = f"Protons/100kProton{energy_str}MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional.root"
        
        if not os.path.exists(filename):
            print(f"Warning: File not found, skipping: {filename}")
            continue

        try:
            with uproot.open(filename) as file:
                if "DetectorData" in file:
                    tree = file["DetectorData"]
                    # Get DepositedEnergy, filtering for Edep > 0, and convert from MeV to keV
                    edep_array = tree["DepositedEnergy"].array(library="np")
                    positive_edep = edep_array[edep_array > 0] * 1000.0  # Convert MeV to keV
                    all_sim_edep.append(positive_edep)
                    print(f"Successfully loaded {len(positive_edep)} events from {filename}")
                else:
                    print(f"Warning: 'DetectorData' TTree not found in {filename}")
        except Exception as e:
            print(f"Error opening or reading {filename}: {e}")

    # Combine all loaded data into a single numpy array
    if not all_sim_edep:
        print("Error: No simulation data was loaded. Returning empty array.")
        return np.array([])
        
    return np.concatenate(all_sim_edep)

# --- Part 2: Load Measured TPX Data ---
# I am mocking the data from your JoeySat_GetData module.
# You should replace this with your actual data loading mechanism.
def load_tpx_data():
    """
    Loads and combines deposited energy data from the TPX detector.
    This function should be replaced with your actual data source.
    """
    print("\nLoading TPX detector data...")
    # This is a placeholder for your actual data loading.
    # For this example, I'll create some dummy data.
    # In your real script, you would use your `JoeySat_GetData` import.
    # from JoeySat_GetData import arrays_23, arrays_24
    
    # --- Dummy Data Generation (REPLACE THIS) ---
    np.random.seed(42)
    dummy_arrays_23 = {'class1': np.random.rand(1000, 6)}
    dummy_arrays_23['class1'][:, 5] = np.random.lognormal(mean=5, sigma=1.5, size=1000)
    dummy_arrays_24 = {'class1': np.random.rand(1200, 6)}
    dummy_arrays_24['class1'][:, 5] = np.random.lognormal(mean=5, sigma=1.6, size=1200)
    arrays_23 = dummy_arrays_23
    arrays_24 = dummy_arrays_24
    # --- End of Dummy Data ---

    # This logic is taken directly from your script
    edep_class1_23 = arrays_23['class1'][:, 5]
    edep_class1_24 = arrays_24['class1'][:, 5]
    combined_edep_class1 = np.concatenate((edep_class1_23, edep_class1_24))
    print(f"Loaded {len(combined_edep_class1)} events from TPX data.")
    return combined_edep_class1

# --- Part 3: Create the Comparison Plot ---

def plot_discrepancy(sim_data, tpx_data):
    """
    Plots the overlaid histograms of the TPX and Geant4 data.
    """
    if tpx_data.size == 0 or sim_data.size == 0:
        print("Cannot create plot. One or both datasets are empty.")
        return

    plt.style.use('seaborn-v0_8-notebook')
    plt.figure(figsize=(12, 8))

    # Use the same logarithmic bins for a fair comparison
    bins = np.logspace(np.log10(1e0), np.log10(3e4), 100)

    # Plot the Geant4 simulation data histogram
    plt.hist(sim_data, bins=bins, color='darkorange', alpha=0.7, 
             label=f'Geant4 Simulation (n={len(sim_data)})', density=True)
    
    # Plot the TPX data histogram
    plt.hist(tpx_data, bins=bins, color='royalblue', alpha=0.7, 
             label=f'JoeySat TPX Data (n={len(tpx_data)})', density=True)

    # Add titles and labels for clarity
    plt.title('Comparison of Deposited Energy: Measured vs. Simulated', fontsize=16)
    plt.xlabel('Deposited Energy (Edep) [keV]', fontsize=14)
    plt.ylabel('Event Counts', fontsize=14)

    # Use logarithmic scales
    #plt.yscale('log')
    plt.xscale('log')

    # Add a grid and legend
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.legend(fontsize=13)
    
    # Set axis limits for better viewing
    plt.xlim(1, 4e4)
    plt.ylim(bottom=0.0) # set bottom to 0.8 to avoid empty space

    plt.tight_layout()
    
    # Save the figure
    output_filename = "Edep_Discrepancy_Plot.png"
    plt.savefig(output_filename, dpi=300)
    print(f"\nPlot successfully saved as {output_filename}")
    # plt.show()


if __name__ == '__main__':
    # Execute the workflow
    sim_edep = load_simulation_data()
    tpx_edep = load_tpx_data()
    plot_discrepancy(sim_edep, tpx_edep)