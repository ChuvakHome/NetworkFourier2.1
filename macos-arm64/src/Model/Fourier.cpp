
#include <Fourier/Model/Fourier.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <Fourier/Utils.hpp>

#define PI 3.141592654f
#define SIN_COS_SERIES 65536

// #define SIN(x) sin_cos_table[(static_cast<unsigned short int>((x) * 65536.0f / PI / 2.0f)) & 0xFFFF].sinval
// #define COS(x) sin_cos_table[(static_cast<unsigned short int>((x) * 65536.0f / PI / 2.0f)) & 0xFFFF].cosval

#define reduced_arg(x) ((x) / (2 * PI))

/* calculates argument needed for SINe and COSine macros */
#define rem(x) ((x) - static_cast<int>((x)))

#define SIN(x) sin_cos_table[(static_cast<std::size_t>(ldexp(rem(reduced_arg(x)), 16))) & 0xFFFF].sinval
#define COS(x) sin_cos_table[(static_cast<std::size_t>(ldexp(rem(reduced_arg(x)), 16))) & 0xFFFF].cosval

// #define SIN(x) sinf(x)
// #define COS(x) cosf(x)

struct sin_cos {
    float sinval;
    float cosval;
};

static bool sin_cos_table_built = false;
static sin_cos sin_cos_table[SIN_COS_SERIES];

/* based on Demiurg's sources */
static void build_sin_cos_table() {
    if (!sin_cos_table_built) {
		for (std::size_t i = 0; i < SIN_COS_SERIES; ++i) {
            sin_cos_table[i] = {
                sinf(static_cast<float>(i) / 65536 * 2 * PI),
                cosf(static_cast<float>(i) / 65536 * 2 * PI)
            };
		}

		sin_cos_table_built = true;
	}
}

void FourierEngine::init() {
    build_sin_cos_table();
}

int FourierEngine::m_lowHarmonic = 0;
int FourierEngine::m_highHarmonic = 128;
std::size_t FourierEngine::m_bitsCount = 0;
float FourierEngine::m_noiseAmplitude = 0.0f;
fourier_series FourierEngine::m_fourierSeries;
float FourierEngine::m_time = 0;

void FourierEngine::setTime(float time) {
    FourierEngine::m_time = time;
}

float FourierEngine::getTime() {
    return FourierEngine::m_time;
}

/* Code from Demiurg's sources */
void FourierEngine::buildFourierSeries(const int *msg, std::size_t bits_count) {
    int k, i;
    FourierEngine::m_bitsCount = bits_count;
    float length = static_cast<float>(m_bitsCount);
	float x, fk;

	FourierEngine::m_fourierSeries.a_coefs[0] = FourierEngine::m_fourierSeries.b_coefs[0] = 0;

	for (x = 0; x < length; x += 0.5) {
		i = static_cast<int>(x * 2);
		FourierEngine::m_fourierSeries.a_coefs[0] += 0.5f * static_cast<float>(msg[i]) / length;
	}

	for (x = -length; x < 0; x += 0.5) {
		i = static_cast<int>(x * 2 + length * 2);
		FourierEngine::m_fourierSeries.a_coefs[0] += 0.5f * static_cast<float>(msg[i]) / length;
	}

	for (k = 1; k < 256; k++) {
		fk = static_cast<float>(k);
		FourierEngine::m_fourierSeries.a_coefs[k] = 0;
		FourierEngine::m_fourierSeries.b_coefs[k] = 0;

		for (x = 0; x < length; x += 0.5) {
			i = static_cast<int>(x * 2);
			FourierEngine::m_fourierSeries.a_coefs[k] += static_cast<float>(msg[i]) / (PI * fk) * (SIN((PI * fk / length) * (x + 0.5f)) - SIN((PI * fk / length) * (x + 0.0f)));
			FourierEngine::m_fourierSeries.b_coefs[k] -= static_cast<float>(msg[i]) / (PI * fk) * (COS((PI * fk / length) * (x + 0.5f)) - COS((PI * fk / length) * (x + 0.0f)));
        }

		for (x = -length; x < 0; x += 0.5) {
			i = static_cast<int>(x * 2 + length * 2);
			FourierEngine::m_fourierSeries.a_coefs[k] += static_cast<float>(msg[i]) / (PI * fk) * (SIN((PI * fk / length) * (x + 0.5f)) - SIN((PI * fk / length) * (x + 0.0f)));
			FourierEngine::m_fourierSeries.b_coefs[k] -= static_cast<float>(msg[i]) / (PI * fk) * (COS((PI * fk / length) * (x + 0.5f)) - COS((PI * fk / length) * (x + 0.0f)));
		}
	}
}

void FourierEngine::setHarmonics(int low, int high) {
    FourierEngine::m_lowHarmonic = low;
    FourierEngine::m_highHarmonic = high;
}

int FourierEngine::getLowHarmonic() {
    return FourierEngine::m_lowHarmonic;
}

int FourierEngine::getHighHarmonic() {
    return FourierEngine::m_highHarmonic;
}

void FourierEngine::setNoiseAmplitude(float amp) {
    FourierEngine::m_noiseAmplitude = amp;
}

static float fourier_series_on_segment(fourier_series series, int start, int end, float x, float len) {
    float sum = 0;

    float kf = static_cast<float>(start);

    for (int k = start; k <= end; ++k) {
        sum += series.a_coefs[k] * COS(kf * x * PI / len) + series.b_coefs[k] * SIN(kf * x * PI / len);

        if (k == 0)
            sum *= 0.5f;

        kf += 1.0f;
    }

    return sum;
}

/* inspired by Demiurg's sources */
float FourierEngine::fourierSeriesFunc(float x) {
	return fourier_series_on_segment(FourierEngine::m_fourierSeries, FourierEngine::m_lowHarmonic, FourierEngine::m_highHarmonic, x, static_cast<float>(FourierEngine::m_bitsCount));
}

/* function is useful when we compute Fourier series after changing low/high harmonic number without editing the message */
float FourierEngine::fourierSeriesFuncFast(float x, float prevFourierValue, int prevLow, int prevHigh) {
    float result = prevFourierValue;
	float length = static_cast<float>(FourierEngine::m_bitsCount);

    int minval = std::min(prevLow, FourierEngine::m_lowHarmonic);
    int maxval = std::max(prevLow, FourierEngine::m_lowHarmonic);

    result += fourier_series_on_segment(FourierEngine::m_fourierSeries, minval, maxval - 1, x, length) * (prevLow >= FourierEngine::m_lowHarmonic ? 1.0f : -1.0f);

    minval = std::min(prevHigh, FourierEngine::m_highHarmonic);
    maxval = std::max(prevHigh, FourierEngine::m_highHarmonic);

    result += fourier_series_on_segment(FourierEngine::m_fourierSeries, minval + 1, maxval, x, length) * (prevHigh <= FourierEngine::m_highHarmonic ? 1.0f : -1.0f);

	return result;
}

float FourierEngine::noiseFunc(float x) {
	float result = 0.0f;

    for (float i = 1; i < FourierEngine::m_highHarmonic; i += 1.0f) // may be: for (i=low+1; i<high; i++)
    	 result += (0.5f / i) * SIN(i * x + i * i * i * i * FourierEngine::m_time);

	result *= FourierEngine::m_noiseAmplitude;

	return result;
}

message parse_message(const char *text) {
    message parsed_message;

	if (text[0] == '\\' && text[1] != '\\')	{	// "\12345ABCD"
        memset(parsed_message.msg, 0, sizeof(parsed_message.msg));
        std::ptrdiff_t j = 0;

        for (std::size_t i = 0; text[i + 1] != 0; ++i) {
            char symb = text[i + 1];

            j = i >> 1;

            int digit;

            if ('0' <= symb && symb <= '9')
                digit = symb - '0';
            else if ('a' <= symb && symb <= 'f')
                digit = symb - 'a' + 10;
            else if ('A' <= symb && symb <= 'F')
                digit = symb - 'A' + 10;
            else
                digit = 0;

            parsed_message.msg[j] = (parsed_message.msg[j] << 4) + digit;
        }

		parsed_message.msg[4] = parsed_message.msg[5] = parsed_message.msg[6] = parsed_message.msg[7] = 0;
		parsed_message.length = strlen(text) / 2;

        for (std::size_t i = 0; i < parsed_message.length / 2; ++i)
            std::swap(parsed_message.msg[i], parsed_message.msg[parsed_message.length - i - 1]);
	}
    else if (text[0] == '\\' && text[1] == '\\')	{	// "\\blah-blah-blah"
		string_copy(parsed_message.msg, text + 1, 8);
		parsed_message.msg[8] = 0;
		parsed_message.length = strlen(parsed_message.msg);
	}
    else if (text[0] != '\\') {					// "blah-blah-blah"
		string_copy(parsed_message.msg, text, 8);
		parsed_message.msg[8] = 0;
		parsed_message.length = strlen(parsed_message.msg);
	}

    parsed_message.empty = false;

    if (parsed_message.length <= 0) {
		parsed_message.length = 1;
		parsed_message.msg[0] = 0;
		parsed_message.msg[1] = 0;
        parsed_message.empty = true;
	}

    return parsed_message;
}

bit_message code_message(message msg) {
    bit_message bitmsg;
    bitmsg.bits_count = msg.length << 3;

    for (std::size_t i = 0; i < msg.length; ++i) {
        std::ptrdiff_t j = i << 3;
        uint8_t x = msg.msg[i];

        for (int k = 0; k < 8; ++k) {
            std::ptrdiff_t index = j + 7 - k;
            bitmsg.msg_bits[index] = x & 1;
            x = x >> 1;
        }
    }

    return bitmsg;
}

bit_message logic_encode_phys(bit_message bitmsg) {
    return bitmsg;
}

bit_message logic_encode_super45(bit_message bitmsg) {
    std::size_t nibbles_count = bitmsg.bits_count >> 2;

    int table[] = {
		0x1E, 0x09, 0x14, 0x15,
		0x0A, 0x0B, 0x0E, 0x0F,

		0x12, 0x13, 0x16, 0x17,
		0x1A, 0x1B, 0x1C, 0x1D
	};

    bit_message bitmsg_super45;
    bitmsg_super45.bits_count = bitmsg.bits_count + nibbles_count;

    std::size_t i = 0, j = 0;

    while (i < nibbles_count && j < MESSAGE_MAX_BIT_LENGTH) {
        std::size_t k = i << 2;

        int nibble = ((bitmsg.msg_bits[k] & 1) << 3) + ((bitmsg.msg_bits[k + 1] & 1) << 2) + ((bitmsg.msg_bits[k + 2] & 1) << 1) + (bitmsg.msg_bits[k + 3] & 1);

        int code = table[nibble];

        bitmsg_super45.msg_bits[j++] = (code >> 4) & 1;
        bitmsg_super45.msg_bits[j++] = (code >> 3) & 1;
        bitmsg_super45.msg_bits[j++] = (code >> 2) & 1;
        bitmsg_super45.msg_bits[j++] = (code >> 1) & 1;
        bitmsg_super45.msg_bits[j++] = code & 1;

        i++;
    }

    return bitmsg_super45;
}

bit_message logic_encode_scrambling(bit_message bitmsg) {
    bit_message scramb_bitmsg = bitmsg;

	for (std::size_t i = 0; i < scramb_bitmsg.bits_count; i++) {
		int a = bitmsg.msg_bits[i];
        int b = i >= 3 ? scramb_bitmsg.msg_bits[i - 3] : 0;
        int c = i >= 5 ? scramb_bitmsg.msg_bits[i - 5] : 0;

		scramb_bitmsg.msg_bits[i] = (a + b + c) & 1;
	}

    return scramb_bitmsg;
}

phys_encode_result phys_encode_nrz(bit_message bitmsg) {
    discrete_diagram result_diagram;
    result_diagram.diagram_points_count = bitmsg.bits_count << 1;
    int level_by_bit[] = { -1, 1 };

    for (std::size_t i = 0; i < bitmsg.bits_count; ++i) {
        int bitval = bitmsg.msg_bits[i];
        std::size_t index = i << 1;
        result_diagram.diagram_points[index] = { static_cast<float>(i + 1) - 0.5f, static_cast<float>(level_by_bit[bitval]) };
        result_diagram.diagram_points[index + 1] = { static_cast<float>(i + 1), static_cast<float>(level_by_bit[bitval]) };
    }

    phys_encode_result encode_result;
    encode_result.result_diagram = result_diagram;
    encode_result.bits_count = bitmsg.bits_count;

    return encode_result;
}

phys_encode_result phys_encode_rz(bit_message bitmsg) {
    discrete_diagram result_diagram;
    result_diagram.diagram_points_count = bitmsg.bits_count << 1;
    int level_by_bit[] = { -1, 1 };

    for (std::size_t i = 0; i < bitmsg.bits_count; ++i) {
        int bitval = bitmsg.msg_bits[i];
        std::size_t index = i << 1;
        result_diagram.diagram_points[index] = { static_cast<float>(i + 1) - 0.5f, static_cast<float>(level_by_bit[bitval]) };
        result_diagram.diagram_points[index + 1] = { static_cast<float>(i + 1), 0.0f };
    }

    phys_encode_result encode_result;
    encode_result.result_diagram = result_diagram;
    encode_result.bits_count = bitmsg.bits_count;

    return encode_result;
}

phys_encode_result phys_encode_ami(bit_message bitmsg) {
    discrete_diagram result_diagram;
    result_diagram.diagram_points_count = bitmsg.bits_count << 1;

    for (std::size_t i = 0; i < bitmsg.bits_count; ++i) {
        std::size_t index = i << 1;
        result_diagram.diagram_points[index] = { static_cast<float>(i + 1) - 0.5f, static_cast<float>(bitmsg.msg_bits[i]) };
        result_diagram.diagram_points[index + 1] = { static_cast<float>(i + 1), static_cast<float>(bitmsg.msg_bits[i]) };
    }

    int sign_coefs[] = { 1, -1 };
    int sign = 1;

    for (std::size_t i = 0; i < result_diagram.diagram_points_count; ++i) {
        std::size_t j = i << 1;

        int val = static_cast<int>(result_diagram.diagram_points[j].y);

        result_diagram.diagram_points[j].y = result_diagram.diagram_points[j + 1].y = static_cast<float>(val * sign);

        sign = sign_coefs[val & 1] * sign;
    }

    phys_encode_result encode_result;
    encode_result.result_diagram = result_diagram;
    encode_result.bits_count = bitmsg.bits_count;

    return encode_result;
}

phys_encode_result phys_encode_m2(bit_message bitmsg) {
    discrete_diagram result_diagram;
    result_diagram.diagram_points_count = bitmsg.bits_count << 1;
    int level_by_bit[2][2] = { {-1, 1}, {1, -1} };

    for (std::size_t i = 0; i < bitmsg.bits_count; ++i) {
        int bitval = bitmsg.msg_bits[i];
        std::size_t index = i << 1;

        result_diagram.diagram_points[index] = { static_cast<float>(i + 1) - 0.5f, static_cast<float>(level_by_bit[bitval][0]) };
        result_diagram.diagram_points[index + 1] = { static_cast<float>(i + 1), static_cast<float>(level_by_bit[bitval][1]) };
    }

    phys_encode_result encode_result;
    encode_result.result_diagram = result_diagram;
    encode_result.bits_count = bitmsg.bits_count;

    return encode_result;
}

/* Decoders */

static inline float signrandf(void) {
    return rand() % 4096 / 2048.0f - 1.0f;
}

// static inline std::ptrdiff_t get_point_index_near(fourier_diagram diagram, float x_near, float step) {
//     float i_float = x_near / step;
//     std::size_t i1 = floor(i_float);
//     std::size_t i2 = ceil(i_float);
//
//     // std::cout << "NEAR: " << i1 << " " << i2 << " " << x_near << " " << i1 << " " << i2 << std::endl;
//
//     if (i2 >= diagram.diagram_points_count)
//         return i1 < diagram.diagram_points_count ? i1 : diagram.diagram_points_count - 1;
//     else
//         return x_near - diagram.diagram_points[i1].x < diagram.diagram_points[i2].x - x_near ? i1 : i2;
// }

// static inline diagram_point get_point_near(fourier_diagram diagram, float x_near, float step) {
//     return diagram.diagram_points[get_point_index_near(diagram, x_near, step)];
// }

// static int signf(float x) {
//     return x > 0 ? 1 : -1;
// }

bit_message phys_decode_nrz(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings) {
    static_cast<void>(diagram);

    bit_message bitmsg;
    bitmsg.bits_count = bits_count;

    for (std::size_t i = 0; i < bits_count; ++i) {
        float x = i + 0.5f + settings.desync * signrandf() * 0.5f;
        float y = FourierEngine::fourierSeriesFunc(x) + FourierEngine::noiseFunc(x);

        if (y > settings.voltage)
            bitmsg.msg_bits[i] = 1;
        else
            bitmsg.msg_bits[i] = 0;
    }

    return bitmsg;
}

// bit_message phys_decode_nrz(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings) {
//     bit_message bitmsg;
//     bitmsg.bits_count = bits_count;
//
//     float min = 0.0f;
//     float max = static_cast<float>(bits_count);
//     float step = max / diagram.diagram_points_count;
//
//     for (std::ptrdiff_t i = 0; i < bits_count; ++i) {
//         float xmid = i + 0.5f;
//         float x = i + 0.5f + settings.desync * signrandf() * 0.25f;
//         float y = FourierEngine::fourierSeriesFunc(x) + FourierEngine::noiseFunc(x);
//
//         diagram_point min_points[] = {
//             { 0.0f, 100.0f },
//             { 0.0f, 100.0f }
//         };
//
//         std::ptrdiff_t start = get_point_index_near(diagram, i, step);
//         std::ptrdiff_t end = get_point_index_near(diagram, i + 1, step);
//
//         for (std::ptrdiff_t j = start + 1; j < end - 1; ++j) {
//             diagram_point p = diagram.diagram_points[j];
//
//             if (fabs(p.y) < fabs(min_points[0].y) && p.x < xmid)
//                 min_points[0] = p;
//             else if (fabs(p.y) < fabs(min_points[1].y) && p.x > xmid)
//                 min_points[1] = p;
//         }
//
//         diagram_point nodes[] = {
//             diagram.diagram_points[start],
//             get_point_near(diagram, (diagram.diagram_points[start].x + min_points[0].x) * 0.5f, step),
//             min_points[0],
//             get_point_near(diagram, (min_points[0].x + x) * 0.5f, step),
//             { x, y },
//             get_point_near(diagram, (min_points[1].x + x) * 0.5f, step),
//             min_points[1],
//             get_point_near(diagram, (diagram.diagram_points[end].x + min_points[1].x) * 0.5f, step),
//             diagram.diagram_points[end]
//         };
//
//         float sq = 0;
//
//         for (std::ptrdiff_t j = 0; j < 7; ++j) {
//             diagram_point p1 = nodes[j];
//             diagram_point p2 = nodes[j + 1];
//
//             sq += (p1.y + p2.y) * 0.5f * (p2.x - p1.x);
//         }
//
//         float v = sq + (1 - sq) * y;
//
//         bitmsg.msg_bits[i] = v > 0 ? 1 : 0;
//     }
//
//     return bitmsg;
// }

bit_message phys_decode_rz(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings) {
    static_cast<void>(diagram);

    bit_message bitmsg;
    bitmsg.bits_count = bits_count;

    // float min = 0.0f;
    // float max = static_cast<float>(bits_count);
    // float step = max / diagram.diagram_points_count;

    for (std::size_t i = 0; i < bits_count; ++i) {
        float x1 = i + 0.25f + settings.desync * signrandf() * 0.25f;
        float y1 = FourierEngine::fourierSeriesFunc(x1) + FourierEngine::noiseFunc(x1);

        float x2 = i + 0.75f + settings.desync * signrandf() * 0.25f;
        float y2 = FourierEngine::fourierSeriesFunc(x2) + FourierEngine::noiseFunc(x2);

        // float x3 = i + 1.0f + settings.desync * signrandf() * 0.25f;
        // float y3 = FourierEngine::fourierSeriesFunc(x3) + FourierEngine::noiseFunc(x3);

        int sign1 = y1 < 0 ? -1 : 1;
        int sign2 = y2 < 0 ? -1 : 1;
        // int sign3 = y3 < 0 ? -1 : 1;

        // sign1 != sign2 || fabs(y2) <= 0.2f

        if (sign1 != sign2 || fabs(y1) > fabs(y2)) {
            if (fabs(y1) < settings.voltage || y1 < -settings.voltage)
                bitmsg.msg_bits[i] = 0;
            else if (y1 > settings.voltage)
                bitmsg.msg_bits[i] = 1;
        }
        else
            bitmsg.msg_bits[i] = -1;

        // if (fabs(y1) > fabs(y2) || sign1 != sign2) {
        //     if (fabs(y1) < settings.voltage)
        //         bitmsg.msg_bits[i] = 0;
        //     else if (y1 > settings.voltage)
        //         bitmsg.msg_bits[i] = 1;
        //     else if (y1 < -settings.voltage)
        //         bitmsg.msg_bits[i] = 0;
        // }
        // else
        //     bitmsg.msg_bits[i] = -1;

        // if (fabs(point2.y) <= 0.2 || fabs(point3.y) <= 0.3 || sign1 != sign2) {
        //     if (fabs(point1.y < settings.voltage))
        //         bitmsg.msg_bits[i] = 0;
        //     else if (point1.y > settings.voltage)
        //         bitmsg.msg_bits[i] = 1;
        //     else if (point1.y < -settings.voltage)
        //         bitmsg.msg_bits[i] = 0;
        // }
        // else
        //     bitmsg.msg_bits[i] = -1;
    }

    return bitmsg;
}

bit_message phys_decode_ami(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings) {
    static_cast<void>(diagram);

    bit_message bitmsg;
    bitmsg.bits_count = bits_count;

    // float min = 0.0f;
    // float max = static_cast<float>(bits_count);
    // float step = max / diagram.diagram_points_count;

    int sign = 1;

    for (std::size_t i = 0; i < bits_count; ++i) {
        float x = i + 0.5f + settings.desync * signrandf() * 0.5f;
        float y = FourierEngine::fourierSeriesFunc(x) + FourierEngine::noiseFunc(x);

        if (fabs(y) < settings.voltage || fabs(y) <= 0.45f)
            bitmsg.msg_bits[i] = 0;
		else {
			if ((sign > 0 && y > 0) || (sign < 0 && y < 0)) {
                bitmsg.msg_bits[i] = 1;
                sign *= -1;
            }
            else
                bitmsg.msg_bits[i] = -1;
		}
    }

    return bitmsg;
}

bit_message phys_decode_m2(fourier_diagram diagram, std::size_t bits_count, receiver_settings settings) {
    static_cast<void>(diagram);

    bit_message bitmsg;
    bitmsg.bits_count = bits_count;

    // float min = 0.0f;
    // float max = static_cast<float>(bits_count);
    // float step = max / diagram.diagram_points_count;

    for (std::size_t i = 0; i < bits_count; ++i) {
        float x1 = i + 0.25f + settings.desync * signrandf() * 0.25f;
        float y1 = FourierEngine::fourierSeriesFunc(x1) + FourierEngine::noiseFunc(x1);

        float x2 = i + 0.75f + settings.desync * signrandf() * 0.25f;
        float y2 = FourierEngine::fourierSeriesFunc(x2) + FourierEngine::noiseFunc(x2);

        if (y1 > y2)
            bitmsg.msg_bits[i] = 1;
        else if (y1 < y2)
            bitmsg.msg_bits[i] = 0;
        else
            bitmsg.msg_bits[i] = -1;
    }

    return bitmsg;
}

bit_message logic_decode_phys(bit_message bitmsg) {
    return bitmsg;
}

/* based on Demiurg's logic decoder */
bit_message logic_decode_super45(bit_message bitmsg) {
    int table[] = {
		0x1E, 0x09, 0x14, 0x15,
		0x0A, 0x0B, 0x0E, 0x0F,

		0x12, 0x13, 0x16, 0x17,
		0x1A, 0x1B, 0x1C, 0x1D
	};

    bit_message decoded_msg;
    memset(decoded_msg.msg_bits, 0, bitmsg.bits_count * sizeof(int));
    decoded_msg.bits_count = bitmsg.bits_count / 5 * 4;

    std::size_t decoded_msg_length = bitmsg.bits_count / 5;
    std::ptrdiff_t index = 0;

    for (std::size_t i = 0; i < decoded_msg_length; ++i) {
        std::size_t j = i * 5;

        int bit5 = ((bitmsg.msg_bits[j] & 1) << 4) + ((bitmsg.msg_bits[j + 1] & 1) << 3) + ((bitmsg.msg_bits[j + 2] & 1) << 2) + ((bitmsg.msg_bits[j + 3] & 1) << 1) + (bitmsg.msg_bits[j + 4] & 1);

        for (j = 0; j < 16 && table[j] != bit5; ++j);

        int val = static_cast<int>(j < 16 ? j : 0);

        decoded_msg.msg_bits[index] = (val >> 3) & 1;
        decoded_msg.msg_bits[index + 1] = (val >> 2) & 1;
        decoded_msg.msg_bits[index + 2] = (val >> 1) & 1;
        decoded_msg.msg_bits[index + 3] = val & 1;

        index += 4;
    }

    return decoded_msg;
}

bit_message logic_decode_scrambling(bit_message bitmsg) {
    bit_message decoded_msg;
    memset(decoded_msg.msg_bits, 0, bitmsg.bits_count * sizeof(int));
    decoded_msg.bits_count = bitmsg.bits_count;

    for (std::size_t i = 0; i < decoded_msg.bits_count; ++i) {
        int a = bitmsg.msg_bits[i];
        int b = i >= 3 ? bitmsg.msg_bits[i - 3] : 0;
        int c = i >= 5 ? bitmsg.msg_bits[i - 5] : 0;

        decoded_msg.msg_bits[i] = (a + b + c) & 1;
    }

    return decoded_msg;
}

message parse_decoded_message(bit_message bitmsg) {
    message recv_msg;
    memset(recv_msg.msg, 0, MESSAGE_MAX_LENGTH + 1);

    std::size_t bytes_count = bitmsg.bits_count >> 3;
    recv_msg.empty = false;
    recv_msg.length = bytes_count;

    for (std::size_t i = 0; i < bytes_count; ++i) {
        std::ptrdiff_t j = i << 3;

        for (std::size_t k = 0; k < 8 && j + k < bitmsg.bits_count; ++k)
            recv_msg.msg[i] = (recv_msg.msg[i] * 2 + (bitmsg.msg_bits[j + k] & 1)) & 0xFF;
    }

    return recv_msg;
}
