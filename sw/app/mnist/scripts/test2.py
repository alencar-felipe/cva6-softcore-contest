import re

points = [{'data': 1024, 'inst': 3709},
 {'data': 1521, 'inst': 3709},
 {'data': 2261, 'inst': 3709},
 {'data': 3360, 'inst': 3709},
 {'data': 4993, 'inst': 3709},
 {'data': 7419, 'inst': 3709},
 {'data': 11025, 'inst': 3709},
 {'data': 1024, 'inst': 2261},
 {'data': 1521, 'inst': 2261},
 {'data': 2261, 'inst': 2261},
 {'data': 3360, 'inst': 2261},
 {'data': 4993, 'inst': 2261},
 {'data': 7419, 'inst': 2261},
 {'data': 11025, 'inst': 2261},
 {'data': 1024, 'inst': 1378},
 {'data': 1521, 'inst': 1378},
 {'data': 2261, 'inst': 1378},
 {'data': 3360, 'inst': 1378},
 {'data': 4993, 'inst': 1378},
 {'data': 7419, 'inst': 1378},
 {'data': 11025, 'inst': 1378},
 {'data': 1024, 'inst': 840},
 {'data': 1521, 'inst': 840},
 {'data': 2261, 'inst': 840},
 {'data': 3360, 'inst': 840},
 {'data': 4993, 'inst': 840},
 {'data': 7419, 'inst': 840},
 {'data': 11025, 'inst': 840},
 {'data': 1024, 'inst': 512},
 {'data': 1521, 'inst': 512},
 {'data': 2261, 'inst': 512},
 {'data': 3360, 'inst': 512},
 {'data': 4993, 'inst': 512},
 {'data': 7419, 'inst': 512},
 {'data': 11025, 'inst': 512}]

for i in points:
# Define the new values you want to substitute
    new_IcacheByteSize = i['inst']
    new_DcacheByteSize = i['data']  # Example value, replace with your desired value

    # Read the original file content
    with open('/home/raffael/Desktop/Project/teste/cva6-softcore-contest/core/include/cv32a6_ima_sv32_fpga_config_pkg.sv', 'r') as file:
        file_content = file.read()

    # Define regular expressions for the lines you want to replace
    icache_pattern = r"localparam CVA6ConfigIcacheByteSize\s*=\s*\d+;"
    dcache_pattern = r"localparam CVA6ConfigDcacheByteSize\s*=\s*\d+;"

    # Replace the parameters with new values using regular expressions
    file_content = re.sub(icache_pattern, f"localparam CVA6ConfigIcacheByteSize = {new_IcacheByteSize};", file_content)
    file_content = re.sub(dcache_pattern, f"localparam CVA6ConfigDcacheByteSize = {new_DcacheByteSize};", file_content)

    # Write the modified content to a new file
    with open(f'testes/{new_IcacheByteSize}i_{new_DcacheByteSize}d.sv', 'w') as new_file:
        new_file.write(file_content)

    print("Parameters replaced and saved to modified_file.")
