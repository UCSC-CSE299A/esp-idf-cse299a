/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h> /* Required for NULL constant */
#include <stdint.h>
#include <stdbool.h>
#include "hal/gdma_types.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"
#include "soc/soc_etm_source.h"
#include "soc/pcr_struct.h"
#include "soc/retention_periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GDMA_CH_RETENTION_GET_MODULE_ID(group_id, pair_id) (SLEEP_RETENTION_MODULE_GDMA_CH0 << (SOC_GDMA_PAIRS_PER_GROUP_MAX * group_id) << pair_id)

#define GDMA_LL_GET_HW(id) (((id) == 0) ? (&GDMA) : NULL)

#define GDMA_LL_CHANNEL_MAX_PRIORITY 5 // supported priority levels: [0,5]

#define GDMA_LL_RX_EVENT_MASK       (0x7F)
#define GDMA_LL_TX_EVENT_MASK       (0x3F)

// any "dummy" peripheral ID can be used for M2M mode
#define GDMA_LL_M2M_FREE_PERIPH_ID_MASK (0xFC32)
#define GDMA_LL_INVALID_PERIPH_ID       (0x3F)

#define GDMA_LL_EVENT_TX_FIFO_UDF   (1<<5)
#define GDMA_LL_EVENT_TX_FIFO_OVF   (1<<4)
#define GDMA_LL_EVENT_RX_FIFO_UDF   (1<<6)
#define GDMA_LL_EVENT_RX_FIFO_OVF   (1<<5)
#define GDMA_LL_EVENT_TX_TOTAL_EOF  (1<<3)
#define GDMA_LL_EVENT_RX_DESC_EMPTY (1<<4)
#define GDMA_LL_EVENT_TX_DESC_ERROR (1<<2)
#define GDMA_LL_EVENT_RX_DESC_ERROR (1<<3)
#define GDMA_LL_EVENT_TX_EOF        (1<<1)
#define GDMA_LL_EVENT_TX_DONE       (1<<0)
#define GDMA_LL_EVENT_RX_ERR_EOF    (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF    (1<<1)
#define GDMA_LL_EVENT_RX_DONE       (1<<0)

#define GDMA_LL_AHB_GROUP_START_ID    0 // AHB GDMA group ID starts from 0
#define GDMA_LL_AHB_NUM_GROUPS        1 // Number of AHB GDMA groups
#define GDMA_LL_AHB_PAIRS_PER_GROUP   3 // Number of GDMA pairs in each AHB group

#define GDMA_LL_TX_ETM_EVENT_TABLE(group, chan, event)                                     \
    (uint32_t[1][3][GDMA_ETM_EVENT_MAX]){{{                                                \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_OUT_EOF_CH0, \
                                          },                                               \
                                          {                                                \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_OUT_EOF_CH1, \
                                          },                                               \
                                          {                                                \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_OUT_EOF_CH2, \
                                          }}}[group][chan][event]

#define GDMA_LL_RX_ETM_EVENT_TABLE(group, chan, event)                                        \
    (uint32_t[1][3][GDMA_ETM_EVENT_MAX]){{{                                                   \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_IN_SUC_EOF_CH0, \
                                          },                                                  \
                                          {                                                   \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_IN_SUC_EOF_CH1, \
                                          },                                                  \
                                          {                                                   \
                                              [GDMA_ETM_EVENT_EOF] = GDMA_EVT_IN_SUC_EOF_CH2, \
                                          }}}[group][chan][event]

#define GDMA_LL_TX_ETM_TASK_TABLE(group, chan, task)                                          \
    (uint32_t[1][3][GDMA_ETM_TASK_MAX]){{{                                                    \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_OUT_START_CH0, \
                                         },                                                   \
                                         {                                                    \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_OUT_START_CH1, \
                                         },                                                   \
                                         {                                                    \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_OUT_START_CH2, \
                                         }}}[group][chan][task]

#define GDMA_LL_RX_ETM_TASK_TABLE(group, chan, task)                                         \
    (uint32_t[1][3][GDMA_ETM_TASK_MAX]){{{                                                   \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_IN_START_CH0, \
                                         },                                                  \
                                         {                                                   \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_IN_START_CH1, \
                                         },                                                  \
                                         {                                                   \
                                             [GDMA_ETM_TASK_START] = GDMA_TASK_IN_START_CH2, \
                                         }}}[group][chan][task]

// TODO: Workaround for C5-beta3 only. C5-mp can still vectorized channels into an array in gdma_struct.h
#define GDMA_LL_CHANNEL_GET_REG_ADDR(dev, ch)      ((volatile gdma_chn_reg_t*[]){&dev->channel0, &dev->channel1, &dev->channel2}[(ch)])

///////////////////////////////////// Common /////////////////////////////////////////

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    PCR.gdma_conf.gdma_clk_en = enable;
}

/**
 * @brief Reset the DMA module
 */
static inline void gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.gdma_conf.gdma_rst_en = 1;
    PCR.gdma_conf.gdma_rst_en = 0;
}

/**
 * @brief Force enable register clock
 */
static inline void gdma_ll_force_enable_reg_clock(gdma_dev_t *dev, bool enable)
{
    dev->misc_conf.clk_en = enable;
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->in_intr[channel].raw.val;
    } else {
        return dev->in_intr[channel].st.val;
    }
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void gdma_ll_rx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->in_intr[channel].ena.val |= mask;
    } else {
        dev->in_intr[channel].ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->in_intr[channel].clr.val = mask;
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *gdma_ll_rx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->in_intr[channel].st);
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_rx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_conf1.in_check_owner = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, disabled by default
 */
static inline void gdma_ll_rx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_conf0.in_data_burst_en = enable;
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_rx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_conf0.indscr_burst_en = enable;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_conf0.in_rst = 1;
    ch->in.in_conf0.in_rst = 0;
}

/**
 * @brief Check if DMA RX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.infifo_status.val & 0x01;
}

/**
 * @brief Check if DMA RX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_rx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.infifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes in RX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_rx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.infifo_status.infifo_cnt;
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t gdma_ll_rx_pop_data(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_pop.infifo_pop = 1;
    return ch->in.in_pop.infifo_rdata;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_link.inlink_addr = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_start(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_link.inlink_start = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_stop(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_link.inlink_stop = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_rx_restart(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_link.inlink_restart = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void gdma_ll_rx_enable_auto_return(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_link.inlink_auto_ret = enable;
}

/**
 * @brief Check if DMA RX descriptor FSM is in IDLE state
 */
static inline bool gdma_ll_rx_is_desc_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.in_link.inlink_park;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_success_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.in_suc_eof_des_addr.val;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_error_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.in_err_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_rx_get_prefetched_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->in.in_dscr.val;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void gdma_ll_rx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_pri.rx_pri = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void gdma_ll_rx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_peri_sel.peri_in_sel = periph_id;
    ch->in.in_conf0.mem_trans_en = (periph == GDMA_TRIG_PERIPH_M2M);
}

/**
 * @brief Disconnect DMA RX channel from peripheral
 */
static inline void gdma_ll_rx_disconnect_from_periph(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_peri_sel.peri_in_sel = GDMA_LL_INVALID_PERIPH_ID;
    ch->in.in_conf0.mem_trans_en = false;
}

/**
 * @brief Whether to enable the ETM subsystem for RX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void gdma_ll_rx_enable_etm_task(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->in.in_conf0.in_etm_en = enable;
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_interrupt_status(gdma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->out_intr[channel].raw.val;
    } else {
        return dev->out_intr[channel].st.val;
    }
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void gdma_ll_tx_enable_interrupt(gdma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->out_intr[channel].ena.val |= mask;
    } else {
        dev->out_intr[channel].ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_clear_interrupt_status(gdma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->out_intr[channel].clr.val = mask;
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *gdma_ll_tx_get_interrupt_status_reg(gdma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->out_intr[channel].st);
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void gdma_ll_tx_enable_owner_check(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf1.out_check_owner = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, disabled by default
 */
static inline void gdma_ll_tx_enable_data_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.out_data_burst_en = enable;
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void gdma_ll_tx_enable_descriptor_burst(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.outdscr_burst_en = enable;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void gdma_ll_tx_set_eof_mode(gdma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.out_eof_mode = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void gdma_ll_tx_enable_auto_write_back(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.out_auto_wrback = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_reset_channel(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.out_rst = 1;
    ch->out.out_conf0.out_rst = 0;
}

/**
 * @brief Check if DMA TX FIFO is full
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_full(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.outfifo_status.val & 0x01;
}

/**
 * @brief Check if DMA TX FIFO is empty
 * @param fifo_level only supports level 1
 */
static inline bool gdma_ll_tx_is_fifo_empty(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.outfifo_status.val & 0x02;
}

/**
 * @brief Get number of bytes in TX FIFO
 * @param fifo_level only supports level 1
 */
static inline uint32_t gdma_ll_tx_get_fifo_bytes(gdma_dev_t *dev, uint32_t channel, uint32_t fifo_level)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.outfifo_status.outfifo_cnt;
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void gdma_ll_tx_push_data(gdma_dev_t *dev, uint32_t channel, uint32_t data)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_push.outfifo_wdata = data;
    ch->out.out_push.outfifo_push = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_set_desc_addr(gdma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_link.outlink_addr = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_start(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_link.outlink_start = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_stop(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_link.outlink_stop = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void gdma_ll_tx_restart(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_link.outlink_restart = 1;
}

/**
 * @brief Check if DMA TX descriptor FSM is in IDLE state
 */
static inline bool gdma_ll_tx_is_desc_fsm_idle(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.out_link.outlink_park;
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_eof_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.out_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t gdma_ll_tx_get_prefetched_desc_addr(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    return ch->out.out_dscr.val;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void gdma_ll_tx_set_priority(gdma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_pri.tx_pri = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void gdma_ll_tx_connect_to_periph(gdma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_peri_sel.peri_out_sel = periph_id;
}

/**
 * @brief Disconnect DMA TX channel from peripheral
 */
static inline void gdma_ll_tx_disconnect_from_periph(gdma_dev_t *dev, uint32_t channel)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_peri_sel.peri_out_sel = GDMA_LL_INVALID_PERIPH_ID;
}

/**
 * @brief Whether to enable the ETM subsystem for TX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void gdma_ll_tx_enable_etm_task(gdma_dev_t *dev, uint32_t channel, bool enable)
{
    volatile gdma_chn_reg_t *ch = (volatile gdma_chn_reg_t *)GDMA_LL_CHANNEL_GET_REG_ADDR(dev, channel);
    ch->out.out_conf0.out_etm_en = enable;
}

#ifdef __cplusplus
}
#endif
