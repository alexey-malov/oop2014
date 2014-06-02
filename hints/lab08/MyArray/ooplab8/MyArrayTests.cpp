#include "stdafx.h"
#include "MyArray.h"
#include <string>

using namespace std;

BOOST_AUTO_TEST_SUITE(MyArrayTests)

BOOST_AUTO_TEST_CASE(CreateEmptyMyArray)
{
	const CMyArray<string> arr;
	BOOST_CHECK(arr.IsEmpty());
	BOOST_CHECK_EQUAL(arr.GetSize(), 0u);
	BOOST_CHECK_THROW(arr[0], out_of_range);
	CMyArray<string> arr2;
	BOOST_CHECK_THROW(arr2[0], out_of_range);
}

BOOST_AUTO_TEST_CASE(AddNewElement)
{
	CMyArray<string> arr;
	string element = "hello";
	BOOST_CHECK_NO_THROW(arr.Push(element));
	BOOST_REQUIRE_EQUAL(arr.GetSize(), 1u);
	BOOST_CHECK_EQUAL(arr[0], element);
}

BOOST_AUTO_TEST_SUITE_END()