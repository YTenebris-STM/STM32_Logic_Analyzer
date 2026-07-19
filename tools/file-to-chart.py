import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Rectangle
import re

def parse_data_file(filename):
    data = []
    
    with open(filename, 'r') as f:
        content = f.read().strip()
    
    bytes_list = re.findall(r'[0-9A-Fa-f]{2}', content)
    
    for i in range(0, len(bytes_list), 2):
        if i + 1 < len(bytes_list):
            state_byte = int(bytes_list[i], 16)
            duration_byte = int(bytes_list[i+1], 16)
            duration = duration_byte + 1
            data.append((state_byte, duration))
    
    return data

def visualize_states(data, title="inputs condition"):
    if not data:
        print("no data to visualize")
        return
    
    fig, ax = plt.subplots(figsize=(15, 8))
    
    num_inputs = 8
    total_duration = sum(duration for _, duration in data)
    
    for input_idx in range(num_inputs):
        current_time = 0
        y_pos = num_inputs - input_idx - 1
        
        for state_byte, duration in data:
            bit_value = (state_byte >> input_idx) & 1
            color = '#4CAF50' if bit_value == 1 else '#f0f0f0'
            edge_color = '#333' if bit_value == 1 else '#999'
            
            rect = Rectangle((current_time, y_pos - 0.4), 
                           duration, 0.8,
                           facecolor=color, 
                           edgecolor=edge_color,
                           linewidth=1.5)
            ax.add_patch(rect)
            
            current_time += duration
    
    ax.set_xlim(0, total_duration)
    ax.set_ylim(-0.5, num_inputs - 0.5)
    ax.set_yticks(range(num_inputs))
    ax.set_yticklabels([f'input {i}' for i in range(num_inputs-1, -1, -1)])
    ax.set_xlabel('time', fontsize=12, fontweight='bold')
    ax.set_ylabel('inputs', fontsize=12, fontweight='bold')
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.grid(True, axis='x', alpha=0.3, linestyle='--')
    
    plt.tight_layout()
    plt.show()

filename = "data.log"   # your file

try:
    data = parse_data_file(filename)
    if data:
        visualize_states(data)
    else:
        print("no data found")
except FileNotFoundError:
    print(f"file '{filename}' not found")
except Exception as e:
    print(f"error: {e}")