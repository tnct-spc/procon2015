#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <type_traits>

// こうするとプライベートメンバ関数のテストができるぞい
#define private public
#include "data_type.hpp"

namespace boost {
    namespace test_tools {
        template <std::size_t N>
        struct print_log_value<std::array<std::string, N>> {
            void operator() (std::ostream & os, std::array<std::string, N> const & container)
            {
                for (auto && element : container) {
                    os << element << std::endl;
                }
            }
        };

        template <typename T, std::size_t N1, std::size_t N2>
        struct print_log_value<std::array<std::array<T, N1>, N2>> {
            void operator() (std::ostream & os, std::array<std::array<T, N1>, N2> const & container)
            {
                for (auto && row : container) {
                    for (auto && element : row) {
                        os << element << " ";
                    }
                    os << std::endl;
                }
            }
        };

        template <>
        struct print_log_value<std::vector<std::string>> {
            void operator() (std::ostream & os, std::vector<std::string> const & container)
            {
                for (auto && element : container) {
                    os << element << std::endl;
                }
            }
        };
    }
}

BOOST_AUTO_TEST_CASE(_split_test)
{
    auto sample = "123,,456,,789,,12,34,56,78,90,,abc,,def,,ghi,,,,,"s;
    std::vector<std::string> result = _split(sample, ",,"s);
    decltype(result) expected = {"123"s,"456"s,"789"s, "12,34,56,78,90"s, "abc"s, "def"s, "ghi"s, ""s, ","s};
    BOOST_CHECK_EQUAL(expected, result);
}

namespace {
    BOOST_AUTO_TEST_SUITE(problem_type_tests)

    auto problem_text = "\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
01000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000100000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000010000001111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
\r\n\
4\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01110000\r\n\
00000000\r\n\
\r\n\
00000000\r\n\
01100000\r\n\
01100000\r\n\
01100000\r\n\
01100000\r\n\
00000000\r\n\
00000000\r\n\
00000000\r\n\
\r\n\
00000000\r\n\
00010000\r\n\
00010000\r\n\
01111000\r\n\
00000000\r\n\
00000000\r\n\
00000000\r\n\
00000000\r\n\
\r\n\
10000000\r\n\
11000000\r\n\
01100000\r\n\
00110000\r\n\
00011000\r\n\
00001100\r\n\
00000110\r\n\
00000011";

    BOOST_AUTO_TEST_CASE(_split_problem_text_test)
    {
        decltype(problem_type::_split_problem_text(problem_text)) expected, result;
        expected = std::make_tuple(
"00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
01000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000100000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000000000001111111111111111\r\n\
00000000010000001111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111\r\n\
11111111111111111111111111111111", 
std::vector<std::string>({
"01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01000000\r\n\
01110000\r\n\
00000000",
"00000000\r\n\
01100000\r\n\
01100000\r\n\
01100000\r\n\
01100000\r\n\
00000000\r\n\
00000000\r\n\
00000000",
"00000000\r\n\
00010000\r\n\
00010000\r\n\
01111000\r\n\
00000000\r\n\
00000000\r\n\
00000000\r\n\
00000000",
"10000000\r\n\
11000000\r\n\
01100000\r\n\
00110000\r\n\
00011000\r\n\
00001100\r\n\
00000110\r\n\
00000011"}));
        result = problem_type::_split_problem_text(problem_text);
        BOOST_CHECK_EQUAL(std::get<0>(expected), std::get<0>(result));
        BOOST_CHECK_EQUAL(std::get<1>(expected), std::get<1>(result));
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        BOOST_CHECK_NO_THROW(problem_type(problem_text));
    }

    BOOST_AUTO_TEST_SUITE_END()
}

namespace {
    BOOST_AUTO_TEST_SUITE(stone_type_tests)

    std::string stone_text = "01000000\r\n01000000\r\n01000000\r\n01000000\r\n01000000\r\n01000000\r\n01110000\r\n00000000";

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        decltype(stone_type::raw_data) expected = {{
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 0, 0, 0, 0, 0, 0}},
            {{0, 1, 1, 1, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0}}
        }};
        auto result = stone_type(stone_text, 0).raw_data;
        BOOST_CHECK_EQUAL(expected, result);
    }

    BOOST_AUTO_TEST_SUITE_END()
}

namespace {
    BOOST_AUTO_TEST_SUITE(field_type_tests)
    BOOST_AUTO_TEST_SUITE_END()
}
