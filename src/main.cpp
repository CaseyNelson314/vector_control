#include "hw.hh"
#include "pi_controller.hh"
#include "loop_cycle_controller.hh"

#include <utility>

/// @brief ACモータ制御クラス
/// @note 制御はdq座標系で行う
class ac_motor
{
    hw_inverter inverter;
    hw_current_sensor current_sensor;
    hw_rotary_encoder rotary_encoder;

public:

    ac_motor(hw_inverter&& inverter, hw_current_sensor&& current_sensor, hw_rotary_encoder&& rotary_encoder)
        : inverter{ std::move(inverter) }
        , current_sensor{ std::move(current_sensor) }
        , rotary_encoder{ std::move(rotary_encoder) }
    {
    }

    /// @brief DQ座標系の電圧をインバータへ出力
    /// @param dq DQ座標系の電圧 [V]
    void write_dq_voltage(const dq_t &dq)
    {
        // dq座標系からUVW座標系へ変換
        const uvw_t uvw = dq.to_uvw(rotary_encoder.read_theta());

        // UVW座標系の電圧をインバータへ出力
        inverter.write_uvw_voltage(uvw);
    }

    /// @brief DQ座標系の電流を取得
    dq_t read_dq_current() const
    {
        // UVW座標系の電流を取得
        const uvw_t uvw = current_sensor.read_uvw_current();

        // UVW座標系からDQ座標系へ変換
        return uvw.to_dq(rotary_encoder.read_theta());
    }

    /// @brief 回転角を取得
    float read_theta() const
    {
        return rotary_encoder.read_theta();
    }
};


//// @brief 電流フィードバック制御クラス
/// @note 制御はdq座標系で行う
class current_feedback_ac_motor
{
    ac_motor motor;
    pi_controller pi_d;
    pi_controller pi_q;
public:

    /// @brief 電流フィードバック制御クラス
    /// @param motor ACモータ制御クラス
    /// @param pi_dq dq軸電流PI制御器 (どちらもパラメーターは同じ)
    current_feedback_ac_motor(ac_motor&& motor, pi_controller&& pi_dq)
        : motor{ std::move(motor) }
        , pi_d{ pi_dq }
        , pi_q{ pi_dq }
    {
    }

    /// @brief 電流フィードバック制御
    /// @param target_current 目標電流 [A]
    void write_target_dq_current(const dq_t &target_current)
    {
        // 現在の電流を取得
        const dq_t current = motor.read_dq_current();

        // PI制御器で電圧を計算
        const dq_t dq_voltage = {
            pi_d.update(current.d, target_current.d),
            pi_q.update(current.q, target_current.q)
        };

        // 計算した電圧をインバータへ出力
        motor.write_dq_voltage(dq_voltage);
    }

    /// @brief 回転角を取得
    float read_theta() const
    {
        return motor.read_theta();
    }
};


/// @brief 速度フィードバック制御クラス
class speed_feedback_ac_motor
{
    current_feedback_ac_motor motor;
    pi_controller pi_speed;
    
public:

    speed_feedback_ac_motor(current_feedback_ac_motor&& motor, pi_controller&& pi_speed)
        : motor{ std::move(motor) }
        , pi_speed{ std::move(pi_speed) }
    {
    }

    /// @brief 速度フィードバック制御
    /// @param target_speed 目標速度 [rad/s]
    void write_target_speed(float target_speed)
    {
        // 現在の回転角を取得
        const float theta = motor.read_theta();

        // 速度を計算
        const float speed = theta; // TODO: 速度を計算する関数を実装

        // 現在の速度と目標度を比較し、PI制御器で出力電流を計算
        const dq_t dq_current = {
            pi_speed.update(speed, target_speed),
            0.0f
        };

        // インバータへ出力
        motor.write_target_dq_current(dq_current);
    }

    /// @brief 回転角を取得
    float read_theta() const
    {
        return motor.read_theta();
    }
};

loop_cycle_controller loop_ctrl{ 1000 }; // 1ms周期

speed_feedback_ac_motor motor {
    current_feedback_ac_motor {
        ac_motor{
            hw_inverter{},
            hw_current_sensor{},
            hw_rotary_encoder{},
        },
        pi_controller{ 0.1f, 0.01f, loop_ctrl.get_loop_cycle_us() },  // dq軸電流PI制御器
    },
    pi_controller{ 0.1f, 0.01f, loop_ctrl.get_loop_cycle_us() }  // 速度PI制御器
};

int main()
{
    for (;;)
    {
        motor.write_target_speed(100.0f); // 目標速度 [rad/s]
        loop_ctrl.wait();
    }
}
