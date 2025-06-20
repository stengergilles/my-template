git clone https://github.com/openssl/openssl.git
cd openssl
./Configure android-arm64
make -j$(nproc)
# Find output in ./libcrypto.a ./libssl.a and ./include
