#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>

#include <libmorton/morton2D.h>
#include "morton_LUT_generators.h"

using namespace libmorton;

template<typename T>
void print_hex_array(const char* name, const T* arr, size_t n) {
    unsigned int width = (sizeof(T) <= 4) ? 8 : 16;
    std::cout << "static const ";
    if (sizeof(T) <= 4) std::cout << "uint_fast32_t "; else std::cout << "uint_fast64_t ";
    std::cout << name << "[" << n << "] = {\n";
    for (size_t i = 0; i < n; ++i) {
        if (i % 8 == 0) std::cout << "\t";
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(width) << (unsigned long long)arr[i];
        if (i + 1 < n) std::cout << ", ";
        if ((i + 1) % 8 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "\n};\n\n";
}

void print_decode_array(const char* name, const uint_fast8_t* arr, size_t n) {
    std::cout << "static const uint_fast8_t " << name << "[" << n << "] = {\n";
    for (size_t i = 0; i < n; ++i) {
        if (i % 16 == 0) std::cout << "\t";
        std::cout << (unsigned int)arr[i];
        if (i + 1 < n) std::cout << ", ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    std::cout << "\n};\n\n";
}

int main() {
    const size_t bits = 8; // 256 entries
    const size_t total = 1 << bits;

    // 16-bit encode tables used in the project are uint_fast16_t for 2D
    uint_fast32_t* x_enc = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    uint_fast32_t* y_enc = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    for (uint_fast32_t i = 0; i < total; ++i) {
        x_enc[i] = (uint_fast32_t)m2D_e_magicbits<uint_fast32_t, uint_fast32_t>(i, 0);
        y_enc[i] = (uint_fast32_t)m2D_e_magicbits<uint_fast32_t, uint_fast32_t>(0, i);
    }

    // decode tables for 2D (65536 entries if using 16-bit index? but project uses 256->512 pattern)
    // For 2D decode the test uses 16-entry? We'll generate 65536 decode entries would be huge.
    // The project's 2D decode LUT in other files uses 256 entries for shifted lookups (uint_fast8_t[256])
    // We'll generate decode LUT of size 256 for x and y using morton2D decoding helper.
    const size_t dtotal = 256;
    uint_fast8_t* dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    for (size_t i = 0; i < dtotal; ++i) {
        uint_fast32_t xx = 0, yy = 0;
        m2D_d_for((uint_fast32_t)i, xx, yy);
        dx[i] = (uint_fast8_t)xx;
        dy[i] = (uint_fast8_t)yy;
    }

    std::cout << "// 2D Encode LUTs (256 entries)" << std::endl;
    print_hex_array("Morton2D_encode_x_256", x_enc, total);
    print_hex_array("Morton2D_encode_y_256", y_enc, total);

    std::cout << "// 2D Decode LUTs (256 entries)" << std::endl;
    print_decode_array("Morton2D_decode_x_256", dx, dtotal);
    print_decode_array("Morton2D_decode_y_256", dy, dtotal);

    return 0;
}
