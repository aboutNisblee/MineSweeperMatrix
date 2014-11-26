/**
 * @file matrix_test.cpp
 *
 *
 * @date 18.11.2012
 * @author Moritz Nisblé moritz.nisble@gmx.de
 */

#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_MODULE "MineSweeperMatrix Test Suite"
#include <boost/test/unit_test.hpp>

#include "matrix.hpp"

struct Fix_matrix_test: public msm::MatrixObserver
{
	Fix_matrix_test() :
			uut(0), last_status(msm::GS_READY), gs_cb_count(0), last_remaining_bomb_count(0), rb_cb_count(0)
	{
	}
	~Fix_matrix_test()
	{
		if (uut != 0)
			delete uut;
	}

	void onGameStatusChanged(msm::Matrix const&, msm::GAMESTATUS new_status)
	{
		last_status = new_status;
		++gs_cb_count;
	}

	void onRemainingBombsChanged(msm::Matrix const&, int32_t new_remaining_bomb_count)
	{
		last_remaining_bomb_count = new_remaining_bomb_count;
		++rb_cb_count;
	}
	void onFieldStatusChanged(msm::Matrix const&, msm::Field const&, msm::FIELDSTATUS)
	{
	}
	void onFieldDelete(msm::Matrix const&, msm::Field const&)
	{
	}

	msm::Matrix* uut;

	msm::GAMESTATUS last_status;
	int gs_cb_count;

	int32_t last_remaining_bomb_count;
	int rb_cb_count;
};

namespace
{
bool check_x(msm::IndexOutOfBoundsException const& ex)
{
	return (ex.idx == 1 && ex.len == 1 && ex.dim == 'X');
}

bool check_y(msm::IndexOutOfBoundsException const& ex)
{
	return (ex.idx == 1 && ex.len == 1 && ex.dim == 'Y');
}
}

BOOST_FIXTURE_TEST_SUITE(matrix_test_suite, Fix_matrix_test)

BOOST_AUTO_TEST_CASE(null_dimensions_test)
{
	msm::Dimensions d;
	uut = new msm::Matrix(d);
	BOOST_REQUIRE(uut != 0);
}

BOOST_AUTO_TEST_CASE(exception_test)
{
	uut = new msm::Matrix();
	BOOST_REQUIRE(uut != 0);

	BOOST_CHECK_THROW((*uut)[0][0], msm::IndexOutOfBoundsException);

	uut->reset(msm::Dimensions(1, 1, 0));
	BOOST_CHECK_EXCEPTION((*uut)[1][0], msm::IndexOutOfBoundsException, check_x);
	BOOST_CHECK_EXCEPTION((*uut)[0][1], msm::IndexOutOfBoundsException, check_y);
}

BOOST_AUTO_TEST_CASE(remaining_bombs_check)
{
	uut = new msm::Matrix(msm::Dimensions(1,1,0));
	BOOST_CHECK(0 == uut->getRemainingBombs());
	(*uut)[0][0].cycleMark();
	BOOST_CHECK(-1 == uut->getRemainingBombs());

	uut->reset(msm::Dimensions(1,1,1));
	BOOST_CHECK(1 == uut->getRemainingBombs());
	(*uut)[0][0].cycleMark();
	BOOST_CHECK(0 == uut->getRemainingBombs());
}

BOOST_AUTO_TEST_CASE(win_check)
{
	uut = new msm::Matrix(msm::Dimensions(1, 1, 0));
	BOOST_REQUIRE(msm::GS_READY == uut->getStatus());
	(*uut)[0][0].reveal();
	BOOST_CHECK(msm::GS_WON == uut->getStatus());

	uut->reset(msm::Dimensions(1, 1, 1));
	BOOST_REQUIRE(msm::GS_READY == uut->getStatus());
	(*uut)[0][0].cycleMark();
	BOOST_CHECK(msm::GS_WON == uut->getStatus());
}

BOOST_AUTO_TEST_CASE(lost_check)
{
	msm::Dimensions d(1, 1, 1);
	uut = new msm::Matrix(d);
	BOOST_REQUIRE(msm::GS_READY == uut->getStatus());
	(*uut)[0][0].reveal();
	BOOST_CHECK(msm::GS_LOST == uut->getStatus());
}

BOOST_AUTO_TEST_CASE(callback_test)
{
	msm::Dimensions d(2, 2, 1);
	uut = new msm::Matrix();
	uut->addObserver(this);

	uut->reset(d);
	BOOST_CHECK(msm::GS_READY == last_status);
	BOOST_CHECK(1 == gs_cb_count);

	(*uut)[0][0].cycleMark();
	BOOST_CHECK(msm::GS_RUNNING == last_status);
	BOOST_CHECK(2 == gs_cb_count);
	BOOST_CHECK(0 == last_remaining_bomb_count);
	BOOST_CHECK(1 == rb_cb_count);
}

BOOST_AUTO_TEST_SUITE_END()
