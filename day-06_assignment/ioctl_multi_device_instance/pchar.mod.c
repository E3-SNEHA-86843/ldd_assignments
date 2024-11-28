#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xeb233a45, "__kmalloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xc7efe48f, "class_create" },
	{ 0x123d089a, "device_create" },
	{ 0x3a079017, "cdev_init" },
	{ 0x19182dd7, "cdev_add" },
	{ 0x1c054136, "device_destroy" },
	{ 0xd1ff2714, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x139f2189, "__kfifo_alloc" },
	{ 0xfa0ae47e, "cdev_del" },
	{ 0xdb760f52, "__kfifo_free" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xe91439c9, "param_ops_int" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x30a80826, "__kfifo_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4578f528, "__kfifo_to_user" },
	{ 0x112425ee, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "645D355A570DB162E136752");
