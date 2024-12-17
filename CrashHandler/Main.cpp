#include <iostream>
#include <ctime>
#include <sstream>
#include <thread>
#include "RC5.h"

#define PSAPI_VERSION 1
#define NOMINMAX

#include <Windows.h>
#include <Psapi.h>
#include <DbgHelp.h>
#include <ShObjIdl.h>

const GUID CLSID_TaskbarList = { 0x56FDF344, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90} };
const GUID IID_ITaskbarList = { 0x56FDF342, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90} };
const GUID IID_ITaskbarList2 = { 0x602D4995, 0xB13A, 0x429b, {0xA6, 0x6E, 0x19, 0x35, 0xE4, 0x4F, 0x43, 0x17} };
const GUID IID_ITaskList3 = { 0xEA1AFB91, 0x9E28, 0x4B86, {0x90, 0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF} };

void ShowTaskbarIcon(HWND WindowHandle)
{
	ITaskbarList* TaskListPtr;
	CoInitialize(nullptr);
	long Result = !CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_SERVER, IID_ITaskbarList, reinterpret_cast<void**>(&TaskListPtr));
	if (Result) TaskListPtr->AddTab(WindowHandle);
	TaskListPtr->Release();
	CoUninitialize();
}

void HideTaskbarIcon(HWND WindowHandle)
{
	ITaskbarList* TaskListPtr;
	CoInitialize(nullptr);
	long Result = !CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_SERVER, IID_ITaskbarList, reinterpret_cast<void**>(&TaskListPtr));
	if (Result) TaskListPtr->DeleteTab(WindowHandle);
	TaskListPtr->Release();
	CoUninitialize();
}

bool isHex(const char* str) {
	if (str == nullptr || strlen(str) == 0) {
		return false;
	}

	std::string hexStr = std::string(str);

	// Remove 0x or 0X prefix if present
	if (hexStr.compare(0, 2, "0x") == 0 || hexStr.compare(0, 2, "0X") == 0) {
		hexStr = hexStr.substr(2);
	}

	// Check if string is empty after removing prefix
	if (hexStr.empty()) {
		return false;
	}

	// Check if all characters are valid hex digits
	for (char c : hexStr) {
		if (!isxdigit(c)) {
			return false;
		}
	}

	return true;
}

unsigned int getHex(const char* str) {
	// Check for null or empty input
	if (str == nullptr || strlen(str) == 0) {
		return 0xFFFF;
	}

	std::string hexStr = std::string(str);

	// Remove 0x or 0X prefix if present
	if (hexStr.compare(0, 2, "0x") == 0 || hexStr.compare(0, 2, "0X") == 0) {
		hexStr = hexStr.substr(2);
	}

	// Check if string is empty after removing prefix
	if (hexStr.empty()) {
		return 0xFFFF;
	}

	// Verify all characters are valid hex digits
	for (char c : hexStr) {
		if (!isxdigit(c)) {
			return 0xFFFF;
		}
	}

	try {
		// Convert string to unsigned int
		unsigned int value = std::stoul(hexStr, nullptr, 16);
		return value;
	}
	catch (...) {
		// Handle any conversion errors
		return 0xFFFF;
	}
}

std::string DecryptRC5(const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& key) {
	RC5Simple rc5;

	// Create mutable copies of the input vectors since RC5Simple methods require non-const references
	std::vector<unsigned char> mutableKey = key;
	std::vector<unsigned char> mutableEncryptedData = encryptedData;

	// Set the decryption key
	rc5.RC5_SetKey(mutableKey);

	// Create vector for decrypted output
	std::vector<unsigned char> decryptedData;

	// Decrypt the data
	rc5.RC5_Decrypt(mutableEncryptedData, decryptedData);

	// Check if there was an error during decryption
	if (rc5.RC5_GetErrorCode() != 0) {
		return "";
	}

	// Convert decrypted data to string
	// Note: This assumes the decrypted data is a null-terminated string
	return std::string(decryptedData.begin(), decryptedData.end());
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

std::string DecryptRC5FromUInt(unsigned int encryptedInt, const std::vector<unsigned char>& key) {
	// Convert unsigned int back to vector
	std::vector<unsigned char> encryptedVector = UIntToEncryptedVector(encryptedInt);

	// Decrypt using existing function
	return DecryptRC5(encryptedVector, key);
}

HWND CreateInvisibleWindow() {
	// Register window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"TempWindowClass";
	RegisterClassEx(&wc);

	// Create invisible window
	HWND hwnd = CreateWindowEx(
		0,
		L"TempWindowClass",
		L"",
		WS_OVERLAPPED,
		0, 0, 0, 0,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	return hwnd;
}

int MessageBoxWithoutTaskbar(HWND hWnd, LPCWSTR text, LPCWSTR caption, UINT type) {
	HWND owner = CreateInvisibleWindow();
	HideTaskbarIcon(owner);
	int result = MessageBoxW(owner, text, caption, type);
	DestroyWindow(owner);
	return result;
}

std::string GetExceptionName(DWORD code)
{
	switch (code)
	{
	case EXCEPTION_ACCESS_VIOLATION:		 return "Access Violation";
	case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype Misalignment";
	case EXCEPTION_BREAKPOINT:               return "Breakpoint";
	case EXCEPTION_SINGLE_STEP:              return "Single Step";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array Bounds Exceeded";
	case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float Denormal Operand";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float Divide By Zero";
	case EXCEPTION_FLT_INEXACT_RESULT:       return "Float Inexact Result";
	case EXCEPTION_FLT_INVALID_OPERATION:    return "Float Invalid Operation";
	case EXCEPTION_FLT_OVERFLOW:             return "Float Overflow";
	case EXCEPTION_FLT_STACK_CHECK:          return "Float Stack Check";
	case EXCEPTION_FLT_UNDERFLOW:            return "Float Underflow";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer Divide By Zero";
	case EXCEPTION_INT_OVERFLOW:             return "Integer Overflow";
	case EXCEPTION_PRIV_INSTRUCTION:         return "Private Instruction";
	case EXCEPTION_IN_PAGE_ERROR:            return "In Page Error";
	case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal Instruction";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Non-continuable Exceptiopn";
	case EXCEPTION_STACK_OVERFLOW:           return "Stack Overflow";
	case EXCEPTION_INVALID_DISPOSITION:      return "Invalid Disposition";
	case EXCEPTION_GUARD_PAGE:               return "Guard Page";
	case EXCEPTION_INVALID_HANDLE:           return "Invalid Handle";
	case 0xE06D7363:						 return "C++ Exception (using throw)";

	default:
		return "Unknown Exception";
	}

}

int main(int argc, char** argv) {
	// Disable Console mode
	//FreeConsole();

	//
	// Launch parameters will be divided into 2 parts:
	//
	// REQUIRED: If the application not launch with these parameters that mentioned in this
	//			 section, the application will not launch and return an error
	// 
	// OPTIONAL: Launching application without these parameters that mentioned in this section
	//           will not cause the error, application can work normally
	//
	// Lists of parameters:
	//
	// ----------------------------------------------------------------------------------------
	//				R  E  Q  U  I  R  E  D    P  A  R  A  M  E  T  E  R  S
	// ----------------------------------------------------------------------------------------

	/**
		-application_identity <hex> (Example usage: -application_identity 0xDEADBEEF)
	*/

	// OPTIONAL PARAMETERS: Currently not implemented. Will be expanded in the future

	if (argc == 1) {
		MessageBoxWithoutTaskbar(NULL,
			L"The application cannot be run! Please contact to your administrator!",
			L"Application error",
			MB_OK | MB_ICONERROR);

		return 1;
	}
	else {
		if (argc < 3) {
			MessageBoxWithoutTaskbar(NULL,
				L"The application cannot be run! Please contact to your administrator!",
				L"Application error",
				MB_OK | MB_ICONERROR);

			return 1;
		}
		else {
			if (strcmp(argv[1], "-application_identity") == 0) {
				// Expected argv[2] have an string as hex
				if (strcmp(argv[2], "") == 0) {
					MessageBoxWithoutTaskbar(NULL,
						L"The parameter is incorrect",
						L"Application Error",
						MB_OK | MB_ICONERROR);

					return 1;
				}
				else {
					// Verify the input
					if (!isHex(argv[2])) {
						MessageBoxWithoutTaskbar(NULL,
							L"The parameter is incorrect",
							L"Application Error",
							MB_OK | MB_ICONERROR);

						return 1;
					}
				}
			}
			else {
				if (!isHex(argv[2])) {
					MessageBoxWithoutTaskbar(NULL,
						L"The parameter is incorrect",
						L"Application Error",
						MB_OK | MB_ICONERROR);

					return 1;
				}
				else {
					unsigned int hexString = getHex(argv[2]);

					std::vector<unsigned char> key = {
						0x00, 0x01, 0x02, 0x03,
						0x04, 0x05, 0x06, 0x07,
						0x08, 0x09, 0x0A, 0x0B,
						0x0C, 0x0D, 0x0E, 0x0F
					};

					std::string decrypted = DecryptRC5FromUInt(hexString, key);

					if (strcmp(decrypted.c_str(), "TU9TNjVPMl9FTVVfV0lOX1g2NA==") == 0) {
						MessageBoxWithoutTaskbar(NULL, L"Done!", L"OK", MB_OK);
						while(1) {
							std::this_thread::sleep_for(std::chrono::milliseconds(100));
						}
					} else {
						while (1) {}
					}
				}
			}
		}
	}

	return 0;
}