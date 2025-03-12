//
// Created by Administrator on 2022/10/26 0026.
//
#include <csos/time.h>
#include <csos/stdlib.h>
#include <rtc.h>
#include <logf.h>

#define MINUTE 60          // 每分钟的秒数
#define HOUR (60 * MINUTE) // 每小时的秒数
#define DAY (24 * HOUR)    // 每天的秒数
#define YEAR (365 * DAY)   // 每年的秒数，以 365 天算

static int month_days[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 每个月开始时的已经过去天数
static int month[13] = {
    0, // 这里占位，没有 0 月，从 1 月开始
    0,
    (31),
    (31 + 29),
    (31 + 29 + 31),
    (31 + 29 + 31 + 30),
    (31 + 29 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)};

time_t startup_time;
int century;

time_t mktime(tm *time)
{
    time_t res;
    // year = time->tm_year + 100 - 70;
    int year = time->tm_year + 30;
    // 这些年经过的秒数时间
    res = YEAR * year;

    // 已经过去的闰年，每个加 1 天
    res += DAY * ((year + 1) / 4);

    // 已经过完的月份的时间
    res += month[time->tm_mon] * DAY;

    // 如果 2 月已经过了，并且当前不是闰年，那么减去一天
    if (time->tm_mon > 2 && ((year + 2) % 4))
        res -= DAY;

    // 这个月已经过去的天
    res += DAY * (time->tm_mday - 1);

    // 今天过去的小时
    res += HOUR * time->tm_hour;

    // 这个小时过去的分钟
    res += MINUTE * time->tm_min;

    // 这个分钟过去的秒
    res += time->tm_sec;

    return res;
}

void time_read_bcd(tm *time)
{
    do
    {
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_min = cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_wday = cmos_read(CMOS_WEEKDAY);
        time->tm_mday = cmos_read(CMOS_DAY);
        time->tm_mon = cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);
        century = cmos_read(CMOS_CENTURY);
    } while (time->tm_sec != cmos_read(CMOS_SECOND));
}

void time_read(tm *time, int timezone)
{
    time_read_bcd(time);
    time->tm_sec = bcd_to_bin(time->tm_sec);
    time->tm_min = bcd_to_bin(time->tm_min);
    time->tm_hour = bcd_to_bin(time->tm_hour) + timezone;
    time->tm_wday = bcd_to_bin(time->tm_wday);
    time->tm_mday = bcd_to_bin(time->tm_mday);
    if (time->tm_hour >= 24)
    {
        time->tm_wday++;
        time->tm_mday++;
        time->tm_hour %= 24;
    }

    if (time->tm_wday >= 8)
    {
        time->tm_wday %= 8;
    }
    time->tm_mon = bcd_to_bin(time->tm_mon);
    int mdays = month_days[time->tm_mon];
    if (time->tm_mday > mdays)
    {
        time->tm_mday -= mdays;
        time->tm_mon++;
    }
    time->tm_year = bcd_to_bin(time->tm_year);
    if (time->tm_mon > 12)
    {
        time->tm_year++;
    }
    time->tm_isdst = -1;
    century = bcd_to_bin(century);
    if (time->tm_year > 99)
    {
        century++;
        time->tm_year %= 100;
    }
}

extern uint32_t int_count0;
extern uint32_t int_count;
extern uint32_t count_rtc;

static tm tmw[5 / 2];
static uint32_t tmwi[sizeof(tmw) / sizeof(tmw[0])];
static uint32_t tmwi2[sizeof(tmw) / sizeof(tmw[0])];
static uint32_t i = 0;
static uint32_t maxint = 0;
uint32_t isRead = 0;
void time_init23()
{
    tm time;
    if (isRead==1)
    {
        time_read(&time, OS_TZ);
    }

    if (i >= sizeof(tmw) / sizeof(tmw[0]))
    {
        i = 0;
    }

    tmw[i++] = time;
    tmwi[i - 1] = int_count0;
    tmwi2[i - 1] = count_rtc;
    if (maxint < i && maxint < sizeof(tmw) / sizeof(tmw[0]))
    {
        maxint = i;
    }
}

static uint32_t count = 0;
void printw()
{
    tm t = tmw[0];
    uint32_t j = maxint;
    uint32_t c = 0;
    logf("isread:%u\tcount:%d\t,maxint:%d\n",isRead, count, j);
    uint32_t ac = 0;
    uint32_t ac2 = 0;
    for (uint32_t i = 0; i < j; i++)
    {
        c++;
        ac += tmwi[i];
        ac2 = tmwi2[i];
        tm time = tmw[i];
        int kernel_memcmp(void *v1, void *v2, uint32_t size);
        if (i == 0 || i + 1 == j || (kernel_memcmp(&time, &t, sizeof(time)) != 0))
        {
            // if(time!=t){
            logf("%d\tSTARTUP TIME: %d%d-%02d-%02d %02d:%02d:%02d\t%u\tac2:%u\tintc:%u", i, century, time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour,
                 time.tm_min,
                 time.tm_sec, c,ac2, ac);
            t = time;
            c = 0;
            ac = 0;
        }
    }
}

void time_init2()
{
    int_count++;
    return;
    // time_init23();
    // tm time;
    if((  count++ )%1000==0){
        // time_read(&time,OS_TZ );
        time_init23();
    }
}

void time_log_rtc()
{
    // isRead = 1;
    // isRead = 0;
    time_init23();
}

void printw_rtc()
{
    tm time;
    if (isRead == 0)
    {
        time_read(&time, OS_TZ);
        for (uint32_t i = 0; i < maxint; i++)
        {
            if (time.tm_sec >= 60)
            {
                time.tm_sec = 0;
            }
            time.tm_sec++;
            tmw[i] = time;
            /* code */
        }
    }
    void printlog();

    printlog();
    printw();
}

void time_init(int timezone)
{
    tm time;
    time_read(&time, timezone);
    startup_time = mktime(&time);
    logf("STARTUP TIME: %d%d-%02d-%02d %02d:%02d:%02d",
         century,
         time.tm_year,
         time.tm_mon,
         time.tm_mday,
         time.tm_hour,
         time.tm_min,
         time.tm_sec);
}