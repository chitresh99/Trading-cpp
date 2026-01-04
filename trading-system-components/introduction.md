# Trading Infrastructure

## 1. High-level picture (mental model)

```
Exchange ↔ Market Data ↔ Strategy ↔ Risk ↔ OMS ↔ Exchange
                          ↕
                        PMS / Accounting
```

But internally this is **NOT** a monolith — it’s **many tightly coupled low-latency services**, often **in-process**, sometimes kernel-bypassed.

---

## 2. Market Data Infrastructure (MDI)

This is where *everything* starts.

## 2.1 Exchange → Firm

Exchanges publish:

* **ITCH / OUCH / FIX / Binary feeds**
* Multicast UDP (most equities/futures)
* TCP (some crypto / FIX)

### Components

**Market Data Handler (MDH)**:

* Written in **C++ / Rust**
* Kernel bypass (Solarflare, Mellanox)
* Uses **DPDK / RDMA / raw sockets**
* No allocations, no locks

Responsibilities:

* Parse binary feed
* Reconstruct order book (L2 / L3)
* Timestamp packets (HW timestamping)
* Normalize feeds across exchanges

```cpp
struct MDUpdate {
    uint64_t ts;
    uint32_t symbol_id;
    Price price;
    Qty qty;
    Side side;
};
```

**Important**:
Market data is **push-based**. Strategies react to *events*, not polling.

---

## 3. Strategy Engine

This is where “alpha” lives — but technically it’s very simple.

### Characteristics

* Runs **in the same process** as market data
* Single-threaded per symbol (to avoid locks)
* Deterministic
* Uses **pre-allocated memory**
* Often no STL, no exceptions

### Flow

```
MD Update → Strategy Logic → Order Intent
```

Example:

```cpp
if (imbalance > threshold && spread <= 1_tick) {
    send_order(BUY, price, qty);
}
```

Strategy does **NOT** talk directly to the exchange.

---

## 4. Risk Management Layer (Inline Risk)

This is **NOT** slow compliance risk.
This is **nanosecond-level pre-trade risk**.

### Inline Risk Checks

* Max position per symbol
* Max notional
* Self-trade prevention
* Fat-finger limits
* Rate limits

**Key idea**:
Risk is often a **function call**, not a service.

```cpp
bool risk_ok(const Order& o) {
    if (pos[o.symbol] + o.qty > limit) return false;
    return true;
}
```

If risk fails → order never reaches OMS.

---

## 5. OMS (Order Management System)

Now to your main question.

## 5.1 What OMS actually does in HFT

In HFT, OMS is **NOT** a UI-driven system like banks.

OMS is:

* Order state machine
* Exchange protocol adapter
* Latency-critical

### Responsibilities

* Assign order IDs
* Track order lifecycle
* Handle ACK / FILL / CANCEL / REJECT
* Maintain live order state

```text
NEW → ACK → PARTIAL_FILL → FILLED
       ↘
        CANCELLED / REJECTED
```

### OMS ≠ Strategy

Strategy says **what** to trade
OMS decides **how** to send it

---

## 5.2 OMS architecture

Typical OMS is:

* **In-process**
* Single writer
* Lock-free queues

```
Strategy → OMS → Exchange Gateway → Exchange
               ← ACK / FILL ←
```

Exchange Gateway:

* Speaks OUCH / FIX / native binary
* Maintains TCP/UDP sessions
* Heartbeats, seq numbers

---

## 6. Exchange Gateway (EG)

Sometimes part of OMS, sometimes separate.

### Why separate?

* One OMS → many exchanges
* Exchange-specific quirks

### Technical details

* Custom TCP stack
* Busy-polling NIC
* Deterministic retry logic
* No syscalls in hot path

Latency budget:

```
Strategy → Wire: ~200–800 ns
```

---

## 7. PMS (Position Management System)

Now PMS — very misunderstood.

## 7.1 PMS is NOT real-time trading

PMS:

* Tracks **positions**, **PnL**
* Used for:

  * Capital allocation
  * Post-trade risk
  * Reporting
* Often **eventually consistent**

### Flow

```
FILL → OMS → PMS
```

PMS updates:

* Position per symbol
* Cash
* Realized / unrealized PnL

---

## 7.2 PMS latency

* Milliseconds to seconds
* Often async
* Can run in **separate process / machine**

HFT trading **does not wait for PMS**.

---

## 8. Post-Trade Systems

After fills:

### 8.1 Trade Capture

* Persist trades
* Write-ahead logs
* Replayable streams

### 8.2 Reconciliation

* Match internal fills vs exchange
* Detect dropped packets
* End-of-day corrections

### 8.3 Risk & Compliance

* Reg-NMS
* Market abuse detection
* Kill-switch audits

---

## 9. Infrastructure Layer

This is where big HFTs win.

## Hardware

* Bare metal
* CPU pinning
* Huge pages
* NUMA-aware memory
* FPGA (market data, risk, quoting)

## OS

* Linux
* `isolcpus`
* `nohz_full`
* Tickless kernel
* Real-time scheduling

## Networking

* Kernel bypass
* Direct exchange cross-connects
* Microwave / laser links

---

## 10. Typical end-to-end flow (step-by-step)

```
1. Exchange sends MD packet
2. MDH parses + timestamps
3. Strategy reacts
4. Inline risk check
5. OMS builds order
6. Exchange gateway sends order
7. Exchange ACKs
8. Fill arrives
9. OMS updates state
10. Strategy reacts to fill
11. PMS updated async
```

All of this can happen in **< 5 microseconds**.

---

# 11. How this differs from non-HFT firms

| Component | Bank / Broker    | HFT        |
| --------- | ---------------- | ---------- |
| OMS       | Heavy, DB-backed | In-memory  |
| Risk      | Service call     | Inline     |
| PMS       | Central          | Async      |
| Latency   | ms–seconds       | ns–µs      |
| Language  | Java             | C++ / Rust |
| Infra     | Cloud            | Bare metal |

---