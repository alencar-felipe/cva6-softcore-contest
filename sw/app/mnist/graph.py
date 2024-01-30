import re
import matplotlib.pyplot as plt
import serial


num_cycles_history = []

cache_history = []
def create_graph(cache_history):
    x_values = [0.2 + idx for idx in range(len(num_cycles_history))]

    bar_width = 0.05
    #print(f"cache_inst_size={cache_history[0][0]}\ncache_dat_size={cache_history[0][1]}\ncache_dline_width={cache_history[0][2]}\ncache_iline_width={cache_history[0][3]}")
    
    plt.figure(figsize=(10, 8))
    plt.bar(x_values, num_cycles_history, color='blue', width=bar_width)
    plt.xlabel('Parâmetros')
    plt.ylabel('numCycles')
    plt.title('Gráfico de Parâmetros vs. numCycles')
    #parameters_label = f"cache_inst_size={cache_history[0][0]}\ncache_dat_size={cache_history[0][1]}\ncache_dline_width={cache_history[0][2]}\ncache_iline_width={cache_history[0][3]}"
    plt.xticks(x_values, [f"Option{idx+1}\n"
                         f"cache_inst_size={cache_history[idx][0]}\n"
                         f"cache_dat_size={cache_history[idx][1]}\n"
                         f"cache_dline_width={cache_history[idx][2]}\n"
                         f"cache_iline_width={cache_history[idx][3]}"
                         for idx in range(len(num_cycles_history))])
    # Atualizar o rótulo do eixo X para refletir os novos valores
    #plt.xticks(x_values, [f"Option{idx+1}\n{parameters_label}" for idx in range(len(num_cycles_history))])
    plt.xlim(0, x_values[-1] + 0.5)  # Ajustar os limites do eixo X

    plt.ylim(0, max(num_cycles_history) * 1.1)

    plt.show()

def read_values_from_file(filename):
    with open(filename, 'r') as file:
        contents = file.read()

    cache_inst_size = int(re.search(r'CVA6ConfigIcacheByteSize = (\d+);', contents).group(1))
    cache_dat_size = int(re.search(r'CVA6ConfigDcacheByteSize = (\d+);', contents).group(1))
    cache_dline_width = int(re.search(r'CVA6ConfigDcacheLineWidth = (\d+);', contents).group(1))
    cache_iline_width = int(re.search(r'CVA6ConfigIcacheLineWidth = (\d+);', contents).group(1))
    #print(f"1)cache_inst_size= {cache_inst_size}\ncache_dat_size = {cache_dat_size}\ncache_dline_width= {cache_dline_width}\ncache_iline_width= {cache_iline_width}")
    return cache_inst_size, cache_dat_size, cache_dline_width, cache_iline_width

def get_num_cycles_from_serial():
    ser = serial.Serial('/dev/ttyUSB0', 115200)
    if ser.isOpen():
        try:
            while True:
                data = ser.readline()
                if data:
                    return int(data.decode('utf-8').strip())
        except Exception as e:
            print("Problem to read the serial port: ", e)
        finally:
            ser.close()
    else:
        print("Not possible to open the serial port.")

def main():
    while True:
        filename = '/home/raffael/Desktop/Project/teste/cva6-softcore-contest/core/include/cv32a6_ima_sv32_fpga_config_pkg.sv'
        num_cycles = get_num_cycles_from_serial()
        if num_cycles is not None:
            cache_inst_size, cache_dat_size, cache_dline_width, cache_iline_width = read_values_from_file(filename)
            cache_history.append([cache_inst_size, cache_dat_size, cache_dline_width, cache_iline_width])
            num_cycles_history.append(num_cycles)

            #print(f"cycles = {num_cycles}\n")
            for sublist in cache_history:
                cache_inst_size, cache_dat_size, cache_dline_width, cache_iline_width = sublist
                #print(f"cache_inst_size={cache_inst_size}\ncache_dat_size={cache_dat_size}\ncache_dline_width={cache_dline_width}\ncache_iline_width={cache_iline_width}")
            create_graph(cache_history)
            num_cycles=0

if __name__ == '__main__':
    main()
