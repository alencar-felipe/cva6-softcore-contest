#pragma once

#include <stdint.h>

static uint32_t l0_weight_data[] = {
    0xdfd306FF, 0x25f7fbf3, 0x0801c810, 0xff08fc11,
    0xf1c6f608, 0x17e10100, 0xe4140324, 0xef1dfb11,
    0x06c3f9fa, 0xffea160a, 0xf5231bfc, 0xf435eb25,
    0x25d2ebfd, 0xfe19200b, 0x1c1c1bd5, 0xf034eb36,
    0xdff9e0f6, 0xfcf100e4, 0x1aede434, 0x10de191d,
    0xefffdaf6, 0xf0e8feeb, 0xf4f01914, 0xf7d3ff1a,
    0x0dfad801, 0xfff9f318, 0xd2161b23, 0xfef3e011,
    0x2cf7d805, 0xfb240215, 0x041b15f8, 0x0332bf21,
    0xe123d6fc, 0xebe801e5, 0x19df172c, 0x0ce213f9,
    0xf128f101, 0xfde4ee02, 0x06d61c1d, 0x0ce20df4,
    0x1d2f09fb, 0xf506da17, 0xf4e5270e, 0xf9cc05f2,
    0x1c240207, 0x132be30c, 0x01fbf5fb, 0xfffdda03,
    0xe21011fe, 0xf1ee0300, 0xfd011614, 0x15fc16d0,
    0xf20a2afe, 0xfeef0ef9, 0x14f2100a, 0xfcf536d0,
    0x051125fa, 0x0c0a190d, 0x01f7e720, 0xf6e631cd,
    0x1a0b33f2, 0x1d2000fd, 0xfaf3c506, 0xf3e40fdf,
    // 0xfdfa080d, 0x0501f6f6, 0x07fb01fc, 0xf2fafefe,
    // 0xebf9f606, 0xd8d8dae0, 0x0209f1d6, 0x33252a11,
    // 0xd2c3c6d3, 0xf7fafff9, 0x242f2823, 0x0b110a10,
    // 0x2506f1df, 0x2c0defdf, 0x1c1df1e1, 0x1a05f2e2,
    // 0x0b0a00f3, 0x1518ebe4, 0x0c1702e5, 0xfd0df900,
    // 0x201601fb, 0x02f3fe00, 0xe3daee01, 0x00190e03,
    // 0x19eae1f7, 0x24f9e8f1, 0x2b06e4e8, 0x200aefee,
    // 0xfeff1725, 0xfbfff0fc, 0x13f5fdeb, 0x1d0cfef1,
    // 0xd5fc2410, 0xf8231434, 0xfb0e1d2c, 0x06200a14,
    // 0x1b1b03c8, 0x151b19e4, 0xf5271c17, 0xc5e71016,
    // 0x1c231401, 0x1b16f0ed, 0xfbe5d6df, 0xf3f7f201,
    // 0x1cf5e408, 0x04d2f41a, 0x01f40619, 0xfa0114fd,
    // 0x36251111, 0x21111a1d, 0x03f2f4f9, 0xdfcdd0d0,
    // 0xebebfbfc, 0xbfe0ff19, 0xda050d13, 0x0f313616,
    // 0x34351d08, 0x32f3d3de, 0xfdcce2e2, 0xe4e6f5fc,
    // 0xf0f4efff, 0x03fef710, 0xfff90c0c, 0xf3f6fc15,
};

static int8_t *l0_weight = (int8_t *) l0_weight_data;