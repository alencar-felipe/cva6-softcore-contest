from sys import argv

_, imm, vd, rs2, rs1, funct3 = argv

instr = 0
instr |= (int(imm) << 25)
instr |= (int(rs2) << 20)
instr |= (int(rs1) << 15)
instr |= (int(funct3) << 12)
instr |= (int(vd) << 7)
instr |= (0x1D << 2)
instr |= 0x03

print(f"{instr:08x}")
