use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        println!("usage: sp1-verify [-plonk,-groth16] [proof] [pub] [vk]");
        return;
    }
    let proof = std::fs::read(&args[2]).unwrap();
    let public = std::fs::read(&args[3]).unwrap();
    let vkey_bytes = std::fs::read(&args[4]).unwrap();
    let vkey = std::str::from_utf8(&vkey_bytes).unwrap();
    println!("verifying...");
    if args[1] == "-plonk" {
        let result =
            verifier::PlonkVerifier::verify(&proof, &public, vkey, &verifier::PLONK_VK_BYTES);
        println!("got: {:?}", result);
    } else if args[1] == "-groth16" {
        let result =
            verifier::Groth16Verifier::verify(&proof, &public, vkey, &verifier::GROTH16_VK_BYTES);
        println!("got: {:?}", result);
    } else {
        println!("please provide -groth16 or -plonk as the first argument");
    }
}
