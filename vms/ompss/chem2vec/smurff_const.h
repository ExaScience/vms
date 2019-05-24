#pragma once
const int num_proteins = 114;
const int num_features = 469;
const int num_latent = 16;
const int num_samples = 32;
const int log_num_samples = 5;

const int mu_shift = 7;
const int mu_wl = 8;
const int mu_iwl = mu_wl - mu_shift;
const int U_shift = 12;
const int U_wl = 16;
const int U_iwl = U_wl - U_shift;
const int B_shift = 16;
const int B_wl = 16;
const int B_iwl = B_wl - B_shift;

const int F_shift = 6;
const int F_wl = 16;
const int F_iwl = F_wl - F_shift;
const int P_shift = 3;
const int P_wl = 8;
const int P_iwl = P_wl - P_shift;
