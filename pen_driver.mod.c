#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf6628fc9, "module_layout" },
	{ 0x3ac8c215, "usb_deregister" },
	{ 0x63afefa5, "usb_register_driver" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x9b5840be, "usb_bulk_msg" },
	{ 0x27e1a049, "printk" },
	{ 0x794f67ca, "usb_register_dev" },
	{ 0x7dfe34c0, "usb_deregister_dev" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v05DCpA838d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "93C1C165F13F3C1D0E89F15");
