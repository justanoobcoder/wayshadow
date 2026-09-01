#include <iostream>

void test_color_parsing();
void test_config_parsing();
void test_config_file_loading();
void test_buffer_operations();

int main() {
    std::cout << "========================================\n";
    std::cout << " Running wayshadow unit tests\n";
    std::cout << "========================================\n";

    test_color_parsing();
    test_config_parsing();
    test_config_file_loading();
    test_buffer_operations();

    std::cout << "========================================\n";
    std::cout << " ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "========================================\n";
    return 0;
}
