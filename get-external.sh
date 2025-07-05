mkdir -p external
cd external
git clone  https://github.com/curl/curl.git
git clone  https://github.com/ocornut/imgui.git
cd imgui
git checkout 69e1fb50cacbde1c2c585ae59898e68c1818d9b7
cd ..
git clone  https://github.com/nlohmann/json.git
git clone  https://github.com/openssl/openssl.git openssl-src
