#include <string>
#include <chrono>
#include <iostream>
#include <array>
#include <random>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <boost/format.hpp>


inline double getTimeInMicroseconds(std::chrono::high_resolution_clock::time_point& previous)
{
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> actionTime = std::chrono::duration_cast<std::chrono::microseconds>(now-previous);
    previous = now;
    return actionTime.count();
}

struct DateTime
{
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minutes;
    unsigned char seconds;
};

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<unsigned short> yearsDist(0, 9999);
static std::uniform_int_distribution<unsigned char> monthsDist(1, 12);
static std::uniform_int_distribution<unsigned char> daysDist(1, 31);
static std::uniform_int_distribution<unsigned char> hoursDist(0, 23);
static std::uniform_int_distribution<unsigned char> minutesDist(0, 59);
static std::uniform_int_distribution<unsigned char> secondsDist(0, 59);

inline DateTime getRandomDateTime()
{
    DateTime ret;
    ret.year = yearsDist(gen);
    ret.month = monthsDist(gen);
    ret.day = daysDist(gen);
    ret.hour = hoursDist(gen);
    ret.minutes = minutesDist(gen);
    ret.seconds = secondsDist(gen);
    return ret;
}
constexpr int numberOfDateTimes = 10000000;
constexpr size_t indexYear = 0;
constexpr size_t lenYear = 4;
constexpr size_t indexMonth = 5;
constexpr size_t lenMonth = 2;
constexpr size_t indexDay = 8;
constexpr size_t lenDay = 2;
constexpr size_t indexHour = 11;
constexpr size_t lenHour = 2;
constexpr size_t indexMin = 14;
constexpr size_t lenMin = 2;
constexpr size_t indexSec = 17;
constexpr size_t lenSec = 2;


constexpr size_t dateTimeStrLen = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1;
constexpr const char * dateTimeStrFormat = "%04d-%02d-%02dT%02d:%02d:%02d";
static std::array<DateTime, numberOfDateTimes> dateTimes;
static std::array<char[dateTimeStrLen], numberOfDateTimes> dateTimeStrArray;

constexpr char digitToChar(unsigned int digit)
{
    return '0' + digit;
}

template <unsigned int from, unsigned int to, unsigned int stringSize>
struct IntToConstCharTable
{
    static_assert(from < to, "Should satisfy \'from < to\'");
    static constexpr unsigned int N = to - from + 1;
    constexpr IntToConstCharTable() : values()
    {
        for(unsigned int i = from; i <= to ; ++i)
        {
            unsigned int val = i;
            int ch = stringSize - 1;
            while(val)
            {
                unsigned int mod = val % 10;
                val = val / 10;

                values[i-from][(size_t)ch] = digitToChar(mod);
                --ch;
            }
            while(ch >= 0)
            {
                values[i-from][(size_t)ch] = '0';
                --ch;
            }
        }
    }
    std::array<char[stringSize], N> values;

    constexpr const char * get(unsigned int i) const
    {
        return values[i-from];
    }
};





inline void convertStringStream(const DateTime& dateTime, char* dateTimeStr)
{
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << (unsigned int)dateTime.year << "-"
       << std::setw(2) << (unsigned int)dateTime.month << "-"
       << std::setw(2) << (unsigned int)dateTime.day << "T"
       << std::setw(2) << (unsigned int)dateTime.hour << ":"
       << std::setw(2) << (unsigned int)dateTime.minutes << ":"
       << std::setw(2) << (unsigned int)dateTime.seconds;
    std::strcpy(dateTimeStr, ss.str().c_str());
}

inline void convertWithSnprintf(const DateTime& dateTime, char* dateTimeStr)
{
    char buf[dateTimeStrLen+6];
    snprintf(buf,
             dateTimeStrLen+6,
             dateTimeStrFormat,
             dateTime.year,
             dateTime.month,
             dateTime.day,
             dateTime.hour,
             dateTime.minutes,
             dateTime.seconds);

    std::strcpy(dateTimeStr, buf);
}

inline void convertWithToString(const DateTime& dateTime, char* dateTimeStr)
{
    std::string out;
    out.reserve(dateTimeStrLen);
    out.append(std::to_string(dateTime.year));
    out.append("-");
    out.append(std::to_string(dateTime.month));
    out.append("-");
    out.append(std::to_string(dateTime.day));
    out.append("T");
    out.append(std::to_string(dateTime.hour));
    out.append(":");
    out.append(std::to_string(dateTime.minutes));
    out.append(":");
    out.append(std::to_string(dateTime.seconds));
    std::strcpy(dateTimeStr, out.c_str());
}

inline void convertWithBoostFormat(const DateTime& dateTime, char* dateTimeStr)
{
    std::ostringstream ss;
    ss << boost::format(dateTimeStrFormat)
          % (unsigned int)dateTime.year
          % (unsigned int)dateTime.month
          % (unsigned int)dateTime.day
          % (unsigned int)dateTime.hour
          % (unsigned int)dateTime.minutes
          % (unsigned int)dateTime.seconds;
    std::strcpy(dateTimeStr, ss.str().c_str());
}

inline void convertWithConstExprLookupTables(const DateTime& dateTime, char* dateTimeStr)
{
    static constexpr auto yearLookup = IntToConstCharTable<0,9999, 4>();
    static constexpr auto monthLookup = IntToConstCharTable<1,12, 2> ();
    static constexpr auto dayLookup = IntToConstCharTable<1,31, 2> ();
    static constexpr auto hourLookup = IntToConstCharTable<0,23, 2> ();
    static constexpr auto minSecLookup = IntToConstCharTable<0,59, 2> ();

    std::strncpy(dateTimeStr+indexYear, yearLookup.get(dateTime.year), lenYear);
    dateTimeStr[indexYear+lenYear] = '-';
    std::strncpy(dateTimeStr+indexMonth, monthLookup.get(dateTime.month), lenMonth);
    dateTimeStr[indexMonth+lenMonth] = '-';
    std::strncpy(dateTimeStr+indexDay, dayLookup.get(dateTime.day), lenDay);
    dateTimeStr[indexDay+lenDay] = 'T';
    std::strncpy(dateTimeStr+indexHour, hourLookup.get(dateTime.hour), lenHour);
    dateTimeStr[indexHour+lenHour] = ':';
    std::strncpy(dateTimeStr+indexMin, minSecLookup.get(dateTime.minutes), lenMin);
    dateTimeStr[indexMin+lenMin] = ':';
    std::strncpy(dateTimeStr+indexSec, minSecLookup.get(dateTime.seconds), lenSec);
    dateTimeStr[indexSec+lenSec] = 0;
}

template <size_t len>
inline void fastCpy(char * dst, const char * src);

template<>
inline void fastCpy<1>(char * dst, const char * src)
{
    dst[0] = src[0];
}

template<>
inline void fastCpy<2>(char * dst, const char * src)
{
    ((short *)dst)[0] = ((short *)src)[0];
}

template<>
inline void fastCpy<4>(char * dst, const char * src)
{
    ((int *)dst)[0] = ((int *)src)[0];
}

inline void convertWithConstExprLookupTablesAndFastCopy(const DateTime& dateTime, char* dateTimeStr)
{
    static constexpr auto yearLookup = IntToConstCharTable<0,9999, 4>();
    static constexpr auto monthLookup = IntToConstCharTable<1,12, 2> ();
    static constexpr auto dayLookup = IntToConstCharTable<1,31, 2> ();
    static constexpr auto hourLookup = IntToConstCharTable<0,23, 2> ();
    static constexpr auto minSecLookup = IntToConstCharTable<0,59, 2> ();

    fastCpy<lenYear>(dateTimeStr+indexYear, yearLookup.get(dateTime.year));
    dateTimeStr[indexYear+lenYear] = '-';
    fastCpy<lenMonth>(dateTimeStr+indexMonth, monthLookup.get(dateTime.month));
    dateTimeStr[indexMonth+lenMonth] = '-';
    fastCpy<lenDay>(dateTimeStr+indexDay, dayLookup.get(dateTime.day));
    dateTimeStr[indexDay+lenDay] = 'T';
    fastCpy<lenHour>(dateTimeStr+indexHour, hourLookup.get(dateTime.hour));
    dateTimeStr[indexHour+lenHour] = ':';
    fastCpy<lenMin>(dateTimeStr+indexMin, minSecLookup.get(dateTime.minutes));
    dateTimeStr[indexMin+lenMin] = ':';
    fastCpy<lenSec>(dateTimeStr+indexSec, minSecLookup.get(dateTime.seconds));
    dateTimeStr[indexSec+lenSec] = 0;
}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
{

    std::chrono::high_resolution_clock::time_point timePoint = std::chrono::high_resolution_clock::now();
    std::cout << "Generating " << numberOfDateTimes << " datetime structs..." << std::endl;
    for(DateTime& dateTime : dateTimes)
    {
        dateTime = getRandomDateTime();
    }
    double genTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << genTime << " s\n"
              << "(" << genTime / numberOfDateTimes * 1000000000 << " ns per signle date time)"<< std::endl;

    std::cout << "\nConverting with sstream..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertStringStream(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double ssTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << ssTime << " s\n"
              << "(" << ssTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    std::cout << "\nConverting with snprintf..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertWithSnprintf(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double snTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << snTime << " s\n"
              << "(" << snTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    std::cout << "\nConverting with std::to_string (no leading zeroes) ..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertWithToString(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double toStrTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << toStrTime << " s\n"
              << "(" << toStrTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    std::cout << "\nConverting with boost::format ..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertWithBoostFormat(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double boostFmtTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << boostFmtTime << " s\n"
              << "(" << boostFmtTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    std::cout << "\nConverting with lookup tables ..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertWithConstExprLookupTables(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double lookUpTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << lookUpTime << " s\n"
              << "(" << lookUpTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    std::cout << "\nConverting with lookup tables and fast copy optimization..." << std::endl;
    for(size_t cnt = 0; cnt < numberOfDateTimes; ++ cnt)
    {
        convertWithConstExprLookupTables(dateTimes[cnt], dateTimeStrArray[cnt]);
    }
    double lookUpFastTime = getTimeInMicroseconds(timePoint);
    std::cout << "Done in " << std::fixed << lookUpFastTime << " s\n"
              << "(" << lookUpFastTime / numberOfDateTimes * 1000000000 << " ns per conversion)"<< std::endl;
    std::cout << "First datetime is: " << dateTimeStrArray[0] << std::endl;

    return 0;
}
