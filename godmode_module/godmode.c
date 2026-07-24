#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h> // For pt_regs

/* 1. Kill IRQ Overload Throttling */
static int pre_mtk_cpu_overutilized(struct kprobe *p, struct pt_regs *regs) {
    // Ghidra dump ke hisaab se param_3 (regs->regs[2] in ARM64) pointer hai jisme result store hota hai
    // Hum overutilized flag ko force 0 (false) kar rahe hain
    unsigned int *overutilized_flag = (unsigned int *)regs->regs[2];
    *overutilized_flag = 0; 
    
    // Original function ko skip karne ke liye (agar zarurat ho)
    return 0; 
}

static struct kprobe kp_overutil = {
    .symbol_name = "mtk_cpu_overutilized",
    .pre_handler = pre_mtk_cpu_overutilized,
};

/* Module Init */
static int __init godmode_init(void) {
    int ret;
    ret = register_kprobe(&kp_overutil);
    if (ret < 0) {
        pr_err("GodMode: Failed to hook mtk_cpu_overutilized\n");
        return ret;
    }
    pr_info("GodMode: Vendor functions hooked successfully without vendor source!\n");
    return 0;
}

static void __exit godmode_exit(void) {
    unregister_kprobe(&kp_overutil);
    pr_info("GodMode: Unloaded\n");
}

module_init(godmode_init);
module_exit(godmode_exit);
MODULE_LICENSE("GPL");
