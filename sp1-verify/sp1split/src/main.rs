use sp1_sdk::{HashableKey, ProverClient, SP1ProofWithPublicValues, SP1Stdin};
use std::env;
use std::fs;
use std::io::Write;

// usage: split-proof $ELF $PROOF

fn write_file(name: &str, bytes: Vec<u8>) {
    let mut file = fs::OpenOptions::new()
        .create(true)
        .write(true)
        .open(name)
        .unwrap();

    file.write_all(&bytes).unwrap();
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let client = ProverClient::new();
    let elf = std::fs::read(&args[1]).unwrap();
    let (_pk, vk) = client.setup(&elf);
    let deserialized_proof =
        SP1ProofWithPublicValues::load(&args[2]).expect("loading proof failed");
    let proof = deserialized_proof.raw_with_checksum();
    let public_inputs = deserialized_proof.public_values.to_vec();
    let vk_hash = vk.bytes32();

    write_file(&format!("{}.proof", &args[2]), proof);
    write_file(&format!("{}.pub", &args[2]), public_inputs);
    write_file(&format!("{}.vk", &args[2]), vk_hash.into());
}
