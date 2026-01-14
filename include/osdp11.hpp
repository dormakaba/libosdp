/*
 * Copyright (c) 2021-2022 Siddharth Chandrasekaran <sidcha.dev@gmail.com>
 * Copyright (c) 2023, dormakaba Deutschland GmbH, DORMA Platz 1, 58256 Ennepetal, Germany
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OSDP11_HPP
#define _OSDP11_HPP

#include <osdp.h>

/**
 * @file: LibOSDP c++11 wrapper for mocking. See osdp.h for documentation.
 */

static constexpr uint8_t OSDP_FILETRANSFER_TYPE_OPAQUE = 1;

namespace OSDP {

class ICommon {
public:
    ICommon() = default;
    virtual ~ICommon() = default;

    virtual void set_log_callback(osdp_log_callback_fn_t cb) = 0;
    virtual const char* get_version() = 0;
    virtual const char* get_source_info() = 0;
    virtual void get_status_mask(uint8_t* bitmask) = 0;
    virtual void get_sc_status_mask(uint8_t* bitmask) = 0;
    virtual int file_register_ops(int pd, struct osdp_file_ops* ops) = 0;
    virtual int file_tx_get_status(int pd, int* size, int* offset) = 0;
};

class Common : public virtual ICommon {
public:
	Common() {}

	void set_log_callback(osdp_log_callback_fn_t cb)
	{
		osdp_set_log_callback(cb);
	}

	const char *get_version()
	{
		return osdp_get_version();
	}

	const char *get_source_info()
	{
		return osdp_get_source_info();
	}

	void get_status_mask(uint8_t *bitmask)
	{
		osdp_get_status_mask(_ctx, bitmask);
	}

	void get_sc_status_mask(uint8_t *bitmask)
	{
		osdp_get_sc_status_mask(_ctx, bitmask);
	}

	int file_register_ops(int pd, struct osdp_file_ops *ops)
	{
		return osdp_file_register_ops(_ctx, pd, ops);
	}

	int file_tx_get_status(int pd, int *size, int *offset)
	{
		return osdp_get_file_tx_status(_ctx, pd, size, offset);
	}

protected:
	osdp_t *_ctx = nullptr;
};

class IControlPanel : public virtual ICommon {
public:
    IControlPanel() = default;
    ~IControlPanel() override = default;
    virtual bool setup(int num_pd, osdp_pd_info_t* info) = 0;
    virtual void refresh() = 0;
    virtual void teardown() = 0;
    virtual int submit_command(int pd, struct osdp_cmd* cmd) = 0;
    virtual void set_event_callback(cp_event_callback_t cb, void* arg) = 0;
    virtual int get_pd_id(int pd, struct osdp_pd_id* id) = 0;
    virtual int get_capability(int pd, struct osdp_pd_cap* cap) = 0;
};

class ControlPanel : public Common, public IControlPanel  {
public:
	ControlPanel()
	{
	}

    ~ControlPanel() override {
        if (_ctx) {
            osdp_cp_teardown(_ctx);
        }
        _ctx = nullptr;
    }

	bool setup(int num_pd, osdp_pd_info_t *info)
	{
		_ctx = osdp_cp_setup(num_pd, info);
		return _ctx != nullptr;
	}

	void refresh()
	{
		if (_ctx) {
			osdp_cp_refresh(_ctx);
		}
	}

	int submit_command(int pd, struct osdp_cmd *cmd)
	{
		if (_ctx) {
			return osdp_cp_submit_command(_ctx, pd, cmd);
		}
		return -1;
	}

	void set_event_callback(cp_event_callback_t cb, void *arg)
	{
		osdp_cp_set_event_callback(_ctx, cb, arg);
	}

	int get_pd_id(int pd, struct osdp_pd_id *id)
	{
		return osdp_cp_get_pd_id(_ctx, pd, id);
	}

	int get_capability(int pd, struct osdp_pd_cap *cap)
	{
		return osdp_cp_get_capability(_ctx, pd, cap);
	}

	void teardown() override {
		if (_ctx) {
			osdp_cp_teardown(_ctx);
		}
		_ctx = nullptr;
	}

	void get_status_mask(uint8_t* bitmask) override {
		if (_ctx) {
			osdp_get_status_mask(_ctx, bitmask);
		}
    }

	void get_sc_status_mask(uint8_t* bitmask) override {
		if (_ctx) {
			osdp_get_sc_status_mask(_ctx, bitmask);
		}
	}

};

class IPeripheralDevice : public virtual ICommon {
public:
    IPeripheralDevice() = default;
    ~IPeripheralDevice() override = default;

    virtual bool setup(osdp_pd_info_t* info) = 0;
    virtual void refresh() = 0;
    virtual void teardown() = 0;
    virtual void set_command_callback(pd_command_callback_t cb, void* arg) = 0;
	virtual int submit_event(struct osdp_event* event) = 0;
};

class PeripheralDevice : public Common, public IPeripheralDevice {
public:
	PeripheralDevice()
	{
	}

	~PeripheralDevice()
	{
        if (_ctx) {
            osdp_pd_teardown(_ctx);
        }
        _ctx = nullptr;
	}

	bool setup(osdp_pd_info_t *info)
	{
        _ctx = osdp_pd_setup(info);
		return _ctx != nullptr;
	}

	void refresh()
	{
		osdp_pd_refresh(_ctx);
	}

	void teardown() override {
		if (_ctx) {
			osdp_pd_teardown(_ctx);
		}
		_ctx = nullptr;
	}

	void set_command_callback(pd_command_callback_t cb, void* arg) override
	{
		osdp_pd_set_command_callback(_ctx, cb, arg);
	}

	int submit_event(struct osdp_event *event)
	{
		return osdp_pd_submit_event(_ctx, event);
	}

	int flush_events()
	{
		return osdp_pd_flush_events(_ctx);
	}
};

} /* namespace OSDP */

#endif  /* _OSDP11_HPP */