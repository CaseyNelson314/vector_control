#pragma once

/// @brief ループ周期を一定に保つクラス
class loop_cycle_controller
{
    uint32_t loop_cycle_us; // ループ周期 [us]

    uint32_t prev_loop_us = 0; // 前回のループ開始時刻 [us]

public:
    loop_cycle_controller(uint32_t loop_cycle_us)
        : loop_cycle_us{ loop_cycle_us }
    {
    }

    void wait()
    {
        // 現在の時刻を取得
        const uint32_t now_us = micros();

        // 前回のループ開始時刻からの経過時間を計算
        const uint32_t elapsed_us = now_us - prev_loop_us;

        delaymicroseconds(loop_cycle_us - elapsed_us); // 残りの時間を待機

        prev_loop_us = now_us + loop_cycle_us;
    }

    uint32_t get_loop_cycle_us() const
    {
        return loop_cycle_us;
    }
};
