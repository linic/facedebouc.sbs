# Cross Compiling Windows Executables with Rust on Linux

## Experience 1: Installing Rust

**Result:** Rust is installed.

**Explanation:**
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Experience 2: First Linux build — fails due to missing OpenSSL

**Result:** Build fails with `error: failed to run custom build command for 'openssl-sys'`.

**Explanation:** `reqwest` v0.11 enables `native-tls` (OpenSSL) by default. `pkg-config` was not installed, so the build system could not locate OpenSSL:
```
Could not find directory of OpenSSL installation
The pkg-config command could not be found.
```
Fix: disable the default `native-tls` feature and use `rustls-tls` instead in `Cargo.toml`:
```toml
reqwest = { version = "0.11", default-features = false, features = ["json", "rustls-tls"] }
```

## Experience 3: Linux build after switching to `rustls-tls`

**Result:** Linux ELF 64-bit executable produced (6.4MB).

**Explanation:**
```bash
source ~/.cargo/env && cd /home/utilisateur/monprog && cargo build --release
```
Note: `CARGO_TARGET_DIR` was set to a non-standard path by the build environment:
```
CARGO_TARGET_DIR=/tmp/cursor-sandbox-cache/.../cargo-target
```
The executable was confirmed at:
```
.../cargo-target/release/monprog: ELF 64-bit LSB pie executable, x86-64
```

## Experience 4: Installing the MinGW cross-compiler

**Result:** `gcc-mingw-w64-x86-64` is installed.

**Explanation:**
```bash
sudo apt-get install -y gcc-mingw-w64-x86-64
```

## Experience 5: Cross-compiling for Windows

**Result:** Windows PE32+ executable produced (9.3MB).

**Explanation:**
```bash
source ~/.cargo/env && cd /home/utilisateur/monprog && cargo build --release --target x86_64-pc-windows-gnu
```
The executable was confirmed at:
```
.../cargo-target/x86_64-pc-windows-gnu/release/monprog.exe: PE32+ executable (console) x86-64, for MS Windows
```

## Summary: build commands

```bash
# Linux
cargo build --release

# Windows .exe
cargo build --release --target x86_64-pc-windows-gnu
```

## Diagnostic commands

```bash
# Verify the cross-compiler is installed
which x86_64-w64-mingw32-gcc && x86_64-w64-mingw32-gcc --version | head -1

# List installed Rust targets
source ~/.cargo/env && rustup target list --installed

# Find output executables
find /home/utilisateur/monprog/target -name "monprog*" -type f ! -name "*.d" ! -name "*.rlib" 2>/dev/null | head -10

# Check CARGO_TARGET_DIR
echo "CARGO_TARGET_DIR=${CARGO_TARGET_DIR:-unset}"

# Verify executable type
file target/release/monprog
file target/x86_64-pc-windows-gnu/release/monprog.exe
```
