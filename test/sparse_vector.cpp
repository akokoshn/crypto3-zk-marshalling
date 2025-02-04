//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#define BOOST_TEST_MODULE crypto3_marshalling_sparse_vector_test

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <iostream>
#include <iomanip>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>

#include <nil/crypto3/multiprecision/cpp_int.hpp>
#include <nil/crypto3/multiprecision/number.hpp>

#include <nil/crypto3/algebra/random_element.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/detail/marshalling.hpp>

#include <nil/crypto3/container/sparse_vector.hpp>

#include <nil/crypto3/marshalling/zk/types/sparse_vector.hpp>

template<typename TIter>
void print_byteblob(TIter iter_begin, TIter iter_end) {
    for (TIter it = iter_begin; it != iter_end; it++) {
        std::cout << std::hex << int(*it) << std::endl;
    }
}

template<typename FpCurveGroupElement>
void print_fp_curve_group_element(FpCurveGroupElement e) {
    std::cout << e.X.data << " " << e.Y.data << " " << e.Z.data << std::endl;
}

template<typename Fp2CurveGroupElement>
void print_fp2_curve_group_element(Fp2CurveGroupElement e) {
    std::cout << "(" << e.X.data[0].data << " " << e.X.data[1].data << ") (" << e.Y.data[0].data << " "
              << e.Y.data[1].data << ") (" << e.Z.data[0].data << " " << e.Z.data[1].data << ")" << std::endl;
}

template<typename Endianness, typename GroupType>
void test_sparse_vector(nil::crypto3::container::sparse_vector<GroupType> val) {

    using namespace nil::crypto3::marshalling;

    std::size_t units_bits = 8;
    using unit_type = unsigned char;
    using sparse_vector_type = types::sparse_vector<nil::marshalling::field_type<Endianness>,
            nil::crypto3::container::sparse_vector<GroupType>>;

    sparse_vector_type filled_val =
            types::fill_sparse_vector<nil::crypto3::container::sparse_vector<GroupType>, Endianness>(val);

    nil::crypto3::container::sparse_vector<GroupType> constructed_val =
            types::make_sparse_vector<nil::crypto3::container::sparse_vector<GroupType>, Endianness>(filled_val);
    BOOST_CHECK(val == constructed_val);

    std::size_t unitblob_size = filled_val.length();

    std::vector<unit_type> cv;
    cv.resize(unitblob_size, 0x00);

    auto write_iter = cv.begin();

    nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());

    sparse_vector_type test_val_read;

    auto read_iter = cv.begin();
    status = test_val_read.read(read_iter, cv.size());

    nil::crypto3::container::sparse_vector<GroupType> constructed_val_read =
            types::make_sparse_vector<nil::crypto3::container::sparse_vector<GroupType>, Endianness>(test_val_read);

    BOOST_CHECK(val == constructed_val_read);
}

template<typename GroupType, typename Endianness, std::size_t TSize>
void test_sparse_vector() {
    std::cout << std::hex;
    std::cerr << std::hex;
    for (unsigned i = 0; i < 128; ++i) {
        std::vector<typename GroupType::value_type> val_container;
        if (!(i % 16) && i) {
            std::cout << std::dec << i << " tested" << std::endl;
        }
        for (std::size_t i = 0; i < TSize; i++) {
            val_container.push_back(nil::crypto3::algebra::random_element<GroupType>());
        }
        test_sparse_vector<Endianness>(nil::crypto3::container::sparse_vector<GroupType>(std::move(val_container)));
    }
}

BOOST_AUTO_TEST_SUITE(sparse_vector_test_suite)

    BOOST_AUTO_TEST_CASE(sparse_vector_bls12_381_g1_be) {
        std::cout << "BLS12-381 g1 group field sparse vector big-endian test started" << std::endl;
        test_sparse_vector<nil::crypto3::algebra::curves::bls12<381>::g1_type<>, nil::marshalling::option::big_endian, 5>();
        std::cout << "BLS12-381 g1 group field sparse vector big-endian test finished" << std::endl;
    }

// BOOST_AUTO_TEST_CASE(sparse_vector_bls12_381_g1_le) {
//     std::cout << "BLS12-381 g1 group field sparse vector little-endian test started" << std::endl;
//     test_sparse_vector<nil::crypto3::algebra::curves::bls12<381>::g1_type<>,
//         nil::marshalling::option::little_endian,
//         5>();
//     std::cout << "BLS12-381 g1 group field sparse vector little-endian test finished" << std::endl;
// }

    BOOST_AUTO_TEST_CASE(sparse_vector_bls12_381_g2_be) {
        std::cout << "BLS12-381 g2 group field sparse vector big-endian test started" << std::endl;
        test_sparse_vector<nil::crypto3::algebra::curves::bls12<381>::g2_type<>, nil::marshalling::option::big_endian, 5>();
        std::cout << "BLS12-381 g2 group field sparse vector big-endian test finished" << std::endl;
    }

// BOOST_AUTO_TEST_CASE(sparse_vector_bls12_381_g2_le) {
//     std::cout << "BLS12-381 g2 group field little-endian test started" << std::endl;
//     test_sparse_vector<nil::crypto3::algebra::curves::bls12<381>::g2_type<>,
//         nil::marshalling::option::little_endian,
//         5>();
//     std::cout << "BLS12-381 g2 group field little-endian test finished" << std::endl;
// }

BOOST_AUTO_TEST_SUITE_END()
