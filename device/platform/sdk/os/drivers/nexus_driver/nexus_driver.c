#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/capability.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
#include <linux/brcmstb/brcmstb.h>
#else
#include <asm/brcmstb/brcmstb.h>
#endif

/************************************************
Common Define
*************************************************/
#define IOCTLDRIVER_MAGIC 'H'

#define IOCTLGPIO_READ						_IOR(IOCTLDRIVER_MAGIC, 1, ioctl_driver_info)
#define IOCTLGPIO_WRITE 					_IOW(IOCTLDRIVER_MAGIC, 2, ioctl_driver_info)
#define IOCTLPWM_GET_CTRL_WORD				_IOR(IOCTLDRIVER_MAGIC, 3, ioctl_driver_info)
#define IOCTLPWM_SET_CTRL_WORD				_IOW(IOCTLDRIVER_MAGIC, 4, ioctl_driver_info)
#define IOCTLPWM_GET_FREQ_MODE				_IOR(IOCTLDRIVER_MAGIC, 5, ioctl_driver_info)
#define IOCTLPWM_SET_FREQ_MODE				_IOW(IOCTLDRIVER_MAGIC, 6, ioctl_driver_info)
#define IOCTLPWM_GET_ONINTERVAL				_IOR(IOCTLDRIVER_MAGIC, 7, ioctl_driver_info)
#define IOCTLPWM_SET_ONINTERVAL				_IOW(IOCTLDRIVER_MAGIC, 8, ioctl_driver_info)
#define IOCTLPWM_GET_PERIOD_INTERVAL		_IOR(IOCTLDRIVER_MAGIC, 9, ioctl_driver_info)
#define IOCTLPWM_SET_PERIOD_INTERVAL		_IOW(IOCTLDRIVER_MAGIC, 10, ioctl_driver_info)
#define IOCTLPWM_START						_IOW(IOCTLDRIVER_MAGIC, 11, ioctl_driver_info)
#define IOCTLPWM_STOP						_IOW(IOCTLDRIVER_MAGIC, 12, ioctl_driver_info)
#define IOCTLPWM_OPEN						_IOW(IOCTLDRIVER_MAGIC, 13, ioctl_driver_info)

#define IOCTLDRIVER_MAXNR	14

#define IOCTLDRIVER_DEV_NAME		"di_so"
#define IOCTLDRIVER_DEV_MAJOR		33

typedef enum PWM_FreqModeType
{
	Variable_Freq_Mode = 0,
	Constant_Freq_Mode = 1
}PWM_FreqModeType;

typedef struct
{
	unsigned int addr;
	unsigned int value;
}__attribute__((packed)) ioctl_driver_info;

static void REG_Write32(unsigned int reg, unsigned int data)
{
    *((volatile uint32_t *)(0x90000000+reg)) = data;
}

static unsigned int REG_Read32(uint32_t reg)
{
    return *((volatile uint32_t *)(0x90000000+reg));
}

static int driver_gpio_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int driver_pwm_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static int driver_gpio_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int driver_pwn_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static long driver_gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	ioctl_driver_info 	ctrl_info;
	int					err, size;

	if (!capable(CAP_SYS_RAWIO))
		return -EPERM;

	if(_IOC_TYPE(cmd) != IOCTLDRIVER_MAGIC)
		return -EINVAL;
	if(_IOC_NR(cmd) >= IOCTLDRIVER_MAXNR)
		return -EINVAL;
	
	size = _IOC_SIZE(cmd);
		
	if(size){
		err = 0;
		copy_from_user((void *)&ctrl_info, (const void *)arg, size);

		//err = gpio_verify_area(ctrl_info.addr);
		//if(err != 0) 
		//	return -EINVAL;
	}
	else{
		return -EINVAL;
	}

	switch(cmd){
		case IOCTLGPIO_READ :
			ctrl_info.value = REG_Read32(ctrl_info.addr);
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLGPIO_WRITE :
			REG_Write32(ctrl_info.addr, ctrl_info.value);
			ctrl_info.value = IOCTLGPIO_WRITE;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		default :
			ctrl_info.value = 0xffff;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			return -EINVAL;
	}

	return 0;
}

static long driver_pwm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	ioctl_driver_info 	ctrl_info;
	int					size;
	unsigned int  		lsb, msb, lval;

	if (!capable(CAP_SYS_RAWIO))
		return -EPERM;

	if(_IOC_TYPE(cmd) != IOCTLDRIVER_MAGIC)
		return -EINVAL;
	if(_IOC_NR(cmd) >= IOCTLDRIVER_MAXNR)
		return -EINVAL;
	
	size = _IOC_SIZE(cmd);
		
	switch(cmd){
		case IOCTLPWM_GET_CTRL_WORD :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			msb = REG_Read32(BCHP_PWM_PWM1_CWORD_MSB);
			lsb = REG_Read32(BCHP_PWM_PWM1_CWORD_LSB);
			ctrl_info.value = ((msb << 8) | (lsb & 0xff));
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_SET_CTRL_WORD :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = (unsigned int)(ctrl_info.value >> 8);
			REG_Write32(BCHP_PWM_PWM1_CWORD_MSB, lval);
			lval = (unsigned int)(ctrl_info.value & 0xff);
			REG_Write32(BCHP_PWM_PWM1_CWORD_LSB, lval);
			ctrl_info.value = IOCTLPWM_SET_CTRL_WORD;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_GET_FREQ_MODE :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = REG_Read32(BCHP_PWM_CTRL2);
			if (lval & BCHP_PWM_CTRL2_pwm1_out_select_MASK)
				ctrl_info.value = Constant_Freq_Mode;
			else
				ctrl_info.value = Variable_Freq_Mode;
				
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_SET_FREQ_MODE :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = REG_Read32(BCHP_PWM_CTRL2);
			if (ctrl_info.value == Variable_Freq_Mode){
				lval |= BCHP_PWM_CTRL2_pwm1_out_select_MASK;
				lval &=  ~BCHP_PWM_CTRL2_pwm1_out_select_MASK;
			}
			else{
				lval &= ~BCHP_PWM_CTRL2_pwm1_out_select_MASK;
				lval |= BCHP_PWM_CTRL2_pwm1_out_select_MASK;
			}
			REG_Write32(BCHP_PWM_CTRL2, lval);
			ctrl_info.value = IOCTLPWM_SET_FREQ_MODE;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_GET_ONINTERVAL :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = REG_Read32(BCHP_PWM_PWM1_ON);
			ctrl_info.value = (lval & BCHP_PWM_PWM1_ON_pwm1_on_MASK);
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_SET_ONINTERVAL :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = (ctrl_info.value & BCHP_PWM_PWM1_ON_pwm1_on_MASK);
			REG_Write32(BCHP_PWM_PWM1_ON, lval);
			ctrl_info.value = IOCTLPWM_SET_ONINTERVAL;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_GET_PERIOD_INTERVAL :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval= REG_Read32(BCHP_PWM_PWM1_PERIOD);
			ctrl_info.value = (lval & BCHP_PWM_PWM1_PERIOD_pwm1_period_MASK);
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_SET_PERIOD_INTERVAL :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = (uint32_t)(ctrl_info.value & BCHP_PWM_PWM1_PERIOD_pwm1_period_MASK);
			REG_Write32(BCHP_PWM_PWM1_PERIOD, lval);
			ctrl_info.value = IOCTLPWM_SET_PERIOD_INTERVAL;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_START :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = REG_Read32(BCHP_PWM_CTRL);
			lval |= BCHP_PWM_CTRL_pwm1_start_MASK;
			REG_Write32(BCHP_PWM_CTRL, lval);
			ctrl_info.value = IOCTLPWM_START;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_STOP :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval = REG_Read32(BCHP_PWM_CTRL);
			lval &= ~BCHP_PWM_CTRL_pwm1_start_MASK;
			REG_Write32(BCHP_PWM_CTRL, lval);
			ctrl_info.value = IOCTLPWM_STOP;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		case IOCTLPWM_OPEN :
			copy_from_user((void *)&ctrl_info, (const void *)arg, size);
			lval |= BCHP_PWM_CTRL_pwm1_opendrainb_MASK;
			lval &= ~BCHP_PWM_CTRL_pwm1_oeb_MASK;
			REG_Write32(BCHP_PWM_CTRL, lval);
			lval |= BCHP_PWM_CTRL2_pwm1_out_select_MASK;
			lval &=  ~BCHP_PWM_CTRL2_pwm1_out_select_MASK;
			REG_Write32(BCHP_PWM_CTRL2, lval);
			ctrl_info.value = IOCTLPWM_OPEN;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			break;

		default :
			ctrl_info.value = 0xffff;
			copy_to_user((void *) arg, (const void *)&ctrl_info, (unsigned long)size);
			return -EINVAL;
		
	}

	return 0;
}

struct file_operations driver_gpio_fops = 
{
	.owner		= THIS_MODULE,
	.unlocked_ioctl		= driver_gpio_ioctl,
	.open		= driver_gpio_open,
	.release	= driver_gpio_release,
};

struct file_operations driver_pwn_fops = 
{
	.owner		= THIS_MODULE,
	.unlocked_ioctl		= driver_pwm_ioctl,
	.open		= driver_pwm_open,
	.release	= driver_pwn_release,
};

static int driver_open(struct inode *inode, struct file *filp)
{
	switch(MINOR(inode->i_rdev))
	{
		case 23 : filp->f_op = &driver_gpio_fops; break;
		case 25 : filp->f_op = &driver_pwn_fops; break;
		default : return -ENXIO;
	}

	if(filp->f_op && filp->f_op->open)
		return filp->f_op->open(inode, filp);

	return 0;
}

struct file_operations driver_fops = 
{
	.owner		= THIS_MODULE,
	.open		= driver_open,
};

static int driver_gpio_init(void)
{
	int rc;
	
	rc = register_chrdev(IOCTLDRIVER_DEV_MAJOR, IOCTLDRIVER_DEV_NAME, &driver_fops);

	if(rc < 0) return rc;

	return 0;
/*
	unsigned long stvalue = *((volatile uint32_t *)(0xb0000000 + BCHP_SUN_TOP_CTRL_STRAP_VALUE_0));
	printk("hello world, strap value = %x\n", stvalue);
	return 0;
*/
}

static void driver_gpio_exit(void)
{
	unregister_chrdev(IOCTLDRIVER_DEV_MAJOR, IOCTLDRIVER_DEV_NAME);
}

module_init(driver_gpio_init);
module_exit(driver_gpio_exit);

MODULE_LICENSE("HUMAX Co.Ltd");
