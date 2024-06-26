#pragma once

#include <stdint.h>

static const uint32_t conv1_weight_data[] = {
    0xfdfa080d, 0x0501f6f6, 0x07fb01fc, 0xf2fafefe, 
    0xebf9f606, 0xd8d8dae0, 0x0209f1d6, 0x33252a11, 
    0xd2c3c6d3, 0xf7fafff9, 0x242f2823, 0x0b110a10, 
    0x2506f1df, 0x2c0defdf, 0x1c1df1e1, 0x1a05f2e2, 
    0x0b0a00f3, 0x1518ebe4, 0x0c1702e5, 0xfd0df900, 
    0x201601fb, 0x02f3fe00, 0xe3daee01, 0x00190e03, 
    0x19eae1f7, 0x24f9e8f1, 0x2b06e4e8, 0x200aefee, 
    0xfeff1725, 0xfbfff0fc, 0x13f5fdeb, 0x1d0cfef1, 
    0xd5fc2410, 0xf8231434, 0xfb0e1d2c, 0x06200a14, 
    0x1b1b03c8, 0x151b19e4, 0xf5271c17, 0xc5e71016, 
    0x1c231401, 0x1b16f0ed, 0xfbe5d6df, 0xf3f7f201, 
    0x1cf5e408, 0x04d2f41a, 0x01f40619, 0xfa0114fd, 
    0x36251111, 0x21111a1d, 0x03f2f4f9, 0xdfcdd0d0, 
    0xebebfbfc, 0xbfe0ff19, 0xda050d13, 0x0f313616, 
    0x34351d08, 0x32f3d3de, 0xfdcce2e2, 0xe4e6f5fc, 
    0xf0f4efff, 0x03fef710, 0xfff90c0c, 0xf3f6fc15,
};

static const int8_t *conv1_weight = (int8_t *) conv1_weight_data;