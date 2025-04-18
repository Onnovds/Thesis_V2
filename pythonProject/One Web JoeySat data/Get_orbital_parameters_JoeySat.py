import numpy as np
import matplotlib.pyplot as plt

# Constants
MU_EARTH = 398600.4418  # Earth's gravitational parameter (km^3/s^2)
R_EARTH = 6378.1  # Earth's radius (km)
OMEGA_EARTH = 7.2921159e-5  # Earth's rotation rate (rad/s)

def latlonalt_to_ecef(longitude, latitude, altitude):
    """Convert latitude, longitude, and altitude to ECEF coordinates"""
    lon_rad = np.radians(longitude)
    lat_rad = np.radians(latitude)
    r = R_EARTH + altitude

    x = r * np.cos(lat_rad) * np.cos(lon_rad)
    y = r * np.cos(lat_rad) * np.sin(lon_rad)
    z = r * np.sin(lat_rad)

    return np.array([x, y, z])

def ecef_to_eci(ecef_positions, times):
    """Convert ECEF to ECI by accounting for Earth's rotation"""
    theta_gmst = OMEGA_EARTH * (times - times[0])  # Earth's rotation angle
    cos_theta = np.cos(theta_gmst)
    sin_theta = np.sin(theta_gmst)

    x_eci = cos_theta * ecef_positions[:, 0] - sin_theta * ecef_positions[:, 1]
    y_eci = sin_theta * ecef_positions[:, 0] + cos_theta * ecef_positions[:, 1]
    z_eci = ecef_positions[:, 2]

    return np.column_stack((x_eci, y_eci, z_eci))

def compute_orbital_elements(times, longitudes, latitudes, altitudes):
    """Compute inclination, RAAN, argument of perigee, and true anomaly"""

    # Convert to ECEF coordinates
    ecef_positions = np.array([latlonalt_to_ecef(lon, lat, alt) 
                                for lon, lat, alt in zip(longitudes, latitudes, altitudes)])
    
    # Convert to ECI coordinates
    eci_positions = ecef_to_eci(ecef_positions, times)
    
    # Compute velocities using finite differences
    velocities = np.diff(eci_positions, axis=0) / np.diff(times)[:, np.newaxis]

    # Take the first three positions and velocities for calculations
    r_vec = eci_positions[:-1]
    v_vec = velocities

    # Compute specific angular momentum
    h_vec = np.cross(r_vec, v_vec)
    h_mag = np.linalg.norm(h_vec, axis=1).mean()

    # Compute inclination (i)
    hz_mean = np.mean(h_vec[:, 2])
    inclination = np.degrees(np.arccos(hz_mean / h_mag))

    # Compute node vector (for RAAN)
    n_vec = np.cross([0, 0, 1], h_vec)  # Cross-product to get ascending node vector
    n_mean = np.mean(n_vec, axis=0)  # Take mean of node vector over time
    n_mag = np.linalg.norm(n_mean)  # Compute magnitude of the mean node vector

    # Ensure valid range for arccos calculation
    if n_mag != 0:
        Omega = np.degrees(np.arccos(n_mean[0] / n_mag))  # Compute RAAN
        if n_mean[1] < 0:  # Adjust quadrant if needed
            Omega = 360 - Omega
    else:
        Omega = 0  # If n_vec is zero, RAAN is undefined; assume 0 for a circular orbit


    # Compute eccentricity vector
    r_mag = np.linalg.norm(r_vec, axis=1)
    v_mag = np.linalg.norm(v_vec, axis=1)
    vr = np.einsum('ij,ij->i', v_vec, r_vec) / r_mag

    ecc_vectors = (np.cross(v_vec, h_vec) / MU_EARTH) - (r_vec.T / r_mag).T
    ecc_mags = np.linalg.norm(ecc_vectors, axis=1).mean()

    # Compute argument of perigee (ω)
    omega = np.degrees(np.arccos(np.dot(n_vec.mean(), ecc_vectors.mean()) / (n_mag * ecc_mags)))

    # Fix NaN errors in true anomaly calculation
    cos_nu = np.einsum('ij,ij->i', ecc_vectors, r_vec) / (ecc_mags * r_mag)
    cos_nu = np.clip(cos_nu, -1, 1)  # Ensure valid range for arccos
    true_anomalies = np.degrees(np.arccos(cos_nu))
    true_anomaly = np.mean(true_anomalies)

    # Handle near-circular orbits
    if ecc_mags < 1e-6:
        true_anomaly = np.degrees(np.arctan2(
            np.einsum('ij,ij->i', r_vec, v_vec) / np.sqrt(MU_EARTH),
            np.einsum('ij,ij->i', r_vec, ecc_vectors) / r_mag
        )).mean()

    return inclination, Omega, omega, true_anomaly

# Example Input Data (Replace with actual NumPy data)
data = np.load("/home/onno/satellite_test/pythonProject/One Web JoeySat data/sampling_lists_23march2024/sampling_list_all.npy")


# Extract values
times = data[:, 4]
longitudes = data[:, 12]
latitudes = data[:, 13]
altitudes = data[:, 14]


np.savetxt("orbit_data.txt", np.column_stack((times, longitudes, latitudes, altitudes)), 
           fmt="%.6f", delimiter=",", header="Time (unixtime),Longitude (deg),Latitude (deg),Altitude (km)", comments="")




# Compute orbital elements
inclination, RAAN, omega, true_anomaly = compute_orbital_elements(times, longitudes, latitudes, altitudes)

print(f"Inclination: {inclination:.4f}°")
print(f"RAAN: {RAAN:.4f}°")
print(f"Argument of Perigee: {omega:.4f}°")
print(f"True Anomaly: {true_anomaly:.4f}°")

# --- Plot Altitude Over Time ---
times_hours = (times - times[0]) / 3600  # Convert seconds to hours
plt.figure(figsize=(8, 5))
plt.plot(times_hours, altitudes, marker='o', linestyle='-', color='b', label="Altitude (km)")
plt.xlabel("Time (hours)")
plt.ylabel("Altitude (km)")
plt.title("Altitude over Time")
plt.grid(True)
plt.legend()
plt.show()




