#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-kmw");
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", false));
    BOOST_CHECK(GetBoolArg("-kmw", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-kmwo"));
    BOOST_CHECK(!GetBoolArg("-kmwo", false));
    BOOST_CHECK(GetBoolArg("-kmwo", true));

    ResetArgs("-kmw=0");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", false));
    BOOST_CHECK(!GetBoolArg("-kmw", true));

    ResetArgs("-kmw=1");
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", false));
    BOOST_CHECK(GetBoolArg("-kmw", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nokmw");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", false));
    BOOST_CHECK(!GetBoolArg("-kmw", true));

    ResetArgs("-nokmw=1");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", false));
    BOOST_CHECK(!GetBoolArg("-kmw", true));

    ResetArgs("-kmw -nokmw");  // -kmw should win
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", false));
    BOOST_CHECK(GetBoolArg("-kmw", true));

    ResetArgs("-kmw=1 -nokmw=1");  // -kmw should win
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", false));
    BOOST_CHECK(GetBoolArg("-kmw", true));

    ResetArgs("-kmw=0 -nokmw=0");  // -kmw should win
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", false));
    BOOST_CHECK(!GetBoolArg("-kmw", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--kmw=1");
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", false));
    BOOST_CHECK(GetBoolArg("-kmw", true));

    ResetArgs("--nokmw=1");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", false));
    BOOST_CHECK(!GetBoolArg("-kmw", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kmw", "eleven"), "eleven");

    ResetArgs("-kmw -bar");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kmw", "eleven"), "");

    ResetArgs("-kmw=");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kmw", "eleven"), "");

    ResetArgs("-kmw=11");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-kmw", "eleven"), "11");

    ResetArgs("-kmw=eleven");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-kmw", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-kmw", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-kmw", 0), 0);

    ResetArgs("-kmw -bar");
    BOOST_CHECK_EQUAL(GetArg("-kmw", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-kmw=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-kmw", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-kmw=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-kmw", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--kmw");
    BOOST_CHECK_EQUAL(GetBoolArg("-kmw"), true);

    ResetArgs("--kmw=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-kmw", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nokmw");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", true));
    BOOST_CHECK(!GetBoolArg("-kmw", false));

    ResetArgs("-nokmw=1");
    BOOST_CHECK(!GetBoolArg("-kmw"));
    BOOST_CHECK(!GetBoolArg("-kmw", true));
    BOOST_CHECK(!GetBoolArg("-kmw", false));

    ResetArgs("-nokmw=0");
    BOOST_CHECK(GetBoolArg("-kmw"));
    BOOST_CHECK(GetBoolArg("-kmw", true));
    BOOST_CHECK(GetBoolArg("-kmw", false));

    ResetArgs("-kmw --nokmw");
    BOOST_CHECK(GetBoolArg("-kmw"));

    ResetArgs("-nokmw -kmw"); // kmw always wins:
    BOOST_CHECK(GetBoolArg("-kmw"));
}

BOOST_AUTO_TEST_SUITE_END()
