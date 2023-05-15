/**
 * Copyright (c) ????		Jochen Schäuble <psionic@psionic.de>
 * Copyright (c) 2003-2004	Joern Engel <joern@wh.fh-wedel.de>
 *
 * Usage:
 *
 * one commend line parameter per device, each in the form:
 *   phram=<name>,<start>,<len>
 * <name> may be up to 63 characters.
 * <start> and <len> can be octal, decimal or hexadecimal.  If followed
 * by "ki", "Mi" or "Gi", the numbers will be interpreted as kilo, mega or
 * gigabytes.
 *
 * Example:
 *	phram=swap,64Mi,128Mi phram=test,900Mi,1Mi
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <asm/io.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>

struct phram_mtd_list {
	struct mtd_info mtd;
	struct list_head list;
};

static LIST_HEAD(phram_list);


static int phram_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	u_char *start = mtd->priv;

	if (instr->addr + instr->len > mtd->size)
		return -EINVAL;

	memset(start + instr->addr, 0xff, instr->len);

	/* This'll catch a few races. Free the thing before returning :)
	 * I don't feel at all ashamed. This kind of thing is possible anyway
	 * with flash, but unlikely.
	 */

	instr->state = MTD_ERASE_DONE;

	mtd_erase_callback(instr);

	return 0;
}

static int phram_point(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, void **virt, resource_size_t *phys)
{
	if (from + len > mtd->size)
		return -EINVAL;

	/* can we return a physical address with this driver? */
	if (phys)
		return -EINVAL;

	*virt = mtd->priv + from;
	*retlen = len;
	return 0;
}

static void phram_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
}

static int phram_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	u_char *start = mtd->priv;

	if (from >= mtd->size)
		return -EINVAL;

	if (len > mtd->size - from)
		len = mtd->size - from;

	memcpy(buf, start + from, len);

	*retlen = len;
	return 0;
}

static int phram_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	u_char *start = mtd->priv;

	if (to >= mtd->size)
		return -EINVAL;

	if (len > mtd->size - to)
		len = mtd->size - to;

	memcpy(start + to, buf, len);

	*retlen = len;
	return 0;
}



static void unregister_devices(void)
{
	struct phram_mtd_list *this, *safe;

	list_for_each_entry_safe(this, safe, &phram_list, list) {
		del_mtd_device(&this->mtd);
		iounmap(this->mtd.priv);
		kfree(this);
	}
}

static int register_device(char *name, unsigned long start, unsigned long len)
{
	struct phram_mtd_list *new;
	int ret = -ENOMEM;

	new = kzalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		goto out0;

	ret = -EIO;
	new->mtd.priv = ioremap(start, len);
	if (!new->mtd.priv) {
		pr_err("ioremap failed\n");
		goto out1;
	}


	new->mtd.name = name;
	new->mtd.size = len;
	new->mtd.flags = MTD_CAP_RAM;
        new->mtd.erase = phram_erase;
	new->mtd.point = phram_point;
	new->mtd.unpoint = phram_unpoint;
	new->mtd.read = phram_read;
	new->mtd.write = phram_write;
	new->mtd.owner = THIS_MODULE;
	new->mtd.type = MTD_RAM;
	new->mtd.erasesize = PAGE_SIZE;
	new->mtd.writesize = 1;

	ret = -EAGAIN;
	if (add_mtd_device(&new->mtd)) {
		pr_err("Failed to register new device\n");
		goto out2;
	}

	list_add_tail(&new->list, &phram_list);
	return 0;

out2:
	iounmap(new->mtd.priv);
out1:
	kfree(new);
out0:
	return ret;
}

static int ustrtoul(const char *cp, char **endp, unsigned int base)
{
	unsigned long result = simple_strtoul(cp, endp, base);

	switch (**endp) {
	case 'G':
		result *= 1024;
	case 'M':
		result *= 1024;
	case 'k':
		result *= 1024;
	/* By dwmw2 editorial decree, "ki", "Mi" or "Gi" are to be used. */
		if ((*endp)[1] == 'i')
			(*endp) += 2;
	}
	return result;
}

static int parse_num32(uint32_t *num32, const char *token)
{
	char *endp;
	unsigned long n;

	n = ustrtoul(token, &endp, 0);
	if (*endp)
		return -EINVAL;

	*num32 = n;
	return 0;
}

static int parse_name(char **pname, const char *token)
{
	size_t len;
	char *name;

	len = strlen(token) + 1;
	if (len > 64)
		return -ENOSPC;

	name = kmalloc(len, GFP_KERNEL);
	if (!name)
		return -ENOMEM;

	strcpy(name, token);

	*pname = name;
	return 0;
}


static inline void kill_final_newline(char *str)
{
	char *newline = strrchr(str, '\n');
	if (newline && !newline[1])
		*newline = 0;
}


#define parse_err(fmt, args...) do {	\
	pr_err(fmt , ## args);	\
	return 1;		\
} while (0)

static int phram_setup(const char *val, struct kernel_param *kp)
{
	char buf[64+12+12], *str = buf;
	char *token[3];
	char *name;
	uint32_t start;
	uint32_t len;
	int i, ret;

	if (strnlen(val, sizeof(buf)) >= sizeof(buf))
		parse_err("parameter too long\n");

	strcpy(str, val);
	kill_final_newline(str);

	for (i=0; i<3; i++)
		token[i] = strsep(&str, ",");

	if (str)
		parse_err("too many arguments\n");

	if (!token[2])
		parse_err("not enough arguments\n");

	ret = parse_name(&name, token[0]);
	if (ret)
		return ret;

	ret = parse_num32(&start, token[1]);
	if (ret) {
		kfree(name);
		parse_err("illegal start address\n");
	}

	ret = parse_num32(&len, token[2]);
	if (ret) {
		kfree(name);
		parse_err("illegal device length\n");
	}

	ret = register_device(name, start, len);
	if (!ret)
		pr_info("%s device: %#x at %#x\n", name, len, start);

	return ret;
}

module_param_call(phram, phram_setup, NULL, NULL, 000);
MODULE_PARM_DESC(phram, "Memory region to map. \"phram=<name>,<start>,<length>\"");

#if defined(HUMAX_PLATFORM_BASE)
#include <linux/limits.h>
#endif

static int __init init_phram(void)
{
#if defined(HUMAX_PLATFORM_BASE)
	char name[5];	
	uint32_t start;
	uint32_t len;
	char str_buffer[MAX_INPUT];
	char orig_str_buffer[MAX_INPUT];
	char *str;
	char *orig_str;
	unsigned long addr = 0, size = 0, dummy = 0;

	str = str_buffer;
	orig_str = orig_str_buffer;
	strlcpy(str, CONFIG_ROOTFS_ON_RAM_SIZE_START_ADDRESS, MAX_INPUT);
	strlcpy(orig_str, CONFIG_ROOTFS_ON_RAM_SIZE_START_ADDRESS, MAX_INPUT);
	
	size = (unsigned long)memparse(str, &str);
	if (*str == '@')
		addr = (unsigned long)memparse(str + 1, &str);

	if ((size > 0x40000000) || (addr > 0x80000000)) {
		printk(KERN_WARNING "bmem: argument '%s' "
			"is out of range, ignoring\n", orig_str);
		return 0;
	}

	if (size == 0) {
		printk(KERN_INFO "bmem: disabling reserved memory\n");
		return 0;
	}

	if ((addr & ~PAGE_MASK) || (size & ~PAGE_MASK)) {
		printk(KERN_WARNING "bmem: ignoring invalid range '%s' "
			"(is it missing an 'M' suffix?)\n", orig_str);
		return 0;
	}
	
	if (addr == 0) {
		printk(KERN_WARNING "bmem: '%s', addr is 0x%X \n", orig_str, addr);
		return 0;
	}

#if defined(CONFIG_ROOTFS_ON_RAM_DUMMY_BYTE)
	dummy += 4;
#endif
#if defined(CONFIG_ROOTFS_ON_RAM_CAS_NA_PAIRING_KEY)
	dummy += 1020;
#endif

	printk("rootfs on ram start address = 0x%X\n", addr+dummy);
	printk("rootfs on ram size = %d MB\n", ((size-dummy)/1024/1024));	

	name[0]='R';name[1]='A';name[2]='M';name[3]='0';name[4]=0;	
	start 	= addr+dummy;		/* RootFS start offset */
	len 	= size-dummy; 	/* RootFS size */
	printk("MTD %c%c%c%c created.\n", name[0], name[1], name[2], name[3]);	

	register_device(name, start, len);
	
#if (CONFIG_LXC_ON_RAM_SIZE)
	printk("MTD LXC created. %dM@0x%x\n", CONFIG_LXC_ON_RAM_SIZE, start + len);
	register_device("LXC", start + len, CONFIG_LXC_ON_RAM_SIZE * 1024 * 1024);
#endif
#endif	

	return 0;
}

static void __exit cleanup_phram(void)
{
	unregister_devices();
}

module_init(init_phram);
module_exit(cleanup_phram);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joern Engel <joern@wh.fh-wedel.de>");
MODULE_DESCRIPTION("MTD driver for physical RAM");
