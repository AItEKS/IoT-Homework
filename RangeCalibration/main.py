import serial
import time
import numpy as np
import matplotlib.pyplot as plt

PORT = '/dev/ttyUSB0'
BAUD = 9600
MIN_DIST = 5
MAX_DIST = 100

def check_coverage(distances, min_d, max_d):
    if not distances:
        return False, 0, 0
    
    d_sorted = np.sort(np.unique(np.round(distances)))
    d_in_range = d_sorted[(d_sorted >= min_d) & (d_sorted <= max_d)]
    
    if len(d_in_range) < 2:
        return False, 0, 0

    actual_min = d_in_range[0]
    actual_max = d_in_range[-1]
    coverage_percent = ((actual_max - actual_min) / (max_d - min_d)) * 100
    
    gaps = np.diff(d_in_range)
    max_gap = np.max(gaps) if len(gaps) > 0 else 0
    
    is_valid = (coverage_percent >= 90) and (max_gap <= 2)
    
    return is_valid, coverage_percent, max_gap

def collect_calibration_data(ser, min_d, max_d):
    ser.reset_input_buffer()
    
    cmd = f"START {min_d} {max_d}\n"
    ser.write(cmd.encode())

    data_dist = []
    data_ir = []

    while True:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                parts = line.split()
                if len(parts) == 2:
                    dist = float(parts[0])
                    ir = int(parts[1])
                    
                    if min_d <= dist <= max_d:
                        data_dist.append(dist)
                        data_ir.append(ir)
                        
                        valid, cov, gap = check_coverage(data_dist, min_d, max_d)
                        
                        if valid:
                            print("\n\nКалибровка успешно завершена! Достаточно данных.")
                            ser.write(b'STOP\n')
                            break
            except ValueError:
                continue
                    
    return np.array(data_dist), np.array(data_ir)

def build_and_plot_models(distances, ir_values):
    plt.figure(figsize=(10, 6))
    plt.scatter(ir_values, distances, s=10, color='gray', label='Измерения', alpha=0.5)
    
    sort_idx = np.argsort(ir_values)
    x_sorted = ir_values[sort_idx]
    
    degrees = [3, 5, 7]
    colors = ['red', 'blue', 'green']
    
    models = {}
    
    for deg, col in zip(degrees, colors):
        coeffs = np.polyfit(ir_values, distances, deg)
        poly_func = np.poly1d(coeffs)
        
        models[deg] = poly_func
        
        plt.plot(x_sorted, poly_func(x_sorted), color=col, linewidth=2, label=f'Полином {deg} ст.')

    plt.xlabel('Значение ИК датчика (AnalogRead)')
    plt.ylabel('Расстояние (см)')
    plt.title('Калибровка ИК дальномера')
    plt.legend()
    plt.grid(True)
    plt.show()
    
    return models

if __name__ == "__main__":
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)
    
    dist_data, ir_data = collect_calibration_data(ser, MIN_DIST, MAX_DIST)
    
    if len(dist_data) > 10:
        models = build_and_plot_models(dist_data, ir_data)
        final_model = models.get(3)
        
        test_ir = 300
        predicted_dist = final_model(test_ir)
        print(f"\nТест: при IR={test_ir}, дистанция = {predicted_dist:.2f} см")
    
    ser.close()