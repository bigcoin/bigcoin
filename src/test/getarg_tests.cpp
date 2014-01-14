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
    ResetArgs("-BIG");
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", false));
    BOOST_CHECK(GetBoolArg("-BIG", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BIGo"));
    BOOST_CHECK(!GetBoolArg("-BIGo", false));
    BOOST_CHECK(GetBoolArg("-BIGo", true));

    ResetArgs("-BIG=0");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", false));
    BOOST_CHECK(!GetBoolArg("-BIG", true));

    ResetArgs("-BIG=1");
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", false));
    BOOST_CHECK(GetBoolArg("-BIG", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBIG");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", false));
    BOOST_CHECK(!GetBoolArg("-BIG", true));

    ResetArgs("-noBIG=1");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", false));
    BOOST_CHECK(!GetBoolArg("-BIG", true));

    ResetArgs("-BIG -noBIG");  // -BIG should win
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", false));
    BOOST_CHECK(GetBoolArg("-BIG", true));

    ResetArgs("-BIG=1 -noBIG=1");  // -BIG should win
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", false));
    BOOST_CHECK(GetBoolArg("-BIG", true));

    ResetArgs("-BIG=0 -noBIG=0");  // -BIG should win
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", false));
    BOOST_CHECK(!GetBoolArg("-BIG", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BIG=1");
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", false));
    BOOST_CHECK(GetBoolArg("-BIG", true));

    ResetArgs("--noBIG=1");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", false));
    BOOST_CHECK(!GetBoolArg("-BIG", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BIG", "eleven"), "eleven");

    ResetArgs("-BIG -bar");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BIG", "eleven"), "");

    ResetArgs("-BIG=");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BIG", "eleven"), "");

    ResetArgs("-BIG=11");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BIG", "eleven"), "11");

    ResetArgs("-BIG=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BIG", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BIG", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BIG", 0), 0);

    ResetArgs("-BIG -bar");
    BOOST_CHECK_EQUAL(GetArg("-BIG", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BIG=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BIG", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BIG=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BIG", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BIG");
    BOOST_CHECK_EQUAL(GetBoolArg("-BIG"), true);

    ResetArgs("--BIG=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BIG", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBIG");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", true));
    BOOST_CHECK(!GetBoolArg("-BIG", false));

    ResetArgs("-noBIG=1");
    BOOST_CHECK(!GetBoolArg("-BIG"));
    BOOST_CHECK(!GetBoolArg("-BIG", true));
    BOOST_CHECK(!GetBoolArg("-BIG", false));

    ResetArgs("-noBIG=0");
    BOOST_CHECK(GetBoolArg("-BIG"));
    BOOST_CHECK(GetBoolArg("-BIG", true));
    BOOST_CHECK(GetBoolArg("-BIG", false));

    ResetArgs("-BIG --noBIG");
    BOOST_CHECK(GetBoolArg("-BIG"));

    ResetArgs("-noBIG -BIG"); // BIG always wins:
    BOOST_CHECK(GetBoolArg("-BIG"));
}

BOOST_AUTO_TEST_SUITE_END()
