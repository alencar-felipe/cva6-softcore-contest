#pragma once

#include <riscv_vector.h>

#define MAXVL (16)

typedef vint32m4_t vec_t;

static inline size_t vsetvlmax()
{
    return __riscv_vsetvlmax_e32m4();
}

static inline vec_t vmv_vx(int32_t rs1, size_t vl)
{
    return __riscv_vmv_v_x_i32m4(rs1, vl);
}

static inline vec_t vmacc_vx(vec_t vd, int32_t rs1, vec_t vs2, size_t vl)
{
    return __riscv_vmacc_vx_i32m4(vd, rs1, vs2, vl);
}

static inline vec_t vmax_vx(vec_t vs2, int32_t rs1, size_t vl)
{
    return __riscv_vmax_vx_i32m4(vs2, rs1, vl);
}

static inline vec_t vlei8(const int8_t *ptr, size_t vl)
{
    vint8m1_t vi8m1 = __riscv_vle8_v_i8m1(ptr, vl);
    vint32m4_t vi32m4 = __riscv_vsext_vf4_i32m4(vi8m1, vl);
    return (vec_t) vi32m4;
}

static inline void vnsrlse8(uint8_t *ptr, vec_t vec, int32_t shift, size_t vl)
{
    vint32m4_t vi32m4;
    vuint32m4_t vu32m4;
    vuint16m2_t vu16m2;
    vuint8m1_t vu8m1;

    vi32m4 = vec;

    vu32m4 = __riscv_vreinterpret_v_i32m4_u32m4(vi32m4);
    vu16m2 = __riscv_vnsrl_wx_u16m2(vu32m4, shift, vl);
    vu8m1 = __riscv_vnsrl_wx_u8m1(vu16m2, 0, vl);

    __riscv_vse8_v_u8m1(ptr, vu8m1, vl);
}
