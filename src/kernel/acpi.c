#include <kernel/acpi.h>
#include <kernel/panic.h>
#include <kernel/ports.h>

#include <string/string.h>

/* ACPI is a power management standard and its information is in the BIOS memory
 *
 * to begin using ACPI we need to find the RSDP (Root System Description Pointer),
 * the RSDP is a data structure where we will find information about
 * 
 *  - ACPI version or revision (1.0 or 2.0) that the machine uses
 *  - the RSDT table address (refer to https://wiki.osdev.org/RSDT)
 *  - the XSDT table address (only for ACPI 2.0)
 * 
 * the RSDT is a data structure also used in the ACPI that contains pointers to
 * the other System Description Tables (that also contain information about the system)
 */

/* limine, the bootloader we are using, uses the stivale protocol, we can easily
 * get the RSDP pointer
 *
 * from https://github.com/stivale/stivale/blob/master/STIVALE2.md#rsdp-structure-tag
 *
 *   struct stivale2_struct_tag_rsdp {
 *       struct stivale2_tag tag;    // Identifier: 0x9e1786930a375e78
 *       uint64_t rsdp;              // Pointer to the ACPI RSDP structure
 *   };
 */

static struct rsdp_ptr_1 *rsdp = 0;
static struct rsdt_ptr *rsdt = 0;
static struct fadt_ptr *fadt = 0;

/* the FADT data structure is the ACPI table that contains information
 * about fixed register blocks that have to do with power management
 *
 * as we explained before, the RSDT contains other tables, one of them will
 * be the FADT, keep in mind the FADT signature is FACP
 */
struct fadt_ptr *get_fadt()
{
    int entries = (rsdt->sdt_head.len - sizeof(rsdt->sdt_head)) / 4;
    for (int i = 0; i < entries; i++)
    {
        struct sdt_header *header = (struct sdt_header *)(uint64_t)rsdt->other_sdt_ptr[i];
        /* the signature seems not to be a NULL terminated string, so we need to compare
         * a fixed amount of characters */
        if (!strncmp(header->signature, "FACP", 4))
            return (struct fadt_ptr *)header;
    }
    return 0;
}

uint64_t get_SLP_TYPa()
{
    /* this is not a good idea at all, since it is a hardcoded version
     * of AML (ACPI Machine Language) parsing (see https://wiki.osdev.org/AML) */
    struct sdt_header *dsdt_addr = (struct sdt_header *)(uint64_t)fadt->dsdt_addr;

    /* if the signature is not DSDT */
    if (strncmp(dsdt_addr->signature, "DSDT", 4))
        panic("[ACPI] DSDT header not found\n");

    char *S5_addr = (char *)(dsdt_addr + sizeof(struct sdt_header));
    int dsdt_len = dsdt_addr->len;
    while (dsdt_len-- > 0)
    {
        if (!strncmp(S5_addr, "_S5_", 4))
            break; /* found it */
        S5_addr++; /* or move the pointer */
    }
    if (dsdt_len <= 0)
        panic("[ACPI] S5 not present\n");

    S5_addr += 5;
    S5_addr += ((*S5_addr & 0xC0) >> 6) + 2;
    if (*S5_addr == 0xA) /* byte prefix */
        S5_addr++;
    return *(S5_addr) << 10;
}

void acpi_shutdown()
{
    port_word_out(fadt->PM1a_control_block, (get_SLP_TYPa()) | (1 << 13));
    port_word_out(0xB004, 0x2000); /* Bochs and old versions of QEMU */
    port_word_out(0x604, 0x2000);  /* QEMU                           */
    port_word_out(0x4004, 0x3400); /* VBox                           */

    panic("[ACPI] shutdown failed\n");
}

void init_acpi(struct stivale2_struct_tag_rsdp *rsdp_tag)
{
    rsdp = (struct rsdp_ptr_1 *)rsdp_tag->rsdp;
    rsdt = (struct rsdt_ptr *)(uint64_t)rsdp->rsdt_addr;
    fadt = get_fadt();

    if (!rsdp)
        panic("[ACPI] rsdp lookup failed\n");
    else if (!rsdt)
        panic("[ACPI] rsdt lookup failed\n");
    else if (!fadt)
        panic("[ACPI] fadt lookup failed\n");

    /* print their memory addresses */
    kprintf("[ACPI] rsdp = 0x%x, rsdt = 0x%x, fadt = 0x%x\n", rsdp, rsdt, fadt);
}