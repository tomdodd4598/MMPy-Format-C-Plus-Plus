#include "format.h"
#include "helpers.h"

#include <iostream>

#define TEST(MMPY, STR)\
std::cout << #STR":\n";\
apply_ordered([](auto const& x) {std::cout << MMPY##_format(x) << "\n";}, xs);\
std::cout << std::endl;\

int main() {
	const auto xs = std::tuple{
		std::vector<std::vector<int>>{{}, {1}, {2, 3}, {4, 5, 6}},
		std::tuple{"\'one\'", "\"two\"", 3, std::list{4.0, 5.0, 6.0}},
		std::map<char, std::set<bool>>{{'a', std::set<bool>{}}, {'b', std::set{true, false}}},
		std::array<float, 5>{1.2f, 2.4f, 3.6f, 4.8f, 5.0f},
	};

	TEST(mm, Mathematica);
	TEST(py, Python);

	return 0;
}
