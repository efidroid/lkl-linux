#ifndef _LKL_LIB_VIRTIO_H
#define _LKL_LIB_VIRTIO_H

#include <lkl_host.h>

#define PAGE_SIZE		4096

/* The following are copied from skbuff.h */
#if (65536/PAGE_SIZE + 1) < 16
#define MAX_SKB_FRAGS 16UL
#else
#define MAX_SKB_FRAGS (65536/PAGE_SIZE + 1)
#endif

#define VIRTIO_REQ_MAX_BUFS	(MAX_SKB_FRAGS + 2)

struct virtio_req {
	__lkl__u16 buf_count;
	struct iovec buf[VIRTIO_REQ_MAX_BUFS];
	__lkl__u32 total_len;
};

struct virtio_dev;

struct virtio_dev_ops {
	int (*check_features)(struct virtio_dev *dev);
	/**
	 * enqueue - queues the request for processing
	 *
	 * Note that the curret implementation assumes that the requests are
	 * processed synchronous and, as such, @virtio_req_complete must be
	 * called by from this function.
	 *
	 * @dev - virtio device
	 * @q	- queue index
	 *
	 * @returns a negative value if the request has not been queued for
	 * processing in which case the virtio device is resposible for
	 * restaring the queue processing by calling @virtio_process_queue at a
	 * later time; 0 or a positive value means that the request has been
	 * queued for processing
	 */
	int (*enqueue)(struct virtio_dev *dev, int q, struct virtio_req *req);
	/*
	 * Acquire/release a lock on the specified queue. Only implemented by
	 * netdevs, all other devices have NULL acquire/release function
	 * pointers.
	 */
	void (*acquire_queue)(struct virtio_dev *dev, int queue_idx);
	void (*release_queue)(struct virtio_dev *dev, int queue_idx);
};

struct virtio_dev {
	__lkl__u32 device_id;
	__lkl__u32 vendor_id;
	__lkl__u64 device_features;
	__lkl__u32 device_features_sel;
	__lkl__u64 driver_features;
	__lkl__u32 driver_features_sel;
	__lkl__u32 queue_sel;
	struct virtio_queue *queue;
	__lkl__u32 queue_notify;
	__lkl__u32 int_status;
	__lkl__u32 status;
	__lkl__u32 config_gen;

	struct virtio_dev_ops *ops;
	int irq;
	void *config_data;
	int config_len;
	void *base;
	__lkl__u32 virtio_mmio_id;
};

int virtio_dev_setup(struct virtio_dev *dev, int queues, int num_max);
int virtio_dev_cleanup(struct virtio_dev *dev);
__lkl__u32 virtio_get_num_bootdevs(void);
/**
 * virtio_req_complete - complete a virtio request
 *
 * @req - the request to be completed
 * @len - the total size in bytes of the completed request
 */
void virtio_req_complete(struct virtio_req *req, __lkl__u32 len);
void virtio_process_queue(struct virtio_dev *dev, __lkl__u32 qidx);
void virtio_set_queue_max_merge_len(struct virtio_dev *dev, int q, int len);

#define container_of(ptr, type, member) \
	(type *)((char *)(ptr) - __builtin_offsetof(type, member))

#endif /* _LKL_LIB_VIRTIO_H */
