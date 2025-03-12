#include <rtc.h>
#include <pic.h>
#include <tty.h>
#include <kernel.h>
#include <interrupt.h>
#include <csos/stdlib.h>
#include <csos/time.h>

uint8_t cmos_read(uint8_t addr)
{
    outb(CMOS_ADDR, CMOS_NMI | addr);
    return inb(CMOS_DATA);
}

void cmos_write(uint8_t addr, uint8_t value)
{
    outb(CMOS_ADDR, CMOS_NMI | addr);
    outb(CMOS_DATA, value);
}

static uint32_t volatile counter = 0;

extern uint32_t count_rtc;
extern uint32_t int_count;
uint32_t log[5];
const uint32_t loglen = sizeof(log) / sizeof(log[0]);
uint32_t logi = 0;
#include <logf.h>
void rtcinit(){
    logi = 0;
}

void printlog()
{
    uint32_t len = logi;
    for (uint32_t i = 2; i < len - 2 && i < loglen - 2; i += 2)
    {
        logf("i:%u\ttime:%u\tint:%u===>cost:%u\thz:%u\n", i / 2, log[i], log[i + 1],
             log[i] - log[i - 2],
             log[i + 1] - log[i + 1 - 2]);
    }
}

void handler_rtc(interrupt_frame_t *frame)
{
    send_eoi(IRQ1_RTC);
    uint8_t vc = cmos_read(CMOS_C);
    // create_alarm(1);

    if (vc & CMOS_C_AF)
        tty_printf("A");
    else if (vc & CMOS_C_PF)
    {
        count_rtc++;
        if (logi < loglen - 2)
        {
            log[logi++] = count_rtc;
            log[logi++] = int_count;
        }
        // tty_printf(".");
        // int_count = 0;
    }

    return;

    int dd = 2;
    dd++;
    while (1)
    {
        dd++;
        dd++;
        dd++;
        /* code */
    }
}

void create_alarm(uint32_t value)
{
    tm time;
    time_read(&time, OS_TZ);

    uint8_t sec = value % 60;
    value /= 60;
    uint8_t min = value % 60;
    value /= 60;
    uint32_t hour = value;

    time.tm_sec += sec;
    if (time.tm_sec >= 60)
    {
        time.tm_sec %= 60;
        time.tm_min += 1;
    }

    time.tm_min += min;
    if (time.tm_min >= 60)
    {
        time.tm_min %= 60;
        time.tm_hour += 1;
    }

    time.tm_hour += (int)hour;
    if (time.tm_hour >= 24)
    {
        time.tm_hour %= 24;
    }

    cmos_write(CMOS_HOUR, bin_to_bcd(time.tm_hour - OS_TZ));
    cmos_write(CMOS_MINUTE, bin_to_bcd(time.tm_min));
    cmos_write(CMOS_SECOND, bin_to_bcd(time.tm_sec));
}

void rtc_init()
{
    // 周期、闹钟中断
    uint8_t prev = cmos_read(CMOS_B);
    cmos_write(CMOS_B, prev | CMOS_B_24HOUR | CMOS_B_AIE | CMOS_B_PIE);
    cmos_read(CMOS_C);
    // 设置闹钟
    create_alarm(1);
    // 设置中断频率
    prev = cmos_read(CMOS_A);
    // cmos_write(CMOS_A, (prev & 0xF0) | 0b1001);
    cmos_write(CMOS_A, (prev & 0xF0) | 0b1110);

    install_interrupt_handler(IRQ1_RTC, (uint32_t)interrupt_handler_rtc);
    irq_enable(IRQ1_RTC);
    irq_enable(IRQ0_CASCADE);
}