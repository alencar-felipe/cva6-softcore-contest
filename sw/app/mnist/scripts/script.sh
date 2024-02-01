for file in sw/app/mnist/scripts/testes/*; do
  echo "file = $file"
  if [ -f "$file" ]; then
    current_file=$(basename "$file")
    cp $file core/include/cv32a6_ima_sv32_fpga_config_pkg.sv
    make -j$(nproc) cva6_fpga
    mv corev_apu/fpga/cva6_fpga.runs/impl_1/cva6_zybo_z7_20.bit sw/app/mnist/scripts/testes/bit/$current_file.bit"
  fi
done