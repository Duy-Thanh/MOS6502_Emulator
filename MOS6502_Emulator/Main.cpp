#include "CPU.h"

#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

#include "RC5.h"

namespace fs = std::filesystem; // C++17 filesystem

std::vector<unsigned char> EncryptRC5(const std::string& data, const std::vector<unsigned char>& key) {
	RC5Simple rc5;

	// Create mutable copies since RC5Simple methods require non-const references
	std::vector<unsigned char> mutableKey = key;

	// Convert string data to vector of unsigned char
	std::vector<unsigned char> inputData(data.begin(), data.end());

	// Create vector for encrypted output
	std::vector<unsigned char> encryptedData;

	// Set the encryption key
	rc5.RC5_SetKey(mutableKey);

	// Encrypt the data
	rc5.RC5_Encrypt(inputData, encryptedData);

	// Check if there was an error during encryption
	if (rc5.RC5_GetErrorCode() != 0) {
		return std::vector<unsigned char>();
	}

	return encryptedData;
}

// Convert encrypted vector to unsigned int
unsigned int EncryptedVectorToUInt(const std::vector<unsigned char>& encrypted) {
	if (encrypted.empty()) {
		return 0;
	}

	unsigned int result = 0;
	// Take up to 4 bytes (size of unsigned int)
	for (size_t i = 0; i < std::min(encrypted.size(), size_t(4)); i++) {
		result |= (static_cast<unsigned int>(encrypted[i]) << (i * 8));
	}
	return result;
}

// Convert unsigned int back to vector for decryption
std::vector<unsigned char> UIntToEncryptedVector(unsigned int value) {
	std::vector<unsigned char> result;
	result.reserve(4);

	for (int i = 0; i < 4; i++) {
		result.push_back(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
	}

	return result;
}

// Modified encryption function that returns unsigned int
unsigned int EncryptRC5ToUInt(const std::string& data, const std::vector<unsigned char>& key) {
	std::vector<unsigned char> encrypted = EncryptRC5(data, key);
	return EncryptedVectorToUInt(encrypted);
}

// Convert unsigned int to hex string
std::string UIntToHexString(unsigned int value) {
	std::stringstream ss;
	ss << "0x" << std::hex << std::uppercase << value;
	return ss.str();
}

// Convert hex string to unsigned int
unsigned int HexStringToUInt(const std::string& hexStr) {
	// Remove 0x prefix if present
	std::string cleanHex = hexStr;
	if (hexStr.substr(0, 2) == "0x" || hexStr.substr(0, 2) == "0X") {
		cleanHex = hexStr.substr(2);
	}

	// Convert hex string to unsigned int
	unsigned int value;
	std::stringstream ss;
	ss << std::hex << cleanHex;
	ss >> value;
	return value;
}

int main() {
	std::cout << "MAIN: Machine starting..." << std::endl;
	std::cout << std::endl << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));

	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n";
	std::cout << "                            MOS6502 EMULATOR                                    " << std::endl;
	std::cout << "\n\n";
	std::cout << "                     MOS6502 COMPLETE SYSTEM EMULATOR                           " << std::endl;
	std::cout << "\n\n";
	std::cout << "              COPYRIGHT (C) 2024 NEKKOCHAN. ALL RIGHT RESERVED.                 " << std::endl;
	std::cout << "\n\n";
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n";

	std::vector<unsigned char> key = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F
	};

	std::string originalText = "TU9TNjVPMl9FTVVfV0lOX1g2NA==";

	// Encrypt to unsigned int
	unsigned int encryptedInt = EncryptRC5ToUInt(originalText, key);

	// Print as hexadecimal
	//std::cout << "Encrypted (hex): 0x" << std::hex << encryptedInt << std::dec << std::endl;

	//if (!encrypted.empty()) {
	//	//// Decrypt
	//	//std::string decrypted = DecryptRC5(encrypted, key);

	//	//// Verify
	//	//if (decrypted == originalText) {
	//	//	// Success!
	//	//}
	//}
	std::string hexString = UIntToHexString(encryptedInt);
	fs::path exePath = fs::current_path() / "CrashHandler.exe";

#ifdef _WIN32
	std::string param = "\"" + exePath.string() + "\" -application_identity " + hexString;
#else
	std::string param = "./CrashHandler -application_identity " + hexString;
#endif

	// Run twice to ensure the message is displayed
	while (1) {
		system(param.c_str());
	}

	MOS6502::MOS6502_CPU cpu;

	std::cout << "MAIN: cpu_var_addr=0x" << std::hex << (uint32_t) & cpu << std::endl;

	bool running = true;

	std::cout << "MAIN: State is set to RUNNING" << std::endl;

	while (running) {
		cpu.MOS6502_CPU_Clock();

		if (cpu.MOS6502_CPU_Complete()) {
			// TODO
		}
	}

	return 0;
}