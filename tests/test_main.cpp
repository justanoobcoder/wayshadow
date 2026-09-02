#include <iostream>

void test_color_parsing();
void test_config_parsing();
void test_config_file_loading();
void test_buffer_operations();
void test_mouse_state();

int main() {
    std::cout << "========================================\n";
    std::cout << " Running wayshadow unit tests\n";
    std::cout << "========================================\n";

    test_color_parsing();
    test_config_parsing();
    test_config_file_loading();
    test_buffer_operations();
    test_mouse_state();

    std::cout << "========================================\n";
    std::cout << " ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "========================================\n";
    return 0;
}
