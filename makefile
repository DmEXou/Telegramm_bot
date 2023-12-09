all:
	clang++ telegram.cpp -o bot_e -std=c++17 -lcurl -lTgBot -lboost_system -lssl -lcrypto -lpthread
