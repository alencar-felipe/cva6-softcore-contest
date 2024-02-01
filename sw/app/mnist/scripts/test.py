import matplotlib.pyplot as plt

num_cycles_history = [2315733,2315259,2315598,'4096x16384',2353088,'16384x4096','32768x2048','65563x1024','131072x512']

cache_history = [[512,131072,128,128],[1024,65563,128,128],[2048,32768,128,128],[4096,16384,128,128],[8192,8192,128,128],[16384,4096,128,128],[32768,2048,128,128],[65563,1024,128,128],[131072,512,128,128]]

x_values = [0.1 + idx for idx in range(len(num_cycles_history))]

bar_width = 0.05
#print(f"cache_inst_size={cache_history[0][0]}\ncache_dat_size={cache_history[0][1]}\ncache_dline_width={cache_history[0][2]}\ncache_iline_width={cache_history[0][3]}")
    
plt.figure(figsize=(6, 6))
plt.bar(x_values, num_cycles_history, color='blue', width=bar_width)
plt.xlabel('Parametros')
plt.ylabel('numCycles')
plt.title('Gráfico de Parâmetros vs. numCycles')
#parameters_label = f"cache_inst_size={cache_history[0][0]}\ncache_dat_size={cache_history[0][1]}\ncache_dline_width={cache_history[0][2]}\ncache_iline_width={cache_history[0][3]}"
plt.xticks(x_values, [f"Option{idx+1}\n"
                     f"cache_inst_size={cache_history[idx][0]}\n"
                     f"cache_dat_size={cache_history[idx][1]}\n"
                     f"cache_dline_width={cache_history[idx][2]}\n"
                     f"cache_iline_width={cache_history[idx][3]}\n"
                     f"num_cycles={num_cycles_history[idx]}"
                     for idx in range(len(num_cycles_history))])
plt.xlim(0, x_values[-1] + 0.5)  # Ajustar os limites do eixo X
plt.ylim(0, max(num_cycles_history) * 1.1)
plt.show()