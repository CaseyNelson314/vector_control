// ハードウエアを表現するクラスたち

#pragma once

#include "type.hh"

/// @brief 3相インバータ
class hw_inverter
{
public:

    void write_uvw_voltage(const uvw_t &uvw)
    {
    }
};

/// @brief 3相電流センサ
class hw_current_sensor
{
public:

    uvw_t read_uvw_current() const
    {
        return {};
    }
};

/// @brief ローターリーエンコーダー
class hw_rotary_encoder
{
public:

    float read_theta() const
    {
        return 0.0f;
    }
};
