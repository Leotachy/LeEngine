
cd ..
conan install . -sbuild_type=Debug -sarch=x86_64 -of=conan/debug --build=missing
conan install . -sbuild_type=Release -sarch=x86_64 -of=conan/release --build=missing
PAUSE