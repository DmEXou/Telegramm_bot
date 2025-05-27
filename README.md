# Telegramm_bot
vcpkg install tgbot-cpp:x64-windows

g++ curl_try.cpp -o TG_bot -std=c++17 -lTgBot -lboost_system -lssl -lcrypto -lpthread -lcurl

clang++ telegram.cpp -o bot_e -std=c++20 -lcurl -lTgBot -lboost_system -lssl -lcrypto -lpthread
