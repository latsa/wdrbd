﻿#include "drbd_windrv.h"	/// SEO:
#include "wsk2.h"		/// SEO:
#include "drbd_wingenl.h"	/// SEO:
#include "idr.h"
#include "../user/libgenl.h"
#include "Drbd_int.h"

extern int drbd_tla_parse(struct nlmsghdr *nlh);

struct genl_ops {
	int   (*doit)(struct sk_buff *skb, struct genl_info *info);
	u8    cmd;
};

extern int drbd_adm_add_minor(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_delete_minor(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_new_resource(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_del_resource(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_down(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_set_role(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_attach(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_disk_opts(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_detach(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_connect(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_net_opts(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_resize(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_start_ov(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_new_c_uuid(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_disconnect(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_invalidate(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_invalidate_peer(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_pause_sync(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_resume_sync(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_suspend_io(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_resume_io(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_outdate(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_resource_opts(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_get_status(struct sk_buff *skb, struct genl_info *info);
extern int drbd_adm_get_timeout_type(struct sk_buff *skb, struct genl_info *info);
/* .dumpit */
extern int drbd_adm_get_status_all(struct sk_buff *skb, struct netlink_callback *cb);
extern void drbd_adm_send_reply(struct sk_buff *skb, struct genl_info *info);

extern int _drbd_adm_get_status(struct sk_buff *skb, struct genl_info * pinfo);

static struct genl_ops drbd_genl_ops[]  = {
	{ _drbd_adm_get_status, DRBD_ADM_GET_STATUS,},
	{ drbd_adm_add_minor, DRBD_ADM_NEW_MINOR,},
	{ drbd_adm_delete_minor, DRBD_ADM_DEL_MINOR,},
	{ drbd_adm_new_resource, DRBD_ADM_NEW_RESOURCE,},
	{ drbd_adm_del_resource, DRBD_ADM_DEL_RESOURCE,},
	{ drbd_adm_resource_opts, DRBD_ADM_RESOURCE_OPTS,},
	{ drbd_adm_connect, DRBD_ADM_CONNECT,},
	{ drbd_adm_net_opts, DRBD_ADM_CHG_NET_OPTS,},
	{ drbd_adm_disconnect, DRBD_ADM_DISCONNECT,},
	{ drbd_adm_attach, DRBD_ADM_ATTACH,},
	{ drbd_adm_disk_opts, DRBD_ADM_CHG_DISK_OPTS,},
	{ drbd_adm_resize, DRBD_ADM_RESIZE,},
	{ drbd_adm_set_role, DRBD_ADM_PRIMARY,},
	{ drbd_adm_set_role, DRBD_ADM_SECONDARY,},
	{ drbd_adm_new_c_uuid, DRBD_ADM_NEW_C_UUID,},
	{ drbd_adm_start_ov, DRBD_ADM_START_OV,},
	{ drbd_adm_detach, DRBD_ADM_DETACH,},
	{ drbd_adm_invalidate, DRBD_ADM_INVALIDATE,},
	{ drbd_adm_invalidate_peer, DRBD_ADM_INVAL_PEER,},
	{ drbd_adm_pause_sync, DRBD_ADM_PAUSE_SYNC,},
	{ drbd_adm_resume_sync, DRBD_ADM_RESUME_SYNC,},
	{ drbd_adm_suspend_io, DRBD_ADM_SUSPEND_IO,},
	{ drbd_adm_resume_io, DRBD_ADM_RESUME_IO,},
	{ drbd_adm_outdate, DRBD_ADM_OUTDATE,},
	{ drbd_adm_get_timeout_type, DRBD_ADM_GET_TIMEOUT_TYPE,},
	{ drbd_adm_down, DRBD_ADM_DOWN,},
};

static const char *drbd_genl_cmd_to_str(__u8 cmd)
{
	switch (cmd) {
	case 2: return "DRBD_ADM_GET_STATUS";
		/* add DRBD minor devices as volumes to resources */
	case 5: return "DRBD_ADM_NEW_MINOR";
	case 6: return "DRBD_ADM_DEL_MINOR";
		/* add or delete resources */
	case 7: return "DRBD_ADM_NEW_RESOURCE";
	case 8: return "DRBD_ADM_DEL_RESOURCE";
	case 9: return "DRBD_ADM_RESOURCE_OPTS";
	case 10: return "DRBD_ADM_CONNECT";
	case 29: return "DRBD_ADM_CHG_NET_OPTS";
	case 11: return "DRBD_ADM_DISCONNECT";
	case 12: return "DRBD_ADM_ATTACH";
	case 28: return "DRBD_ADM_CHG_DISK_OPTS";
	case 13: return "DRBD_ADM_RESIZE";
	case 14: return "DRBD_ADM_PRIMARY";
	case 15: return "DRBD_ADM_SECONDARY";
	case 16: return "DRBD_ADM_NEW_C_UUID";
	case 17: return "DRBD_ADM_START_OV";
	case 18: return "DRBD_ADM_DETACH";
	case 19: return "DRBD_ADM_INVALIDATE";
	case 20: return "DRBD_ADM_INVAL_PEER";
	case 21: return "DRBD_ADM_PAUSE_SYNC";
	case 22: return "DRBD_ADM_RESUME_SYNC";
	case 23: return "DRBD_ADM_SUSPEND_IO";
	case 24: return "DRBD_ADM_RESUME_IO";
	case 25: return "DRBD_ADM_OUTDATE";
	case 26: return "DRBD_ADM_GET_TIMEOUT_TYPE";
	case 27: return "DRBD_ADM_DOWN";
	default:
		return "unknown";
	}
}

// globals

struct nlattr *global_attrs[128];

#ifndef _WIN32_CT
struct task_struct g_nlThread;
#endif
extern struct mutex g_genl_mutex;

static ERESOURCE    genl_multi_socket_res_lock;
static ERESOURCE    genl_info_res_lock;

PTR_ENTRY gSocketList =
{
    .slink = { .Next = NULL },
    .ptr = NULL,
};

/**
* @brief    Multicast를 위한 socket 포인터를 list에 push 한다.
*/
static void push_msocket_entry(void * ptr)
{
    if (!ptr)
    {
        return;
    }

    PPTR_ENTRY entry = (PPTR_ENTRY)ExAllocatePoolWithTag(NonPagedPool, sizeof(PTR_ENTRY), 'dbrd');
    entry->ptr = ptr;

    MvfAcquireResourceExclusive(&genl_multi_socket_res_lock);

    PushEntryList(&gSocketList.slink, &(entry->slink));
    //WDRBD_TRACE("Added entry(0x%p), slink(0x%p), socket(0x%p)\n", entry, entry->slink, entry->ptr);

    MvfReleaseResource(&genl_multi_socket_res_lock);
}

/**
* @brief    socket 포인터 list에서 argument 포인터를 list에서 pop한다.
*/
static PPTR_ENTRY pop_msocket_entry(void * ptr)
{
    PPTR_ENTRY ret = NULL;
    PSINGLE_LIST_ENTRY iter = &gSocketList.slink;

    MvfAcquireResourceExclusive(&genl_multi_socket_res_lock);

    while (iter)
    {
        PPTR_ENTRY socket_entry = (PPTR_ENTRY)CONTAINING_RECORD(iter->Next, PTR_ENTRY, slink);

        if (socket_entry && socket_entry->ptr == ptr)
        {
            //WDRBD_TRACE("socket_entry(0x%p), slink(0x%p), socket(0x%p) found in list\n", socket_entry, socket_entry->slink, socket_entry->ptr);
            iter->Next = PopEntryList(iter->Next);

            ExFreePool(socket_entry);
            ret = socket_entry;
            break;
        }
        iter = iter->Next;
    }

    MvfReleaseResource(&genl_multi_socket_res_lock);

    return NULL;
}

/**
* @brief    multicast를 위한 전역 소켓 리스트 변수(gSocketList)를 활용하여
*           리스트내에 있는 socket으로 모두 send를 보내는 일을 한다.
*/
int drbd_genl_multicast_events(struct msg_buff * skb, const struct sib_info *sib)
{
    int ret = 0;

    if (!skb)
    {
        return EINVAL;
    }

    PSINGLE_LIST_ENTRY iter = &gSocketList.slink;

    MvfAcquireResourceShared(&genl_multi_socket_res_lock);

    while (iter)
    {
        PPTR_ENTRY socket_entry = (PPTR_ENTRY)CONTAINING_RECORD(iter->Next, PTR_ENTRY, slink);

        if (socket_entry)
        {
            //WDRBD_TRACE("send socket(0x%p), data(0x%p), len(%d)\n", socket_entry->ptr, skb->data, skb->len);
            int sent = Send(socket_entry->ptr, skb->data, skb->len, 0, 0);
            if (sent != skb->len)
            {
                WDRBD_WARN("Failed to send socket(0x%x)\n", socket_entry->ptr);
            }
        }

        iter = iter->Next;
    }

    MvfReleaseResource(&genl_multi_socket_res_lock);

    nlmsg_free(skb);

    return ret;
}

NTSTATUS reply_error(int type, int flags, int error, struct genl_info * pinfo)
{
    struct sk_buff * reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '07DW');

    if (reply_skb)
    {
        struct nlmsghdr * nlh = nlmsg_put(reply_skb, pinfo->nlhdr->nlmsg_pid,
            pinfo->nlhdr->nlmsg_seq, type, GENL_HDRLEN, flags);

        if (nlh)
        {
            struct nlmsgerr * err = nlmsg_data(nlh);
            err->error = -error;
            err->msg.nlmsg_len = 0;

            drbd_adm_send_reply(reply_skb, pinfo);
        }
        nlmsg_free(reply_skb);
    }
    else
    {
        return STATUS_NO_MEMORY;
    }
              
    return STATUS_SUCCESS;
}

int reply_status(struct sk_buff * reply_skb, struct netlink_callback * cb, struct genl_info * info)
{
    int err = drbd_adm_get_status_all(reply_skb, cb);
    struct nlmsghdr * nlh = NULL;

    if (0 == err)
    {
        nlh = nlmsg_put(reply_skb, cb->nlh->nlmsg_pid, cb->nlh->nlmsg_seq, NLMSG_DONE, GENL_HDRLEN, NLM_F_MULTI);
    }
    else if (err < 0)
    {
        nlh = nlmsg_put(reply_skb, cb->nlh->nlmsg_pid, cb->nlh->nlmsg_seq, NLMSG_DONE, GENL_HDRLEN, NLM_F_ACK);
        // -ENODEV : occured by first drbdadm adjust. response?
        WDRBD_WARN("drbd_adm_get_status_all err = %d\n", err);
    }

    if (nlh)
    {
        struct genlmsghdr * hdr = nlmsg_data(nlh);
        hdr->cmd = 0;
        hdr->version = 0;
        hdr->reserved = 0;
    }

    drbd_adm_send_reply(reply_skb, info);

    WDRBD_TRACE_NETLINK("send_reply(%d) seq(%d)\n", err, cb->nlh->nlmsg_seq);

    return err;
}

#ifndef WSK_EVENT_CALLBACK
VOID NTAPI NetlinkClientThread(PVOID p)
{
	extern int drbd_tla_parse(struct nlmsghdr *nlh);

	PWSK_SOCKET		Socket = (PWSK_SOCKET) p;
	LONG			readcount;

#ifdef _WIN32_CT
    ct_add_thread(KeGetCurrentThread(), "drbdcmd", FALSE, '25DW');
    WDRBD_INFO("Netlink Client Thread(%s-0x%p) IRQL(%d) socket(0x%p)------------- start!\n", current->comm, current->pid, KeGetCurrentIrql(), p);
#else
    g_nlThread.pid = g_nlThread.current_thr = KeGetCurrentThread();
    g_nlThread.has_sig_event = FALSE;
	strcpy(g_nlThread.comm, "drbdcmd");
    DbgPrint("\n");
	WDRBD_INFO("Netlink Client Thread(%p:%s) IRQL(%d)-------- start!\n", g_nlThread.current_thr, g_nlThread.comm, KeGetCurrentIrql());
#endif

    size_t sock_buf_size = NLMSG_GOODSIZE;
    void * psock_buf = ExAllocatePoolWithTag(NonPagedPool, sock_buf_size, 'dbrd');

    if (!psock_buf)
    {
        WDRBD_ERROR("Failed to allocate NP memory. size(%d)\n", sock_buf_size);
        goto cleanup;
    }

	while (TRUE)
	{
	    readcount = Receive(Socket, psock_buf, sock_buf_size, 0, 0);
        if (readcount > 0)
		{
            if (strstr(psock_buf, DRBD_EVENT_SOCKET_STRING))
			{
			    WDRBD_TRACE("DRBD_EVENT_SOCKET_STRING received. socket(0x%p)\n", Socket);
                push_msocket_entry(Socket);
                continue;
			}

			int err;
            struct nlmsghdr *nlh = (struct nlmsghdr*)psock_buf;

			struct genl_info info = {
				.seq = nlh->nlmsg_seq,
				.nlhdr = nlh,
				.genlhdr = nlmsg_data(nlh),
				.userhdr = genlmsg_data(nlmsg_data(nlh)),
				.attrs = global_attrs,
				.snd_seq = nlh->nlmsg_seq,
				.snd_portid = nlh->nlmsg_pid,
				.NetlinkSock = Socket
			};

			u8 cmd = info.genlhdr->cmd;

            drbd_tla_parse(nlh);

            if (!nlmsg_ok(nlh, readcount))
            {
                WDRBD_ERROR("rx message(%d) crashed!\n", readcount);
                continue;
            }

#ifdef NL_PACKET_MSG
            WDRBD_TRACE("rx(%d), cmd(%d), len(%d), flags(0x%x), type(0x%x), seq(%d), pid(%d)\n",
                readcount, cmd, nlh->nlmsg_len, nlh->nlmsg_flags, nlh->nlmsg_type, nlh->nlmsg_seq, nlh->nlmsg_pid);
#endif
            // check whether resource suspended
            struct drbd_genlmsghdr * gmh = info.userhdr;
            if (gmh)
            {
                struct drbd_conf * mdev = minor_to_mdev(gmh->minor);
                if (mdev && (drbd_suspended(mdev) || test_bit(SUSPEND_IO, &mdev->flags)))
                {
                    reply_error(NLMSG_ERROR, NLM_F_MULTI, EIO, &info);
                    WDRBD_WARN("minor(%d) suspended\n", gmh->minor);
                    break;
                }
            }

            // check dumpit first. if any, simulate dumpit
            if ((cmd == DRBD_ADM_GET_STATUS) && (nlh->nlmsg_flags == (NLM_F_DUMP | NLM_F_REQUEST)))
            {
                struct sk_buff *reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '35DW');
                if (reply_skb)
                {
                    struct netlink_callback cb;
                    memset(&cb, 0, sizeof(struct netlink_callback));
                    cb.nlh = nlh;

                    NTSTATUS status = mutex_lock(&g_genl_mutex);
                    if (STATUS_SUCCESS == status)
                    {
                        int ret = reply_status(reply_skb, &cb, &info);

                        while (ret > 0)
                        {
                            nlmsg_free(reply_skb);
                            reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '45DW');
                            if (reply_skb)
                            {
                                ret = reply_status(reply_skb, &cb, &info);
                            }
                            else
                            {
                                err = -ENOMEM;
                                WDRBD_WARN("no memory\n");
                                break;
                            }
                        }

                        mutex_unlock(&g_genl_mutex);
                    }
                    else if (STATUS_TIMEOUT == status)
                    {
                        WDRBD_WARN("time out\n");
                    }
                    
                    nlmsg_free(reply_skb);
                }
                else
                {
                    err = -ENOMEM;
                    WDRBD_WARN("no memory\n");
                }
            }
            else
            {
                int i, n = sizeof(drbd_genl_ops) / sizeof((drbd_genl_ops)[0]);
                
                for (i = 0; i < n; i++)
                {
                    if (drbd_genl_ops[i].cmd == cmd)
                    {
                        WDRBD_INFO("drbd cmd=%s\n", drbd_genl_cmd_to_str(cmd));

                        NTSTATUS status = mutex_lock(&g_genl_mutex);
                        if (STATUS_SUCCESS == status)
                        {
                            err = drbd_genl_ops[i].doit(NULL, &info);
                            mutex_unlock(&g_genl_mutex);
                        }
                        break;
                    }
                }
            }
            //break; // only one recv pkt
		}
		else if (readcount == 0)
		{
			WDRBD_INFO("peer closed\n"); // disconenct 명령시
			break;
		}
		else
		{
			WDRBD_ERROR("Receive error = 0x%x\n", readcount);
            break;
		}
    }

#if 0
    {
        // wait for peer socket close 
 		char buf;
 		int ret;
 		if ((ret = Receive(Socket, &buf, 1, 0, 1000)) > 0)
 		{
 			WDRBD_WARN("NetLink CLI done with unexpected real rx. rx count=%d. Ignored.\n", ret);
 		}
        WDRBD_TRACE("2nd Receive ret(%d)\n", ret);
 	}
#endif

cleanup:
    pop_msocket_entry(Socket);
    kfree(psock_buf);
	CloseSocket(Socket);
#ifdef _WIN32_CT
    ct_delete_thread(KeGetCurrentThread());
#else
    g_nlThread.current_thr = 0;
#endif

	WDRBD_INFO("done\n");
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID NTAPI NetlinkServerThread(PVOID p)
{
	PWSK_SOCKET		clientSocket = NULL;
	SOCKADDR_IN		LocalAddress = {0}, RemoteAddress = {0};
	NTSTATUS		Status = STATUS_UNSUCCESSFUL;
	PWSK_SOCKET		Socket = NULL;
    LONG            loop = 0;

	// Init WSK
	Status = SocketsInit();
	if (!NT_SUCCESS(Status)) {
		WDRBD_ERROR("SocketsInit() failed with status 0x%08X\n", Status);
        BUG();
		return; 
	}

    ExInitializeResourceLite(&genl_multi_socket_res_lock);

#ifdef _WIN32_CT
    ct_add_thread(KeGetCurrentThread(), "NetlinkServer", FALSE, '55DW');
#endif

    while (TRUE)
    {
        WDRBD_INFO("Start.(loop:%d)\n", ++loop);
        if (loop > 10)
        {
            // DRBD_DOC: DV: may be occured by DV only
            WDRBD_ERROR("too many retry. give up listen daemon.\n");
            // BUG();
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        // start NetLinkServe daemon
#ifdef WSK_ACCEPT_EVENT_CALLBACK
        clientSocket = CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSK_FLAG_LISTEN_SOCKET);
#else
        clientSocket = CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, WSK_FLAG_LISTEN_SOCKET);
#endif
        if (clientSocket == NULL) {
            WDRBD_ERROR("CreateSocket() returned NULL\n");
            continue;
        }

        LocalAddress.sin_family = AF_INET;
        LocalAddress.sin_addr.s_addr = INADDR_ANY;
        LocalAddress.sin_port = HTONS(g_netlink_tcp_port);

        Status = Bind(clientSocket, (PSOCKADDR) &LocalAddress);
        if (!NT_SUCCESS(Status)) {
            WDRBD_ERROR("Bind() failed with status 0x%08X\n", Status);
            CloseSocket(clientSocket);
            // EVENT_LOG
            continue;
        }

        while (TRUE)
        {
            HANDLE hThread = NULL;
            int	timeout = 10;
            if ((Socket = Accept(clientSocket, (PSOCKADDR) &LocalAddress, (PSOCKADDR) &RemoteAddress, &Status, timeout)) == NULL)
            {
                if (Status == STATUS_TIMEOUT)
                {
                    continue;
                }
                else
                {
                    WDRBD_ERROR("accept error=0x%x. retry!\n", Status); // EVENTLOG
                    continue;
                }
            }

            Status = PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, NetlinkClientThread, Socket);
            if (!NT_SUCCESS(Status)) {
                WDRBD_ERROR("client start failed with status 0x%08X\n", Status);
                break;
            }
            ZwClose(hThread);
        }
    }

#ifdef WIN32_CT
    ct_delete_thread(KeGetCurrentThread());
#endif

	Status = CloseSocket(clientSocket);
    ExDeleteResourceLite(&genl_multi_socket_res_lock);

	PsTerminateSystemThread(STATUS_SUCCESS);
}
#endif

int genlmsg_unicast_wrapper(struct msg_buff *skb, struct genl_info *info)
{
	int sent;

	if (info->NetlinkSock == 0)
	{
		return -1; // return non-zero!
	}

	if ((sent = Send(info->NetlinkSock, skb->data, skb->len, 0, 0)) == (skb->len))
	{
		return 0; // success
	}
	else
	{
        WDRBD_WARN("sent Error=0x%x. sock=%p, data=%p sz=%d\n", sent, info->NetlinkSock, skb->data, skb->len);
		return -2; // return non-zero!
	}
}

#ifdef WSK_EVENT_CALLBACK
NPAGED_LOOKASIDE_LIST drbd_workitem_mempool;
NPAGED_LOOKASIDE_LIST genl_info_mempool;
NPAGED_LOOKASIDE_LIST genl_msg_mempool;

LIST_ENTRY  genl_info_list_head;

typedef struct _NETLINK_WORK_ITEM{
    WORK_QUEUE_ITEM Item;
    PWSK_SOCKET Socket;
} NETLINK_WORK_ITEM, *PNETLINK_WORK_ITEM;

#if 0
void NTAPI NetlinkThread(void * pctx)
{
    extern int drbd_tla_parse(struct nlmsghdr *nlh);

    PWSK_SOCKET		Socket = (PWSK_SOCKET)pctx;
    LONG			readcount;

#ifdef _WIN32_CT
    DbgPrint("\n");
    ct_add_thread(KeGetCurrentThread(), "drbdcmd", FALSE, '25DW');
    WDRBD_INFO("Thread(%s-0x%p) IRQL(%d) socket(0x%p)------------- start!\n", current->comm, current->pid, KeGetCurrentIrql(), pctx);
#endif

    size_t sock_buf_size = NLMSG_GOODSIZE;
    void * psock_buf = ExAllocatePoolWithTag(NonPagedPool, sock_buf_size, 'dbrd');

    if (!psock_buf)
    {
        WDRBD_ERROR("Failed to allocate NP memory. size(%d)\n", sock_buf_size);
        goto cleanup;
    }

    while (TRUE)
    {
        readcount = Receive(Socket, psock_buf, sock_buf_size, 0, 0);

        if (readcount > 0)
        {
            if (strstr(psock_buf, DRBD_EVENT_SOCKET_STRING))
            {
                WDRBD_TRACE("DRBD_EVENT_SOCKET_STRING received. socket(0x%p)\n", Socket);
                push_msocket_entry(Socket);
                continue;
            }

            int err;
            struct nlmsghdr *nlh = (struct nlmsghdr*)psock_buf;

            struct genl_info info = {
                .seq = nlh->nlmsg_seq,
                .nlhdr = nlh,
                .genlhdr = nlmsg_data(nlh),
                .userhdr = genlmsg_data(nlmsg_data(nlh)),
                .attrs = global_attrs,
                .snd_seq = nlh->nlmsg_seq,
                .snd_portid = nlh->nlmsg_pid,
                .NetlinkSock = Socket
            };

            u8 cmd = info.genlhdr->cmd;

            drbd_tla_parse(nlh);

            if (!nlmsg_ok(nlh, readcount))
            {
                WDRBD_ERROR("rx message(%d) crashed!\n", readcount);
                continue;
            }

#ifdef NL_PACKET_MSG
            WDRBD_TRACE("rx(%d), cmd(%d), len(%d), flags(0x%x), type(0x%x), seq(%d), pid(%d)\n",
                readcount, cmd, nlh->nlmsg_len, nlh->nlmsg_flags, nlh->nlmsg_type, nlh->nlmsg_seq, nlh->nlmsg_pid);
#endif
            // check whether resource suspended
            struct drbd_genlmsghdr * gmh = info.userhdr;
            if (gmh)
            {
                struct drbd_conf * mdev = minor_to_mdev(gmh->minor);
                if (mdev && (drbd_suspended(mdev) || test_bit(SUSPEND_IO, &mdev->flags)))
                {
                    reply_error(NLMSG_ERROR, NLM_F_MULTI, EIO, &info);
                    WDRBD_WARN("minor(%d) suspended\n", gmh->minor);
                    break;
                }
            }

            // check dumpit first. if any, simulate dumpit
            if ((cmd == DRBD_ADM_GET_STATUS) && (nlh->nlmsg_flags == (NLM_F_DUMP | NLM_F_REQUEST)))
            {
                //WDRBD_INFO("drbd cmd=DRBD_ADM_GET_STATUS. nlh->nlmsg_flags == (NLM_F_DUMP | NLM_F_REQUEST)\n");
                /// SEO: DRBD_ADM_GET_STATUS의 실패 메시지가 출력되지 않으면 성공으로 간주합니다.
                cli_info( gmh->minor, "Command=%s(%u)\n", drbd_genl_cmd_to_str(cmd), cmd );
                struct sk_buff *reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '35DW');
                if (reply_skb)
                {
                    struct netlink_callback cb;
                    memset(&cb, 0, sizeof(struct netlink_callback));
                    cb.nlh = nlh;
                    NTSTATUS status = mutex_lock(&g_genl_mutex);

                    if (STATUS_SUCCESS == status)
                    {
                        int ret = reply_status(reply_skb, &cb, &info);

                        while (ret > 0)
                        {
                            nlmsg_free(reply_skb);
                            reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '45DW');
                            if (reply_skb)
                            {
                                ret = reply_status(reply_skb, &cb, &info);
                            }
                            else
                            {
                                err = -ENOMEM;
                                WDRBD_WARN("no memory\n");
                                cli_warn( gmh->minor, "Command=%s(%u) Error=genlmsg_new failed.\n", drbd_genl_cmd_to_str(cmd), cmd );
                                break;
                            }
                        }
                        mutex_unlock(&g_genl_mutex);
                    }
                    else if (STATUS_TIMEOUT == status)
                    {
                        WDRBD_WARN("time out\n");
                        cli_warn( gmh->minor, "Command=%s(%u) Error=Time out\n", drbd_genl_cmd_to_str(cmd), cmd );
                    }

                    nlmsg_free(reply_skb);
                }
                else
                {
                    err = -ENOMEM;
                    WDRBD_WARN("no memory\n");
                    cli_warn( gmh->minor, "Command=%s(%u) Error=genlmsg_new failed.\n", drbd_genl_cmd_to_str(cmd), cmd );
                }
            }
            else
            {
                int i, n = sizeof(drbd_genl_ops) / sizeof((drbd_genl_ops)[0]);

                for (i = 0; i < n; i++)
                {
                    if (drbd_genl_ops[i].cmd == cmd)
                    {
                        WDRBD_INFO("drbd cmd=%s\n", drbd_genl_cmd_to_str(cmd));
                        cli_info( gmh->minor, "Command=%s(%u)\n", drbd_genl_cmd_to_str(cmd), cmd );
                        NTSTATUS status = mutex_lock(&g_genl_mutex);
                        if (STATUS_SUCCESS == status)
                        {
                            err = drbd_genl_ops[i].doit(NULL, &info);
                            mutex_unlock(&g_genl_mutex);
                        }
                        break;
                    }
                }
            }
            //break; // only one recv pkt
        }
        else if (readcount == 0)
        {
            //WDRBD_INFO("peer closed\n"); // disconenct 명령시
            break;
        }
        else
        {
            WDRBD_ERROR("Receive error = 0x%x\n", readcount);
            break;
        }
    }

cleanup:
    pop_msocket_entry(Socket);
    kfree(psock_buf);
    CloseSocket(Socket);
#ifdef _WIN32_CT
    ct_delete_thread(KeGetCurrentThread());
#endif

    WDRBD_INFO("done\n");
    PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID
WorkNetlinkThread(PVOID context)
{
    ASSERT(context);
    PWSK_SOCKET sock = ((PNETLINK_WORK_ITEM)context)->Socket;
    HANDLE hThread = NULL;
    NTSTATUS status = PsCreateSystemThread(
        &hThread,
        THREAD_ALL_ACCESS,
        NULL,
        NULL,
        NULL,
        NetlinkThread,
        sock);

    if (!NT_SUCCESS(status))
    {
        WDRBD_ERROR("Failed to create client thread. status(0x%x)\n", status);
    }
    else
    {
        ZwClose(hThread);
    }
    
    ExFreeToNPagedLookasideList(&drbd_workitem_mempool, context);
}
#endif
void _add_genl_info_list(struct genl_info * pinfo)
{
    MvfAcquireResourceExclusive(&genl_info_res_lock);

    if (!IsListEmpty(&genl_info_list_head))
    {
        for (PLIST_ENTRY entry = genl_info_list_head.Flink;
            entry != &genl_info_list_head;
            entry = entry->Flink)
        {
            struct genl_info * pinfo = (struct genl_info *)CONTAINING_RECORD(entry, struct genl_info, ListEntry);
            WDRBD_TRACE_NETLINK("left socket(0x%p)\n", pinfo->NetlinkSock);
        }
    }

    InsertTailList(&genl_info_list_head, &pinfo->ListEntry);

    MvfReleaseResource(&genl_info_res_lock);
}

void _remove_genl_info_list(struct genl_info * pinfo)
{
    MvfAcquireResourceExclusive(&genl_info_res_lock);

    if (!IsListEmpty(&genl_info_list_head))
    {
        RemoveEntryList(&pinfo->ListEntry);
    }

    MvfReleaseResource(&genl_info_res_lock);
}

struct genl_info * genl_info_new(struct nlmsghdr * nlh, PWSK_SOCKET socket)
{
    struct genl_info * pinfo = ExAllocateFromNPagedLookasideList(&genl_info_mempool);

    if (!pinfo)
    {
        WDRBD_ERROR("Failed to allocate (struct genl_info) memory. size(%d)\n",
            sizeof(struct genl_info));
        return NULL;
    }

    RtlZeroMemory(pinfo, sizeof(struct genl_info));

    pinfo->seq = nlh->nlmsg_seq;
    pinfo->nlhdr = nlh;
    pinfo->genlhdr = nlmsg_data(nlh);
    pinfo->userhdr = genlmsg_data(nlmsg_data(nlh));
    pinfo->attrs = global_attrs;
    pinfo->snd_seq = nlh->nlmsg_seq;
    pinfo->snd_portid = nlh->nlmsg_pid;
    pinfo->NetlinkSock = socket;

    return pinfo;
}

__inline
void _genlmsg_init(struct msg_buff * pmsg, size_t size)
{
    RtlZeroMemory(pmsg, size);

    pmsg->tail = 0;
    pmsg->end = size - sizeof(*pmsg);
}

struct msg_buff *genlmsg_new(size_t payload, gfp_t flags, ULONG Tag)
{
    struct msg_buff *skb;

    if (NLMSG_GOODSIZE == payload)
    {
        payload = NLMSG_GOODSIZE - sizeof(*skb);
        skb = ExAllocateFromNPagedLookasideList(&genl_msg_mempool);
    }
    else
    {
        skb = kcalloc(1, sizeof(*skb) + payload, GFP_KERNEL, Tag);
    }

    if (!skb)
        return NULL;

    _genlmsg_init(skb, sizeof(*skb) + payload);

    return skb;
}

void
InitWskNetlink(void * pctx)
{
    NTSTATUS    status;
    PWSK_SOCKET netlink_socket = NULL;
    SOCKADDR_IN LocalAddress = {0};

    // Init WSK
    status = SocketsInit();
    if (!NT_SUCCESS(status))
    {
        WDRBD_ERROR("Failed to init. status(0x%x)\n", status);
        return;
    }

    // Init WSK Event Callback
    status = InitWskEvent();
    if (!NT_SUCCESS(status))
    {
        return;
    }

    //WDRBD_INFO("Start\n");

    netlink_socket = CreateSocketEvent(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP,
        WSK_FLAG_LISTEN_SOCKET);

    if (!netlink_socket)
    {
        WDRBD_ERROR("Failed to create socket\n");
        goto end;
    }

    LocalAddress.sin_family = AF_INET;
    LocalAddress.sin_addr.s_addr = INADDR_ANY;
    LocalAddress.sin_port = HTONS(g_netlink_tcp_port);

    status = Bind(netlink_socket, (PSOCKADDR)&LocalAddress);
    if (!NT_SUCCESS(status))
    {
        WDRBD_ERROR("Failed to bind. status(0x%x)\n", status);
        CloseSocket(netlink_socket);
    }

    netlink_server_socket = netlink_socket;

    ExInitializeNPagedLookasideList(&drbd_workitem_mempool, NULL, NULL,
        0, sizeof(struct _NETLINK_WORK_ITEM), '27DW', 0);
    ExInitializeNPagedLookasideList(&genl_info_mempool, NULL, NULL,
        0, sizeof(struct genl_info), '37DW', 0);
    ExInitializeNPagedLookasideList(&genl_msg_mempool, NULL, NULL,
        0, NLMSG_GOODSIZE, '47DW', 0);

    InitializeListHead(&genl_info_list_head);
    ExInitializeResourceLite(&genl_info_res_lock);
    ExInitializeResourceLite(&genl_multi_socket_res_lock);

end:
    ReleaseProviderNPI();

    PsTerminateSystemThread(status);
}

NTSTATUS
ReleaseWskNetlink()
{
    ExDeleteNPagedLookasideList(&drbd_workitem_mempool);
    ExDeleteNPagedLookasideList(&genl_info_mempool);
    ExDeleteNPagedLookasideList(&genl_msg_mempool);

    ExDeleteResourceLite(&genl_info_res_lock);
    ExDeleteResourceLite(&genl_multi_socket_res_lock);
    
    return CloseWskEventSocket();
}

static int _drbd_adm_get_status(struct sk_buff *skb, struct genl_info * pinfo)
{
    NT_ASSERT(pinfo);

    struct nlmsghdr * nlh = pinfo->nlhdr;

    if (nlh->nlmsg_flags != (NLM_F_DUMP | NLM_F_REQUEST))
    {
        return 0;
    }

    struct sk_buff * reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL, '35DW');

    if (reply_skb)
    {
        struct netlink_callback cb;
        RtlZeroMemory(&cb, sizeof(struct netlink_callback));
        cb.nlh = nlh;

        int ret = reply_status(reply_skb, &cb, pinfo);

        while (ret > 0)
        {
            RtlZeroMemory(reply_skb, NLMSG_GOODSIZE);
            _genlmsg_init(reply_skb, NLMSG_GOODSIZE);

            ret = reply_status(reply_skb, &cb, pinfo);
        }

        nlmsg_free(reply_skb);
    }

    return 0;
}

VOID
NetlinkWorkThread(PVOID context)
{
    ASSERT(context);

    PWSK_SOCKET socket = ((PNETLINK_WORK_ITEM)context)->Socket;
    LONG readcount, minor = 0;
    int err;
    struct genl_info * pinfo = NULL;

#ifdef _WIN32_CT
    ct_add_thread(KeGetCurrentThread(), "drbdcmd", FALSE, '25DW');
    //WDRBD_INFO("Thread(%s-0x%p) IRQL(%d) socket(0x%p)------------- start!\n", current->comm, current->pid, KeGetCurrentIrql(), pctx);
#endif

    void * psock_buf = ExAllocateFromNPagedLookasideList(&genl_msg_mempool);

    if (!psock_buf)
    {
        WDRBD_ERROR("Failed to allocate NP memory. size(%d)\n", NLMSG_GOODSIZE);
        goto cleanup;
    }

    while (TRUE)
    {
        readcount = Receive(socket, psock_buf, NLMSG_GOODSIZE, 0, 0);

        if (readcount == 0)
        {
            //WDRBD_INFO("peer closed\n"); // disconenct 명령??
            goto cleanup;
        }
        else if(readcount < 0)
        {
            WDRBD_ERROR("Receive error = 0x%x\n", readcount);
            goto cleanup;
        }

        if (strstr(psock_buf, DRBD_EVENT_SOCKET_STRING))
        {
            WDRBD_TRACE("DRBD_EVENT_SOCKET_STRING received. socket(0x%p)\n", socket);
            push_msocket_entry(socket);
            continue;
        }

        struct nlmsghdr *nlh = (struct nlmsghdr *)psock_buf;

        pinfo = genl_info_new(nlh, socket);
        if (!pinfo)
        {
            WDRBD_ERROR("Failed to allocate (struct genl_info) memory. size(%d)\n", sizeof(struct genl_info));
            goto cleanup;
        }

        drbd_tla_parse(nlh);
        if (!nlmsg_ok(nlh, readcount))
        {
            WDRBD_ERROR("rx message(%d) crashed!\n", readcount);
            goto cleanup;
        }

        WDRBD_TRACE_NETLINK("rx(%d), cmd(%d), len(%d), flags(0x%x), type(0x%x), seq(%d), pid(%d)\n",
            readcount, pinfo->genlhdr->cmd, nlh->nlmsg_len, nlh->nlmsg_flags, nlh->nlmsg_type, nlh->nlmsg_seq, nlh->nlmsg_pid);

        // check whether resource suspended
        struct drbd_genlmsghdr * gmh = pinfo->userhdr;
        if (gmh)
        {
            minor = gmh->minor;
            struct drbd_conf * mdev = minor_to_mdev(minor);
            if (mdev && (drbd_suspended(mdev) || test_bit(SUSPEND_IO, &mdev->flags)))
            {
                reply_error(NLMSG_ERROR, NLM_F_MULTI, EIO, pinfo);
                WDRBD_WARN("minor(%d) suspended\n", gmh->minor);
                goto cleanup;
            }
        }

        int i;
        u8 cmd = pinfo->genlhdr->cmd;
        struct genl_ops * pops = NULL;
        for (i = 0; i < sizeof(drbd_genl_ops) / sizeof((drbd_genl_ops)[0]); i++)
        {
            if (drbd_genl_ops[i].cmd == cmd)
            {
                pops = &drbd_genl_ops[i];
                break;
            }
        }
        
        if (pops)
        {
            //WDRBD_INFO("drbd cmd(%s)\n", drbd_genl_cmd_to_str(cmd));
            cli_info(gmh->minor, "Command (%s:%u)\n", drbd_genl_cmd_to_str(cmd), cmd);
            
            if (mutex_trylock(&g_genl_mutex))
            {
                _add_genl_info_list(pinfo);
                err = drbd_genl_ops[i].doit(NULL, pinfo);
                mutex_unlock(&g_genl_mutex);
                _remove_genl_info_list(pinfo);
            }
            else
            {
                WDRBD_WARN("Failed to acquire the mutex\n");
                for (PLIST_ENTRY entry = genl_info_list_head.Flink;
                    entry != &genl_info_list_head;
                    entry = entry->Flink)
                {
                    struct genl_info * pinfo = (struct genl_info *)CONTAINING_RECORD(entry, struct genl_info, ListEntry);
                    WDRBD_WARN("cmd(%s:%d), flags(0x%x), pid(%d)\n",
                        drbd_genl_cmd_to_str(pinfo->genlhdr->cmd), pinfo->genlhdr->cmd, pinfo->nlhdr->nlmsg_flags, pinfo->nlhdr->nlmsg_pid);
                }
            }

            goto cleanup;
        }
        else
        {
            WDRBD_WARN("Not validated cmd(%d)\n", cmd);
        }
    }

    // wait for peer socket close 
    char buf;
    int ret;
    if ((ret = Receive(socket, &buf, 1, 0, 1000)) > 0)
    {
        WDRBD_WARN("NetLink CLI done with unexpected real rx. rx count=%d. Ignored.\n", ret);
    }

cleanup:
    pop_msocket_entry(socket);
    Disconnect(socket);
    CloseSocket(socket);
#ifdef _WIN32_CT
    ct_delete_thread(KeGetCurrentThread());
#endif
    ExFreeToNPagedLookasideList(&drbd_workitem_mempool, context);
    if (pinfo)
        ExFreeToNPagedLookasideList(&genl_info_mempool, pinfo);
    if (psock_buf)
        ExFreeToNPagedLookasideList(&genl_msg_mempool, psock_buf);

    if (err)
    {
        cli_info(minor, "done. error(%d)\n", err);
    }
    else
    {
        cli_info(minor, "done\n");
    }
}

// Listening socket callback which is invoked whenever a new connection arrives.
NTSTATUS
WSKAPI
NetlinkAcceptEvent(
_In_  PVOID         SocketContext,
_In_  ULONG         Flags,
_In_  PSOCKADDR     LocalAddress,
_In_  PSOCKADDR     RemoteAddress,
_In_opt_  PWSK_SOCKET AcceptSocket,
_Outptr_result_maybenull_ PVOID *AcceptSocketContext,
_Outptr_result_maybenull_ CONST WSK_CLIENT_CONNECTION_DISPATCH **AcceptSocketDispatch
)
{
    UNREFERENCED_PARAMETER(Flags);

    if (AcceptSocket == NULL)
    {
        // If WSK provider makes a WskAcceptEvent callback with NULL 
        // AcceptSocket, this means that the listening socket is no longer
        // functional. The WSK client may handle this situation by trying
        // to create a new listening socket or by restarting the driver, etc.
        // In this sample, we will attempt to close the existing listening
        // socket and create a new one. Note that the AcceptEvent
        // callback is guaranteed to be invoked with a NULL AcceptSocket
        // by the WSK subsystem only *once*. So, we can safely use the same
        // operation context that was originally used for enqueueing the first
        // WskSampleStartListen operation on the listening socket. The
        // WskSampleStartListen operation will close the existing listening
        // socket and create a new one.
        return STATUS_REQUEST_NOT_ACCEPTED;
    }

    SOCKADDR_IN * premote = (SOCKADDR_IN *)RemoteAddress;
    SOCKADDR_IN * plocal = (SOCKADDR_IN *)LocalAddress;

    DbgPrint("\n");
    WDRBD_TRACE_NETLINK("%u.%u.%u.%u:%u -> %u.%u.%u.%u:%u connected\n",
        premote->sin_addr.S_un.S_un_b.s_b1,
        premote->sin_addr.S_un.S_un_b.s_b2,
        premote->sin_addr.S_un.S_un_b.s_b3,
        premote->sin_addr.S_un.S_un_b.s_b4,
        HTON_SHORT(premote->sin_port),
        plocal->sin_addr.S_un.S_un_b.s_b1,
        plocal->sin_addr.S_un.S_un_b.s_b2,
        plocal->sin_addr.S_un.S_un_b.s_b3,
        plocal->sin_addr.S_un.S_un_b.s_b4,
        HTON_SHORT(plocal->sin_port));

    PNETLINK_WORK_ITEM netlinkWorkItem = ExAllocateFromNPagedLookasideList(&drbd_workitem_mempool);

    if (!netlinkWorkItem)
    {
        WDRBD_ERROR("Failed to allocate NP memory. size(%d)\n", sizeof(NETLINK_WORK_ITEM));
        return STATUS_REQUEST_NOT_ACCEPTED;
    }

    netlinkWorkItem->Socket = AcceptSocket;

    ExInitializeWorkItem(&netlinkWorkItem->Item,
        NetlinkWorkThread,
        netlinkWorkItem);

    ExQueueWorkItem(&netlinkWorkItem->Item, DelayedWorkQueue);

    return STATUS_SUCCESS;
}

#endif