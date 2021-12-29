#include <Regression.h>
#include <unity.h>

regression::Approximation<20> approximation;

template <size_t N>
void assertSlope(const regression::Point (&points)[N], const float expectedSlope, const float expectedYIntercept)
{
    for (auto p : points)
    {
        approximation.addPoint(p);
    }

    regression::Result result = approximation.getLeastSquares();
    TEST_ASSERT_EQUAL_FLOAT(expectedSlope, result.m);
    TEST_ASSERT_EQUAL_FLOAT(expectedYIntercept, result.yIntercept);
}

void setUp(void)
{
    approximation.reset();
}

void test_full(void)
{
    const regression::Point pointsFull[] = {
        {0, 0},
        {1, 2},
        {3, 6},
        {8, 16},
        {10, 20},
        {20, 40},
        {22, 44},
        {1, 2},
        {2, 4},
        {6, 12},
    };

    assertSlope(pointsFull, 2.0, 0);
}

void test_not_full(void)
{
    const regression::Point pointsNotFull[] = {
        {0, 0},
        {6, 12},
    };

    assertSlope(pointsNotFull, 2.0, 0);
}

void test_overflow_buffer(void)
{
    const regression::Point pointsOverflow[] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {10, 20},
        {20, 40},
        {22, 44},
    };

    assertSlope(pointsOverflow, 2.0, 0);
}

void test_floating_point(void)
{
    const regression::Point pointsFP[] = {
        {2, 3.5},
        {4, 4.5},
    };

    assertSlope(pointsFP, 0.5, 2.5);
}

void test_size_variable(void)
{
    regression::Approximation<2> dualPointApproximation;
    const regression::Point points[] = {
        {0, 0},
        {1, 1},
        {0, 50},
        {1, 100},
    };

    for (auto p : points)
    {
        dualPointApproximation.addPoint(p);
    }

    auto result = dualPointApproximation.getLeastSquares();
    TEST_ASSERT_EQUAL_FLOAT(50, result.m);
    TEST_ASSERT_EQUAL_FLOAT(50, result.yIntercept);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_full);
    RUN_TEST(test_not_full);
    RUN_TEST(test_overflow_buffer);
    RUN_TEST(test_size_variable);
    return UNITY_END();
}