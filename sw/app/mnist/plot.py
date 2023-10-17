import matplotlib.pyplot as plt

from elftools.elf.elffile import ELFFile
from pprint import pprint

def elf_func_ranges(elf_path):
    res = {}

    with open(elf_path, 'rb') as f:
        elffile = ELFFile(f)

        if not elffile.has_dwarf_info():
            print("This ELF file doesn't have DWARF information.")
            return []

        dwarf_info = elffile.get_dwarf_info()

        for CU in dwarf_info.iter_CUs():
            for DIE in CU.iter_DIEs():
                if DIE.tag == 'DW_TAG_subprogram':
                    attr = DIE.attributes
                    if 'DW_AT_name' not in attr:
                        continue
                    if 'DW_AT_low_pc' not in attr:
                        continue
                    if 'DW_AT_high_pc' not in attr:
                        continue

                    name = attr['DW_AT_name'].value.decode('utf-8')    
                    low_pc = attr['DW_AT_low_pc'].value
                    high_pc = low_pc + attr['DW_AT_high_pc'].value
                    res[name] = (low_pc, high_pc)

        return res


def find_function_by_address(address, func_ranges):
    for name, (low, high) in func_ranges.items():
        if low <= address <= high:
            return name
    return '<unknown>'

if __name__ == '__main__':
    func_ranges = elf_func_ranges('mnist')
    histogram = {}
    with open('histogram.txt', 'r') as file:    
        for line in file:
            address, value = line.split()
            address = int(address, 16)
            value = int(value)
            name = find_function_by_address(address, func_ranges)
            if not name:
                continue
            if name in histogram:
                histogram[name] += value
            else:
                histogram[name] = value

    tuples = sorted(histogram.items(), key=lambda item: item[1], reverse=True)
    labels, values = zip(*tuples)

    plt.title('mnist (-g -Og --fno-inline-small-functions)',
        fontname='Courier New', fontweight='bold')
    plt.bar(range(len(labels)), values)
    plt.xticks(range(len(labels)), labels, rotation=45,
        fontname='Courier New', fontweight='bold')
    plt.xlabel('Functions')
    plt.ylabel('Executed Instructions')
    plt.yscale('log')
    plt.subplots_adjust(bottom=0.20)
    plt.show()