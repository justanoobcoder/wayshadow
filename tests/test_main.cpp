#include <iostream>

void TestColorParsing();
void TestConfigParsing();
void TestConfigFileLoading();
void TestBufferOperations();
void TestMouseState();

int main() {
    std::cout << "========================================\n";
    std::cout << " Running wayshadow unit tests\n";
    std::cout << "========================================\n";

    TestColorParsing();
    TestConfigParsing();
    TestConfigFileLoading();
    TestBufferOperations();
    TestMouseState();

    std::cout << "========================================\n";
    std::cout << " ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "========================================\n";
    return 0;
}
