from math import ceil
import numpy as np

LAYERS = {
    "layer0": {
        "lon": 16,
        "loy": 11,
        "lox": 11,
        "lin": 1,
        "liy": 24,
        "lix": 24,
        "lwy": 4,
        "lwx": 4,
        "sy": 2,
        "sx": 2,
        "bias": 128,
        "shift": 8,
        "input_file": "resources/input.bin",
        "weight_file": "resources/l0_weight.bin",
        "expected_file": "resources/l0_expected.bin"
    },
    "layer1": {
        "lon": 24,
        "loy": 4,
        "lox": 4,
        "lin": 16,
        "liy": 11,
        "lix": 11,
        "lwy": 5,
        "lwx": 5,
        "sy": 2,
        "sx": 2,
        "bias": 128,
        "shift": 8,
        "input_file": "resources/l0_expected.bin",
        "weight_file": "resources/l1_weight.bin",
        "expected_file": "resources/l1_expected.bin"
    },
    "layer2": {
        "lon": 150,
        "loy": 1,
        "lox": 1,
        "lin": 384,
        "liy": 1,
        "lix": 1,
        "lwy": 1,
        "lwx": 1,
        "sy": 1,
        "sx": 1,
        "bias": 128,
        "shift": 8,
        "input_file": "resources/l1_expected.bin",
        "weight_file": "resources/l2_weight.bin",
        "expected_file": "resources/l2_expected.bin"
    },
    "layer3": {
        "lon": 10,
        "loy": 1,
        "lox": 1,
        "lin": 150,
        "liy": 1,
        "lix": 1,
        "lwy": 1,
        "lwx": 1,
        "sy": 1,
        "sx": 1,
        "bias": 1024,
        "shift": 11,
        "input_file": "resources/l2_expected.bin",
        "weight_file": "resources/l3_weight.bin",
        "expected_file": "resources/l3_expected.bin"
    }
}


class Xadac:

    VLEN = 128
    V8LEN = VLEN // 8
    V32LEN = VLEN // 32
    VNUM = 32

    def __init__(self) -> None:
        self.vrf = np.random.randint(
            low=0,
            high=256,
            size=(self.VNUM, self.V8LEN),
            dtype=np.uint8
        )

        self.vrf_i8 = self.vrf.view(np.int8)
        self.vrf_u8 = self.vrf.view(np.uint8)
        self.vrf_i32 = self.vrf.view(np.int32)

    def vload(self, vd, rs1, imm):
        assert imm <= self.V8LEN
        mem = np.frombuffer(rs1, dtype=np.uint8)
        for i in range(self.V8LEN):
            ptr = i % min(imm, len(mem))
            self.vrf_u8[vd][i] = mem[ptr]

    def vbias(self, vd, rs1, imm):
        assert imm <= self.V32LEN

        for i in range(self.V32LEN):
            self.vrf_i32[vd][i] = rs1 if i < imm else 0

    def vmacc(self, vd, vs1, vs2, imm):
        assert imm <= 4
        for i in range(self.V32LEN):
            for j in range(imm):
                self.vrf_i32[vd][i] += (
                    self.vrf_i8[vs1][i * imm + j] *
                    self.vrf_u8[vs2][i * imm + j]
                )

    def vactv(self, vs3, rs1, rs2, imm):
        assert imm <= self.V32LEN
        ptr = np.frombuffer(rs1, dtype=np.uint8)
        for i in range(imm):
            sum = self.vrf_i32[vs3][i]
            sum = (sum >> rs2 if sum > 0 else 0) & 0xFF
            ptr[i] = np.uint8(sum)


def load_data(layer: dict) -> None:
    load_data = {
        "input": ("liy", "lix", "lin"),
        "weight": ("lon", "lwy", "lwx", "lin"),
        "expected": ("loy", "lox", "lon")
    }

    for name, kshape in load_data.items():
        shape = tuple(layer[k] for k in kshape)
        data = np.fromfile(layer[f"{name}_file"], dtype=np.uint8)
        layer[name] = data[:np.prod(shape)].reshape(shape)

    layer["weight"] = layer["weight"].view(np.int8)


def hexdump(data):
    data_uint32 = data.reshape(-1).view(np.uint32)
    for i in range(0, len(data_uint32), 4):
        line = data_uint32[i:i+4]
        print(" ".join(f"{x:08x}" for x in line))


def gen_conv(
    lon: int,
    loy: int,
    lox: int,
    lin: int,
    liy: int,
    lix: int,
    lwy: int,
    lwx: int,
    sy: int,
    sx: int,
    bias: int,
    shift: int,
    **kwargs
) -> None:
    loa = 1
    lob = 1
    lia = 1
    lib = 1

    s_vec = "1"
    w_vec = "2"
    i_vec = "3"

    b_reg = "x1"
    s_reg = "x2"
    w_reg = "x3"
    i_reg = "x4"
    oy_reg = "x5"
    ox_reg = "x6"

    for oy in range(loy):
        for ox in range(lox):
            i_reg = sy*lix*lin*oy + sx*lin*ox
            for oa in range(loa):
                print(f"xadac.vbias {s_vec}, {b_reg}, {lob}")
                for wy in range(lwy):
                    for wx in range(lwx):
                        for ia in range(lia):
                            print(f"xadac.vload {w_vec}, {w_reg}, {lob*lib}")
                            print(f"xadac.vload {i_vec}, {i_reg}, {lib}")
                            print(f"xadac.vmacc {s_vec}, {w_vec}, {i_vec}, {lib}")
                            print(f"addi {w_reg}, {w_reg}, {lib}")
                            print(f"addi {i_reg}, {i_reg}, {lib}")
                print(f"xadac.vactv {s_vec}, {s_reg}, {shift}, {lob}")
            print(f"addi {ox_reg}, {ox_reg}, 1")
        print(f"addi {oy_reg}, {oy_reg}, 1")


def base_conv(
    lon: int,
    loy: int,
    lox: int,
    lin: int,
    lwy: int,
    lwx: int,
    sy: int,
    sx: int,
    bias: int,
    shift: int,
    input: np.ndarray,
    weight: np.ndarray,
    expected: np.ndarray,
    **kwargs
) -> None:

    output = np.zeros((loy, lox, lon), dtype=np.uint8)

    for oy in range(loy):
        for ox in range(lox):
            for on in range(lon):
                sum = bias
                for wy in range(lwy):
                    for wx in range(lwx):
                        for in_ in range(lin):
                            iy = sy*oy + wy
                            ix = sx*ox + wx
                            sum += input[iy][ix][in_] * weight[on][wy][wx][in_]

                output[oy][ox][on] = (sum >> shift if sum > 0 else 0) & 0xFF

    assert np.array_equal(output, expected)


def reform_weight(layer):
    lon = layer["lon"]
    lin = layer["lin"]
    lwx = layer["lwx"]
    lwy = layer["lwy"]
    weight = layer["weight"]

    loa = layer["loa"] = ceil(lon / Xadac.V32LEN)
    lob = layer["lob"] = Xadac.V32LEN

    lia = layer["lia"] = ceil(lin / 4)
    lib = layer["lib"] = min(4, lin)

    kshape = ("loa", "lwx", "lwy", "lia", "lob", "lib")
    shape = tuple(layer[k] for k in kshape)

    reformed = np.zeros(shape, dtype=np.int8)

    print(layer["weight_file"])

    for oa in range(loa):
        for wx in range(lwx):
            for wy in range(lwy):
                for ia in range(lia):
                    for ob in range(lob):
                        for ib in range(lib):
                            on = oa*lob + ob
                            in_ = ia*lib + ib

                            if on >= lon or in_ >= lin:
                                continue

                            reformed[oa][wx][wy][ia][ob][ib] = (
                                weight[on][wx][wy][in_]
                            )

    layer["reformed_weight"] = reformed


def asm_array(arr, name):
    if len(arr) % 4 != 0:
        padding_length = 4 - (len(arr) % 4)
        padding = np.zeros(padding_length, dtype=np.uint8)
        arr = np.concatenate((arr, padding))

    arr_32bit = arr.view(np.uint32)

    asm = "    .data\n"
    asm += f"{name}:\n"
    for i in range(0, len(arr_32bit), 4):
        words = arr_32bit[i:i+4]
        words_hex = ", ".join(f"0x{word:08x}" for word in words)
        asm += f"    .word {words_hex}\n"

    return asm


def xadac_conv(
    lon: int,
    loy: int,
    lox: int,
    lin: int,
    lix: int,
    lwy: int,
    lwx: int,
    sy: int,
    sx: int,
    bias: int,
    shift: int,
    input: np.ndarray,
    reformed_weight: np.ndarray,
    expected: np.ndarray,
    loa: int,
    lob: int,
    lia: int,
    lib: int,
    **kwargs
) -> str:

    xadac = Xadac()

    input = np.frombuffer(input, dtype=np.uint8)
    weight = np.frombuffer(reformed_weight, dtype=np.uint8)
    expected = np.frombuffer(expected, dtype=np.uint8)
    output = np.zeros((loy*lox*lon), dtype=np.uint8)

    i_ptr = 0
    o_ptr = 0
    for oy in range(loy):
        for ox in range(lox):
            w_ptr = 0
            for oa in range(loa):
                xadac.vbias(1, bias, lob)

                for wy in range(lwy):
                    for wx in range(lwx):
                        for ia in range(lia):
                            xadac.vload(2, weight[w_ptr:], lob*lib)
                            xadac.vload(3, input[i_ptr:], lib)
                            xadac.vmacc(1, 2, 3, lib)
                            w_ptr += lob*lib
                            i_ptr += lib

                    i_ptr += (lix - lwx)*lin

                i_ptr -= lib*lia*lwx*lwy + lwy*(lix - lwx)*lin

                imm = min(lob, lon - oa*lob)
                xadac.vactv(1, output[o_ptr:], shift, imm)
                o_ptr += imm

            i_ptr += sx*lin
        i_ptr += sy*lix*lin - lox*sx*lin

    assert np.array_equal(output, expected)


def xadac_conv_asm(
    name: str,
    lon: int,
    loy: int,
    lox: int,
    lin: int,
    lix: int,
    lwy: int,
    lwx: int,
    sy: int,
    sx: int,
    bias: int,
    shift: int,
    reformed_weight: np.ndarray,
    loa: int,
    lob: int,
    lia: int,
    lib: int,
    **kwargs
) -> str:

    asm = ""

    def asm_vbias(vd, rs1, imm):
        nonlocal asm
        asm += f"    xadac.vbias {vd}, {rs1}, {imm}\n"

    def asm_vload(vd, rs1, imm):
        nonlocal asm
        asm += f"    xadac.vload {vd}, {rs1}, {imm}\n"

    def asm_vmacc(vd, vs1, vs2, imm):
        nonlocal asm
        asm += f"    xadac.vmacc {vd}, {vs1}, {vs2}, {imm}\n"

    def asm_vactv(vs3, rs1, rs2, imm):
        nonlocal asm
        asm += f"    xadac.vactv {vs3}, {rs1}, {rs2}, {imm}\n"

    def asm_la(reg, symbol):
        nonlocal asm
        asm += f"    la {reg}, {symbol}\n"

    def asm_li(reg, value):
        nonlocal asm
        asm += f"    li {reg}, {value}\n"

    def asm_ret():
        nonlocal asm
        asm += "    ret\n"

    def asm_inc(reg, imm):
        nonlocal asm
        asm += f"    addi {reg}, {reg}, {imm}\n"

    def asm_newline():
        nonlocal asm
        asm += "\n"

    asm += "    .text\n"
    asm += f"    .globl {name}\n"
    asm += f"    .type {name}, @function\n"
    asm += f"{name}:\n"

    s_vec = "1"
    w_vec = "2"
    i_vec = "3"

    o_reg = "a0"
    w_reg = "t0"
    i_reg = "a1"

    bias_reg = "t1"
    shift_reg = "t2"

    asm_li(bias_reg, bias)
    asm_li(shift_reg, shift)
    for oy in range(loy):
        for ox in range(lox):
            asm_la(w_reg, "weight")
            for oa in range(loa):
                asm_vbias(s_vec, bias_reg, lob)
                for wy in range(lwy):
                    for wx in range(lwx):
                        for ia in range(lia):
                            asm_vload(w_vec, w_reg, lob*lib)
                            asm_vload(i_vec, i_reg, lib)
                            asm_vmacc(s_vec, w_vec, i_vec, lib)
                            asm_inc(w_reg, lob*lib)
                            asm_inc(i_reg, lib)

                    asm_inc(i_reg, (lix - lwx)*lin)

                asm_inc(i_reg, - lib*lia*lwx*lwy - lwy*(lix - lwx)*lin)

                imm = min(lob, lon - oa*lob)
                asm_vactv(s_vec, o_reg, shift_reg, imm)
                asm_inc(o_reg, imm)

            asm_inc(i_reg, sx*lin)

        asm_inc(i_reg, sy*lix*lin - lox*sx*lin)

    asm_ret()
    asm_newline()

    weight = np.frombuffer(reformed_weight, dtype=np.uint8)
    asm += asm_array(weight, "weight")

    return asm


if __name__ == "__main__":
    for name, params in LAYERS.items():
        load_data(params)
        base_conv(**params)
        reform_weight(params)
        xadac_conv(**params)
        asm = xadac_conv_asm(name=name, **params)
        with open(f"{name}.S", "w") as f:
            f.write(asm)
        # break
