//
// Geometry.h
//
// Basic types and functions relating to geometry

//---------------------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------------------

const float oneEightyOverPi = 180. / PI;  // multiply by this for radians to degrePI
const float piByTwo         = PI * 0.5;
const float twoPi           = PI * 2.0;
const float piOverOneEighty = PI / 180.;  // multiply by this for degrees to radians
const float sqrtTwo         = sqrt(2.0);

typedef enum { LEFT_SIDE, RIGHT_SIDE } SIDE;

// Amazingly, the builtin radiansToDegrees rounds/truncates to integers.
// These macros ensure that such bogosity doesn't ever bother us.
#define degreesToRadians(deg)   ((deg) * piOverOneEighty)
#define radiansToDegrees(rad)   ((rad) * oneEightyOverPi)

#if 0 // no-one actually needs this yet
// atan2 is a standard routine usually found in your C runtime library, but it is absent
// from RobotC's library, so we recreate it here. You can read more about this in
// http://en.wikipedia.org/wiki/Atan2, but in short, it returns the arctan of y/x but
// (a) takes into account the actual quandrant in which the point (x,y) is found, and
// (b) can correctly deal with the case where x is zero.
float atan2(float y, float x)
    {
    if (0.0 == x)
        {
        if (y > 0.0)
            return piByTwo;
        else if (y == 0.0)
            return 0.0;
        else
            return -piByTwo;
        }

    float ratio = y / x;
    float absRatio = abs(ratio);

    if (ratio > 1.0 || ratio < -1.0)
        {
        // steep angle
        float rad = atan(1.0 / absRatio);
        if (x >= 0.0)
            {
            if (y >= 0.0)
                return piByTwo - rad;   // quandrant I
            else
                return -piByTwo + rad;  // quadrant IV
            }
        else
            {
            if (y >= 0.0)
                return piByTwo + rad;   // quadrant II
            else
                return -piByTwo - rad;  // quandrant III
            }
        }
    else
        {
        // shallow angle
        float rad = atan(absRatio);
        if (x >= 0.0)
            {
            if (y >= 0.0)
                return rad;             // quadrant I
            else
                return -rad;            // quadrant IV
            }
        else
            {
            if (y >= 0.0)
                return PI - rad;        // quandrant II
            else
                return -PI + rad;       // quandrant III
            }
        }
    }
#endif

//---------------------------------------------------------------------------------------
// ANGLE type
//---------------------------------------------------------------------------------------

typedef float ANGLE;
// We defined a typedef for ANGLE so that we could more easily change our mind later
// as to whether we really needed floating point, though that may have been more
// effort that it was worth. And, probably, we can't reasonably change it now.
//
// ANGLE is always in degrees.

#define NormalizeAngleVar(angle)                            \
/* Normalize the angle into the range [-180,180) */         \
    {                                                       \
    while (angle >= 180.0) angle -= 360.0;                  \
    while (angle < -180.0) angle += 360.0;                  \
    }

//---------------------------------------------------------------------------------------
// POINT type
//---------------------------------------------------------------------------------------

typedef struct
    {
    float   x;
    float   y;
    } POINT;

#define AssignPoint(ptTo, ptFrom)       \
    /* assign one point to another */   \
    {                                   \
    ptTo.x = ptFrom.x;                  \
    ptTo.y = ptFrom.y;                  \
    }

#define AssignPointXY(ptResult, xVal, yVal) { ptResult.x = xVal; ptResult.y = yVal; }

#define diffToFrom(pt, ptA, ptB)    \
    {                               \
    pt.x = ptA.x - ptB.x;           \
    pt.y = ptA.y - ptB.y;           \
    }
#define diffFromTo(pt, ptA, ptB)    \
    {                               \
    pt.x = ptB.x - ptA.x;           \
    pt.y = ptB.y - ptA.y;           \
    }

#define dist(ptFrom, ptTo)   sqrt(square(ptTo.x-ptFrom.x) + square(ptTo.y-ptFrom.y))
#define sqdist(ptFrom, ptTo)     (square(ptTo.x-ptFrom.x) + square(ptTo.y-ptFrom.y))

//---------------------------------------------------------------------------------------
// Some basic geometric calculations
//---------------------------------------------------------------------------------------

// What's the point on a circle of indicated center and radius and indicated angle (CCW) from horizontal
#define pointOnCircle(pt, ptCenter, radius, radians)    \
    {                                                   \
    pt.x = (radius) * cos(radians);                     \
    pt.y = (radius) * sin(radians);                     \
    pt.x += ptCenter.x;                                 \
    pt.y += ptCenter.y;                                 \
    }

void intersectLineCircle(OUT POINT& ptU, OUT POINT& ptV, IN const POINT& ptFrom, IN const POINT& ptTo, IN const POINT& ptC, IN float r)
// Return the two points U & V which are the intersection of the line [ptFrom, ptTo] with
// the circle centered at ptC with radius r
    {
	const float v00 = ptFrom.y - ptTo.y;
	const float v01 = ptFrom.x - ptTo.x;
	const float v02 = square(v00) + square(v01);
	const float v03 = 1 / v02;
	const float v04 = v00 * (ptTo.y - ptC.y) + v01 * (ptTo.x - ptC.x);
	const float v05 = -timesTwo(v04);
	const float v06 = sqrt(timesFour(square(v04)) - timesFour(v02 * (((square(ptC.x) + square(ptC.y) + square(ptTo.x) + square(ptTo.y)) - timesTwo(ptC.x * ptTo.x + ptC.y * ptTo.y)) - square(r))));
	const float v07 = v05 + v06;
	const float v08 = v05 - v06;
	ptU.x = ptTo.x + 0.5 * v01 * v03 * v08;
	ptU.y = ptTo.y + 0.5 * v00 * v03 * v08;
	ptV.x = ptTo.x + 0.5 * v01 * v03 * v07;
	ptV.y = ptTo.y + 0.5 * v00 * v03 * v07;
    }

void linearOrder(OUT int& iCompare, const POINT& ptFrom, const POINT& ptTo, const POINT& ptProbe, float sqdFromTo)
// Given two points on a line, ptFrom and ptTo, answer <0, 0, or >0 according to how a third
// point, ptProbe relates. Positive on the line is in the direction from ptFrom towards ptTo.
// sqdFromTo is a cached calculation of the squared distance from ptFrom to ptTo
    {
//        sqdFromTo    = sqdist(ptFrom, ptTo);
    float sqdProbeFrom = sqdist(ptProbe, ptFrom);
    float sqdProbeTo   = sqdist(ptProbe, ptTo);

    // If the longest distance is sqdFromTo, then the probe is in the middle
    if (sqdFromTo > sqdProbeFrom && sqdFromTo > sqdProbeTo)
        iCompare = 0;

    // So probe is to one side or the other, but which? If ptFrom is in the middle
    // then it's to the left, otherwise, it's to the right
    else if (sqdProbeTo > sqdProbeFrom && sqdProbeTo > sqdFromTo)
        iCompare = -1;

    else
        iCompare = 1;
    }

// We are given two points, ptFrom and ptTo, on a circle. A third point, ptProbe, is
// also on the circle. Collectively, the three points span less than 180 degrees. Then,
// with the direction of the circle indicated by the circular vector from ptFrom to
// ptTo, answer -1, 0, or 1 respectively according to whether ptProbe is before,
// between, or after the two other points.
#define circularOrder(iCompare, ptFrom, ptTo, ptProbe, sqdFromTo)   \
    {                                                               \
    linearOrder(iCompare, ptFrom, ptTo, ptProbe, sqdFromTo);        \
    }
