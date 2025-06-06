//! This crate provides verifiers for SP1 Groth16 and Plonk BN254 proofs in a no-std environment.
//! It is patched for efficient verification within the SP1 ZKVM context.

#![cfg_attr(not(feature = "std"), no_std)]
extern crate alloc;

/// The PLONK verifying key for this SP1 version.
pub const PLONK_VK_BYTES: &'static [u8] = include_bytes!("../bn254-vk/plonk_vk.bin");

/// The Groth16 verifying key for this SP1 version.
pub const GROTH16_VK_BYTES: &'static [u8] = include_bytes!("../bn254-vk/groth16_vk.bin");

mod constants;
mod converter;
mod error;
mod groth16;
mod utils;

pub use groth16::Groth16Verifier;
pub use utils::*;

mod plonk;
pub use plonk::PlonkVerifier;

#[cfg(test)]
mod tests;
