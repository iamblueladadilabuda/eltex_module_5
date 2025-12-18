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
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x63cd0125, "remove_proc_entry" },
	{ 0x37a0cba, "kfree" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x2dbde678, "proc_create" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x8da0819, "kmalloc_caches" },
	{ 0xd0c3484c, "kmalloc_trace" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D54CC14FF433279629FC322");
