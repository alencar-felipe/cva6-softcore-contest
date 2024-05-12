from math import ceil
import numpy as np

LAYERS = {
    "l0": {
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
    "l1": {
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
    "l2": {
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
    "l3": {
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


def xadac_conv(
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
    reformed_weight: np.ndarray,
    expected: np.ndarray,
    loa: int,
    lob: int,
    lia: int,
    lib: int,
    **kwargs
) -> None:

    xadac = Xadac()

    weight = reformed_weight
    output = np.zeros((loy, lox, lon), dtype=np.uint8)

    for oy in range(loy):
        for ox in range(lox):
            for oa in range(loa):

                sum = np.zeros(lob, dtype=np.int32)

                for ob in range(lob):
                    sum[ob] = bias

                xadac.vbias(1, bias, lob)

                for wy in range(lwy):
                    for wx in range(lwx):

                        iy = sy*oy + wy
                        ix = sx*ox + wx

                        for ia in range(lia):

                            xadac.vload(2, weight[oa][wy][wx][ia], lob*lib)
                            xadac.vload(3, input[iy][ix][ia*lib:], lib)
                            xadac.vmacc(1, 2, 3, lib)

                            for ob in range(lob):
                                for ib in range(lib):
                                    in_ = ia*lib + ib

                                    if in_ >= lin:
                                        # assert 0
                                        continue

                                    sum[ob] += (
                                        input[iy][ix][in_] *
                                        weight[oa][wy][wx][ia][ob][ib]
                                    )

                for ob in range(lob):
                    on = oa*lob + ob
                    if on >= lon:
                        continue
                    output[oy][ox][on] = (
                        (sum[ob] >> shift if sum[ob] > 0 else 0) & 0xFF
                    )

                xadac.vactv(1, output[oy][ox][oa*lob:], shift, min(lob, lon - oa*lob) )

    # hexdump(output)

    assert np.array_equal(output, expected)


if __name__ == "__main__":
    for layer in LAYERS.values():
        load_data(layer)
        base_conv(**layer)
        reform_weight(layer)
        xadac_conv(**layer)
