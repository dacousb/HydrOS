#pragma once

#include <stdint.h>
#include <stivale2.h>

struct rsdp_ptr_1
{
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_addr;
} __attribute__((packed));

/* all the System Description Tables have the
 * following header */
struct sdt_header
{
    char signature[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct rsdt_ptr
{
    struct sdt_header sdt_head;
    uint32_t other_sdt_ptr[];
} __attribute__((packed));

/* ACPI describe registers as following */
struct gen_addr_struct
{
    uint8_t addr_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t addr;
};

/* simplified from https://wiki.osdev.org/FADT */
struct fadt_ptr
{
    struct sdt_header sdt_head;
    uint32_t firmware_ctrl;
    uint32_t dsdt_addr;

    uint8_t reserved_1;

    uint8_t power_man_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_control;
    uint32_t PM1a_event_block;
    uint32_t PM1b_event_block;
    uint32_t PM1a_control_block;
    uint32_t PM1b_control_block;
    uint32_t PM2_control_block;
    uint32_t PM_timer_block;
    uint32_t GPE0_block;
    uint32_t GPE1_block;
    uint8_t PM1_event_len;
    uint8_t PM1_control_len;
    uint8_t PM2_control_len;
    uint8_t PM_timer_len;
    uint8_t GPE0_len;
    uint8_t GPE1_len;
    uint8_t GPE1_base;
    uint8_t C_state_control;
    uint16_t worst_C2_latency;
    uint16_t worst_C3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;

    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    uint16_t reserved_boot_arch_flags; /* reserved in ACPI 1.0 */

    uint8_t reserved_2;
    uint32_t flags;

    struct gen_addr_struct reset_reg;

    uint8_t reset_value;
    uint8_t reserved_3[3];
} __attribute__((packed));

void init_acpi(struct stivale2_struct_tag_rsdp *rsdp_tag);
void acpi_shutdown();