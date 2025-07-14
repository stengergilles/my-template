mkdir -p external
cd external

# Clone curl
git clone https://github.com/curl/curl.git

# Clone imgui and checkout specific commit
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout 69e1fb50cacbde1c2c585ae59898e68c1818d9b7
cd ..

# Clone nlohmann/json
git clone https://github.com/nlohmann/json.git

# Clone openssl
git clone https://github.com/openssl/openssl.git openssl-src

# Create fonts directory and download DroidSans.ttf
mkdir -p fonts
echo "Downloading DroidSans.ttf..."
curl -L "http://fonts.gstatic.com/s/roboto/v15/zN7GBFwfMP4uA6AR0HCoLQ.ttf" -o "fonts/DroidSans.ttf"
if [ -f "fonts/DroidSans.ttf" ]; then
    echo "DroidSans.ttf downloaded successfully to external/fonts/"
else
    echo "Failed to download DroidSans.ttf. Please check the URL or download manually."
fi

# Download and extract Font Awesome Free Web (using v6.5.1 as a placeholder)
FA_VERSION="6.5.1"
FA_ZIP="fontawesome-free-${FA_VERSION}-web.zip"
FA_URL="https://github.com/FortAwesome/Font-Awesome/releases/download/${FA_VERSION}/${FA_ZIP}"
FA_EXTRACT_DIR="fontawesome-free-${FA_VERSION}-web"
FA_TARGET_DIR="fontawesome"

echo "Downloading Font Awesome Free v${FA_VERSION} from ${FA_URL}..."
curl -L ${FA_URL} -o ${FA_ZIP}

if [ -f "${FA_ZIP}" ]; then
    echo "Extracting ${FA_ZIP} to ${FA_TARGET_DIR}..."
    mkdir -p ${FA_TARGET_DIR}
    unzip -q ${FA_ZIP} -d ${FA_EXTRACT_DIR}
    mv ${FA_EXTRACT_DIR}/* ${FA_TARGET_DIR}/
    rm -r ${FA_EXTRACT_DIR}
    rm ${FA_ZIP}
    echo "Font Awesome extracted to external/${FA_TARGET_DIR}"
else
    echo "Failed to download Font Awesome. Please check the URL or download manually."
fi

git clone https://github.com/juliettef/IconFontCppHeaders.git

cd ..
