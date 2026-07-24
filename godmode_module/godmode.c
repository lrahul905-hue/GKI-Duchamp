#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>

/* 1. Kill IRQ Overload Throttling */
static int pre_mtk_cpu_overutilized(struct kprobe *p, struct pt_regs *regs) {
    /* 
     * ARM64 mein arguments registers (x0, x1, x2...) mein aate hain.
     * param_3 (regs[2]) pointer hai jahan 'overutilized' flag store hota hai.
     * Hum ise zabardasti 0 (false) set kar rahe hain.
     */
    unsigned int *overutilized_flag = (unsigned int *)regs->regs[2];
    if (overutilized_flag) {
        *overutilized_flag = 0; 
    }
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
        pr_err("GodMode: Failed to hook mtk_cpu_overutilized (Error: %d)\n", ret);
        return ret;
    }
    pr_info("GodMode: Vendor limits hooked successfully! Throttling Disabled.\n");
    return 0;
}

static void __exit godmode_exit(void) {
    unregister_kprobe(&kp_overutil);
    pr_info("GodMode: Unloaded\n");
}

module_init(godmode_init);
module_exit(godmode_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("GodMode");
MODULE_DESCRIPTION("Bypass MediaTek EAS Limits via Kprobes");
