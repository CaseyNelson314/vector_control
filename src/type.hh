#pragma once

struct dq_t;
struct uvw_t;

/// @brief UVW座標系
struct uvw_t
{
    float u;
    float v;
    float w;

    /// @brief UVW座標系からDQ座標系へ変換
    dq_t to_dq(float theta) const;
};

/// @brief DQ座標系
struct dq_t 
{
    float d;
    float q;

    /// @brief DQ座標系からUVW座標系へ変換
    uvw_t to_uvw(float theta) const;
};


inline dq_t uvw_t::to_dq(float theta) const
{
    return {};
}

inline uvw_t dq_t::to_uvw(float theta) const
{
    return {};
}
