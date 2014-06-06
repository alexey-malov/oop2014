#include "stdafx.h"
#include "MyArray.h"
#include <string>

using namespace std;


struct ThrowingObject
{
	static int defaultConstructionThrowCounter; // для проверки конструктора по умолчанию
	int throwCounter; // для проверки в конструкторе копирования
	int assignmentThrowCounter; // в операторе присваивания
	int value; // значение для нашей тестовой функции findMax
	ThrowingObject(int copyThrowCounter = 0, int assignmentThrowCounter = 0, int value = 0)
		:throwCounter(copyThrowCounter)
		, assignmentThrowCounter(assignmentThrowCounter)
		, value(value)
	{
		if ((defaultConstructionThrowCounter > 0) && (--defaultConstructionThrowCounter == 0))
		{
			throw exception("Oops!");
		}
	}

	ThrowingObject(ThrowingObject const& rhs)
		: throwCounter(rhs.throwCounter)
		, assignmentThrowCounter(rhs.assignmentThrowCounter)
		, value(rhs.value)
	{
		if ((throwCounter > 0) && (--throwCounter == 0))
		{
			throw exception("Oops!");
		}
	}

	ThrowingObject & operator=(ThrowingObject const& rhs)
	{
		if (this != addressof(rhs))
		{
			if (rhs.assignmentThrowCounter == 1)
			{
				throw exception("Oops!");
			}
			value = rhs.value;
			throwCounter = rhs.throwCounter;
			assignmentThrowCounter = rhs.assignmentThrowCounter;
			--assignmentThrowCounter;
		}
		return *this;
	}


};

int ThrowingObject::defaultConstructionThrowCounter = 0;

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
	BOOST_CHECK(!arr.IsEmpty());
	BOOST_REQUIRE_EQUAL(arr.GetSize(), 1u);
	BOOST_CHECK_EQUAL(arr[0], element);

	BOOST_CHECK_NO_THROW(arr.Push(element));
	BOOST_CHECK_EQUAL(arr[1], element);
	BOOST_REQUIRE_EQUAL(arr.GetSize(), 2u);
	BOOST_CHECK_EQUAL(arr[0], element);

}

BOOST_AUTO_TEST_CASE(ThrowingExceptionWhenAddingElementToEmptyArray)
{
	CMyArray<ThrowingObject> arr;
	BOOST_CHECK_THROW(arr.Push(ThrowingObject(0, 1)), exception);
	BOOST_CHECK(arr.IsEmpty());
}

BOOST_AUTO_TEST_CASE(ThrowingExceptionWhenAddingElementToNonEmptyArray)
{
	{
		CMyArray<ThrowingObject> arr;
		BOOST_CHECK_NO_THROW(arr.Push(ThrowingObject(0, 2, 42)));
		BOOST_CHECK_THROW(arr.Push(ThrowingObject()), exception);
		BOOST_CHECK_EQUAL(arr.GetSize(), 1u);
		BOOST_CHECK_EQUAL(arr[0].value, 42);
	}

}

BOOST_AUTO_TEST_CASE(TestCopyConstruction)
{
	CMyArray<string> arr;
	arr.Push("test");
	arr.Push("string");
	CMyArray<string> arr2(arr);
	BOOST_CHECK_EQUAL(arr.GetSize(), 2u);
	BOOST_CHECK_EQUAL(arr2.GetSize(), 2u);
	BOOST_CHECK_EQUAL(arr2[0], "test");
	BOOST_CHECK_EQUAL(arr2[1], "string");

	BOOST_CHECK_EQUAL(arr[0], "test");
	BOOST_CHECK_EQUAL(arr[1], "string");
}

BOOST_AUTO_TEST_SUITE_END()