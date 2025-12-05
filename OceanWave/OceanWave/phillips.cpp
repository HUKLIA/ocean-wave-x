#include"phillips.h"

float urand()
{
	return rand() / (float)RAND_MAX;
}
// Generates Gaussian random number with mean 0 and standard deviation 1.
float gauss()
{
	float u1 = urand();
	float u2 = urand();
	if (u1 < 1e-6f)
	{
		u1 = 1e-6f;
	}
	return sqrtf(-2 * logf(u1)) * cosf(2 * PI_F * u2);
}

float phillips(float Kx, float Ky, float Vdir, float V, float dir_depend, float TA)
{

	float k_squared = Kx * Kx + Ky * Ky;
	if (k_squared == 0.0f)
	{
		return 0.0f;
	}
	// largest possible wave from constant wind of velocity v
	float L = V * V / g;
	float k_x = Kx / sqrtf(k_squared);
	float k_y = Ky / sqrtf(k_squared);
	float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);
	float phillips = TA * expf(-1.0f / (k_squared * L * L)) / (k_squared * k_squared) *
		w_dot_k * w_dot_k;
	// filter out waves moving opposite to wind
	if (w_dot_k < 0.0f)
	{
		phillips *= dir_depend;
	}
	// damp out waves with very small length w << l
	//float w = L / 10000;
	//phillips *= expf(-k_squared * w * w);
	return phillips;
}
// Generate base heightfield in frequency space
void generate_h0(std::complex<float>* h0, float TA)
{
	for (unsigned int y = 0; y <= meshSize; y++)
	{
		for (unsigned int x = 0; x <= meshSize; x++)
		{
			float kx = (-(int)meshSize / 2.0f + x) * (2.0f * PI_F / patchSize);
			float ky = (-(int)meshSize / 2.0f + y) * (2.0f * PI_F / patchSize);
			float P = sqrtf(phillips(kx, ky, windDir, windSpeed, dirDepend, TA));
			if (kx == 0.0f && ky == 0.0f)
			{
				P = 0.0f;
			}
			float Er = gauss();
			float Ei = gauss();
			float h0_re = Er * P * SQRT_HALF_F;
			float h0_im = Ei * P * SQRT_HALF_F;
			int i = y * spectrumW + x;
			h0[i] = std::complex<float>(h0_re, h0_im);
		}
	}
}

complex<float> htilde0(int n_prime, int m_prime, float TA)
{
	std::complex<float> r(gauss(), gauss());
	float kx = (-(int)meshSize / 2.0f + n_prime) * (2.0f * PI_F / patchSize);
	float ky = (-(int)meshSize / 2.0f + m_prime) * (2.0f * PI_F / patchSize);
	return r * sqrtf(phillips(kx, ky, windDir, windSpeed, dirDepend, TA));
}

complex<float> complex_exp(float arg)
{
	return std::complex<float>(cosf(arg), sinf(arg));
}
// Generate a Spectrum texture.
void GenerateTextureSpectrumData(float* imageR, float* imageG, float* imageB, float* imageRN, float* imageGIN, float t,
	float* htdx1, float* htdy1, float* htsx1, float* htsy1,
	float* htdx2, float* htdy2, float* htsx2, float* htsy2, float TA)
{
	const UINT rowPitch = WIDTH * 1;
	const UINT textureSize = rowPitch * HEIGHT;
	const int N_PLUS_1 = meshSize + 1;
	std::complex<float>* h0 = new std::complex<float>[N_PLUS_1 * N_PLUS_1];
	std::complex<float>* h0mk_conj = new std::complex<float>[N_PLUS_1 * N_PLUS_1];
	for (int m_prime = 0; m_prime < N_PLUS_1; ++m_prime)
	{
		for (int n_prime = 0; n_prime < N_PLUS_1; ++n_prime)
		{
			const int index = m_prime * N_PLUS_1 + n_prime;
			h0[index] = htilde0(n_prime, m_prime, TA);
			h0mk_conj[index] = std::conj(htilde0(-n_prime, -m_prime, TA));
		}
	}

	// generate ht (htilde)
	for (int m_prime = 0; m_prime < int(meshSize); ++m_prime)
	{
		for (int n_prime = 0; n_prime < int(meshSize); ++n_prime)
		{
			const int index = m_prime * N_PLUS_1 + n_prime;
			const int n = m_prime * rowPitch + n_prime * 1;
			complex<float> h0_k = h0[index];
			complex<float> h0_mk = h0mk_conj[index];
			// dispersion
			float kx = (-(int)meshSize / 2.0f + n_prime) * (2.0f * PI_F / patchSize);
			float ky = (-(int)meshSize / 2.0f + m_prime) * (2.0f * PI_F / patchSize);
			float k_len = sqrtf(kx * kx + ky * ky);
			float w = sqrtf(g * k_len);
			complex<float> ht = h0_k * complex_exp(w * t) + h0_mk * complex_exp(-
				w * t);
			// fabs() used for display the image only
			imageR[n] = fabs(ht.real());//red
			imageG[n] = (ht.imag());//green
			imageB[n] = 0;//blue
			//imageTR[y * WIDTH + x] = ht_RE[y * N + x] * 0.5 + 0.5f;//red
			//imageTG[y * WIDTH + x] = ht_IM[y * N + x] * 0.5 + 0.5f;//green
			//imageTB[y * WIDTH + x] = 0;//blue
			imageRN[n] = ht.real();
			imageGIN[n] = ht.imag();

			complex<float> htdx(0.0f, 0.0f);
			complex<float> htdy(0.0f, 0.0f);

			complex<float> htslopex(0.0f, 0.0f);
			complex<float> htslopey(0.0f, 0.0f);
			complex<float> htsx(0, kx);
			complex<float> htsy(0, ky);

			htslopex = ht * htsx;
			htslopey = ht * htsy;

			if (k_len >= 0.000001f) {
				complex<float> dxk(0, -kx / k_len);
				complex<float> dyk(0, -ky / k_len);
				htdx = ht * dxk;
				htdy = ht * dyk;
			}

			htdx1[n] = htdx.real();
			htdy1[n] = htdy.real();
			htsx1[n] = htsx.real();
			htsy1[n] = htsy.real();
			htdx2[n] = htdx.imag();
			htdy2[n] = htdx.imag();
			htsx2[n] = htdx.imag();
			htsy2[n] = htdx.imag();
		}
	}

}
