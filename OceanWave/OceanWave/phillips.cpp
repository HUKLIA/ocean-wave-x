#include "phillips.h"
#include <cmath>
#include <cstdlib>

float urand() {
    return rand() / static_cast<float>(RAND_MAX);
}

// Generates Gaussian random number with mean 0 and standard deviation 1
float gauss() {
    float u1 = urand();
    float u2 = urand();
    if (u1 < 1e-6f) {
        u1 = 1e-6f;
    }
    return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * PI_F * u2);
}

float phillips(float Kx, float Ky, const SimParams& params) {
    float k_squared = Kx * Kx + Ky * Ky;
    if (k_squared == 0.0f) {
        return 0.0f;
    }

    // Largest possible wave from constant wind of velocity V
    float L = params.windSpeed * params.windSpeed / GRAVITY;

    float k_len = sqrtf(k_squared);
    float k_x = Kx / k_len;
    float k_y = Ky / k_len;

    float w_dot_k = k_x * cosf(params.windDir) + k_y * sinf(params.windDir);

    float ph = params.amplitude * expf(-1.0f / (k_squared * L * L)) /
        (k_squared * k_squared) * w_dot_k * w_dot_k;

    // Filter out waves moving opposite to wind
    if (w_dot_k < 0.0f) {
        ph *= params.dirDepend;
    }

    return ph;
}

std::complex<float> htilde0(int n_prime, int m_prime, const SimParams& params) {
    std::complex<float> r(gauss(), gauss());
    float kx = (-(static_cast<int>(GRID_SIZE) / 2.0f) + n_prime) * (2.0f * PI_F / params.patchSize);
    float ky = (-(static_cast<int>(GRID_SIZE) / 2.0f) + m_prime) * (2.0f * PI_F / params.patchSize);
    return r * sqrtf(phillips(kx, ky, params));
}

std::complex<float> complex_exp(float arg) {
    return std::complex<float>(cosf(arg), sinf(arg));
}

void generateSpectrumData(
    float* heightReal, float* heightImag,
    float* dispX_real, float* dispX_imag,
    float* dispY_real, float* dispY_imag,
    float* slopeX_real, float* slopeX_imag,
    float* slopeY_real, float* slopeY_imag,
    float t, const SimParams& params)
{
    const unsigned int rowPitch = WIDTH;
    const int N_PLUS_1 = GRID_SIZE + 1;

    // Allocate h0 arrays
    std::complex<float>* h0 = new std::complex<float>[N_PLUS_1 * N_PLUS_1];
    std::complex<float>* h0mk_conj = new std::complex<float>[N_PLUS_1 * N_PLUS_1];

    // Generate initial spectrum
    for (int m_prime = 0; m_prime < N_PLUS_1; ++m_prime) {
        for (int n_prime = 0; n_prime < N_PLUS_1; ++n_prime) {
            const int index = m_prime * N_PLUS_1 + n_prime;
            h0[index] = htilde0(n_prime, m_prime, params);
            h0mk_conj[index] = std::conj(htilde0(-n_prime, -m_prime, params));
        }
    }

    // Generate h-tilde for current time
    for (int m_prime = 0; m_prime < static_cast<int>(GRID_SIZE); ++m_prime) {
        for (int n_prime = 0; n_prime < static_cast<int>(GRID_SIZE); ++n_prime) {
            const int index = m_prime * N_PLUS_1 + n_prime;
            const int n = m_prime * rowPitch + n_prime;

            std::complex<float> h0_k = h0[index];
            std::complex<float> h0_mk = h0mk_conj[index];

            // Dispersion relation
            float kx = (-(static_cast<int>(GRID_SIZE) / 2.0f) + n_prime) * (2.0f * PI_F / params.patchSize);
            float ky = (-(static_cast<int>(GRID_SIZE) / 2.0f) + m_prime) * (2.0f * PI_F / params.patchSize);
            float k_len = sqrtf(kx * kx + ky * ky);
            float omega = sqrtf(GRAVITY * k_len);

            // h-tilde at time t
            std::complex<float> ht = h0_k * complex_exp(omega * t) + h0_mk * complex_exp(-omega * t);

            heightReal[n] = ht.real();
            heightImag[n] = ht.imag();

            // Displacement vectors
            std::complex<float> htdx(0.0f, 0.0f);
            std::complex<float> htdy(0.0f, 0.0f);

            if (k_len >= 0.000001f) {
                std::complex<float> dxk(0.0f, -kx / k_len);
                std::complex<float> dyk(0.0f, -ky / k_len);
                htdx = ht * dxk;
                htdy = ht * dyk;
            }

            dispX_real[n] = htdx.real();
            dispX_imag[n] = htdx.imag();
            dispY_real[n] = htdy.real();
            dispY_imag[n] = htdy.imag();

            // Slope vectors
            std::complex<float> htsx = ht * std::complex<float>(0.0f, kx);
            std::complex<float> htsy = ht * std::complex<float>(0.0f, ky);

            slopeX_real[n] = htsx.real();
            slopeX_imag[n] = htsx.imag();
            slopeY_real[n] = htsy.real();
            slopeY_imag[n] = htsy.imag();
        }
    }

    delete[] h0;
    delete[] h0mk_conj;
}