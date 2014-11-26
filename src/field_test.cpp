/**
 * @file field_test.cpp
 *
 * @date 18.11.2012
 * @author Moritz Nisblé moritz.nisble@gmx.de
 */

#include <boost/test/unit_test.hpp>

#include "field.hpp"

struct Fix_field_test: public msm::FieldObserver
{
	Fix_field_test()
	{
		/*
		 * Manually creating a small 2x2
		 * test matrix with the layout:
		 * 1 1
		 * 1 X
		 */

		x[0] = new msm::Field(0, 0);
		x[1] = new msm::Field(1, 0);

		y[0] = new msm::Field(0, 1);
		y[1] = new msm::Bomb(1, 1);

		x[0]->addNeighbour(x[1]);
		x[0]->addNeighbour(y[0]);
		x[0]->addNeighbour(y[1]);

		x[1]->addNeighbour(x[0]);
		x[1]->addNeighbour(y[0]);
		x[1]->addNeighbour(y[1]);

		y[0]->addNeighbour(x[0]);
		y[0]->addNeighbour(x[1]);
		y[0]->addNeighbour(y[1]);

		y[1]->addNeighbour(x[0]);
		y[1]->addNeighbour(x[1]);
		y[1]->addNeighbour(y[0]);

		y[1]->informNeighbours();

		cb_count = 0;
		last_status = msm::FS_HIDDEN;
	}
	~Fix_field_test()
	{
		delete x[0];
		delete x[1];
		delete y[0];
		delete y[1];
	}

	void onFieldStatusChanged(msm::Field const&, msm::FIELDSTATUS new_status)
	{
		++cb_count;
		last_status = new_status;
	}

	void onFieldDelete(msm::Field const&)
	{
	}

	msm::Field* x[2];
	msm::Field* y[2];

	int cb_count;
	msm::FIELDSTATUS last_status;
};

BOOST_FIXTURE_TEST_SUITE(field_test_suite, Fix_field_test)

BOOST_AUTO_TEST_CASE(reveal_test)
{
	BOOST_CHECK(x[0]->reveal() == 1);
	BOOST_CHECK(msm::FS_UNHIDDEN == x[0]->getStatus());

	BOOST_CHECK(x[1]->reveal() == 1);
	BOOST_CHECK(msm::FS_UNHIDDEN == x[1]->getStatus());

	BOOST_CHECK(y[0]->reveal() == 1);
	BOOST_CHECK(msm::FS_UNHIDDEN == y[0]->getStatus());

	BOOST_CHECK(y[1]->reveal() == msm::FS_BOMB);
	BOOST_CHECK(msm::FS_BOMB == y[1]->getStatus());
}

BOOST_AUTO_TEST_CASE(mark_cycle_test)
{
	BOOST_CHECK(msm::FS_HIDDEN == x[0]->getStatus());
	x[0]->cycleMark();
	BOOST_CHECK(msm::FS_MARKED == x[0]->getStatus());
	x[0]->cycleMark();
	BOOST_CHECK(msm::FS_QUERIED == x[0]->getStatus());
	x[0]->cycleMark();
	BOOST_CHECK(msm::FS_HIDDEN == x[0]->getStatus());
}

BOOST_AUTO_TEST_CASE(callback_test)
{
	x[0]->addObserver(this);

	BOOST_REQUIRE(0 == cb_count);
	x[0]->cycleMark();
	BOOST_CHECK(1 == cb_count);
	BOOST_CHECK(msm::FS_MARKED == last_status);
}

BOOST_AUTO_TEST_SUITE_END()
