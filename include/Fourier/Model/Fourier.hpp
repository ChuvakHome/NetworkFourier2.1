
#ifndef _MODEL_FOURIER_HPP_
#define _MODEL_FOURIER_HPP_

#include <cstddef>

#define MESSAGE_MAX_LENGTH 8
#define MESSAGE_MAX_BIT_LENGTH MESSAGE_MAX_LENGTH * 10

#include <Fourier/Model/Diagram.hpp>

struct fourier_series {
    float a_coefs[256];
    float b_coefs[256];
};

struct message {
    char msg[MESSAGE_MAX_LENGTH + 1];
    bool empty;
    std::size_t length;
};

struct bit_message {
    int msg_bits[MESSAGE_MAX_BIT_LENGTH];
    std::size_t bits_count;
};

struct phys_encode_result {
    discrete_diagram result_diagram;
    std::size_t bits_count;
};

struct receiver_settings {
    float desync;
    float voltage;
};

class FourierEngine {
public:
    static void init();
    static void setTime(float time);
    static float getTime();
    static void buildFourierSeries(const int *msg, std::size_t bits_count);
    static void setHarmonics(int low = 0, int high = 128);
    static int getLowHarmonic();
    static int getHighHarmonic();
    static void setNoiseAmplitude(float amp);
    static float fourierSeriesFunc(float x);
    static float fourierSeriesFuncFast(float x, float prevFourierValue, int prevLow, int prevHigh);
    static float noiseFunc(float x);
private:
    FourierEngine() {}
    static int m_lowHarmonic;
    static int m_highHarmonic;
    static std::size_t m_bitsCount;
    static float m_noiseAmplitude;
    static float m_time;
    static fourier_series m_fourierSeries;
};

message parse_message(const char *text);

bit_message code_message(message msg);

bit_message logic_encode_phys(bit_message bitmsg);
bit_message logic_encode_super45(bit_message bitmsg);
bit_message logic_encode_scrambling(bit_message bitmsg);

phys_encode_result phys_encode_nrz(bit_message bitmsg);
phys_encode_result phys_encode_rz(bit_message bitmsg);
phys_encode_result phys_encode_ami(bit_message bitmsg);
phys_encode_result phys_encode_m2(bit_message bitmsg);

bit_message phys_decode_nrz(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings);
bit_message phys_decode_rz(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings);
bit_message phys_decode_ami(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings);
bit_message phys_decode_m2(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings);

bit_message logic_decode_phys(bit_message bitmsg);
bit_message logic_decode_super45(bit_message bitmsg);
bit_message logic_decode_scrambling(bit_message bitmsg);

message parse_decoded_message(bit_message bitmsg);

#endif
