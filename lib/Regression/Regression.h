#pragma once;

#include <CircularBuffer.h>
#include <math.h>
#include <climits>

namespace regression
{
    struct Point
    {
        long x;
        float y;
    };

    struct Result
    {
        float m;
        float yIntercept;
    };

    template <size_t N>
    class Approximation
    {
    public:
        void reset();
        void addPoint(Point p);
        /**
         * Retrieves the approcimation of the given data points using least squares.
         * @return approximation
         */
        Result getLeastSquares();
        long getXAtY(float y);
        long getXAtY(float y, Result res);

    private:
        CircularBuffer<Point, N> buffer;
    };

    template <size_t N>
    void Approximation<N>::reset()
    {
        buffer.clear();
    }

    template <size_t N>
    void Approximation<N>::addPoint(Point p)
    {
        buffer.push(p);
    }

    template <size_t N>
    Result Approximation<N>::getLeastSquares()
    {
        float sumX = 0;
        float sumY = 0;
        float sumX2 = 0;
        float sumXY = 0;

        int bufferSize = buffer.size();
        for (int i = 0; i < bufferSize; i++)
        {
            Point p = buffer[i];
            sumX += p.x;
            sumY += p.y;
            sumX2 += p.x * p.x;
            sumXY += p.x * p.y;
        }

        float m = (bufferSize * sumXY - sumX * sumY) / (bufferSize * sumX2 - sumX * sumX);
        float yIntercept = (sumY - m * sumX) / bufferSize;

        return {
            m,
            yIntercept,
        };
    }
    
    template<size_t N>
    long Approximation<N>::getXAtY(float y) 
    {
        return getXAtY(y, getLeastSquares());
    }
    
    template<size_t N>
    long Approximation<N>::getXAtY(float y, Result res) 
    {
        if (isnan(res.m)) {
            return LONG_MAX;
        }

        return (y - res.yIntercept) / res.m;
    }
}