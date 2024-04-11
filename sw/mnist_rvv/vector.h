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
    volatile register zero = 0;
    vint8m1_t vi8m1 = __riscv_vle8_v_i8m1(ptr, vl);
    vint16m2_t vi16m2 = __riscv_vwadd_vx_i16m2(vi8m1, zero, vl);
    vint32m4_t vi32m4 = __riscv_vwadd_vx_i32m4(vi16m2, zero, vl);
    return (vec_t) vi32m4;
}

static inline void vnclip_vse8(
    uint8_t *ptr,
    vec_t vec,
    int32_t shift,
    size_t vl
)
{
    asm (
        "vsetvli x0, %[vl], e16, m2, ta, ma\n\t"
        "vnclipu.wi	%[vec], %[vec], %[shift]\n\t"
        "vsetvli x0, %[vl], e8, m1, ta, ma\n\t"
        "vnclipu.wi	%[vec], %[vec],0\n\t"
        "vse8.v %[vec], (%[ptr])\n\t"
        :
        :
        [vl] "r" (vl),
        [vec] "vr" (vec),
        [shift] "I" (shift),
        [ptr] "r" (ptr)
    );
}
